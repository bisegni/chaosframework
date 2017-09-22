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
