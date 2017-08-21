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
    //std::cout << "key:" << key << " attribute:" << attribute << " value:" << value << std::endl;
}

void HandlerMonitor::consumeValueNotFound(const std::string& key,
                                          const std::string& attribute) {
    //std::cout << "No data found for key:" << key << " attribute:" << attribute << std::endl;
}
