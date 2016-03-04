/*
 *	HandlerMonitor.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 03/03/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__HandlerMonitor_h
#define __CHAOSFramework__HandlerMonitor_h

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

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
