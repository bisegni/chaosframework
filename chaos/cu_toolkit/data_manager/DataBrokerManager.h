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

#ifndef __CHAOSFramework_E89CBC0E_6B48_465C_8200_3ABE5701D169_DataBrokerManager_h
#define __CHAOSFramework_E89CBC0E_6B48_465C_8200_3ABE5701D169_DataBrokerManager_h

#include <chaos/common/utility/Singleton.h>

namespace chaos{
    namespace cu {
        namespace data_manager {
            
            //!forward declaration
            class DataBroker;
            
            //! data broker is the managemnt center for all dataset
            /*!
             every instance of data broker will be used by a control unit
             to define, publish and update the dataset versus other nodes(CDS 
             or other CU.
             */
            class DataBrokerManager:
            public chaos::common::utility::Singleton<DataBrokerManager> {
                friend class chaos::common::utility::Singleton<DataBrokerManager>;
                DataBrokerManager();
                ~DataBrokerManager();
            public:
                
                DataBroker *newBrokerInstance();
                void releaseBrokerInstance(DataBroker *broker);
                
            };
            
        }
    }
}

#endif /* __CHAOSFramework_E89CBC0E_6B48_465C_8200_3ABE5701D169_DataBrokerManager_h */
