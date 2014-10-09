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

#include <boost/format.hpp>
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::data::cache;

using namespace chaos::cu;
using namespace chaos::cu::control_manager;

using namespace boost;
using namespace boost::chrono;

#define RTCULAPP_ LAPP_ << "[Real Time Control Unit:"<<getCUInstance()<<"] - "

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
    	setupConfiguration.addInt64Value(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY , schedule_dalay);
    }
}

/*!
 Init the  RT Control Unit scheduling for device
 */
void RTAbstractControlUnit::init(void *initData) throw(CException) {
	//call parent impl
	AbstractControlUnit::init(initData);
	scheduler_run = false;
    //RTCULAPP_ << "Initialize the DSAttribute handler engine for device:" << DatasetDB::getDeviceID();
    //utility::StartableService::initImplementation(attributeHandlerEngine, (void*)NULL, "DSAttribute handler engine", __PRETTY_FUNCTION__);
	
	RTCULAPP_ << "Initializing shared attribute cache " << DatasetDB::getDeviceID();
	utility::InizializableService::initImplementation((AttributeValueSharedCache*)attribute_value_shared_cache, (void*)NULL, "attribute_value_shared_cache", __PRETTY_FUNCTION__);
}

/*!
 Starto the  Control Unit scheduling for device
 */
void RTAbstractControlUnit::start() throw(CException) {
	//call parent impl
	AbstractControlUnit::start();
	
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
	utility::InizializableService::deinitImplementation((AttributeValueSharedCache*)attribute_value_shared_cache, "attribute_value_shared_cache", __PRETTY_FUNCTION__);

	
   // RTCULAPP_ << "Deinitializing the DSAttribute handler engine for device:" << DatasetDB::getDeviceID();
   // utility::StartableService::deinitImplementation(attributeHandlerEngine, "DSAttribute handler engine", __PRETTY_FUNCTION__);
}

/*!
 return the appropriate thread for the device
 */
void RTAbstractControlUnit::threadStartStopManagment(bool startAction) throw(CException) {
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
}

/*!
 Event for update some CU configuration
 */
CDataWrapper* RTAbstractControlUnit::updateConfiguration(CDataWrapper* updatePack, bool& detachParam) throw (CException) {
	CDataWrapper *result = AbstractControlUnit::updateConfiguration(updatePack, detachParam);
	if(updatePack->hasKey(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY)){
		//we need to configure the delay  from a run() call and the next
		uint64_t uSecdelay = updatePack->getUInt64Value(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY);
		//check if we need to update the scehdule time
		if(uSecdelay != schedule_dalay){
			RTCULAPP_ << "Update schedule delay in:" << uSecdelay << " microsecond";
			schedule_dalay = uSecdelay;
			//send envet for the update
			//----------------------
			// we need to optimize and be sure that event channel
			// is mandatory so we can left over the 'if' check
			//----------------------
			if(device_event_channel) device_event_channel->notifyForScheduleUpdateWithNewValue(DatasetDB::getDeviceID(), uSecdelay);
		}
	}
	return result;
}

/*
 Execute the scehduling for the device
 */
void RTAbstractControlUnit::executeOnThread() {
	uint64_t acq_timestamp = 0;
	
	while(scheduler_run) {
		//set the acquiition time stamp and update it on cache
		acq_timestamp = TimingUtil::getTimeStamp();
		cache_output_attribute_vector[timestamp_acq_cache_index]->setValue(&acq_timestamp, sizeof(uint64_t), false);
		
		unitRun();
		
		//! check if the output dataset need to be pushed
		pushOutputDataset();
		
		//check if we are in sequential or in threaded mode
		boost::this_thread::sleep_for(boost::chrono::microseconds(schedule_dalay));
	}
}

void RTAbstractControlUnit::pushOutputDataset() {
	AttributesSetting& output_attribute_cache = attribute_value_shared_cache->getSharedDomain(AttributeValueSharedCache::SVD_OUTPUT);
	
	//check if something as changed
	if(!output_attribute_cache.hasChanged()) return;
	
	CDataWrapper *output_attribute_dataset = keyDataStorage->getNewOutputAttributeDataWrapper();
	if(!output_attribute_dataset) return;
	
	//write device id for first
	ValueSetting * value_set = cache_output_attribute_vector[timestamp_acq_cache_index-1];
	output_attribute_dataset->addStringValue(value_set->name.c_str(), value_set->getValuePtr<const char>());
	
	//write acq ts for second
	value_set = cache_output_attribute_vector[timestamp_acq_cache_index];
	output_attribute_dataset->addInt64Value(value_set->name.c_str(), *value_set->getValuePtr<int64_t>());
	
	//add all other output channel
	for(int idx = 0;
		idx < cache_output_attribute_vector.size() - 2;
		idx++) {
		//
		ValueSetting * value_set = cache_output_attribute_vector[idx];
		switch(value_set->type) {
			case DataType::TYPE_BOOLEAN:
				output_attribute_dataset->addBoolValue(value_set->name.c_str(), *value_set->getValuePtr<bool>());
				break;
			case DataType::TYPE_INT32:
				output_attribute_dataset->addInt32Value(value_set->name.c_str(), *value_set->getValuePtr<int32_t>());
				break;
			case DataType::TYPE_INT64:
				output_attribute_dataset->addInt64Value(value_set->name.c_str(), *value_set->getValuePtr<int64_t>());
				break;
			case DataType::TYPE_DOUBLE:
				output_attribute_dataset->addDoubleValue(value_set->name.c_str(), *value_set->getValuePtr<double>());
				break;
			case DataType::TYPE_STRING:
				output_attribute_dataset->addStringValue(value_set->name.c_str(), value_set->getValuePtr<const char>());
				break;
			case DataType::TYPE_BYTEARRAY:
				output_attribute_dataset->addBinaryValue(value_set->name.c_str(), value_set->getValuePtr<char>(), value_set->size);
				break;
		}
	}
	
	//now we nede to push the outputdataset
	keyDataStorage->pushDataSet(data_manager::KeyDataStorageDomainOutput, output_attribute_dataset);
	
	//reset chagned attribute into output dataset
	output_attribute_cache.resetChangedIndex();
}

