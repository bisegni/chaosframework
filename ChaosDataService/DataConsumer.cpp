/*
 *	DataConsumer.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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

#include "DataConsumer.h"

using namespace chaos::data_service;

DataConsumer::DataConsumer(){
    
}

DataConsumer::~DataConsumer() {
    
}

void DataConsumer::init(void *init_data) throw (chaos::CException) {
    server_channel = static_cast<DirectIOCDataWrapperServerChannel*>(init_data);
}

void DataConsumer::start() throw (chaos::CException) {
    
}

void DataConsumer::stop() throw (chaos::CException) {
    
}

void DataConsumer::deinit() throw (chaos::CException) {
    
}


void DataConsumer::consumeCDataWrapper(uint8_t channel_tag, chaos::common::data::CDataWrapper *data_wrapper) {
    
}
