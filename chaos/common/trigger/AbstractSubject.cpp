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