#define CHECK_FOR_RANGE_VALUE(t, v, attr_name)\
t max = attributeInfo.maxRange.size()?boost::lexical_cast<t>(attributeInfo.maxRange):std::numeric_limits<t>::max();\
t min = attributeInfo.maxRange.size()?boost::lexical_cast<t>(attributeInfo.minRange):std::numeric_limits<t>::min();\
if(v < min || v > max) throw CException(-1,  boost::str(boost::format("Invalid value (%1%) [max:%2% Min:%3%] for attribute %4%") % v % attr_name % attributeInfo.minRange % attributeInfo.maxRange).c_str(), __PRETTY_FUNCTION__);\

#define CHECK_FOR_STRING_RANGE_VALUE(v, attr_name)\
if(attributeInfo.minRange.size() && v < attributeInfo.minRange ) throw CException(-1, boost::str(boost::format("Invalid value (%1%) [max:%2% Min:%3%] for attribute %4%") % v % attr_name % attributeInfo.minRange % attributeInfo.maxRange).c_str(), __PRETTY_FUNCTION__);\
if(attributeInfo.maxRange.size() && v > attributeInfo.maxRange ) throw CException(-1, boost::str(boost::format("Invalid value (%1%) [max:%2% Min:%3%] for attribute %4%") % v % attr_name %attributeInfo.minRange % attributeInfo.maxRange).c_str(), __PRETTY_FUNCTION__);\


/*
 Receive the evento for set the dataset input element
 */
CDataWrapper* RTAbstractControlUnit::setDatasetAttribute(CDataWrapper *dataset_attribute_values, bool& detachParam) throw (CException) {
	CHAOS_ASSERT(dataset_attribute_values)
	std::vector<std::string> in_attribute_name;
	RangeValueInfo attributeInfo;
	
	//get all input attribute name
	getDatasetAttributesName(DataType::Input , in_attribute_name);
	
	if(dataset_attribute_values->hasKey(DatasetDefinitionkey::DEVICE_ID)) {
		std::string _messageDeviceID = dataset_attribute_values->getStringValue(DatasetDefinitionkey::DEVICE_ID);
		
		//compare the message device id and the local
		for (std::vector<std::string>::iterator iter = in_attribute_name.begin();
			 iter != in_attribute_name.end();
			 iter++) {
			//execute attribute handler
			const char * cAttrName = iter->c_str();
			
			//check if the attribute name is present
			if(dataset_attribute_values->hasKey(cAttrName)) {
				
				ValueSetting * attribute_cache_value = attribute_value_shared_cache->getVariableValue(SharedCacheInterface::SVD_INPUT, iter->c_str());
				
				//get attribute info
				getAttributeRangeValueInfo(*iter, attributeInfo);
				//call handler
				switch (attribute_cache_value->type) {
					case DataType::TYPE_BOOLEAN: {
						bool bv = dataset_attribute_values->getBoolValue(cAttrName);
						attribute_cache_value->setValue(&bv, sizeof(bool));
						break;
					}
					case DataType::TYPE_INT32: {
						int32_t i32v = dataset_attribute_values->getInt32Value(cAttrName);
						CHECK_FOR_RANGE_VALUE(int32_t, i32v, cAttrName)
						attribute_cache_value->setValue(&i32v, sizeof(int32_t));
						break;
					}
					case DataType::TYPE_INT64: {
						int64_t i64v = dataset_attribute_values->getInt64Value(cAttrName);
						CHECK_FOR_RANGE_VALUE(int64_t, i64v, cAttrName)
						attribute_cache_value->setValue(&i64v, sizeof(int64_t));
						break;
					}
					case DataType::TYPE_DOUBLE: {
						double dv = dataset_attribute_values->getDoubleValue(cAttrName);
						CHECK_FOR_RANGE_VALUE(double, dv, cAttrName)
						attribute_cache_value->setValue(&dv, sizeof(double));
						break;
					}
					case DataType::TYPE_STRING: {
						std::string str = dataset_attribute_values->getStringValue(cAttrName);
						CHECK_FOR_STRING_RANGE_VALUE(str, cAttrName)
						attribute_cache_value->setValue(str.c_str(), (uint32_t)str.size());
						break;
					}
					case DataType::TYPE_BYTEARRAY: {
						int bin_size = 0;
						const char *binv = dataset_attribute_values->getBinaryValue(cAttrName, bin_size);
						attribute_cache_value->setValue(binv, bin_size);
						break;
					}
				}
			}
		}
		//inform subclas for the change
		unitInputAttributeChangedHandler();
	}
	//at this time notify the wel gone setting of comand
	//if(deviceEventChannel) deviceEventChannel->notifyForAttributeSetting(DatasetDB::getDeviceID(), 0);
	return NULL;
}
