/*
 *	persistence_types.h
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
#ifndef CHAOSFramework_persistence_types_h
#define CHAOSFramework_persistence_types_h

#include <vector>
#include <string>
#include <stdint.h>

namespace chaos {
    namespace metadata_service{
        namespace persistence {
            
            typedef enum AutoFlag {
                AUTO_LOAD,
                AUTO_INIT,
                AUTO_START
            }AutoFlag;
            
            struct NodeSearchIndex {
                uint64_t seq;
                std::string node_uid;
                
                NodeSearchIndex():
                seq(0),
                node_uid(){};
                NodeSearchIndex(uint64_t _seq,
                                const std::string& _node_uid):
                seq(_seq),
                node_uid(_node_uid){};
                NodeSearchIndex(const NodeSearchIndex& _nsi):
                seq(_nsi.seq),
                node_uid(_nsi.node_uid){};
            };
        }
    }
}
#endif
