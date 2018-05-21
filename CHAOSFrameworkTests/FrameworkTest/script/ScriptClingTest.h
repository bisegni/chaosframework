//
//  ScriptClingTest.h
//  CHAOSFrameworkTests
//
//  Created by Claudio Bisegni on 03/05/2018.
//  Copyright © 2018 bisegni. All rights reserved.
//

#ifndef ScriptClingTest_h
#define ScriptClingTest_h
#include <chaos/common/script/AbstractScriptableClass.h>
#include <chaos/common/chaos_types.h>
#ifdef CLING_VIRTUAL_MACHINE
class TestApi:
public chaos::common::script::TemplatedAbstractScriptableClass<TestApi> {
    
public:
    TestApi();
    ~TestApi();
protected:
    int echo(const chaos::common::script::ScriptInParam& input_parameter,
             chaos::common::script::ScriptOutParam& output_parameter);
    
};
#endif
#endif /* ScriptClingTest_h */
