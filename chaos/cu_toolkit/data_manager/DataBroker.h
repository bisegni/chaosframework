/*
 *	DataBroker.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 25/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__555554A_6C80_44FA_B263_6A03AA133D4D_DataBroker_h
#define __CHAOSFramework__555554A_6C80_44FA_B263_6A03AA133D4D_DataBroker_h

#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/direct_io/channel/DirectIODeviceServerChannel.h>

#include <chaos/cu_toolkit/data_manager/data_manager_types.h>
#include <chaos/cu_toolkit/data_manager/publishing/publishing.h>
#include <chaos/cu_toolkit/data_manager/manipulation/manipulation.h>
#include <chaos/cu_toolkit/data_manager/trigger_system/trigger_system.h>

#include <chaos/cu_toolkit/data_manager/action/action.h>

#include <boost/shared_ptr.hpp>
namespace chaos {
    namespace cu {
        namespace data_manager {
            
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string,
                                      boost::shared_ptr<action::DataBrokerAction>,
                                      MapDataBrokerAction);
            
            //! main class for the data broker
            /*!
             DatBroker is a multilayer architecture that manage and publish
             dataset iwth thei attribute. Has also functionality for trigger algorithm on
             determinated dataset's attribute.
             */
            class DataBroker:
            public chaos::DeclareAction,
            public chaos::common::utility::InizializableService,
            public chaos::common::direct_io::channel::DirectIODeviceServerChannel::DirectIODeviceServerChannelHandler {
                //! is the node uid that host the databrker and will be sed as
                //! rpc domain
                const std::string owner_node_uid;
                //!root sublayer managment
                manipulation::DataBrokerEditor  dataset_manager;
                publishing::PublishingManager   publishing_manager;
                trigger_system::EventManager    event_manager;
                
                //!contain instance of action used by remote service that act on data broker sublayer
                chaos::common::utility::LockableObject<MapDataBrokerAction> map_databroker_action;
                
                template<typename T>
                T* getAction(const std::string& action_name) {
                    chaos::common::utility::LockableObjectWriteLock wl;
                    map_databroker_action.getWriteLock(wl);
                    
                    if(map_databroker_action().count(action_name)) {
                        return static_cast<T*>(map_databroker_action()[action_name].get());
                    } else {
                        //we need to create new action
                        boost::shared_ptr<action::DataBrokerAction> tmp_ptr(chaos::common::utility::ObjectFactoryRegister<action::DataBrokerAction>::getInstance()->getNewInstanceByName(action_name));
                        CHAOS_ASSERT(tmp_ptr.get());
                        tmp_ptr->setManagers(&dataset_manager,
                                             &publishing_manager,
                                             &event_manager);
                        map_databroker_action().insert(MapDataBrokerActionPair(tmp_ptr->getName(),
                                                                               tmp_ptr));
                        return static_cast<T*>(tmp_ptr.get());
                    }
                }
            protected:
                //---------------- DirectIODeviceServerChannelHandler -----------------------
                int consumePutEvent(chaos::common::direct_io::channel::opcode_headers::DirectIODeviceChannelHeaderPutOpcode *header,
                                    void *channel_data,
                                    uint32_t channel_data_len);
                
                int consumeGetEvent(chaos::common::direct_io::channel::opcode_headers::DirectIODeviceChannelHeaderGetOpcode *header,
                                    void *channel_data,
                                    uint32_t channel_data_len,
                                    chaos::common::direct_io::channel::opcode_headers::DirectIODeviceChannelHeaderGetOpcodeResult *result_header,
                                    void **result_value);
                
                int consumeDataCloudQuery(chaos::common::direct_io::channel::opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloud *header,
                                          const std::string& search_key,
                                          uint64_t search_start_ts,
                                          uint64_t search_end_ts);
                
                //---------------- RPC Action -----------------------
                chaos::common::data::CDataWrapper* alterDataset(chaos::common::data::CDataWrapper *set_data,
                                                                bool& detach_data);
                //!RPC action that permit to change dataset attribute value
                chaos::common::data::CDataWrapper* alterDatasetAttributeValue(chaos::common::data::CDataWrapper *set_data,
                                                                              bool& detach_data);
                //!RPC action that permit to manipulate the endpoint for a dataset
                chaos::common::data::CDataWrapper* alterPublishingEndpoint(chaos::common::data::CDataWrapper *set_data,
                                                                           bool& detach_data);
            public:
                DataBroker(const std::string& _owner_node_uid);
                ~DataBroker();
                
                void init(void* init_data) throw(CException);
                
                void deinit() throw(CException);
                
                int addNewDataset(const std::string& name,
                                  const DataType::DatasetType type,
                                  const std::string& shared_key);
                
                //!Return the dataset editor
                /*!
                 Return the dataset identified by the name,
                 if dataset is not present it will be created
                 \param dataset_name the dataset name
                 \param create if true, in case the dataset is not present it will be created.
                 \return teh datase editor
                 */
                std::auto_ptr<manipulation::DatasetEditor> getEditorForDataset(const std::string& dataset_name);
                
                //!Return the cache wrapper for the determinated dataset
                std::auto_ptr<manipulation::DatasetCacheWrapper> getDatasetCacheForDataset(const std::string& dataset_name);
                
                //!return all the rpc action inplemented by overall sub layers
                void getDeclaredActionInstance(std::vector<const DeclareAction *>& declared_action_list);
                
                std::auto_ptr<chaos::common::data::CDataWrapper> serialize();
                
                void deserialize(std::auto_ptr<chaos::common::data::CDataWrapper> data_serailization);
            };
            
        }
    }
}

#endif /* __CHAOSFramework__555554A_6C80_44FA_B263_6A03AA133D4D_DataBroker_h */