/*
 *	TransformDeviceTracker.h
 *	!CHOAS
 *	Created by Antonucci Flaminio.
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

#ifndef CachingSystem_TransformDeviceTracker_h
#define CachingSystem_TransformDeviceTracker_h
#include "chaos/common/caching_system/caching_thread/trackers/AbstractDeviceTracker.h"
#include "chaos/common/caching_system/caching_thread/tracker_interface/DataTransformFilterInterface.h"

namespace chaos {
    
    
    namespace caching_system {
        namespace caching_thread{
            //template <typename T>
            template<typename T> class DataFetcherInterface;
            // template<typename T> class DataTransformFilterInterface;
            
            template <typename T>
            class TransformDeviceTracker : public AbstractDeviceTracker<T> {
            private:
                DataTransformFilterInterface<T>* dataTransform;
                
            public:
                
                TransformDeviceTracker(caching_system::DataTransformFilterInterface<T>* dataTransform,caching_system::DataFetcherInterface<T>* fetcher,uint64_t hertz,uint64_t validity): AbstractDeviceTracker<T>( fetcher, hertz,validity){
                    this->dataTransform=dataTransform;
                }
                
                //ConcreteDeviceTracker(caching_system::DataFetcherInterface<T>* fetcher): AbstractDeviceTracker<T>(fetcher){}
                
                void doTracking(DataElement<T>* newElement){
                    
                    dataTransform->trasformData(newElement);
                    
                    
                    SmartPointer<T>* mySmartData=this->highResQueue->enqueue(newElement,true);
                    
                    
                    this->insertInSubBuffers(mySmartData,newElement);
                    
                    
                    
                    delete mySmartData;
                    
                    
                    
                }
                
                
                
                
                
                
            };
            
        }
    }
}

#endif
