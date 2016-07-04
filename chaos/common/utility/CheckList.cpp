/*
 *	CheckList.cpp
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

#include <chaos/common/utility/CheckList.h>

using namespace chaos::common::utility;

void CheckList::addElement(int element_to_do){
    list_element_to_do.insert( element_to_do);
}

void CheckList::removeElement(int element_to_remove) {
    list_element_to_do.erase(element_to_remove);
    list_element_done.erase(element_to_remove);
}

void CheckList::doneOnElement(int element_done){
    list_element_to_do.erase(element_done);
    list_element_done.insert(element_done);
}

void CheckList::redoElement(int element_to_redo){
    list_element_to_do.insert(element_to_redo);
    list_element_done.erase(element_to_redo);
}

std::set<int> CheckList::elementToDo() {
    return std::set<int>(list_element_to_do);
}

std::set<int> CheckList::elementDone() {
    return std::set<int>(list_element_done);
}
