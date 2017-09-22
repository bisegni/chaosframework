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

#include <chaos/common/utility/AggregatedCheckList.h>

using namespace chaos::common::utility;


#define ACK_INFO INFO_LOG(AggregatedCheckList)

AggregatedCheckList::AggregatedCheckList():
map(this){}

AggregatedCheckList::~AggregatedCheckList() {}

void AggregatedCheckList::addCheckList(const std::string& check_list_name) {
    DEBUG_CODE(ACK_INFO << "Add new checklist for:" << check_list_name;)
    map.registerElement(check_list_name, new CheckList());
}

void AggregatedCheckList::removeCheckList(const std::string& check_list_name) {
    DEBUG_CODE(ACK_INFO << "Remove checklist for:" << check_list_name;)
    map.deregisterElementKey(check_list_name);
}

CheckList *AggregatedCheckList::getSharedCheckList(const std::string& check_list_name) {
    return map.accessItem(check_list_name);
}

void AggregatedCheckList::freeObject(const KOCheckListContainer::TKOCElement& element_to_dispose) {
    DEBUG_CODE(ACK_INFO << "Auto remove checklist for:" << element_to_dispose.key;)
    delete(element_to_dispose.element);
}