/*	
 *	OutputDataBuffer.h
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
#ifndef OutputDataBuffer_H
#define OutputDataBuffer_H
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <chaos/common/io/IODataDriver.h>
#include <chaos/common/general/Configurable.h>
#include <chaos/common/data/CDataWrapper.h> 
#include <chaos/common/pqueue/ChaosProcessingQueue.h>

namespace chaos {
    
    /*
     Output buffer for live data
     */
    class OutputDataBuffer : public CObjectProcessingQueue<CDataWrapper>, public Configurable {
        IODataDriver *ioDriver;
        
    protected:
        
        /*
         Process the oldest element in buffer
         */
        virtual void processBufferElement(CDataWrapper*, ElementManagingPolicy&) throw(CException);
    
    public:
        OutputDataBuffer();
        OutputDataBuffer(IODataDriver*);
        virtual ~OutputDataBuffer();
        
        /*
         Initialization method for output buffer
         */
        void init(int) throw(CException);

        /*
         Deinitialization method for output buffer
         */
        void deinit() throw(CException);

        /*
         Set the live io driver
         */
        void setIODriver(IODataDriver*);
        
        /*
         Return the internat instance of datadriver
         with shared_ptr
         */
        IODataDriver *getIODataDriver();
        
        /*
         Update the configuration
         */
        CDataWrapper* updateConfiguration(CDataWrapper*);
    };
    
}
#endif
