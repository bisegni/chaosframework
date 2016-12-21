/*
 *	AbstractSandbox.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 21/12/2016 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/batch_command/AbstractSandbox.h>

using namespace chaos::common::batch_command;

CommandInfoAndImplementation::CommandInfoAndImplementation(chaos::common::data::CDataWrapper *_cmdInfo, BatchCommand *_cmdImpl) {
    cmdInfo = _cmdInfo;
    cmdImpl = _cmdImpl;
}

CommandInfoAndImplementation::~CommandInfoAndImplementation() {
    deleteInfo();
    deleteImpl();
}

void CommandInfoAndImplementation::deleteInfo() {
    if (!cmdInfo) return;
    delete(cmdInfo);
    cmdInfo = NULL;
}

void CommandInfoAndImplementation::deleteImpl() {
    if (!cmdImpl) return;
    delete(cmdImpl);
    cmdImpl = NULL;
}

AbstractSandbox::AbstractSandbox():
command_sequence_id(0){}

AbstractSandbox::~AbstractSandbox(){}

void AbstractSandbox::addCommandID(BatchCommand *command_impl) {
    if(command_impl == NULL) return;
    command_impl->unique_id = ++command_sequence_id;
}
