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
