/*
 *	IDSTControlUnitBatchCommand.h
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

#ifndef __CHAOSFramework__IDSTControlUnitBatchCommand__
#define __CHAOSFramework__IDSTControlUnitBatchCommand__

#include "../mds_service_batch.h"

namespace chaos {
    namespace metadata_service{
        namespace batch {
            class MDSBatchExcecutor;
            namespace control_unit {


                typedef enum IDSTAction{
                    ACTION_INIT = 0,
                    ACTION_START,
                    ACTION_STOP,
                    ACTION_DEINIT
                }IDSTAction;

                    //!Batch command for send init/deinit/start/stopm command
                class IDSTControlUnitBatchCommand:
                public metadata_service::batch::MDSBatchCommand {
                    DECLARE_MDS_COMMAND_ALIAS
                    uint32_t retry_number;

                    CNetworkAddress * control_unit_address;
                    std::auto_ptr<chaos::common::data::CDataWrapper> message;
                    std::auto_ptr<RequestInfo> request;

                    std::string cu_id;

                    IDSTAction  action;
                    
                    std::auto_ptr<CDataWrapper> initialize(const std::string& cu_uid);
                    std::auto_ptr<CDataWrapper> start(const std::string& cu_uid);
                    std::auto_ptr<CDataWrapper> stop(const std::string& cu_uid);
                    std::auto_ptr<CDataWrapper> deinitialize(const std::string& cu_uid);
                    boost::shared_ptr<CDataWrapper> mergeDatasetAttributeWithSetup(boost::shared_ptr<CDataWrapper> element_in_dataset,
                                                                                   boost::shared_ptr<CDataWrapper> element_in_setup);
                public:
                    IDSTControlUnitBatchCommand();
                    ~IDSTControlUnitBatchCommand();
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

#endif /* defined(__CHAOSFramework__IDSTControlUnitBatchCommand__) */
