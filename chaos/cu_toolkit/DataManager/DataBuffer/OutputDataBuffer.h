    //
    //  OutputDataBuffer.h
    //  ChaosFramework
    //
    //  Created by Claudio Bisegni on 17/03/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

#ifndef OutputDataBuffer_H
#define OutputDataBuffer_H
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
        ~OutputDataBuffer();
        
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
