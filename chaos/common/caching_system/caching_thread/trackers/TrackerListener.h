//
//  TrackerListener.h
//  CHAOSFramework
//
//  Created by Flaminio Antonucci on 07/01/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef CHAOSFramework_TrackerListener_h
#define CHAOSFramework_TrackerListener_h

namespace chaos {
    
    namespace caching_system {
        namespace caching_thread{
            
           
            template <class T>
            class TrackerListener {
            public:
                
                TrackerListener(){}
               virtual void addedNewelement(SmartPointer<T>* n,uint64_t timeStamp) = 0;
            };
            
            
        }
    }
}
#endif
