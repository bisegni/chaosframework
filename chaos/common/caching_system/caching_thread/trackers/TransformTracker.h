//
//  TranformTracker.h
//  CHAOSFramework
//
//  Created by Flaminio Antonucci on 07/01/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef CHAOSFramework_TranformTracker_h
#define CHAOSFramework_TranformTracker_h
#include <chaos/common/caching_system/caching_thread/tracker_interface/DataFetcherInterface.h>

namespace chaos {
    
    namespace caching_system {
        namespace caching_thread{
            template<typename T> class DataFetcherInterface;
            template <class T, class U>
            class AbstractDeviceTracker;
            
            template <typename T, typename D>
            
            class TransformTracker : public TrackerListener<T>, public AbstractDeviceTracker<D,D>
            {
                friend class AbstractDeviceTracker<T>;
                
                
            private:
                EmbeddedDataTransform<T,D>* filter;
                TransformTracker(
                                 EmbeddedDataTransform<T,D>* filter,
                                 uint64_t hertz,
                                 uint64_t validity): TrackerListener<T>(),AbstractDeviceTracker<D, D>( hertz,validity){
                    
                    
                    this->filter=filter;
                }
                ~TransformTracker(){
                    std::cout<<"chiamo distrutture transform\n";
                    this->shutDownTracking();
                
                }
                
            public:
                
                              
              
                
                void addedNewelement(SmartPointer<T>* n,uint64_t timeStamp) {
                    //calculate new value with passed filter
                    
                    D* newData=this->filter->trasformData(n->getData());
                    
                    
                    //create a new data element
                    DataElement<D>*  filteredData=new DataElement<D>(newData,timeStamp);

                    
                    
                    //then, add it to the tracker
                    boost::mutex::scoped_lock  lock(*this->bufferMapMutex);
                    
                    doTracking(filteredData);
                    
                }
                
            };
            
        }
    }
}


#include <chaos/common/caching_system/caching_thread/trackers/AbstractDeviceTracker.h>



#endif
