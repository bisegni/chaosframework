/*
 *	UIToolkitCMDLineExample.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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
#include <iostream>
#include <string>
#include <vector>
#include <chaos/common/global.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/network/CNodeNetworkAddress.h>
#include <chaos/ui_toolkit/ChaosUIToolkit.h>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/ui_toolkit/HighLevelApi/HLDataApi.h>
#include <stdio.h>
#include <chaos/common/bson/bson.h>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

using namespace std;
using namespace chaos;
using namespace chaos::common::network;
using namespace chaos::ui;
using namespace bson;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::date_time;
namespace chaos_batch = chaos::common::batch_command;

#define OPT_NODE_ADDRESS    "node_address"
#define OPT_RT_TEST			"round_trip_test"
#define OPT_RT_TEST_ITER	"round_trip_test_iteration"
#define OPT_TIMEOUT         "timeout"

typedef std::vector<int>			OpcodeSequence;
typedef std::vector<int>::iterator	OpcodeSequenceIterator;

typedef struct RttStat {
	int64_t trx_min ;
	int64_t trx_max ;
	int64_t rec_min ;
	int64_t rec_max ;
	int64_t rtt_min ;
	int64_t rtt_max ;
	int64_t calc_trx ;
	int64_t calc_rec ;
	int64_t calc_rtt ;
        RttStat(){trx_min = 0;trx_max = 0;rec_min = 0;rec_max = 0;rtt_min = 0;rtt_max = 0;calc_trx = 0;calc_rec = 0; calc_rtt = 0;}
}RttStat;

void performRTTTest(chaos::common::direct_io::DirectIOPerformanceSession *session, uint32_t iteration, uint32_t timeout);
void performRTTTestFetcher(chaos::common::direct_io::RttResultFetcher *fetcher, uint32_t iteration, RttStat *stat);

int main (int argc, char* argv[] ) {
	uint32_t timeout = 0;
	OpcodeSequence command_sequence;
	CNetworkAddress *device_network_address = NULL;
    try {
		
		
        //! [UIToolkit Attribute Init]
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<string>(OPT_NODE_ADDRESS, "The netwrok address of the remote node(ip:port of rpc server)");
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_RT_TEST, "Perform the roundtrip test");
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>(OPT_RT_TEST_ITER, "The number of iteration for the round tirp test");
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>(OPT_TIMEOUT, "Timeout", 2000, &timeout);
		//! [UIToolkit Attribute Init]
        
        //! [UIToolkit Init]
        ChaosUIToolkit::getInstance()->init(argc, argv);
        //! [UIToolkit Init]
        
        if(!ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_NODE_ADDRESS)){
            throw chaos::CException(-1, "Remote node address not set", __PRETTY_FUNCTION__);
        }
        
		if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_RT_TEST)) {
			command_sequence.push_back(1);
		}
		device_network_address = new CNetworkAddress();
		device_network_address->ip_port = ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<std::string>(OPT_NODE_ADDRESS);
		chaos::common::message::PerformanceNodeChannel *perf_channel = LLRpcApi::getInstance()->getNewPerformanceChannel(device_network_address);
		chaos::common::direct_io::DirectIOPerformanceSession *session = NULL;
		if(!perf_channel->getPerformanceSession(&session, timeout) && session) {
			for (OpcodeSequenceIterator iter = command_sequence.begin();
				 iter != command_sequence.end();
				 iter++) {
				switch (*iter) {
					case 1:
						if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_RT_TEST_ITER)) {
							performRTTTest(session,
										   ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<uint32_t>(OPT_RT_TEST_ITER),
										   timeout);
						} else {
							performRTTTest(session,
										   1,
										   timeout);
						}
						
						break;
						
					default:
						break;
				}
			}
			//clear the session
			perf_channel->releasePerformanceSession(session, timeout);
		}else {
			std::cout<< " session not created " << std::endl;
		}
		
		if(perf_channel) LLRpcApi::getInstance()->deleteMessageChannel(perf_channel);
    } catch (CException& e) {
        std::cerr << e.errorCode << " - "<< e.errorDomain << " - " << e.errorMessage << std::endl;
    }
    try {
        //! [UIToolkit Deinit]
        ChaosUIToolkit::getInstance()->deinit();
        //! [UIToolkit Deinit]
    } catch (CException& e) {
        std::cerr << e.errorCode << " - "<< e.errorDomain << " - " << e.errorMessage << std::endl;
    }
    return 0;
}

void performRTTTest(chaos::common::direct_io::DirectIOPerformanceSession *session, uint32_t iteration, uint32_t timeout) {
	//ok
	RttStat stat;
	boost::shared_ptr<boost::thread> thread(new boost::thread(performRTTTestFetcher, session->getRttResultQueue(), iteration, &stat));
	for (uint32_t idx = 0; idx < iteration; idx++) {
		if(session->sendRttTest(timeout) == -1){
				std::cout << "Err sending message";
		}
		
	}
	thread->join();
	std::cout << std::endl;
	std::cout << "Round Trip test reults----------------------------------------------------" << std::endl;
	std::cout << "Transmissione ts ->" << stat.trx_min << "us to " << stat.trx_max << "us " << std::endl;
	std::cout << "Receive ts ->" << stat.rec_min << " to " << stat.rec_max  << "us " << std::endl;
	std::cout << "Roundtrip ts ->" << stat.rtt_min << " to " << stat.rtt_max  << "us " << std::endl;
	std::cout << "END Round Trip test reults----------------------------------------------------" << std::endl;
}

void performRTTTestFetcher(chaos::common::direct_io::RttResultFetcher *fetcher, uint32_t iteration, RttStat *stat) {
	int idx = 0;
	chaos::common::direct_io::channel::opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr test_result = NULL;
	while (idx < iteration) {
		if(fetcher->getNext(test_result)) {
			boost::posix_time::ptime time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration duration( time.time_of_day() );
			stat->calc_trx = test_result->field.receiver_rt_ts - test_result->field.start_rt_ts;
			stat->calc_rec = duration.total_microseconds() - test_result->field.receiver_rt_ts;
			stat->calc_rtt = duration.total_microseconds()  - test_result->field.start_rt_ts;
			if(!idx) {
				stat->trx_min = stat->trx_max = test_result->field.receiver_rt_ts - test_result->field.start_rt_ts;
				stat->rec_min = stat->rec_max = duration.total_microseconds() - test_result->field.receiver_rt_ts;
				stat->rtt_min = stat->rtt_max = duration.total_microseconds()  - test_result->field.start_rt_ts;
			} else {
				stat->trx_max = std::max(stat->trx_max, stat->calc_trx);
				stat->trx_min = std::min(stat->trx_min, stat->calc_trx);
				stat->rec_max = std::max(stat->rec_max, stat->calc_rec);
				stat->rec_min = std::min(stat->rec_min, stat->calc_rec);
				stat->rtt_max = std::max(stat->rtt_max, stat->calc_rtt);
				stat->rtt_min = std::min(stat->rtt_min, stat->calc_rtt);
			}
			idx++;
		}
	}
}