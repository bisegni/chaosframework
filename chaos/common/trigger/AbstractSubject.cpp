/*
 *	AbstractSubject.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 01/08/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/utility/UUIDUtil.h>

#include <chaos/common/trigger/AbstractSubject.h>

using namespace chaos::common::utility;
using namespace chaos::common::trigger;

AbstractSubject::AbstractSubject(const std::string& _recevier_name):
subject_name(_recevier_name),
subject_uuid(UUIDUtil::generateUUIDLite()){}

AbstractSubject::~AbstractSubject() {}

const std::string& AbstractSubject::getSubjectUUID() const {
    return subject_uuid;
}

const std::string& AbstractSubject::getSubjectName() const {
    return subject_name;
}
