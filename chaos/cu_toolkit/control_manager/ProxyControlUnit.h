/*
 *	ProxyControlUnit.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 24/01/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_EED208A4_A989_4DB4_9D1D_DB99F051738C_ProxyControlUnit_h
#define __CHAOSFramework_EED208A4_A989_4DB4_9D1D_DB99F051738C_ProxyControlUnit_h

#include <chaos/cu_toolkit/control_manager/ControlUnitApiInterface.h>
#include <chaos/cu_toolkit/control_manager/AbstractControlUnit.h>

#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
namespace chaos {
    using namespace boost;
    using namespace boost::chrono;
    
    namespace cu {
        namespace control_manager {
            //forward declarations
            class ControManager;
            class AbstractExecutionUnit;
            
            class ProxyControlUnit:
            public AbstractControlUnit {
                friend class ControlManager;
                friend class DomainActionsScheduler;
                friend class AbstractExecutionUnit;
                PUBLISHABLE_CONTROL_UNIT_INTERFACE(ProxyControlUnit)
                
                ChaosSharedPtr<ControlUnitApiInterface> api_interface_pointer;
                
                /*!
                 Define the control unit DataSet and Action into
                 a CDataWrapper
                 */
                void _defineActionAndDataset(chaos::common::data::CDataWrapper& setup_configuration) throw(CException);
                
                //! init rt control unit
                void init(void *initData) throw(CException);
                
                //! start rt control unit
                void start() throw(CException);
                
                //! stop rt control unit
                void stop() throw(CException);
                
                //! deinit rt control unit
                void deinit() throw(CException);

            protected:

                void unitDefineActionAndDataset() throw(CException);
                
                void unitDefineCustomAttribute();
                
                void unitInit() throw(CException);
                
                void unitStart() throw(CException);
                
                void unitStop() throw(CException);
                
                void unitDeinit() throw(CException);
                
                /*!
                 Event for update some CU configuration
                 */
                virtual chaos::common::data::CDataWrapper* updateConfiguration(chaos::common::data::CDataWrapper* update_pack,
                                                                               bool& detach_param) throw (CException);
                
                
            public:
                
                /*! default constructor
                 \param _control_unit_param is a string that contains parameter to pass during the contorl unit creation
                 \param _control_unit_drivers driver information
                 */
                ProxyControlUnit(const std::string& _control_unit_id);
                
                ProxyControlUnit(const std::string& _control_unit_id,
                                 const std::string& _control_unit_param,
                                 const ControlUnitDriverList& _control_unit_drivers);
                
                ~ProxyControlUnit();

                ChaosSharedPtr<ControlUnitApiInterface> getProxyApiInterface();
            };
        }
    }
}

#endif /* __CHAOSFramework_EED208A4_A989_4DB4_9D1D_DB99F051738C_ProxyControlUnit_h */
