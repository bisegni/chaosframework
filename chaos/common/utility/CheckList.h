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

#ifndef __CHAOSFramework__CheckList_hpp
#define __CHAOSFramework__CheckList_hpp

#include <string>

#include <set>

namespace chaos {
    namespace common {
        namespace utility {
            
            //! Check list utility class
            /*!
             this class implement a dinamic check list to use within complex class that has many software layer
             to manage. Passing to init, deinit, start stop state, many time need to be manage only
             some of them. In this case this class can help to keep track waht element remain to enable.
             */
            class CheckList {
                std::set<int> list_element_to_do;
                std::set<int> list_element_done;
            public:
                //! add a new element in the check-list
                void addElement(int element_to_do);
                
                //! remove an element from the check-list
                void removeElement(int element_to_remove);
                
                //! tag an element of the check list as done
                void doneOnElement(int element_to_do);
                
                //!tag an element of the check list as to-do
                void redoElement(int element_to_do);
                
                //! give a copy of the set with the element to do
                std::set<int> elementToDo();
                
                //! give a copy of the set with the element done
                std::set<int> elementDone();
            };
            
        }
    }
}

#endif /* CheckList_h */
