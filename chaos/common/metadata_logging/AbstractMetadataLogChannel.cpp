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

CDataWrapper *AbstractMetadataLogChannel::getNewLogEntry(const std::string& log_emitter,
                                                         const std::string& log_subject,
                                                         const std::string& log_domain) {
    CDataWrapper *log_entry = new CDataWrapper();
    log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER,
                              log_emitter);
    log_entry->addInt64Value(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_TIMESTAMP,
                             TimingUtil::getTimeStamp());
    log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN,
                              log_domain);
    log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SUBJECT,
                              log_subject);
    return log_entry;
}

int AbstractMetadataLogChannel::sendLog(chaos::common::data::CDataWrapper *log_message,
                                        int32_t priority) {
    CHAOS_ASSERT(logging_manager);
    return logging_manager->pushLogEntry(log_message,
                                         priority);
}