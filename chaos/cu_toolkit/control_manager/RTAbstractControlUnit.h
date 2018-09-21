/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
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
				uint64_t schedule_delay;
				boost::scoped_ptr<boost::thread>  scheduler_thread;

				/*!
				 Define the control unit DataSet and Action into
				 a CDataWrapper
				 */
				void _defineActionAndDataset(chaos::common::data::CDataWrapper& setup_configuration);
                
				//! init rt control unit
				void init(void *initData);
				
				//! start rt control unit
				void start();
				
				//! stop rt control unit
				void stop();
				
				//! deinit rt control unit
				void deinit();
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
				virtual void unitRun() = 0;
				
				//! set the dafult run schedule time intervall
				void setDefaultScheduleDelay(uint64_t _defaultScheduleDelay);
				
				/*
				 return the appropriate thread for the device
				 */
				inline void threadStartStopManagment(bool startAction);
				
                //!inherited method by @AbstractControlUnit
                void propertyUpdatedHandler(const std::string& group_name,
                                            const std::string& property_name,
                                            const chaos::common::data::CDataVariant& old_value,
                                            const chaos::common::data::CDataVariant& new_value);
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
