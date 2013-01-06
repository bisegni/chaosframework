//
//  CombinedTrackers.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 1/6/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef CHAOSFramework_CombinedTrackers_h
#define CHAOSFramework_CombinedTrackers_h

#include <vector>
#include <chaos/common/caching_system/caching_thread/trackers/AbstractDeviceTracker.h>

namespace chaos {
    
    namespace caching_system {
        namespace caching_thread{
            
            template<typename T>
            class DataFetcherInterface;
            
            template <typename T>
            class CombinedTrackers : public AbstractDeviceTracker<T> {
                
                std::vector< AbstractDeviceTracker<T> *> childTrackers;
                
            public:
                
                CombinedTrackers(caching_system::DataFetcherInterface<T>* fetcher,uint64_t hertz,uint64_t validity): AbstractDeviceTracker<T>( fetcher, hertz,validity){}
                
                virtual ~CombinedTrackers(){
                    for (typename std::vector< AbstractDeviceTracker<T>* >::iterator iter = childTrackers.begin(); iter != childTrackers.end(); iter++) {
                        delete(*iter);
                    }
                }
                
                void doTracking(DataElement<T>* newElement){
                    SmartPointer<T>* mySmartData=this->highResQueue->enqueue(newElement,true);
                    this->insertInSubBuffers(mySmartData,newElement);
                    
                    //add data to child
                    for (typename  std::vector< AbstractDeviceTracker<T> * >::iterator iter = childTrackers.begin(); iter != childTrackers.end(); iter++) {
                        (*iter)->doTracking(newElement);
                    }
                    
                    delete mySmartData;
                }
                
                void addTracker(AbstractDeviceTracker<T> *newTrackerToAdd) {
                    childTrackers.push_back(newTrackerToAdd);
                }
            };
            
        }
    }
}

#endif
