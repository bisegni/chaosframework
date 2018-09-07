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
                virtual chaos::common::data::CDWUniquePtr updateConfiguration(chaos::common::data::CDWUniquePtr update_pack);
                
                
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
