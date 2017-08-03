/*
 *	AbstractProtocolAdapter.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 02/08/2017 INFN, National Institute of Nuclear Physics
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

#include <chaos_micro_unit_toolkit/connection/protocol_adapter/AbstractProtocolAdapter.h>
using namespace chaos::micro_unit_toolkit::connection::protocol_adapter;

AbstractProtocolAdapter::AbstractProtocolAdapter(ProtocolType _impl_type,
                                                 const std::string& _protocol_endpoint):
protocol_type(_impl_type),
protocol_endpoint(_protocol_endpoint),
connection_status(ConnectionStateDisconnected){}

AbstractProtocolAdapter::~AbstractProtocolAdapter() {}


void AbstractProtocolAdapter::setHandler(ProtocolAdapterHandler *_handler) {
    handler =_handler;
}
