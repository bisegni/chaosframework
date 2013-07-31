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

#include <chaos/common/general/Configurable.h>
#include <chaos/common/action/ActionDescriptor.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/utility/ArrayPointer.h>
#include <chaos/common/pqueue/CObjectHandlerProcessingQueue.h>

#include <chaos/cu_toolkit/DataManager/KeyDataStorage.h>
#include <chaos/cu_toolkit/ControlManager/DeviceSchemaDB.h>

#define CU_IDENTIFIER_C_STREAM "_" << getCUInstance()
#define INIT_STATE      0
#define START_STATE     1


namespace chaos{
    
    //forward event channel declaration
    namespace event{
        namespace channel {
            class InstrumentEventChannel;
        }
    }
    
    namespace cu {
        
        namespace ec = event::channel;
        
        class ControManager;

        /*!
         Base class for control unit execution task
         */
        class AbstractControlUnit : public DeclareAction, protected DeviceSchemaDB {
            friend class ControlManager;
            friend class DomainActionsScheduler;
            
            int32_t scheduleDelay;
            string jsonSetupFilePath;
            boost::chrono::seconds  lastAcquiredTime;
            
            /*!
             Add a new KeyDataStorage for a specific key
             */
            void setKeyDataStorage(KeyDataStorage *_keyDatStorage);

            
            /*!
             Define the control unit DataSet and Action into
             a CDataWrapper
             */
            void _defineActionAndDataset(CDataWrapper&) throw(CException);
            
            /*!
             Initialize the Custom Contro Unit and return the configuration
             */
            virtual CDataWrapper* _init(CDataWrapper*, bool& detachParam) throw(CException);
            
            /*!
             Deinit the Control Unit
             */
            virtual CDataWrapper* _deinit(CDataWrapper*, bool& detachParam) throw(CException);
            
            /*!
             Starto the  Control Unit scheduling for device
             */
            virtual CDataWrapper* _start(CDataWrapper*, bool& detachParam) throw(CException);
            
            /*!
             Stop the Custom Control Unit scheduling for device
             */
            virtual CDataWrapper* _stop(CDataWrapper*, bool& detachParam) throw(CException);
            
            /*!
             Define the control unit DataSet and Action into
             a CDataWrapper
             */
            void _undefineActionAndDataset() throw(CException);
            

        protected:
            //CU instance, this te fine the current isntance code
            //it's dynamically assigned
            string cuInstance;
            //
            KeyDataStorage*  keyDataStorage;
            
            int  deviceState;
            
            CUStateKey::ControlUnitState deviceExplicitState;
            //!mutex for multithreading managment of sand box
            /*!
             The muthex is needed because the call to the action can occours in different thread
             */
            boost::recursive_mutex managing_cu_mutex;
            
            ec::InstrumentEventChannel *deviceEventChannel;
            
            //! Get all managem declare action instance
            /*!
             Return all theinstance that implement action to be published into network broker.
             Subclass can override this method to inclued his cation instancer
             */
            virtual void _getDeclareActionInstance(std::vector<const DeclareAction *>& declareActionInstance);
            
            /*!
             Receive the evento for set the dataset input element
             */
            CDataWrapper* _setDatasetAttribute(CDataWrapper*, bool&) throw (CException);
            
            /*!
             Get the current control unit state
             */
            CDataWrapper* _getState(CDataWrapper*, bool& detachParam) throw(CException);

            
            //------standard call-------------------------
            /*!
             Return the tart configuration for the Control Unit instance
             */
            virtual void defineActionAndDataset() throw(CException)   = 0;
            /*!
             Initialize the Custom Contro Unit and return the configuration
             */
            virtual void init() throw(CException) = 0;
            
            /*!
             Internal implementation of the runmethod, that (for now) will schedule the slowcommand sandbox
             */
            virtual void start() throw(CException) = 0;
            
            /*!
             Execute the Control Unit work
             */
            virtual void stop() throw(CException) = 0;
            
            /*!
             Deinit the Control Unit
             */
            virtual void deinit() throw(CException) = 0;
            /*!
             Receive the event for set the dataset input element, this virtual method
             is empty because can be used by controlunit implementation
             */
            virtual CDataWrapper* setDatasetAttribute(CDataWrapper*, bool& detachParam) throw (CException) = 0;
            /*!
             Event for update some CU configuration
             */
            virtual CDataWrapper* updateConfiguration(CDataWrapper*, bool&) throw (CException);
            
            /*!
             Set the default schedule delay for the sandbox
             */
            void setDefaultScheduleDelay(int32_t _sDelay);
            
        public:
            /*!
             Construct a new CU with an identifier
             */
            AbstractControlUnit();
            /*!
             Destructor a new CU with an identifier
             */
            virtual ~AbstractControlUnit();
            
            //---------------------utility API-------------
            
            /*!
             Add the new attribute in the dataset for at the CU dataset
             */
            void addAttributeToDataSet(const char*const deviceID,
                                       const char*const attributeName,
                                       const char*const attributeDescription,
                                       DataType::DataType attributeType,
                                       DataType::DataSetAttributeIOAttribute attributeDirection,
                                       uint32_t maxDimension = 0);
            
            
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
            
            /*!
             return the CU instance
             */
            const char * getCUInstance();
        };
    }
}
#endif
