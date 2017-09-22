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

#ifndef __CHAOSFramework_FF7E7DB6_479C_4F4D_A771_12817BC6CCB6_PublishingManager_h
#define __CHAOSFramework_FF7E7DB6_479C_4F4D_A771_12817BC6CCB6_PublishingManager_h

#include <chaos/common/chaos_types.h>

#include <chaos/common/utility/LockableObject.h>

#include <chaos/cu_toolkit/data_manager/data_manager_types.h>
#include <chaos/cu_toolkit/data_manager/publishing/PublishTarget.h>
#include <chaos/cu_toolkit/data_manager/publishing/publishing_types.h>

#include <boost/shared_ptr.hpp>
namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace publishing {
                
                //!structure used by publishable manager for the management of the target
                struct PublishableTargetManagerElement {
                    ChaosSharedPtr<PublishTarget> target;
                    bool auto_remove;
                    
                    PublishableTargetManagerElement();
                    PublishableTargetManagerElement(const PublishableTargetManagerElement& src);
                    PublishableTargetManagerElement(ChaosSharedPtr<PublishTarget> _target,
                                                    bool _auto_remove);
                };
                
                //!hash table that collect publishtarget shared pointer with theyr names
                typedef ChaosSharedPtr<PublishableTargetManagerElement>  PublishableTargetManagerElementShrdPtr;
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string, PublishableTargetManagerElementShrdPtr, PublishableElementNameMap);
                
                //! is the central class that collect all endpoint and manages they lifetime
                class PublishingManager {
                    
                    //! reference to master dataset container
                    chaos::common::utility::LockableObject<DatasetElementContainer>& container_dataset;
                    
                    //!map that correlate target name with publishable target instance
                    chaos::common::utility::LockableObject<PublishableElementNameMap>  map_name_target;
                public:
                    
                    PublishingManager(chaos::common::utility::LockableObject<DatasetElementContainer>& container_dataset);
                    
                    ~PublishingManager();
                    
                    //!add a new target givin the name
                    bool addNewTarget(const std::string& target_name,
                                      bool auto_remove);
                    
                    //! remove a target givin his name
                    bool removeTarget(const std::string& target_name);
                    
                    //! add a new endpoint to the target
                    bool addURLToTarget(const std::string& target_name,
                                        const std::string& server_url_new);
                    
                    //! remove and endpoint identified by url form the target
                    bool removeURLServer(const std::string& target_name,
                                         const std::string& server_url_erase);
                    
                    //!add a dataset into a target
                    bool addDatasetToTarget(const std::string& target_name,
                                            const std::string& dataset_name,
                                            const PublishElementAttribute& publishable_attribute);
                    
                    //!remove a dataset from a target
                    bool removeDatasetFromTarget(const std::string& target_name,
                                                 const std::string& dataset_name);
                    
                    //!change the publishing mode for this target
                    void setPublishAttributeOnDataset(const std::string& target_name,
                                                      const std::string& dataset_name,
                                                      const PublishElementAttribute& publishable_attribute);
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework_FF7E7DB6_479C_4F4D_A771_12817BC6CCB6_PublishingManager_h */
