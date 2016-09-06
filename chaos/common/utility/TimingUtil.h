/*
 *	TimingUtil.h
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
#ifndef ChaosFramework_TimingUtil_h
#define ChaosFramework_TimingUtil_h
#include <chaos/common/global.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#define TU_HEADER "[TimingUtil]- "
#define TU_LAPP LAPP_ << TU_HEADER
#define TU_LDBG LDBG_ << TU_HEADER << __PRETTY_FUNCTION__
#define TU_LERR LERR_ << TU_HEADER << __PRETTY_FUNCTION__ <<"("<<__LINE__<<") "


namespace chaos {
    namespace common {
        namespace utility {

            /*
             Class for give some method util for timing purpose
             */
            class TimingUtil {
                static const char* formats[];
                static const size_t formats_n;
            public:
                
                //!Return the current timestamp in milliseconds
                static inline uint64_t getTimeStamp() {
                    try{
                        return (boost::posix_time::microsec_clock::universal_time()-EPOCH).total_milliseconds();
                    } catch(boost::exception_detail::clone_impl< boost::exception_detail::error_info_injector<boost::gregorian::bad_day_of_month> >& bad_day_exce) {
                        TU_LERR << "Bad day exception";
                        return 0;
                    }
                    
                }
                
                //!Return the current timestamp in milliseconds
                static inline uint64_t getTimeStampInMicrosends() {
                    try{
                        return (boost::posix_time::microsec_clock::universal_time()-EPOCH).total_microseconds();
                    } catch(boost::exception_detail::clone_impl< boost::exception_detail::error_info_injector<boost::gregorian::bad_day_of_month> >& bad_day_exce) {
                        TU_LERR << "Bad day exception";
                        return 0;
                    }
                    
                }
                
                //!Return the current utc timestamp in milliseconds
                static inline uint64_t getLocalTimeStamp() {
                    try{
                        return (boost::posix_time::microsec_clock::local_time()-EPOCH).total_milliseconds();
                    } catch(boost::exception_detail::clone_impl< boost::exception_detail::error_info_injector<boost::gregorian::bad_day_of_month> >& bad_day_exce) {
                        TU_LERR << "Bad day exception";
                        return 0;
                    }
                }
                
                //!Return the current utc timestamp in milliseconds
                static inline uint64_t getLocalTimeStampInMicroseconds() {
                    try{
                        return (boost::posix_time::microsec_clock::local_time()-EPOCH).total_microseconds();
                    } catch(boost::exception_detail::clone_impl< boost::exception_detail::error_info_injector<boost::gregorian::bad_day_of_month> >& bad_day_exce) {
                        TU_LERR << "Bad day exception";
                        return 0;
                    }
                }
                
                static inline std::locale getLocale(int i){
                    boost::posix_time::time_input_facet* tf=new boost::posix_time::time_input_facet();
                    tf->format(formats[i]);
                    std::locale loc(std::locale::classic(),tf);
                    return loc;
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
                static inline uint64_t getTimestampFromString(const std::string& timestamp) {
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
                        return (time-EPOCH).total_milliseconds();
                    } else {
                        return 0;
                    }
                }
                
                //! return the timestam from now to dealy , in the past(false) or future(true)
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
            };
            const char * TimingUtil::formats[] = {
                "%Y-%m-%dT%H:%M:%S.%f",
                "%Y-%m-%d %H:%M:%S.%f",
                "%Y-%m-%d %H:%M:%S",
                "%Y-%m-%dT%H:%M:%S",
                "%Y-%m-%d %H:%M",
                "%Y-%m-%dT%H:%M",
                "%Y-%m-%d %H",
                "%Y-%m-%dT%H",
                "%Y-%m-%d",
                "%Y-%m",
                "%Y"};
            const size_t TimingUtil::formats_n = 11;
        }
    }
}
#endif
