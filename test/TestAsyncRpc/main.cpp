    //
    //  main.cpp
    //  TestAsyncRpc
    //
    //  Created by Claudio Bisegni on 19/01/14.
    //  Copyright (c) 2014 INFN. All rights reserved.
    //

#include <string>
#include <iostream>

#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/message/DeviceMessageChannel.h>

#include <chaos/ui_toolkit/ChaosUIToolkit.h>
using namespace chaos;
using namespace chaos::ui;
using namespace chaos::common::data;
using namespace chaos::common::message;

typedef enum {
    CUPhaseInit,
    CUPhaseStart,
    CUPhaseStop,
    CUPhaseDeinit
} CUPhase;

#define CHECH_START_STATE(s,w)\
if(s != w) {\
std::cout << " CU is not in the " #w " state" << std::endl;\
return -100;\
}

#define CHECH_START_STATE_W2(s,w1,w2)\
if(s != w1 &&\
s!= w2) {\
std::cout << " CU is not in the " #w1 " or " #w2 " state" << std::endl;\
return -100;\
}

int executePhase(DeviceMessageChannel *message_channel,
                 CUPhase phase,
                 bool wait = false) {
    unsigned int max_retry = 3;
    int err = ErrorCode::EC_NO_ERROR;
    CUStateKey::ControlUnitState state_to_reach = CUStateKey::UNDEFINED;
    CUStateKey::ControlUnitState state = CUStateKey::UNDEFINED;

    if((message_channel->isOnline() == false)) {
        std::cout << " Channel is offline" << std::endl;
        return -1;
    }
    
    if((err = message_channel->getState(state,5000))) {
        std::cout << " Error " << err << " getting state" << std::endl;
        return err;
    }

    std::cout << "Send ";

    switch (phase) {
        case CUPhaseInit:
            state_to_reach = CUStateKey::INIT;
            std::cout << " initilization ";
            CHECH_START_STATE(state, CUStateKey::DEINIT)
            err = message_channel->initDeviceToDefaultSetting();
            break;
        case CUPhaseStart:
            state_to_reach = CUStateKey::START;
            std::cout << " starting ";
            CHECH_START_STATE_W2(state, CUStateKey::INIT, CUStateKey::STOP)
            err = message_channel->startDevice();
            break;
        case CUPhaseStop:
            state_to_reach = CUStateKey::STOP;
            std::cout << " stopping ";
            CHECH_START_STATE(state, CUStateKey::START)
            err = message_channel->stopDevice();
            break;
        case CUPhaseDeinit:
            state_to_reach = CUStateKey::DEINIT;
            std::cout << " deinitilizzation ";
            CHECH_START_STATE_W2(state, CUStateKey::INIT, CUStateKey::STOP)
            err = message_channel->deinitDevice();
            break;
    }
    std::cout << " phase to the id:" << std::endl;
    if(!err && wait) {
        do{
            max_retry--;
            sleep(1);
            err = message_channel->getState(state);
        } while(max_retry>0&&
                err == 0 &&
                state != CUStateKey::FATAL_ERROR &&
                state != CUStateKey::RECOVERABLE_ERROR &&
                state != state_to_reach);
        if(state == state_to_reach){
            std::cout << "Phase has been reached" << std::endl;
        } else {
            std::cout << "Pahse has NOT BEEN reached" << std::endl;
        }
        return (state == state_to_reach) ?0:err;
    } else {
        return err;
    }
}

int main(int argc, char* argv[] ) {
    std::string device_id;
    uint32_t test_run = 0;
    uint32_t test_type = 0;
    uint32_t current_run = 0;
    uint32_t test_run_delay = 0;
    try{
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>("test-type",
                                                                                             "specifi the test[0 RPC Info, 1-Echo, 2-linear cu phase load,init,start,stop,deinit,uload], 3-random cu phase load,init,start,stop,deinit,uload]",
                                                                                             &test_type);
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>("test-run",
                                                                                             "Specify how many times the test need to be executed",
                                                                                             1,
                                                                                             &test_run);
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>("test-run-delay",
                                                                                             "Specify how many millisecondssecond next test needs to be launched",
                                                                                             1000,
                                                                                             &test_run_delay);
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<std::string>("device-id",
                                                                                                "specifi the device id for the test",
                                                                                                &device_id);
            //! [UIToolkit Init]
        chaos::ui::ChaosUIToolkit::getInstance()->init(argc, argv);

        DeviceMessageChannel *device_message_channel = NetworkBroker::getInstance()->getDeviceMessageChannelFromAddress(new CDeviceNetworkAddress(device_id),true);
        if(device_message_channel == NULL)  throw chaos::CException(5, "Error allocating device emssage channel", "device controller creation");
        device_message_channel->setAutoReconnection(true);
        while(current_run++ < test_run) {
            std::cout << "Execute test run:" << current_run << "-------------------------------------------" << std::endl;
            switch(test_type) {
                case 0:{
                    std::cout << "Test RPC information call on device id:" << device_id << std::endl;
                    std::auto_ptr<MessageRequestFuture> message_future = device_message_channel->checkRPCInformation();
                    if(message_future->wait(5000)){
                        if(message_future->getResult()) {
                            std::cout << message_future->getResult()->getJSONString() << std::endl;
                        } else {
                            std::cout << "No data received"<< std::endl;
                        }
                    } else {
                        std::cout << "Timeout reached"<< std::endl;
                    }
                    break;
                }

                case 1:{
                    CDataWrapper echo_data;
                    echo_data.addStringValue("echo_key", "echo_value");
                    std::cout << "Test RPC echo call on device id:" << device_id << std::endl;
                    std::cout << "Sending data:" << echo_data.getJSONString() << std::endl;

                    std::auto_ptr<MessageRequestFuture> message_future = device_message_channel->echoTest(&echo_data);
                    if(message_future->wait(5000)){
                        if(message_future->getResult()) {
                            std::cout << "Received data:" << message_future->getResult()->getJSONString() << std::endl;
                        }else {
                            std::cout << "No data received"<< std::endl;
                        }
                    } else {
                        std::cout << "Timeout reached"<< std::endl;
                    }
                    break;
                }

                case 2:{
                    std::cout << "Test linear call on control unit phases:" << device_id << std::endl;
                    for(int p = CUPhaseInit;
                        p <= CUPhaseDeinit;
                        p++) {
                        executePhase(device_message_channel,
                                     static_cast<CUPhase>(p),
                                     true);
                    }
                    break;
                }
                case 3:{
                    std::cout << "Test random call on control unit phases:" << device_id << std::endl;

                    break;
                }
                default:
                    break;
            }
            
            usleep(test_run_delay*1000);
        }
        NetworkBroker::getInstance()->disposeMessageChannel(device_message_channel);
            //test echo and rpc information on device
        ChaosUIToolkit::getInstance()->deinit();
        
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex);
    }
    return 0;
}

