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
#ifndef ChaosFramework_TimingUtil_h
#define ChaosFramework_TimingUtil_h

#include <stdint.h>

#include <chaos/common/global.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/async_central/TimerHandler.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_time.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

#define TU_LAPP INFO_LOG(TimingUtil)
#define TU_LDBG DBG_LOG(TimingUtil)
#define TU_LERR ERR_LOG(TimingUtil)

namespace chaos {
    namespace common {
        namespace utility {
            /*
             Class for give some method util for timing purpose
             */
            class TimingUtil:
            public chaos::common::async_central::TimerHandler,
            public chaos::common::utility::Singleton<TimingUtil> {
                friend class chaos::common::utility::Singleton<TimingUtil>;
                const uint32_t      calibration_offset_bound;
                const bool          calibration_enable_status;
                const std::string   remote_ntp_server;
                static int64_t      timestamp_calibration_offset;
                static const char* formats[];
                static const size_t formats_n;
                
                //NTP packet definition
                typedef struct {
                    unsigned li   : 2;       // Only two bits. Leap indicator.
                    unsigned vn   : 3;       // Only three bits. Version number of the protocol.
                    unsigned mode : 3;       // Only three bits. Mode. Client will pick mode 3 for client.
                    
                    uint8_t stratum;         // Eight bits. Stratum level of the local clock.
                    uint8_t poll;            // Eight bits. Maximum interval between successive messages.
                    uint8_t precision;       // Eight bits. Precision of the local clock.
                    
                    uint32_t rootDelay;      // 32 bits. Total round trip delay time.
                    uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
                    uint32_t refId;          // 32 bits. Reference clock identifier.
                    
                    uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
                    uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.
                    
                    uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds.
                    uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second.
                    
                    uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
                    uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.
                    
                    uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
                    uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.
                } ntp_packet;
                
                static inline boost::posix_time::time_duration getUTCOffset() {
                    using namespace boost::posix_time;
                    typedef boost::date_time::c_local_adjustor<ptime> local_adj;
                    const ptime utc_now = second_clock::universal_time();
                    const ptime now = local_adj::utc_to_local(utc_now);
                    return now - utc_now;
                }
            protected:
                TimingUtil();
                void timeout();
                void getNTPTS(uint64_t& ntp_received_ts,
                              uint64_t& ntp_reansmitted_ts);
            public:
                
                void enableTimestampCalibration();
                void disableTimestampCalibration();
                
                //!Return the current timestamp in milliseconds
                static inline uint64_t getTimeStamp() {
                    try{
                        return (boost::posix_time::microsec_clock::universal_time()- EPOCH + boost::posix_time::milliseconds(timestamp_calibration_offset)).total_milliseconds();
                    } catch(boost::exception_detail::clone_impl< boost::exception_detail::error_info_injector<boost::gregorian::bad_day_of_month> >& bad_day_exce) {
                        TU_LERR << "Bad day exception";
                        return 0;
                    }
                    
                }
                
                //!Return the current timestamp in milliseconds
                static inline uint64_t getTimeStampInMicroseconds() {
                    try{
                        return (boost::posix_time::microsec_clock::universal_time()- EPOCH + boost::posix_time::milliseconds(timestamp_calibration_offset)).total_microseconds();
                    } catch(boost::exception_detail::clone_impl< boost::exception_detail::error_info_injector<boost::gregorian::bad_day_of_month> >& bad_day_exce) {
                        TU_LERR << "Bad day exception";
                        return 0;
                    }
                    
                }
                
                //!Return the current utc timestamp in milliseconds
                static inline uint64_t getLocalTimeStamp() {
                    try{
                        return (boost::posix_time::microsec_clock::local_time() - EPOCH  + boost::posix_time::milliseconds(timestamp_calibration_offset)).total_milliseconds();
                    } catch(boost::exception_detail::clone_impl< boost::exception_detail::error_info_injector<boost::gregorian::bad_day_of_month> >& bad_day_exce) {
                        TU_LERR << "Bad day exception";
                        return 0;
                    }
                }
                
                //!Return the current utc timestamp in milliseconds
                static inline uint64_t getLocalTimeStampInMicroseconds() {
                    try{
                        return (boost::posix_time::microsec_clock::local_time() - EPOCH + boost::posix_time::milliseconds(timestamp_calibration_offset)).total_microseconds();
                    } catch(boost::exception_detail::clone_impl< boost::exception_detail::error_info_injector<boost::gregorian::bad_day_of_month> >& bad_day_exce) {
                        TU_LERR << "Bad day exception";
                        return 0;
                    }
                }
                
                static inline std::locale getLocale(int i){
                    return std::locale(std::locale::classic(), new boost::posix_time::time_input_facet(formats[i]));
                }
                //!chack if a string is well format for date representation
                static bool dateWellFormat(const std::string& timestamp) {
                    boost::posix_time::ptime time;
                    size_t i=0;
                    for(; i<formats_n; ++i) {
                        std::istringstream is(timestamp);
                        std::locale loc=getLocale(i);
                        
                        is.imbue(loc);
                        is >> time;
                        if(time != boost::posix_time::ptime()) break;
                    }
                    return i != formats_n;
                }
                
                //!convert string timestamp to uint64 ["2012-02-20T00:26:39Z"]
                static inline uint64_t getTimestampFromString(const std::string& timestamp,bool isutc=false) {
                    boost::posix_time::ptime time;
                    size_t i=0;
                    for(; i<formats_n; ++i) {
                        std::istringstream is(timestamp);
                        std::locale loc=getLocale(i);
                        is.imbue(loc);
                        is >> time;
                        if(time != boost::posix_time::ptime()) break;
                    }
                    if(i != formats_n) {
                        if(isutc){
                            return (time-EPOCH).total_milliseconds();
                        }
                        return ((time-getUTCOffset())-EPOCH).total_milliseconds();
                    } else {
                        return 0;
                    }
                }
                
                //! return the timestam from now to delay(milliseconds , in the past(false) or future(true)
                static inline uint64_t getTimestampWithDelay(uint64_t delay,
                                                             bool past_future = false) {
                    boost::posix_time::time_duration ts;
                    if(past_future) {
                        ts = boost::posix_time::milliseconds(getTimeStamp() + delay);
                    } else {
                        ts = boost::posix_time::milliseconds(getTimeStamp() - delay);
                    }
                    return ts.total_milliseconds();
                }
                
                //! return the timestam from now to dealy , in the past(false) or future(true)
                static inline std::string toString(uint64_t since_epoc_ms,
                                                   const std::string& format = std::string("%d-%m-%Y %H:%M:%S")) {
                    boost::posix_time::time_facet * facet = new boost::posix_time::time_facet(format.c_str());
                    std::ostringstream stream;
                    stream.imbue(std::locale(stream.getloc(), facet));
                    stream << boost::posix_time::ptime(EPOCH + boost::posix_time::milliseconds(since_epoc_ms));
                    return stream.str();
                }
                //! return the timestam from now to dealy , in the past(false) or future(true)
                static inline std::string toStringFromMicroseconds(uint64_t since_epoc_us,
                                                   const std::string& format = std::string("%d-%m-%Y %H:%M:%S")) {
                    boost::posix_time::time_facet * facet = new boost::posix_time::time_facet(format.c_str());
                    std::ostringstream stream;
                    stream.imbue(std::locale(stream.getloc(), facet));
                    stream << boost::posix_time::ptime(EPOCH + boost::posix_time::microseconds(since_epoc_us));
                    return stream.str();
                }
            };
        }
    }
}
#endif
