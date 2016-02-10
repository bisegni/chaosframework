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

#include <chaos/common/message/MultiAddressMessageChannel.h>
#include <chaos/common/metadata_logging/AbstractMetadataLogChannel.h>

#include <chaos/common/global.h>
#include <chaos/common/utility/UUIDUtil.h>

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::message;
using namespace chaos::common::metadata_logging;

AbstractMetadataLogChannel::AbstractMetadataLogChannel():
instance_uuid(UUIDUtil::generateUUID()){}

AbstractMetadataLogChannel::~AbstractMetadataLogChannel() {}

void AbstractMetadataLogChannel::setMessageChannel(MultiAddressMessageChannel *_mds_channel) {
    mds_channel = _mds_channel;
}

const std::string& AbstractMetadataLogChannel::getInstanceUUID() {
    return instance_uuid;
}

CDataWrapper *AbstractMetadataLogChannel::getNewLogEntry(const std::string& log_domain) {
    CDataWrapper *log_entry = new CDataWrapper();
    log_entry->addStringValue("domain", log_domain);
    return log_entry;
}

int AbstractMetadataLogChannel::sendLog(chaos::common::data::CDataWrapper *log_message,
                                        bool wait_for_ack,
                                        uint32_t timeout) {
    CHAOS_ASSERT(mds_channel);
    int err = 0;
    const std::string metadata_logging_domain = "metadata_logging";
    const std::string metadata_logging_action = "submit_log";
    
    if(wait_for_ack) {
        std::auto_ptr<MultiAddressMessageRequestFuture> result = mds_channel->sendRequestWithFuture(metadata_logging_domain,
                                                                                                    metadata_logging_action,
                                                                                                    log_message);
    } else {
        mds_channel->sendMessage(metadata_logging_domain,
                                 metadata_logging_action,
                                 log_message);
        CHK_AND_DELETE_OBJ_POINTER(log_message);
    }
    return err;
}