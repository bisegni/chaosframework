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
