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

#ifndef __CHAOSFramework__HandlerMonitor_h
#define __CHAOSFramework__HandlerMonitor_h

#include <chaos_metadata_service_client/ChaosMetadataServiceClient.h>

class HandlerMonitor:
public chaos::metadata_service_client::monitor_system::QuantumKeyAttributeStringHandler {
    const std::string node_id;
public:
    HandlerMonitor(const std::string& _node_id,
                   const std::string& attribute_key);
    ~HandlerMonitor();
    void init();
    void deinit();
protected:

    void consumeValue(const std::string& key,
                              const std::string& attribute,
                              const std::string& value);
    
    void consumeValueNotFound(const std::string& key,
                              const std::string& attribute);
};

#endif /* __CHAOSFramework__HandlerMonitor_h */
