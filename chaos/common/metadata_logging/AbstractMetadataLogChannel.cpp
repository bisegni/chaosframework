/*
 *	AbstractMetadataLogChannel.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 10/02/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/global.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/message/MultiAddressMessageChannel.h>
#include <chaos/common/metadata_logging/MetadataLoggingManager.h>
#include <chaos/common/metadata_logging/AbstractMetadataLogChannel.h>

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::message;
using namespace chaos::common::metadata_logging;

AbstractMetadataLogChannel::AbstractMetadataLogChannel():
instance_uuid(UUIDUtil::generateUUID()),
logging_manager(NULL){}

AbstractMetadataLogChannel::~AbstractMetadataLogChannel() {}

void AbstractMetadataLogChannel::setLoggingManager(MetadataLoggingManager *_logging_manager) {
    logging_manager = _logging_manager;
}

const std::string& AbstractMetadataLogChannel::getInstanceUUID() {
    return instance_uuid;
}

CDataWrapper *AbstractMetadataLogChannel::getNewLogEntry(const std::string& node_uid,
                                                         const std::string& log_domain) {
    CDataWrapper *log_entry = new CDataWrapper();
    log_entry->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID,
                              node_uid);
    log_entry->addInt64Value(MetadataServerNodeDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_TIMESTAMP,
                             TimingUtil::getTimeStamp());
    log_entry->addStringValue(MetadataServerNodeDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN,
                              log_domain);
    return log_entry;
}

int AbstractMetadataLogChannel::sendLog(chaos::common::data::CDataWrapper *log_message,
                                        int32_t priority) {
    CHAOS_ASSERT(logging_manager);
    return logging_manager->pushLogEntry(log_message,
                                         priority);
}