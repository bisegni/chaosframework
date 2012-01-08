    //
    //  TimingUtil.h
    //  ChaosFramework
    //
    //  Created by Bisegni Claudio on 30/07/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

#ifndef ChaosFramework_TimingUtil_h
#define ChaosFramework_TimingUtil_h

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
