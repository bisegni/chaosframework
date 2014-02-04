/*
 *	ZMQDirectIOServerDataPack.cpp
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
#include <chaos/common/direct_io/impl/ZMQDirectIOServerDataPack.h>
using namespace chaos::common::direct_io::impl;

ZMQDirectIOServerDataPack::ZMQDirectIOServerDataPack(ZMQSocketAndMessage *_socket_message):socket_message(_socket_message) {
	
}
ZMQDirectIOServerDataPack::~ZMQDirectIOServerDataPack() {
	
}

void ZMQDirectIOServerDataPack::getDataPtr() {
	
}

uint32_t ZMQDirectIOServerDataPack::getDataSize() {
	return 0;
}

uint32_t ZMQDirectIOServerDataPack::sendAnswer(void *data_ptr, uint32_t data_len) {
	return 0;
}