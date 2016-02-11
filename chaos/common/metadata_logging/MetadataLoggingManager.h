/*
 *	MetadataLoggingManager.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 02/02/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__MetadataLoggingManager_h
#define __CHAOSFramework__MetadataLoggingManager_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/message/MultiAddressMessageChannel.h>
#include <chaos/common/pqueue/CObjectProcessingPriorityQueue.h>
#include <chaos/common/metadata_logging/AbstractMetadataLogChannel.h>

#include <boost/thread.hpp>

#include <map>
#include <string>

namespace chaos {
    //! forward decalration
    
    namespace common {
        namespace metadata_logging {
            
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, AbstractMetadataLogChannel*, MetadataLoggingInstancesMap);
            //! Metadata Logging Service
            /*!
             The metadata logging service, permit to store into !CHAOS MDS, metadata information.
             They can be, error, command or other thing that nodes need to be store on MDS.
             */
            class MetadataLoggingManager:
            protected chaos::common::utility::InizializableService,
            public chaos::common::utility::Singleton<MetadataLoggingManager>,
            protected chaos::common::pqueue::CObjectProcessingPriorityQueue<chaos::common::data::CDataWrapper> {
                                friend class AbstractMetadataLogChannel;
                friend class chaos::common::utility::Singleton<MetadataLoggingManager>;
                
                const std::string metadata_logging_domain;
                const std::string metadata_logging_action;
                
                boost::mutex mutext_maps;
            
                std::map<std::string, boost::shared_ptr< chaos::common::utility::ObjectInstancer<AbstractMetadataLogChannel> > > map_instancer;
                
                MetadataLoggingInstancesMap map_instance;
                
                //!internal nework broker for creation and deallocaiton of mds channels
                chaos::common::message::MultiAddressMessageChannel *message_channel;
                
                MetadataLoggingManager();
                ~MetadataLoggingManager();
                
                //!internal function to permit the forwarding of th elog entry to the mds
                int sendLogEntry(chaos::common::data::CDataWrapper *log_entry);
                
                void processBufferElement(chaos::common::data::CDataWrapper *log_entry,
                                          ElementManagingPolicy& element_policy) throw(CException);
            protected:
                //!submit the log entry in the logging queue
                int pushLogEntry(chaos::common::data::CDataWrapper *log_entry,
                                 int32_t priority = 0);
            public:
                void init(void *init_data) throw(chaos::CException);
                void deinit() throw(chaos::CException);
                
                chaos::common::message::MultiAddressMessageChannel *getMessageChannelInstance();
                
                void registerChannel(const std::string& channel_alias,
                                     chaos::common::utility::ObjectInstancer<AbstractMetadataLogChannel> *instancer);
                
                AbstractMetadataLogChannel *getChannel(const std::string channel_alias);
                void releaseChannel(AbstractMetadataLogChannel *channel_instance);
            };
            
        }
    }
}

#endif /* __CHAOSFramework__MetadataLoggingManager_h */
