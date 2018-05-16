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
    int consumeEchoEvent(chaos::common::data::BufferSPtr input_data,
                         chaos::common::data::BufferSPtr& output_data);
};

class DirectIOEchoDelayedHandler:
public DirectIOEchoHandler {
    unsigned int echo_received_number;
protected:
    //! endpoint entry method
    int consumeEchoEvent(chaos::common::data::BufferSPtr input_data,
                         chaos::common::data::BufferSPtr& output_data);
public:
    DirectIOEchoDelayedHandler();
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
