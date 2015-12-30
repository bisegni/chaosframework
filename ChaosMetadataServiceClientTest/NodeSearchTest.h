/*
 *	NodeSearch
 *	!CHAOS
 *	Created by Claudio Bisegni on 14/09/15.
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

#ifndef CHAOS_NODESEARCH_H
#define CHAOS_NODESEARCH_H

#include <string>
#include <stdint.h>

class NodeSearchTest {
  uint32_t page_length;
 public:
  NodeSearchTest(uint32_t page_length);
  void testSearch(const std::string& search_string);
};


#endif //CHAOS_NODESEARCH_H
