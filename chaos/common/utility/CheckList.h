/*
 *	CheckList.hpp
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
                std::set<std::string> list_element_to_do;
                std::set<std::string> list_element_done;
            public:
                //! add a new element in the check-list
                void addElement(const std::string& element_to_do);
                
                //! remove an element from the check-list
                void removeElement(const std::string& element_to_remove);
                
                //! tag an element of the check list as done
                void doneOnElement(const std::string& element_to_do);
                
                //!tag an element of the check list as to-do
                void redoElement(const std::string& element_to_do);
                
                //! give a copy of the set with the element to do
                std::set<std::string> elementToDo();
            };
            
        }
    }
}

#endif /* CheckList_h */
