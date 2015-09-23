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

//! ad a new element in the check-list
void CheckList::addElement(const std::string& element_to_do){
    list_element_to_do.insert(element_to_do);
}

//! tag an element of the check list as done
void CheckList::doneOnElement(const std::string& element_done){
    list_element_to_do.erase(element_done);
    list_element_done.insert(element_done);
}

//!tag an element of the check list as to-do
void CheckList::redoElement(const std::string& element_to_redo){
    list_element_to_do.insert(element_to_redo);
    list_element_done.erase(element_to_redo);
}

//! give a copy of the set with the element to do
std::set<std::string> CheckList::elementToDo() {
    return std::set<std::string>(list_element_to_do);
}