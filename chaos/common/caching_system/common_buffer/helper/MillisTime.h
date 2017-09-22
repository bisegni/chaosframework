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

#ifndef CachingSystem_MillisTime_h
#define CachingSystem_MillisTime_h
#include <sys/time.h>

namespace chaos {

namespace my_time {
    
    long inline getMillisTimestamp(){
        
        struct timeval start;
        
        long mtime, seconds, useconds;
        
        gettimeofday(&start, NULL);
      
        
        seconds  = start.tv_sec;
        useconds = start.tv_usec;
        
        mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
        
        return mtime;
        
        
        
    }


}
    
}

#endif
