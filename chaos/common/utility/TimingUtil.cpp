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

#include <stdlib.h>

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

TimingUtil::TimingUtil():
calibration_enable_status(GlobalConfiguration::getInstance()->getOption<bool>(InitOption::OPT_TIME_CALIBRATION)),
calibration_offset_bound(GlobalConfiguration::getInstance()->getOption<uint32_t>(InitOption::OPT_TIME_CALIBRATION_OFFSET_BOUND)),
remote_ntp_server(GlobalConfiguration::getInstance()->getOption<std::string>(InitOption::OPT_TIME_CALIBRATION_NTP_SERVER)){}


void TimingUtil::enableTimestampCalibration() {
    const bool has_ntp_server = GlobalConfiguration::getInstance()->hasOption(InitOption::OPT_TIME_CALIBRATION_NTP_SERVER);
    if(calibration_enable_status == false) throw CException(-1, "Calibration is not enable for this process", __PRETTY_FUNCTION__);
    if(has_ntp_server == false) throw CException(-2, "Calibration need to have an ntp server host name", __PRETTY_FUNCTION__);
    if(remote_ntp_server.size() == 0) throw CException(-3, "NTP server need to be valid", __PRETTY_FUNCTION__);
    
    AsyncCentralManager::getInstance()->addTimer(this, 0, 60000);
}

void TimingUtil::disableTimestampCalibration() {
    AsyncCentralManager::getInstance()->removeTimer(this);
}

void TimingUtil::timeout() {
    uint64_t pre_ts = getTimeStamp();
    uint64_t ntp_ts = getNTPTS();
    //uint64_t post_ts = getTimeStamp();
    int64_t diff = ntp_ts - pre_ts; //how much ntp time diff from us
    if(std::llabs(diff) > calibration_offset_bound) {
        timestamp_calibration_offset = diff;
        TU_LAPP << CHAOS_FORMAT("Timestamp calibration has been applyed for %1% milliseconds with pre_ts:%2% ntpts:%3%",%diff%pre_ts%ntp_ts);
    } else {
        TU_LDBG << CHAOS_FORMAT("Timestamp calibration acquired data pre_ts:%1% ntpts:%2% diff:%3% ", %pre_ts%ntp_ts%diff);
    }
}

uint64_t TimingUtil::getNTPTS() {
    time_t result_time = 0;
    boost::array<char, 48> asio_buffer;
    ntp_packet *packet = reinterpret_cast<ntp_packet*>(asio_buffer.data());
    //reset ntp packet
    memset( asio_buffer.data(), 0, 48 );
    *( ( char * ) asio_buffer.data() + 0 ) = 0x1b; //00011011
    
    boost::asio::io_service io_service;
    boost::asio::ip::udp::resolver resolver(io_service);
    boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(),
                                                remote_ntp_server,
                                                "ntp");
    
    boost::asio::ip::udp::endpoint receiver_endpoint = *resolver.resolve(query);
    
    boost::asio::ip::udp::socket socket(io_service);
    socket.open(boost::asio::ip::udp::v4());
    
    
    
    socket.send_to(boost::asio::buffer(asio_buffer), receiver_endpoint);
    boost::asio::ip::udp::endpoint sender_endpoint;
    
    try{
        size_t len = socket.receive_from(boost::asio::buffer(asio_buffer),
                                         sender_endpoint);
        if(len != sizeof(ntp_packet)) {throw chaos::CException(-1, "Error receiving data from ntp", __PRETTY_FUNCTION__);}
        packet->txTm_s = ntohl(packet->txTm_s);
        packet->txTm_f = ntohl(packet->txTm_f);
        const uint64_t sec_part = ((uint64_t)packet->txTm_s - 2208988800ULL)  * 1000; //seconds since 1970
        const double msec_part = ((double)packet->txTm_f / /*(double)std::numeric_limits<int32_t>::max()*/4294967296.0) * 1000;
        //timeRecv = recvBuf[43] << 24 | recvBuf[42] << 16 | recvBuf[41] << 8 | recvBuf[40];
        //timeRecv = htonll((time_t)timeRecv);
        result_time = sec_part + (uint64_t)msec_part;
        
    } catch (std::exception& e){
        throw chaos::CException(-2, e.what(), __PRETTY_FUNCTION__);
        
    }
    
    return (uint64_t)result_time;
}
