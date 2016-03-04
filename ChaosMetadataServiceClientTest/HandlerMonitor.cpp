/*
 *	HandlerMonitor.cpp
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

#include "HandlerMonitor.h"

using namespace chaos::metadata_service_client;

HandlerMonitor::HandlerMonitor(const std::string& _node_id,
                               const std::string& attribute_key):
QuantumKeyAttributeStringHandler(attribute_key),
node_id(_node_id){
}

HandlerMonitor::~HandlerMonitor() {
    
}

void HandlerMonitor::init() {
    //register this class with monitor system has handler
    ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandler(node_id,
                                                                      10,
                                                                      this);
    
}

void HandlerMonitor::deinit() {
    ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandler(node_id,
                                                                         10,
                                                                         this);
}

void HandlerMonitor::consumeValue(const std::string& key,
                                  const std::string& attribute,
                                  const std::string& value) {
    std::cout << "key:" << key << " attribute:" << attribute << " value:" << value << std::endl;
}

void HandlerMonitor::consumeValueNotFound(const std::string& key,
                                          const std::string& attribute) {
    std::cout << "No data found for key:" << key << " attribute:" << attribute << std::endl;
}