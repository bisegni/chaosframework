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

#ifndef __CHAOSFrameworkTests__5083295_046E_406A_8A00_87FE658807DE_TestBatchCommand_h
#define __CHAOSFrameworkTests__5083295_046E_406A_8A00_87FE658807DE_TestBatchCommand_h

#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/batch_command/BatchCommand.h>

namespace chaos {
    namespace common {
        namespace batch_command {
            namespace test {
                class TestCommandExecutor;
                
                //batch comand test
                class TestBatchCommand:
                public chaos::common::batch_command::BatchCommand {
                    friend class TestCommandExecutor;
                    
                    uint64_t create_ts;
                    
                    uint64_t set_ts;
                    
                    uint64_t end_ts;
                    
                    uint64_t cicle_count;
                protected:
                    //! default constructor
                    TestBatchCommand();
                    
                    //! default destructor
                    ~TestBatchCommand();
                    
                    void setHandler(chaos::common::data::CDataWrapper *data);
                    
                    // inherited method
                    void acquireHandler();
                    
                    // inherited method
                    void ccHandler();
                    
                    // inherited method
                    bool timeoutHandler();
                };
                
                class TestCommandSetOnly:
                public TestBatchCommand {
                    // inherited method
                    uint8_t implementedHandler();
                };
                
                class TestCommandComplete:
                public TestBatchCommand {
                    // inherited method
                    uint8_t implementedHandler();
                };
            }
        }
    }
}

#endif /* __CHAOSFrameworkTests__5083295_046E_406A_8A00_87FE658807DE_TestBatchCommand_h */
