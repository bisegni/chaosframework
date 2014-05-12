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
#include <chaos/common/utility/StartableService.h>

#include <chaos/common/data/DatasetDB.h>

#include <chaos/cu_toolkit/DataManager/KeyDataStorage.h>
#include <chaos/cu_toolkit/driver_manager/DriverErogatorInterface.h>

#define CU_IDENTIFIER_C_STREAM "_" << getCUInstance()
#define INIT_STATE      0
#define START_STATE     1

#define CU_OPT_IN_MEMORY_DATABASE "cu_db_in_memory"

namespace chaos{
    
    //forward event channel declaration
    namespace event{
        namespace channel {
            class InstrumentEventChannel;
        }
    }
    
    namespace cu {
        
		namespace cu_driver = chaos::cu::driver_manager::driver;
        namespace ec = event::channel;
        namespace chaos_data = chaos::common::data;

        class ControManager;
		
            //!  Base class for control unit !CHAOS node
        /*!
            This is the abstraction of the contorl unit node of CHAOS. This class extends DeclareAction
            class that permit to publish rpc method for control the control unit life cycle. Most of the API
            that needs to be used to create device and his dataset are contained into the DeviceSchemaDB class.
         */
        class AbstractControlUnit :
		public chaos::cu::driver_manager::DriverErogatorInterface,
		public DeclareAction,
		protected chaos_data::DatasetDB,
		public utility::StartableService {
            
            //frinedly class declaration
            friend class ControlManager;
            friend class DomainActionsScheduler;
            friend class SCAbstractControlUnit;
			friend class RTAbstractControlUnit;
            
			std::string control_unit_type;
			
			//! list of the accessor of the driver requested by the unit implementation
			std::vector< cu_driver::DriverAccessor *> accessorInstances;
			
            /*!
             Add a new KeyDataStorage for a specific key
             */
            void setKeyDataStorage(KeyDataStorage *_keyDatStorage);
            
            /*!
             Initialize the Custom Contro Unit and return the configuration
             */
            virtual chaos_data::CDataWrapper* _init(chaos_data::CDataWrapper*, bool& detachParam) throw(CException);
            
            /*!
             Deinit the Control Unit
             */
            virtual chaos_data::CDataWrapper* _deinit(chaos_data::CDataWrapper*, bool& detachParam) throw(CException);
            
            /*!
             Starto the  Control Unit scheduling for device
             */
            virtual chaos_data::CDataWrapper* _start(chaos_data::CDataWrapper*, bool& detachParam) throw(CException);
            
            /*!
             Stop the Custom Control Unit scheduling for device
             */
            virtual chaos_data::CDataWrapper* _stop(chaos_data::CDataWrapper*, bool& detachParam) throw(CException);
            
            /*!
             Define the control unit DataSet and Action into
             a CDataWrapper
             */
            void _undefineActionAndDataset() throw(CException);
			
			// Startable Service method
            void init(void *initData) throw(CException);
            
            // Startable Service method
            void start() throw(CException);
            
            // Startable Service method
            void stop() throw(CException);
            
            // Startable Service method
            void deinit() throw(CException);
			
        protected:
            //  It's is the dynamically assigned instance of the CU. it will be used
            // as domain for the rpc action.
            string cuInstance;
            
            //! Momentary driver for push data into the central memory
            KeyDataStorage*  keyDataStorage;
            
            //! Event channel to permit the fire of the device event
            ec::InstrumentEventChannel *deviceEventChannel;
            
            //! Define the dataset information of the device implementeted into the CU
            /*!
                This method configure the CDataWrapper whit all th einromation for describe the implemented device
             */
            virtual void _defineActionAndDataset(chaos_data::CDataWrapper&) throw(CException);
            
            //! Get all managed declare action instance
            /*!
                Return all the instance that implements actions to be published into network broker.
                Subclass can override this method to inclued their action instancer
             */
            virtual void _getDeclareActionInstance(std::vector<const DeclareAction *>& declareActionInstance);
            
                //! Set the dataset input element values
            /*!
                This method is called when the input attribute of the dataset need to be valorized,
                subclass need to perform all the appropiate action to set these attribute
             */
            chaos_data::CDataWrapper* _setDatasetAttribute(chaos_data::CDataWrapper*, bool&) throw (CException);
            
                //! Return the state of the control unit
            /*!
                Return the current control unit state identifyed by ControlUnitState types
                fitted into the CDatawrapper with the key CUStateKey::CONTROL_UNIT_STATE
             */
            chaos_data::CDataWrapper* _getState(chaos_data::CDataWrapper*, bool& detachParam) throw(CException);

			//! Return the information about the type of the current instace of control unit
            /*!
			 Return unit fitted into cdata wrapper:
				CU type: string type associated with the key @CUDefinitionKey::CS_CM_CU_TYPE
             */
            chaos_data::CDataWrapper* _getInfo(chaos_data::CDataWrapper*, bool& detachParam) throw(CException);
			
                //! Abstract Method that need to be used by the sublcass to define the dataset
            /*!
                Subclass, in this method can call the api to create the dataset, after this method
                this class will collet all the information and send all to the MDS server.
             */
            virtual void unitDefineActionAndDataset() throw(CException) = 0;
            
			//! Abstract method for the definition of the driver
            /*!
				This method permit to the control unit implementation to define the necessary drivers it needs
			 \param neededDriver need to be filled with the structure DrvRequestInfo filled with the information
			 about the needed driver [name, version and initialization param if preset statically]
             */
            virtual void unitDefineDriver(std::vector<cu_driver::DrvRequestInfo>& neededDriver) = 0;
			
                //! Abstract method for the initialization of the control unit
            /*!
                This is where the subclass need to be inizialize their environment, usually the hardware initialization. An exception
                will stop the Control Unit live.
             */
            virtual void unitInit() throw(CException) = 0;
            
                //! Abstract method for the start of the control unit
            /*!
                This is where the subclass need to be start all the staff needed by normal control process. An exception
                will stop the Control Unit live and perform the deinitialization of the control unit.
             */
            virtual void unitStart() throw(CException) = 0;
            
            //! Abstract method for the stop of the control unit
            /*!
                This is where the subclass need to be stop all the staff needed for pause the control process. An exception
                will stop the Control Unit live and perform the deinitialization of the control unit.
             */
            virtual void unitStop() throw(CException) = 0;
            
            //! Abstract method for the deinit of the control unit
            /*!
                This is where the subclass need to be deinit all the staff that has been allocatate into the init method.
                Usually the hardware deallocation etc..
             */
            virtual void unitDeinit() throw(CException) = 0;
            
                //Abstract method used to sublcass to set theri needs
            /*!
                Receive the event for set the dataset input element, this virtual method
                is empty because can be used by controlunit implementation
             */
            virtual chaos_data::CDataWrapper* setDatasetAttribute(chaos_data::CDataWrapper*, bool& detachParam) throw (CException) = 0;
            
                // Infrastructure configuration update
            /*!
                This methdo is called when some configuration is received. All the Control Unit subsystem is
                checked to control waht is needed to update. Subclass that override this method need first inherited
                the parent one and the check if the CDataWrapper contains something usefull for it.
             */
            virtual chaos_data::CDataWrapper* updateConfiguration(chaos_data::CDataWrapper*, bool&) throw (CException);
            
			//! return the accessor by an index
			/*
				The index parameter correspond to the order that the driver infromation are
				added by the unit implementation into the function AbstractControlUnit::unitDefineDriver.
			 */
			driver_manager::driver::DriverAccessor * getAccessoInstanceByIndex(int idx);
			
        public:
            
            //! Default Contructor
			/*!
			 \param _control_unit_type the superclass need to set the control unit type for his implementation
			 */
            AbstractControlUnit(std::string _control_unit_type);
            
            //! default destructor
            virtual ~AbstractControlUnit();
            
            
                //![API] Api for publish a lass method as RPC action
            /*!
                This is a convenient method to register actiona associated to this instance of the class, class pointer
                is associated directly to the current instance of this class and domain is fixed to the Control Unit instance.
                Developer can use this method or the more flexible DeclareAction::addActionDescritionInstance.
             */
            template<typename T>
            AbstActionDescShrPtr addActionDescritionInstance(T* objectReference,
                                                             typename ActionDescriptor<T>::ActionPointerDef actionHandler,
                                                             const char*const actionAliasName,
                                                             const char*const actionDescription) {
                //call the DeclareAction action register method, the domain will be associated to the control unit isntance
                return DeclareAction::addActionDescritionInstance(objectReference, actionHandler, cuInstance.c_str(), actionAliasName, actionDescription);
            }
            
            //! Return the contro unit instance
            const char * getCUInstance();
        };
    }
}
#endif
