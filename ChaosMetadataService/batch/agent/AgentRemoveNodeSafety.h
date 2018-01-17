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

#ifndef __CHAOSFramework_EEF22B9E_E40A_487A_AEAD_E0E8BA56C374_AgentRemoveNodeSafety_h
#define __CHAOSFramework_EEF22B9E_E40A_487A_AEAD_E0E8BA56C374_AgentRemoveNodeSafety_h

#include "../mds_service_batch.h"
#include <chaos/common/chaos_types.h>
#include <chaos/common/event/event.h>
namespace chaos {
    namespace metadata_service{
        namespace batch {
            class MDSBatchExcecutor;
            
            namespace agent {
                
                class AgentRemoveNodeSafety:
                public metadata_service::batch::MDSBatchCommand {
                    DECLARE_MDS_COMMAND_ALIAS
                    typedef enum ScanPhase {
                        ScanPhaseNext,
                        ScanPhaseProcess,
                        ScanPhaseEnd
                    } ScanPhase_t;
                    
                    std::string agent_uid;
                    ScanPhase_t scan_phase;
                    ChaosStringVector associated_nodes;
                    ChaosUniquePtr<RequestInfo> request;
                    ChaosUniquePtr<chaos::common::data::CDataWrapper>           message_data;
                    service_common::data::agent::VectorAgentAssociationIterator current_association_it;
                    
                    ChaosUniquePtr<chaos::common::data::CDataWrapper>       agent_node_information;
                    service_common::data::agent::VectorAgentAssociation     association_list;
                    chaos::common::event::channel::AlertEventChannel        *alert_event_channel;
                    
                    chaos::common::data::CDWUniquePtr composeStopMessage(service_common::data::agent::AgentAssociation& agent_association,
                                                                         bool kill = true);
                    
                    bool processStopOperationPhases();
                public:
                    AgentRemoveNodeSafety();
                    ~AgentRemoveNodeSafety();
                    
                protected:
                    // inherited method
                    void setHandler(chaos_data::CDataWrapper *data);
                    
                    // inherited method
                    void acquireHandler();
                    
                    // inherited method
                    void ccHandler();
                    
                    // inherited method
                    bool timeoutHandler();
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_EEF22B9E_E40A_487A_AEAD_E0E8BA56C374_AgentRemoveNodeSafety_h */
