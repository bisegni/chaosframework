/*
 *	LoadUnloadControlUnit.h
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
#ifndef __CHAOSFramework__LoadUnloadControlUnit__
#define __CHAOSFramework__LoadUnloadControlUnit__

#include "../mds_service_batch.h"

#include <memory>
namespace chaos {
    namespace metadata_service{
        namespace batch {
            class MDSBatchExcecutor;

            namespace unit_server {

                typedef enum LoadUnloadPhase {
                    LUL_SEND_LOAD_COMMAND,
                    LUL_SEND_UNLOAD_COMMAND,
                    LUL_WAIT_ANSWER,
                    LUL_END,
                    LUL_ERR
                } LoadUnloadPhase;

                class LoadUnloadControlUnit:
                public metadata_service::batch::MDSBatchCommand {
                    DECLARE_MDS_COMMAND_ALIAS
                    uint32_t retry_number;

                    bool load;
                    std::string cu_id;
                    std::string cu_type;
                    std::string us_address;

                    chaos::common::message::MessageChannel *message_channel;
                    std::auto_ptr<chaos::common::message::MessageRequestFuture> request_future;


                    LoadUnloadPhase phase;
                public:
                    LoadUnloadControlUnit();
                    ~LoadUnloadControlUnit();
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


#endif /* defined(__CHAOSFramework__LoadUnloadControlUnit__) */
