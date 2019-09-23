//
//  test_types.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 23/09/2019.
//  Copyright © 2019 INFN. All rights reserved.
//

#ifndef test_types_h
#define test_types_h

#include <chaos/common/chaos_types.h>

namespace chaos {
namespace metadata_service {
namespace test {

class Test {
public:
    virtual void executeTest() = 0;
};

CHAOS_DEFINE_VECTOR_FOR_TYPE(ChaosSharedPtr<Test>, TestList);

class TestContainer:
public Test {
protected:
    TestList test_suite;
public:
    void executeTest() {
        for(ChaosSharedPtr<Test> cur_test: test_suite) {
               cur_test->executeTest();
        }
    }
};

}
}
}
#endif /* test_types_h */
