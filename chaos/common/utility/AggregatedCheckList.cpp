/*
 *	AggregatedCheckList.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/utility/AggregatedCheckList.h>

using namespace chaos::common::utility;

void AggregatedCheckList::addCheckList(const std::string& check_list_name) {
    KOCheckListContainer::registerElement(check_list_name, new CheckList());
}

void AggregatedCheckList::removeCheckList(const std::string& check_list_name) {
    KOCheckListContainer::deregisterElementKey(check_list_name);
}

CheckList *AggregatedCheckList::getSharedCheckList(const std::string& check_list_name) {
    return KOCheckListContainer::accessItem(check_list_name);
}

void AggregatedCheckList::freeObject(std::string key, CheckList* element) {
    free(element);
}