/*
 *	AggregatedCheckList.h
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

#ifndef __CHAOSFramework__AggregatedCheckList_hpp
#define __CHAOSFramework__AggregatedCheckList_hpp

#include <chaos/common/utility/CheckList.h>
#include <chaos/common/utility/TemplatedKeyObjectContainer.h>

#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace common {
        namespace utility {
            
            typedef TemplatedKeyObjectContainer<std::string, CheckList* > KOCheckListContainer;
            
            class AggregatedCheckList:
            protected KOCheckListContainer {
            protected:
                void freeObject(std::string key, CheckList* element);
            public:
                void addCheckList(const std::string& check_list_name);
                void removeCheckList(const std::string& check_list_name);
                CheckList *getSharedCheckList(const std::string& check_list_name);
            };
            
        }
    }
}

#endif /* AggregatedCheckList_hpp */
