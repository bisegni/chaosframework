/*
 *	RTAbstractControlUnit.h
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

#ifndef __CHAOSFramework__RTAbstractControlUnit__
#define __CHAOSFramework__RTAbstractControlUnit__

#include <chaos/cu_toolkit/ControlManager/DSAttributeHandlerExecutionEngine.h>
#include <chaos/cu_toolkit/ControlManager/handler/TDSObjectHandler.h>
#include <chaos/cu_toolkit/ControlManager/AbstractControlUnit.h>

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
	using namespace std;
    namespace cu {
		
        class ControManager;
        
        using namespace cu::handler;
        
        class RTAbstractControlUnit : public AbstractControlUnit  {
            friend class ControlManager;
            friend class DomainActionsScheduler;
			
			uint64_t schedule_dalay;
			bool scheduler_run;
			boost::scoped_ptr<boost::thread>  scheduler_thread;
            
            cu::DSAttributeHandlerExecutionEngine *attributeHandlerEngine;
            
            /*!
             Define the control unit DataSet and Action into
             a CDataWrapper
             */
            void _defineActionAndDataset(chaos_data::CDataWrapper&) throw(CException);
            
            
            void init(void *initData) throw(CException);
            
            /*
             Internal implementation of the runmethod, that (for now) will schedule the slowcommand sandbox
             */
            void start() throw(CException);
            
            /*
             Execute the Control Unit work
             */
            void stop() throw(CException);
            
            void deinit() throw(CException);
        protected:
			
            virtual void unitRun() throw(CException) = 0;
			
            
            void pushDataSet(chaos_data::CDataWrapper *acquiredData);
            
            void setDefaultScheduleDelay(uint64_t _defaultScheduleDelay);
            /*!
             return a new instance of CDataWrapper filled with a mandatory data
             according to key
             */
            chaos_data::CDataWrapper *getNewDataWrapper();
            
            /*
             return the appropriate thread for the device
             */
            inline void threadStartStopManagment(bool startAction) throw(CException);
            
            
            /*!
             Event for update some CU configuration
             */
            virtual chaos_data::CDataWrapper* updateConfiguration(chaos_data::CDataWrapper*, bool&) throw (CException);
            
            /*!
             Thread method for the scheduler
             */
            void executeOnThread();
            
            /*!
             Add the new attribute in the dataset for at the CU dataset with an associated handler
             */
            template<typename T>
            void addTemplatedAttributeToDataSet(const char*const attributeName,
                                                const char*const attributeDescription,
                                                DataType::DataType attributeType,
                                                DataType::DataSetAttributeIOAttribute attributeDirection,
                                                TDSAttributeHandler<T>* handler) {
                
                //first add the attribute information in the schema
                DatasetDB::addAttributeToDataSet(attributeName, attributeDescription, attributeType, attributeDirection);
                
                
                //check if there is a valid handler
                if(handler != NULL) {
                    if(handler->attributeName.size() == 0){
                        //add the attribute name to the handler
                        handler->attributeName.assign(attributeName);
                    }
                    //the ad the handler for that
                    addHandlerForDSAttribute(handler);
                }
            }
            
            
            /*!
             Add the new attribute in the dataset for at the CU dataset with an associated handler
             */
            template<typename T>
            void addInputInt32AttributeToDataSet(const char*const attributeName,
                                                 const char*const attributeDescription,
                                                 T* objectPointer,
                                                 typename TDSObjectHandler<T, int32_t>::TDSHandler  objectHandler) {
                
                addTemplatedAttributeToDataSet(attributeName, attributeDescription, DataType::TYPE_INT32, DataType::Input, new TDSObjectHandler<T, int32_t>(objectPointer, objectHandler));
            }
            
            /*!
             Add the new attribute in the dataset for at the CU dataset with an associated handler
             */
            template<typename T>
            void addInputInt64AttributeToDataSet(const char*const attributeName,
                                                 const char*const attributeDescription,
                                                 T* objectPointer,
                                                 typename TDSObjectHandler<T, int64_t>::TDSHandler  objectHandler) {
                
                addTemplatedAttributeToDataSet(attributeName, attributeDescription, DataType::TYPE_INT64, DataType::Input, new TDSObjectHandler<T, int64_t>(objectPointer, objectHandler));
            }
            
            /*!
             Add the new attribute in the dataset for at the CU dataset with an associated handler
             */
            template<typename T>
            void addInputDoubleAttributeToDataSet(const char*const attributeName,
                                                  const char*const attributeDescription,
                                                  T* objectPointer,
                                                  typename TDSObjectHandler<T, double>::TDSHandler  objectHandler) {
                
                addTemplatedAttributeToDataSet(attributeName, attributeDescription, DataType::TYPE_DOUBLE, DataType::Input, new TDSObjectHandler<T, double>(objectPointer, objectHandler));
            }
            
            /*!
             Add the new attribute in the dataset for at the CU dataset with an associated handler
             */
            template<typename T>
            void addInputBooleanAttributeToDataSet(const char*const attributeName,
                                                   const char*const attributeDescription,
                                                   T* objectPointer,
                                                   typename TDSObjectHandler<T, bool>::TDSHandler  objectHandler) {
                
                addTemplatedAttributeToDataSet(attributeName, attributeDescription, DataType::TYPE_BOOLEAN, DataType::Input, new TDSObjectHandler<T, bool>(objectPointer, objectHandler));
            }
            
            /*!
             Add the new attribute in the dataset for at the CU dataset with an associated handler
             */
            template<typename T>
            void addInputStringAttributeToDataSet(const char*const attributeName,
                                                  const char*const attributeDescription,
                                                  T* objectPointer,
                                                  typename TDSObjectHandler<T, std::string>::TDSHandler  objectHandler) {
                
                addTemplatedAttributeToDataSet(attributeName, attributeDescription, DataType::TYPE_STRING, DataType::Input, new TDSObjectHandler<T, std::string>(objectPointer, objectHandler));
            }
            
            /*!
             Add the new attribute in the dataset for at the CU dataset with an associated handler
             */
            template<typename T>
            void addInputStructAttributeToDataSet(const char*const attributeName,
                                                  const char*const attributeDescription,
                                                  T* objectPointer,
                                                  typename TDSObjectHandler<T, chaos_data::CDataWrapper>::TDSHandler  objectHandler) {
                
                addTemplatedAttributeToDataSet(attributeName, attributeDescription, DataType::TYPE_STRUCT, DataType::Input, new TDSObjectHandler<T, chaos_data::CDataWrapper>(objectPointer, objectHandler));
            }
            
            //! add an handler for a determinate device id and attribute set
            /*!
             *  This function permit to attach an handler to an attribute name. When the control unit
             *  receive by RPC an evento to set, a determinate attribute, to an specified value, this
             *  handler will be called.
             *  \param attrName is the name of the attribute where the handler need to be attached
             *  \param classHandler is the pointer to handler that need to be attached
             *  \exception something is gone wrong
             */
            void addHandlerForDSAttribute(cu::handler::DSAttributeHandler * classHandler)  throw (CException);
            
            /*
             Receive the event for set the dataset input element, this virtual method
             is empty because can be used by controlunit implementation
             */
            chaos_data::CDataWrapper* setDatasetAttribute(chaos_data::CDataWrapper *datasetAttributeValues, bool& detachParam) throw (CException);
        public:
            
			//! default constructor
            RTAbstractControlUnit(const std::string& _control_unit_id);
			/*!
			 Parametrized constructor
			 \param _control_unit_id unique id for the control unit
			 \param _control_unit_drivers driver information
			 */
            RTAbstractControlUnit(const std::string& _control_unit_id, const ControlUnitDriverList& _control_unit_drivers);
            ~RTAbstractControlUnit();
        };
        
    }
}

#endif /* defined(__CHAOSFramework__RTAbstractControlUnit__) */
