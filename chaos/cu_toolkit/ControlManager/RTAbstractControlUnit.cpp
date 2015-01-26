/*
 *	RTAbstractControlUnit.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#include <limits>
#include <chaos/common/event/channel/InstrumentEventChannel.h>
#include <chaos/cu_toolkit/ControlManager/RTAbstractControlUnit.h>


using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::data::cache;
using namespace chaos::common::utility;

using namespace chaos::cu;
using namespace chaos::cu::control_manager;

using namespace boost;
using namespace boost::chrono;
#define RTCUL_HEAD "[Real Time Control Unit:"<<getCUInstance()<<"] - "
#define RTCULAPP_ LAPP_ << RTCUL_HEAD
#define RTCULDBG_ LDBG_ << RTCUL_HEAD
#define RTCULERR_ LERR_ << RTCUL_HEAD << __PRETTY_FUNCTION__ <<"(" << __LINE__ << ") - "

RTAbstractControlUnit::RTAbstractControlUnit(const std::string& _control_unit_id,
											 const std::string& _control_unit_param):
AbstractControlUnit(CUType::RTCU,
					_control_unit_id,
					_control_unit_param) {
	//allocate the handler engine
	// attributeHandlerEngine = new DSAttributeHandlerExecutionEngine(this);
	
	//associate the shared cache of the executor to the asbtract control unit one
	attribute_value_shared_cache = new AttributeValueSharedCache();
}

/*!
 Parametrized constructor
 \param _control_unit_id unique id for the control unit
 \param _control_unit_drivers driver information
 */
RTAbstractControlUnit::RTAbstractControlUnit(const std::string& _control_unit_id,
											 const std::string& _control_unit_param,
											 const ControlUnitDriverList& _control_unit_drivers):
AbstractControlUnit(CUType::RTCU,
					_control_unit_id,
					_control_unit_param,
					_control_unit_drivers) {
	//allocate the handler engine
	//attributeHandlerEngine = new DSAttributeHandlerExecutionEngine(this);
	
	//associate the shared cache of the executor to the asbtract control unit one
	attribute_value_shared_cache = new AttributeValueSharedCache();
}

RTAbstractControlUnit::~RTAbstractControlUnit() {
	//release attribute shared cache
	if(attribute_value_shared_cache) {
		delete(attribute_value_shared_cache);
	}
	
	//release handler engine
	//if(attributeHandlerEngine) {
	//    delete attributeHandlerEngine;
	//}
}

void RTAbstractControlUnit::setDefaultScheduleDelay(uint64_t _schedule_dalay) {
	schedule_dalay = _schedule_dalay;
}

/*
 fill the CDataWrapper with AbstractCU system configuration, this method
 is called after getStartConfiguration directly by sandbox. in this method
 are defined the action for the input element of the dataset
 */
void RTAbstractControlUnit::_defineActionAndDataset(CDataWrapper& setupConfiguration)  throw(CException) {
	AbstractControlUnit::_defineActionAndDataset(setupConfiguration);
	//add the scekdule dalay for the sandbox
	if(schedule_dalay){
		//in this case ovverrride the config file
		setupConfiguration.addInt64Value(CUDefinitionKey::THREAD_SCHEDULE_DELAY , schedule_dalay);
	}
}

/*!
 Init the  RT Control Unit scheduling for device
 */
void RTAbstractControlUnit::init(void *initData) throw(CException) {
	//call parent impl
	AbstractControlUnit::init(initData);
	scheduler_run = false;
	last_hearbeat_time = 0;
	//RTCULAPP_ << "Initialize the DSAttribute handler engine for device:" << DatasetDB::getDeviceID();
	//StartableService::initImplementation(attributeHandlerEngine, (void*)NULL, "DSAttribute handler engine", __PRETTY_FUNCTION__);
	
	RTCULAPP_ << "Initializing shared attribute cache " << DatasetDB::getDeviceID();
	InizializableService::initImplementation((AttributeValueSharedCache*)attribute_value_shared_cache, (void*)NULL, "attribute_value_shared_cache", __PRETTY_FUNCTION__);
}

/*!
 Starto the  Control Unit scheduling for device
 */
void RTAbstractControlUnit::start() throw(CException) {
	//call parent impl
	AbstractControlUnit::start();
	
	//prefetch handle for heartbeat and acuisition ts cached value
	hb_cached_value = attribute_value_shared_cache->getAttributeValue(DOMAIN_SYSTEM, DataPackSystemKey::DP_SYS_HEARTBEAT);
	run_acquisition_ts_handle = reinterpret_cast<uint64_t**>(&attribute_value_shared_cache->getAttributeValue(DOMAIN_OUTPUT,
																											  DataPackCommonKey::DPCK_TIMESTAMP)->value_buffer);
	
	RTCULAPP_ << "Starting thread for device:" << DatasetDB::getDeviceID();
	threadStartStopManagment(true);
	RTCULAPP_ << "Thread started for device:" << DatasetDB::getDeviceID();
}

