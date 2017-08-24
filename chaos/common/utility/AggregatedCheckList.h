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

#ifndef __CHAOSFramework__AggregatedCheckList_hpp
#define __CHAOSFramework__AggregatedCheckList_hpp

#include <chaos/common/utility/CheckList.h>
#include <chaos/common/utility/TemplatedKeyObjectContainer.h>

#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace common {
        namespace utility {
            
#define CHAOS_CHECK_LIST_START_SCAN_TO_DO(cl, cln)\
std::set<int> cl ## to_do = cl.getSharedCheckList(cln)->elementToDo();\
for(std::set<int>::iterator it = cl ## to_do.begin();\
it != cl ## to_do.end();\
it++) {\
switch(*it) {\
            
#define CHAOS_CHECK_LIST_DONE(cl, cln, id)\
case id:\
cl.getSharedCheckList(cln)->doneOnElement(id);
            
#define CHAOS_CHECK_LIST_END_SCAN_TO_DO(cl, cln)\
}\
}
            
#define CHAOS_CHECK_LIST_START_SCAN_DONE(cl, cln)\
std::set<int> cl ## done = cl.getSharedCheckList(cln)->elementDone();\
for(std::set<int>::reverse_iterator it = cl ## done.rbegin();\
it != cl ## done.rend();\
it++) {\
switch(*it) {\
            
#define CHAOS_CHECK_LIST_REDO(cl, cln, id)\
case id:\
cl.getSharedCheckList(cln)->redoElement(id);
            
#define CHAOS_CHECK_LIST_END_SCAN_DONE(cl, cln)\
}\
}
            
            
            typedef TemplatedKeyObjectContainer<std::string, CheckList* > KOCheckListContainer;
            
            /*!
             Represent an aggregaiton of one or more named check lists.
             */
            class AggregatedCheckList:
            protected KOCheckListContainer::FreeHandler {
                KOCheckListContainer map;
            protected:
                void freeObject(const KOCheckListContainer::TKOCElement& elemet_to_delete);
            public:
                AggregatedCheckList();
                ~AggregatedCheckList();
                void addCheckList(const std::string& check_list_name);
                void removeCheckList(const std::string& check_list_name);
                CheckList *getSharedCheckList(const std::string& check_list_name);
            };
            
        }
    }
}

#endif /* AggregatedCheckList_hpp */
