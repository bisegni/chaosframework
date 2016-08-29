/*
 *	TestBatchDefault.h
 *
 *	!CHAOS [CHAOSFrameworkTests]
 *	Created by bisegni.
 *
 *    	Copyright 29/08/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFrameworkTests_EA26A8EF_AE10_48EE_B28D_D8770E555D71_TestBatchDefault_h
#define __CHAOSFrameworkTests_EA26A8EF_AE10_48EE_B28D_D8770E555D71_TestBatchDefault_h

#include "TestBatchCommand.h"

#include <chaos/common/utility/TimingUtil.h>

namespace chaos {
    namespace common {
        namespace batch_command {
            namespace test {
                class TestCommandExecutor;
                
                //batch comand test
                class TestBatchDefaultCommand:
                public TestBatchCommand {
                    friend class TestCommandExecutor;

                public:
                    //! default constructor
                    TestBatchDefaultCommand();
                    
                    //! default destructor
                    ~TestBatchDefaultCommand();
                    
                    void setHandler(chaos::common::data::CDataWrapper *data);
                    
                    // inherited method
                    void acquireHandler();
                    
                    // inherited method
                    void ccHandler();
                    
                    // inherited method
                    bool timeoutHandler();
                    
                    uint8_t implementedHandler();
                };
                
            }
        }
    }
}

#endif /* __CHAOSFrameworkTests_EA26A8EF_AE10_48EE_B28D_D8770E555D71_TestBatchDefault_h */