/*!
 Stop the Custom Control Unit scheduling for device
 */
void RTAbstractControlUnit::stop() throw(CException) {
	//call parent impl
	AbstractControlUnit::stop();
	
	//manage the thread
	RTCULAPP_ << "Stopping thread for device:" << DatasetDB::getDeviceID();
	threadStartStopManagment(false);
	RTCULAPP_ << "Thread for device stopped:" << DatasetDB::getDeviceID();
}

/*!
 Init the  RT Control Unit scheduling for device
 */
void RTAbstractControlUnit::deinit() throw(CException) {
	//call parent impl
	AbstractControlUnit::deinit();
	
	RTCULAPP_ << "Initializing shared attribute cache " << DatasetDB::getDeviceID();
	InizializableService::deinitImplementation((AttributeValueSharedCache*)attribute_value_shared_cache, "attribute_value_shared_cache", __PRETTY_FUNCTION__);
	
	
	// RTCULAPP_ << "Deinitializing the DSAttribute handler engine for device:" << DatasetDB::getDeviceID();
	// StartableService::deinitImplementation(attributeHandlerEngine, "DSAttribute handler engine", __PRETTY_FUNCTION__);
}

/*!
 return the appropriate thread for the device
 */
void RTAbstractControlUnit::threadStartStopManagment(bool startAction) throw(CException) {
	try{
		if(startAction) {
			if(scheduler_thread.get() && scheduler_run){
				RTCULAPP_ << "thread already running";
				throw CException(-5, "Thread for device already running", "RTAbstractControlUnit::threadStartStopManagment");
			}
			scheduler_run = true;
			scheduler_thread.reset(new boost::thread(boost::bind(&RTAbstractControlUnit::executeOnThread, this)));
#if defined(__linux__) || defined(__APPLE__)
			int retcode;
			int policy;
			struct sched_param param;
			pthread_t threadID = (pthread_t) scheduler_thread->native_handle();
			if ((retcode = pthread_getschedparam(threadID, &policy, &param)) != 0)  {
				return;
			}
			
			policy = SCHED_RR;
			param.sched_priority = sched_get_priority_max(SCHED_RR);
			if ((retcode = pthread_setschedparam(threadID, policy, &param)) != 0) {
				errno = retcode;
			}
			
			if ((retcode = pthread_getschedparam(threadID, &policy, &param)) != 0) {
				errno = retcode;
				return;
			}
#endif
		} else {
			if(!scheduler_run){
				RTCULAPP_ << "thread already runnign";
				throw CException(-5, "Thread for device already running", "RTAbstractControlUnit::threadStartStopManagment");
			}
			RTCULAPP_ << "Stopping and joining scheduling thread";
			scheduler_run = false;
			scheduler_thread->join();
			RTCULAPP_ << "Thread stopped";
		}
	} catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::thread_resource_error> >& exc) {
		RTCULERR_ << exc.what();
		throw CException(-1, exc.what(), std::string(__PRETTY_FUNCTION__));
	}
}

/*!
 Event for update some CU configuration
 */
CDataWrapper* RTAbstractControlUnit::updateConfiguration(CDataWrapper* updatePack, bool& detachParam) throw (CException) {
	CDataWrapper *result = AbstractControlUnit::updateConfiguration(updatePack, detachParam);
	if(updatePack->hasKey(CUDefinitionKey::THREAD_SCHEDULE_DELAY)){
		//we need to configure the delay  from a run() call and the next
		uint64_t uSecdelay = updatePack->getUInt64Value(CUDefinitionKey::THREAD_SCHEDULE_DELAY);
		//check if we need to update the scehdule time
		if(uSecdelay != schedule_dalay){
			RTCULAPP_ << "Update schedule delay in:" << uSecdelay << " microsecond";
			schedule_dalay = uSecdelay;
			//send envet for the update
			//----------------------
			// we need to optimize and be sure that event channel
			// is mandatory so we can left over the 'if' check
			//----------------------
			//if(device_event_channel) device_event_channel->notifyForScheduleUpdateWithNewValue(DatasetDB::getDeviceID(), uSecdelay);
		}
	}
	return result;
}

/*
 Execute the scehduling for the device
 */
void RTAbstractControlUnit::executeOnThread() {
	while(scheduler_run) {
		//set the acquiition time stamp and update it on cache
		if((**run_acquisition_ts_handle = TimingUtil::getTimeStamp()) > last_hearbeat_time+1000) {
			//copy to hearbeat cached value
			last_hearbeat_time = **run_acquisition_ts_handle;
			hb_cached_value->setValue(&last_hearbeat_time, sizeof(uint64_t));
			//fire system data set
			pushSystemDataset();
		}
		//! exec the control unit step
		unitRun();
		
		//! check if the output dataset need to be pushed
		pushOutputDataset();
		
		//check if we are in sequential or in threaded mode
		boost::this_thread::sleep_for(boost::chrono::microseconds(schedule_dalay));
	}
}
