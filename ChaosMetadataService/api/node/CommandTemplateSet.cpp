/*
 *	SetCommandTemplate.cpp
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

#include "CommandTemplateSet.h"

#include <boost/format.hpp>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

#define N_SCT_INFO INFO_LOG(CommandTemplateSet)
#define N_SCT_DBG  DBG_LOG(CommandTemplateSet)
#define N_SCT_ERR  ERR_LOG(CommandTemplateSet)

CommandTemplateSet::CommandTemplateSet():
AbstractApi("commandTemplateSet"){
    
}

CommandTemplateSet::~CommandTemplateSet() {
    
}

CDataWrapper *CommandTemplateSet::execute(CDataWrapper *api_data,
                                          bool& detach_data) throw(chaos::CException) {
    int err = 0;
    uint64_t sequence = 0;
    bool presence = false;
    CHECK_CDW_THROW_AND_LOG(api_data, N_SCT_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, "template_list", N_SCT_ERR, -2, "The attribute template_list is mandatory")
    
    //get the data access
    GET_DATA_ACCESS(NodeDataAccess, n_da, -3)
    GET_DATA_ACCESS(UtilityDataAccess, u_da, -4)
    
    ChaosUniquePtr<CMultiTypeDataArrayWrapper> tempalte_list(api_data->getVectorValue("template_list"));
    for(int idx = 0;
        idx < tempalte_list->size();
        idx++) {
        
        ChaosUniquePtr<CDataWrapper> template_element(tempalte_list->getCDataWrapperElementAtIndex(idx));
        
        if(!template_element->hasKey("template_name")||
           !template_element->hasKey(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID)) {
            N_SCT_ERR << "template with no all mandatory key: " << template_element->getJSONData();
            continue;
        }
        
        const std::string template_name = template_element->getStringValue("template_name");
        const std::string command_unique_id = template_element->getStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID);
        
        //load command description for validation
        if((err = n_da->checkCommandPresence(command_unique_id,
                                      presence))) {
           LOG_AND_TROW_FORMATTED(N_SCT_ERR, -5, "Error checking the presence of the for command uid %1%", %command_unique_id)
        } else if(!presence) {
            LOG_AND_TROW_FORMATTED(N_SCT_ERR, -6, "The uid %1% not have any command associated to it", %command_unique_id)
        }
        
        //check if it is presence, otherwhise we need to add the sequence
        if((err = n_da->checkCommandTemplatePresence(template_name, command_unique_id, presence))){
            LOG_AND_TROW_FORMATTED(N_SCT_ERR, -7, "Error checking the presence of the template %1% for command uid %2%", %template_name%command_unique_id)
        } else if(!presence) {
            //we need to add the sequence
            if((err = u_da->getNextSequenceValue("nodes_command_template", sequence))) {
                LOG_AND_TROW_FORMATTED(N_SCT_ERR, -8, "Error getting the sequence for new template %1% for command uid %2%", %template_name%command_unique_id)
            }
            
            //we have the sequence
            template_element->addInt64Value("seq", sequence);
        }
        if((err = n_da->setCommandTemplate(*template_element))) {
            LOG_AND_TROW_FORMATTED(N_SCT_ERR, -9, "Error setting the template %1% of command uid %2%", %template_name%command_unique_id)
        }
    }
    return NULL;
}
