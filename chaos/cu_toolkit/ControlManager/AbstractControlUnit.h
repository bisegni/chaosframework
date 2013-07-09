/*
 *	ControlUnit.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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
#ifndef ControlUnit_H
#define ControlUnit_H
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <map>
#include <set>
#include <string>
#include <vector>

#include <boost/chrono.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include <chaos/common/data/CUSchemaDB.h>
#include <chaos/common/general/Configurable.h>
#include <chaos/common/action/ActionDescriptor.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/utility/ArrayPointer.h>
#include <chaos/common/thread/CThread.h>
#include <chaos/common/pqueue/CObjectHandlerProcessingQueue.h>
#include <chaos/common/thread/CThreadExecutionTask.h>

#include <chaos/cu_toolkit/DataManager/KeyDataStorage.h>
#include <chaos/cu_toolkit/ControlManager/DSAttributeHandlerExecutionEngine.h>
#include <chaos/cu_toolkit/ControlManager/handler/TDSObjectHandler.h>
#include <chaos/cu_toolkit/ControlManager/slcmd/SlowCommandExecutor.h>

#define CU_IDENTIFIER_C_STREAM getCUName() << "_" << getCUInstance()
#define INIT_STATE      0
#define START_STATE     1

#define CREATE_HANDLER(class, type, abstractPointer)\
TDSObjectHandler<T, double> *typedHandler = NULL;\
typename TDSObjectHandler<T, double>::TDSHandler handlerPointer = objectMethodHandler;\
abstractPointer = typedHandler = new TDSObjectHandler<T, double>(objectPointer, handlerPointer);\

namespace chaos{
    
    //forward event channel declaration
    namespace event{
        namespace channel {
            class InstrumentEventChannel;
        }
    }
    
    namespace cu {
        
        class ControManager;
        class ControlUnitThread;
        
        using namespace std;
        using namespace cu::handler;
        //using namespace cu::control_manager::slow_command;
        
        class ActionData {
            
        public:
            void purge(){
                if(actionData) delete(actionData);
            }
            CDataWrapper *actionData;
            boost::function<CDataWrapper*(CDataWrapper*, bool)> actionDef;
        };

        /*!
         Base class for control unit execution task
         */
        class AbstractControlUnit : protected cu::control_manager::slow_command::SlowCommandExecutor, public DeclareAction, protected CUSchemaDB, public CThreadExecutionTask {
            friend class ControlManager;
            friend class DomainActionsScheduler;
            
            int32_t scheduleDelay;
            string jsonSetupFilePath;
            boost::chrono::seconds  lastAcquiredTime;
            
            std::map<std::string, cu::DSAttributeHandlerExecutionEngine*> attributeHandlerEngineForDeviceIDMap;
            
            //!mutex for multithreading managment of sand box
            /*!
             The muthex is needed because the call to the action can occours in different thread
             */
            boost::recursive_mutex managing_cu_mutex;
            //
            KeyDataStorage*  keyDataStorage;
            
            CObjectHandlerProcessingQueue< ActionData*>*  actionParamForDeviceMap;
            
            CThread*   schedulerThread;
            
            boost::chrono::seconds  heartBeatIntervall;
            
            int  deviceState;
            
            CUStateKey::ControlUnitState deviceExplicitState;
            
            
            event::channel::InstrumentEventChannel *deviceEventChannel;
            
            /*!
             Add a new KeyDataStorage for a specific key
             */
            void setKeyDataStorage(KeyDataStorage*);

            
            /*!
             Define the control unit DataSet and Action into
             a CDataWrapper
             */
            void _defineActionAndDataset(CDataWrapper&) throw(CException);
            
            /*!
             Define the control unit DataSet and Action into
             a CDataWrapper
             */
            void _undefineActionAndDataset() throw(CException);
            
            /*!
             Initialize the Custom Contro Unit and return the configuration
             */
            CDataWrapper* _init(CDataWrapper*, bool& detachParam) throw(CException);
            
            /*!
             Deinit the Control Unit
             */
            CDataWrapper* _deinit(CDataWrapper*, bool& detachParam) throw(CException);
            
            /*!
             Starto the  Control Unit scheduling for device
             */
            CDataWrapper* _start(CDataWrapper*, bool& detachParam) throw(CException);
            
            /*!
             Stop the Custom Control Unit scheduling for device
             */
            CDataWrapper* _stop(CDataWrapper*, bool& detachParam) throw(CException);
            
            /*!
             Receive the evento for set the dataset input element
             */
            CDataWrapper* _setDatasetAttribute(CDataWrapper*, bool&) throw (CException);
            
            /*!
             Get the current control unit state
             */
            CDataWrapper* _getState(CDataWrapper*, bool& detachParam) throw(CException);
            
            /*!
             return the appropriate thread for the device
             */
            inline void threadStartStopManagment(bool startAction) throw(CException);
        public:
            /*!
             Construct a new CU with an identifier
             */
            AbstractControlUnit(const char *);
            /*!
             Construct a new CU with an identifier
             */
            AbstractControlUnit();
            /*!
             Destructor a new CU with an identifier
             */
            virtual ~AbstractControlUnit();
            
            /*!
             return the CU name
             */
            const char * getCUName();
            /*!
             return the CU instance
             */
            const char * getCUInstance();
            
            
        protected:
            //CU Identifier
            string cuName;
            //CU instance, this te fine the current isntance code
            //it's dynamically assigned
            string cuInstance;
            
            boost::shared_ptr<CDataWrapper> _internalSetupConfiguration;
            
            //------standard call-------------------------
            /*!
             Return the tart configuration for the Control Unit instance
             */
            virtual void defineActionAndDataset() throw(CException)   = 0;
            
            /*!
             Initialize the Custom Contro Unit and return the configuration
             */
            virtual void init(const string& deviceID) throw(CException) = 0;
            
            /*!
             Execute the Control Unit work
             */
            virtual void run(const string& deviceID) throw(CException) = 0;
            
            /*!
             Execute the Control Unit work
             */
            virtual void stop(const string& deviceID) throw(CException) = 0;
            
            /*!
             Deinit the Control Unit
             */
            virtual void deinit(const string& deviceID) throw(CException) = 0;
            
            /*!
             Receive the event for set the dataset input element, this virtual method
             is empty because can be used by controlunit implementation
             */
            virtual CDataWrapper* setDatasetAttribute(CDataWrapper*, bool&) throw (CException){return NULL;};
            
            /*!
             Event for update some CU configuration
             */
            virtual CDataWrapper* updateConfiguration(CDataWrapper*, bool&) throw (CException);
            
            //----------------thread entry point---------------
            /*!
             Execute the scehduling for the device
             */
            void executeOnThread(const string&) throw(CException);
            
            
            //---------------------utility API-------------
            
            /*!
             Add the new attribute in the dataset for at the CU dataset
             */
            void addAttributeToDataSet(const char*const deviceID,
                                       const char*const attributeName,
                                       const char*const attributeDescription,
                                       DataType::DataType attributeType,
                                       DataType::DataSetAttributeIOAttribute attributeDirection);
            
            /*!
             Add the new attribute in the dataset for at the CU dataset with an associated handler
             */
            template<typename T>
            void addTemplatedAttributeToDataSet(const char*const deviceID,
                                                const char*const attributeName,
                                                const char*const attributeDescription,
                                                DataType::DataType attributeType,
                                                DataType::DataSetAttributeIOAttribute attributeDirection,
                                                TDSAttributeHandler<T>* handler) {
                
                //first add the attribute information in the schema
                CUSchemaDB::addAttributeToDataSet(deviceID, attributeName, attributeDescription, attributeType, attributeDirection);
                
                
                //check if there is a valid handler
                if(handler != NULL) {
                    if(handler->attributeName.size() == 0){
                        //add the attribute name to the handler
                        handler->attributeName.assign(attributeName);
                    }
                    //the ad the handler for that
                    AbstractControlUnit::addHandlerForDSAttribute(deviceID, handler);
                }
            }
            
            
            /*!
             Add the new attribute in the dataset for at the CU dataset with an associated handler
             */
            template<typename T>
            void addInputInt32AttributeToDataSet(const char*const deviceID,
                                                 const char*const attributeName,
                                                 const char*const attributeDescription,
                                                 T* objectPointer,
                                                 typename TDSObjectHandler<T, int32_t>::TDSHandler  objectHandler) {
                
                addTemplatedAttributeToDataSet(deviceID, attributeName, attributeDescription, DataType::TYPE_INT32, DataType::Input, new TDSObjectHandler<T, int32_t>(objectPointer, objectHandler));
            }
            
            /*!
             Add the new attribute in the dataset for at the CU dataset with an associated handler
             */
            template<typename T>
            void addInputInt64AttributeToDataSet(const char*const deviceID,
                                                 const char*const attributeName,
                                                 const char*const attributeDescription,
                                                 T* objectPointer,
                                                 typename TDSObjectHandler<T, int64_t>::TDSHandler  objectHandler) {
                
                addTemplatedAttributeToDataSet(deviceID, attributeName, attributeDescription, DataType::TYPE_INT64, DataType::Input, new TDSObjectHandler<T, int64_t>(objectPointer, objectHandler));
            }
            
            /*!
             Add the new attribute in the dataset for at the CU dataset with an associated handler
             */
            template<typename T>
            void addInputDoubleAttributeToDataSet(const char*const deviceID,
                                                  const char*const attributeName,
                                                  const char*const attributeDescription,
                                                  T* objectPointer,
                                                  typename TDSObjectHandler<T, double>::TDSHandler  objectHandler) {
                
                addTemplatedAttributeToDataSet(deviceID, attributeName, attributeDescription, DataType::TYPE_DOUBLE, DataType::Input, new TDSObjectHandler<T, double>(objectPointer, objectHandler));
            }
            
            /*!
             Add the new attribute in the dataset for at the CU dataset with an associated handler
             */
            template<typename T>
            void addInputBooleanAttributeToDataSet(const char*const deviceID,
                                                   const char*const attributeName,
                                                   const char*const attributeDescription,
                                                   T* objectPointer,
                                                   typename TDSObjectHandler<T, bool>::TDSHandler  objectHandler) {
                
                addTemplatedAttributeToDataSet(deviceID, attributeName, attributeDescription, DataType::TYPE_BOOLEAN, DataType::Input, new TDSObjectHandler<T, bool>(objectPointer, objectHandler));
            }
            
            /*!
             Add the new attribute in the dataset for at the CU dataset with an associated handler
             */
            template<typename T>
            void addInputStringAttributeToDataSet(const char*const deviceID,
                                                  const char*const attributeName,
                                                  const char*const attributeDescription,
                                                  T* objectPointer,
                                                  typename TDSObjectHandler<T, std::string>::TDSHandler  objectHandler) {
                
                addTemplatedAttributeToDataSet(deviceID, attributeName, attributeDescription, DataType::TYPE_STRING, DataType::Input, new TDSObjectHandler<T, std::string>(objectPointer, objectHandler));
            }
            
            /*!
             Add the new attribute in the dataset for at the CU dataset with an associated handler
             */
            template<typename T>
            void addInputStructAttributeToDataSet(const char*const deviceID,
                                                  const char*const attributeName,
                                                  const char*const attributeDescription,
                                                  T* objectPointer,
                                                  typename TDSObjectHandler<T, CDataWrapper>::TDSHandler  objectHandler) {
                
                addTemplatedAttributeToDataSet(deviceID, attributeName, attributeDescription, DataType::TYPE_STRUCT, DataType::Input, new TDSObjectHandler<T, CDataWrapper>(objectPointer, objectHandler));
            }
            
            /*!
             Create a new action description, return the description for let the user to add parameter
             */
            template<typename T>
            AbstActionDescShrPtr addActionDescritionInstance(T* actonObjectPointer,
                                                             typename ActionDescriptor<T>::ActionPointerDef actionHandler,
                                                             const char*const actionAliasName,
                                                             const char*const actionDescription) {
                //call the DeclareAction action register method, the domain will be associated to the control unit isntance
                return DeclareAction::addActionDescritionInstance(actonObjectPointer, actionHandler, cuInstance.c_str(), actionAliasName, actionDescription);
            }
            
            //! add an handler for a determinate device id and attribute set
            /*!
             *  This function permit to attach an handler to an attribute name. When the control unit
             *  receive by RPC an evento to set, a determinate attribute, to an specified value, this
             *  handler will be called.
             *  \param deviceID is the identification of the device that contain the attribute
             *  \param attrName is the name of the attribute where the handler need to be attached
             *  \param classHandler is the pointer to handler that need to be attached
             *  \exception something is gone wrong
             */
            void addHandlerForDSAttribute(const char * deviceID,
                                          cu::handler::DSAttributeHandler * classHandler)  throw (CException);
            
            //--------------Contro Unit Service Method----------------
            /*!
             Init the dataset ad other values by a json file
             */
            void initWithJsonFilePath(const char*const _jsonSetupFilePath) {
                jsonSetupFilePath.assign(_jsonSetupFilePath);
            }
            
            /*!
             Set the default schedule delay for the sandbox
             */
            void setDefaultScheduleDelay(int32_t _sDelay){scheduleDelay = _sDelay;};
            
            /*!
             load the json file setupped into jsonSetupFilePath class attributed
             */
            void loadCDataWrapperForJsonFile(CDataWrapper&)  throw (CException);
            
            /*!
             Send device data to output buffer
             */
            void pushDataSet(CDataWrapper*);
            
            /*!
             get latest device data 
             */
            ArrayPointer<CDataWrapper> *getLastDataSet();
            
            /*!
             return a new instance of CDataWrapper filled with a mandatory data
             according to key
             */
            CDataWrapper *getNewDataWrapper();        
        };
    }
}
#endif
