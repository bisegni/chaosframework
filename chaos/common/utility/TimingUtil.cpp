/*
 *	TimingUtil.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 08/08/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/async_central/AsyncCentralManager.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#include <boost/array.hpp>
#include <boost/asio.hpp>

using namespace chaos::common::utility;
using namespace chaos::common::async_central;

int64_t TimingUtil::timestamp_calibration_offset = 0;

const char* TimingUtil::formats[]={
    "%Y-%m-%dT%H:%M:%S.%fZ",
    "%Y-%m-%d %H:%M:%S.%fZ",
    "%Y-%m-%d %H:%M:%SZ",
    "%Y-%m-%dT%H:%M:%SZ",
    "%Y-%m-%d %H:%MZ",
    "%Y-%m-%dT%H:%MZ",
    "%Y-%m-%d %HZ",
    "%Y-%m-%dT%HZ",
    "%Y-%m-%dZ",
    "%Y-%mZ",
    "%YZ"
};

const size_t TimingUtil::formats_n = 11;


void TimingUtil::enableTimestampCalibration() {
    const bool calibration_ebale_status = GlobalConfiguration::getInstance()->getOption<bool>(InitOption::OPT_TIME_CALIBRATION);
    if(calibration_ebale_status == false) throw CException(-1, "Calibration is not enable for this process", __PRETTY_FUNCTION__);
    
    const bool has_ntp_server = GlobalConfiguration::getInstance()->hasOption(InitOption::OPT_TIME_CALIBRATION_NTP_SERVER);
    if(has_ntp_server == false) throw CException(-2, "Calibration need to have an ntp server host name", __PRETTY_FUNCTION__);
    
    remote_ntp_server = GlobalConfiguration::getInstance()->getOption<std::string>(InitOption::OPT_TIME_CALIBRATION_NTP_SERVER);
    if(remote_ntp_server.size() == 0) throw CException(-3, "NTP server need to be valid", __PRETTY_FUNCTION__);
    AsyncCentralManager::getInstance()->addTimer(this, 0, 60000);
}

void TimingUtil::disableTimestampCalibration() {
    AsyncCentralManager::getInstance()->removeTimer(this);
}

void TimingUtil::timeout() {
    uint64_t pre_ts = getTimeStamp();
    uint64_t ntp_ts = getNTPTS();
    uint64_t post_ts = getTimeStamp();
    TU_LDBG << CHAOS_FORMAT("Timestamp calibration pre_ts:%1% ntpts:%2% post_ts:%3%", %pre_ts%ntp_ts%post_ts);
}

uint64_t TimingUtil::getNTPTS() {
    time_t timeRecv = 0;
    time_t timeRecvFrac = 0;
    boost::asio::io_service io_service;
    
    boost::asio::ip::udp::resolver resolver(io_service);
    boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(),
                                                remote_ntp_server,
                                                "ntp");
    
    boost::asio::ip::udp::endpoint receiver_endpoint = *resolver.resolve(query);
    
    boost::asio::ip::udp::socket socket(io_service);
    socket.open(boost::asio::ip::udp::v4());
    
    boost::array<unsigned char, 48> sendBuf  = {010,0,0,0,0,0,0,0,0};
    
    socket.send_to(boost::asio::buffer(sendBuf), receiver_endpoint);
    
    boost::array<unsigned long, 1024> recvBuf;
    boost::asio::ip::udp::endpoint sender_endpoint;
    
    try{
        size_t len = socket.receive_from(boost::asio::buffer(recvBuf),
                                         sender_endpoint);
        
        timeRecv = ntohl((time_t)recvBuf[4]);
        timeRecvFrac = ntohl((time_t)recvBuf[5]);
        //timeRecv = recvBuf[43] << 24 | recvBuf[42] << 16 | recvBuf[41] << 8 | recvBuf[40];
        //timeRecv = htonll((time_t)timeRecv);
        timeRecv-= 2208988800U;  //Unix time starts from 01/01/1970 == 2208988800U
        
    }catch (std::exception& e){
        
        std::cerr << e.what() << std::endl;
        
    }
    
    return (uint64_t)timeRecv;
}
