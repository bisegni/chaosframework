//
//  DirectIOTest.hpp
//  FrameworkTest
//
//  Created by bisegni on 15/05/2018.
//  Copyright © 2018 bisegni. All rights reserved.
//

#ifndef DirectIOTest_hpp
#define DirectIOTest_hpp

#include <gtest/gtest.h>
#include <chaos/common/direct_io/DirectIO.h>

class DirectIOEchoHandler:
public chaos::common::direct_io::channel::DirectIOSystemAPIServerChannel::DirectIOSystemAPIServerChannelHandler {
    
protected:
    
    //! endpoint entry method
    int consumeDataPack(chaos::common::direct_io::DirectIODataPackSPtr  data_pack,
                        chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer);
};


class DirectIOTest:
public testing::Test {
protected:
    chaos::common::direct_io::DirectIOServerEndpoint *endpoint;
    chaos::common::direct_io::channel::DirectIOSystemAPIServerChannel *server_channel;
public:
    DirectIOTest();
    ~DirectIOTest();
    void SetUp();
    void TearDown();
};
#endif /* DirectIOTest_hpp */
