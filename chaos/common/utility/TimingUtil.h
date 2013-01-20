/*	
 *	TimingUtil.h
 *	!CHOAS
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

namespace chaos {
    using namespace boost::posix_time;
    
    /*
     Class for give some method util for timing purpose
     */
    class TimingUtil {
    public:
        /*
         Return the current timestamp
         */
        inline int64_t getTimeStamp() {
            return (microsec_clock::universal_time()-EPOCH).total_milliseconds();
        }
    };
}
#endif
