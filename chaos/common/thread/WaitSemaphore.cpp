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

#include "WaitSemaphore.h"

namespace chaos {
    
   
      
    void WaitSemaphore::wait() {
           
            if(inWait) return;
            inWait = true;
            while((!answered)){ boost::unique_lock<boost::mutex> lock( wait_answer_mutex );wait_answer_condition.wait(lock);};
            inWait = false;
            answered = false;
        }
    void WaitSemaphore::waitRaw() {
           
            boost::unique_lock<boost::mutex> lock( wait_answer_mutex );
            wait_answer_condition.wait(lock);
        }
       
    void WaitSemaphore::wait(unsigned long millisecToWait) {
            boost::unique_lock<boost::mutex> lock( wait_answer_mutex );
            if(inWait) return;
            inWait = true;
            do {} while(wait_answer_condition.timed_wait(lock, posix_time::milliseconds(millisecToWait)) && !answered);
            inWait = false;
            answered = false;
        }
        
	
    void WaitSemaphore::waitUSec(uint64_t microsecondsToWait) {
            boost::unique_lock<boost::mutex> lock( wait_answer_mutex );
            if(inWait) return;
            inWait = true;
            do {} while(wait_answer_condition.timed_wait(lock, posix_time::microseconds(microsecondsToWait)) && !answered);
            inWait = false;
            answered = false;
        }
		
       
}
