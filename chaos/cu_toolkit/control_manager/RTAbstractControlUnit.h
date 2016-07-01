/*
 *	RTAbstractControlUnit.h
 *	!CHAOS
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

#ifndef __CHAOSFramework__RTAbstractControlUnit__
#define __CHAOSFramework__RTAbstractControlUnit__

#include <chaos/cu_toolkit/control_manager/AbstractControlUnit.h>

#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>

#define CREATE_HANDLER(class, type, abstractPointer)\
TDSObjectHandler<T, double> *typedHandler = NULL;\
typename TDSObjectHandler<T, double>::TDSHandler handlerPointer = objectMethodHandler;\
abstractPointer = typedHandler = new TDSObjectHandler<T, double>(objectPointer, handlerPointer);

namespace chaos_data = chaos::common::data;

namespace chaos {
	using namespace boost;
	using namespace boost::chrono;

    namespace cu {
		namespace control_manager {
                //forward declarations
            class ControManager;
            class AbstractExecutionUnit;

			class RTAbstractControlUnit:
            public AbstractControlUnit {
				friend class ControlManager;
				friend class DomainActionsScheduler;
                friend class AbstractExecutionUnit;

                bool scheduler_run;
				uint64_t schedule_dalay;
				boost::scoped_ptr<boost::thread>  scheduler_thread;

				/*!
				 Define the control unit DataSet and Action into
				 a CDataWrapper
				 */
				void _defineActionAndDataset(chaos_data::CDataWrapper& setup_configuration) throw(CException);
                
				//! init rt control unit
				void init(void *initData) throw(CException);
				
				//! start rt control unit
				void start() throw(CException);
				
				//! stop rt control unit
				void stop() throw(CException);
				
				//! deinit rt control unit
				void deinit() throw(CException);
                //!redefine private for protection
                AbstractSharedDomainCache* _getAttributeCache();
			protected:
                
                /*! default constructor
                 \param _control_unit_param is a string that contains parameter to pass during the contorl unit creation
                 \param _control_unit_drivers driver information
                 */
                RTAbstractControlUnit(const std::string& _alternate_type,
                                      const std::string& _control_unit_id,
                                      const std::string& _control_unit_param);
                /*!
                 Parametrized constructor
                 \param _control_unit_id unique id for the control unit
                 \param _control_unit_param is a string that contains parameter to pass during the contorl unit creation
                 \param _control_unit_drivers driver information
                 */
                RTAbstractControlUnit(const std::string& _alternate_type,
                                      const std::string& _control_unit_id,
                                      const std::string& _control_unit_param,
                                      const ControlUnitDriverList& _control_unit_drivers);
                
				//! schdule a run of the rt control unit
				virtual void unitRun() throw(CException) = 0;
				
				//! set the dafult run schedule time intervall
				void setDefaultScheduleDelay(uint64_t _defaultScheduleDelay);
				
				/*
				 return the appropriate thread for the device
				 */
				inline void threadStartStopManagment(bool startAction) throw(CException);
				
				/*!
				 Event for update some CU configuration
				 */
				virtual chaos_data::CDataWrapper* updateConfiguration(CDataWrapper* update_pack, bool& detach_param) throw (CException);
				
				/*!
				 Thread method for the scheduler
				 */
				void executeOnThread();
			public:
				
				/*! default constructor
				 \param _control_unit_param is a string that contains parameter to pass during the contorl unit creation
				 \param _control_unit_drivers driver information
				 */
				RTAbstractControlUnit(const std::string& _control_unit_id,
									  const std::string& _control_unit_param);
				/*!
				 Parametrized constructor
				 \param _control_unit_id unique id for the control unit
				 \param _control_unit_param is a string that contains parameter to pass during the contorl unit creation
				 \param _control_unit_drivers driver information
				 */
				RTAbstractControlUnit(const std::string& _control_unit_id,
									  const std::string& _control_unit_param,
									  const ControlUnitDriverList& _control_unit_drivers);

				~RTAbstractControlUnit();
			};
		}
        
    }
}

#endif /* defined(__CHAOSFramework__RTAbstractControlUnit__) */
