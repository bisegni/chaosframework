/*	
 *	MultiBufferDataStorage.h
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

#ifndef MultiBufferDataStorage_H
#define MultiBufferDataStorage_H

#include <chaos/common/general/Configurable.h>
#include <chaos/common/io/IODataDriver.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/pqueue/ChaosProcessingQueue.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/cu_toolkit/DataManager/DataBuffer/OutputDataBuffer.h>



namespace chaos {
    using namespace std;
    
#define DEFAULT_OBUFFER_THREAD_NUMEBR   1
#define TAG_BUFFER_LIVE                 1
#define TAG_BUFFER_HST                  2
    
    /*
     THis class implement the use of two OutputDataBuffer, to permit the CU
     to send data to the live and the history database
     */
    class MultiBufferDataStorage : public CObjectProcessingQueueListener<CDataWrapper>, public Configurable {
        int32_t liveThreadNumber;
            //live data offset in msec to waith 
            //according to pack timestamp
            //for send data pack to the buffer
        int32_t liveMsecOffset;
        
            //last timestamp sent to the buffer
        int32_t liveLastMsecSent;
   
            //live data offset in msec to waith 
            //according to pack timestamp
            //for send data pack to the buffer
        int32_t historyUSecOffset;
        
            //last timestamp sent to the buffer
        int64_t historyLastMsecSent;
    protected:
            //live data output buffer
        OutputDataBuffer *liveOBuffer;
        
            //history data output buffer
        OutputDataBuffer *hstOBuffer;
            
            //timining utility class
        TimingUtil *timingUtil;
    public:
        
        /*
         Constructor
         */
        MultiBufferDataStorage();
        
        /*
         Destructor
         */
        ~MultiBufferDataStorage();
        
        /*
         Initialize the Multisequence buffer
         */
        virtual void init(CDataWrapper*);
        
        /*
         Deinitialize the Multisequence buffer
         */
        virtual void deinit();
        /*
         OBuffer listener
         */
        virtual bool elementWillBeProcessed(int bufferTag, CDataWrapper*);        
        /*
          OBuffer listener
         */
        virtual bool elementWillBeDiscarded(int, CDataWrapper*);
         /*
          Push raw data in to buffer
         */
        virtual void pushDataSet(CDataWrapper*);
        
        /*
         Set the first buffer
         */
        void setLiveOBuffer(OutputDataBuffer*);
        
        /*
         Set the first buffer
         */
        void setHstOBuffer(OutputDataBuffer*); 
        
        /*
         Permit to be live configurable
         */
        virtual CDataWrapper* updateConfiguration(CDataWrapper*);
        
        /*
         Return the IODataDriver pointer for the input buffer tag
         The pointer MUST NOT BE DEALOCATED it 's managed
         internale to the buffer
         */
        IODataDriver* getBufferIODataDriver(int);
    };
}
#endif
