/*
 *	AggregateDeviceTracker.h
 *	!CHOAS
 *	Created by Flaminio Antonucci.
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

#ifndef CachingSystem_AgregateDeviceTracker_h
#define CachingSystem_AgregateDeviceTracker_h

#include <chaos/common/caching_system/caching_thread/tracker_interface/DataAggregationInterface.h>
//#include <DataAggregationInterface.h"
#include <chaos/common/caching_system/common_buffer/helper/MillisTime.h>
//#include "MillisTime.h"
namespace chaos {

namespace caching_system {
    namespace caching_thread{
        //template <typename T>
        //template<typename T> class DataFetcherInterface;
        //template<class T,class T2> class DataAggregationFilterInterface;

        
        template <typename T,typename U>
        
        class AggregateDeviceTracker : public AbstractDeviceTracker<U,T>{
            
        private:
            uint64_t bufferLenght;
            
            DataAggregationFilterInterface<T,U>* dataAggregation;
            
            
            std::vector<DataElement<T>*>*  buffer;
            
            
            void clearBuffer(){
            
                for (int i=0; i<buffer->size(); i++) {
                    
                    DataElement<T>* temp=buffer->at(i);
                    delete temp;
                }
                
                buffer->clear();
            }

        public:
            
            
            AggregateDeviceTracker(caching_system::DataAggregationFilterInterface<T,U>* aggregator,
                                   caching_system::DataFetcherInterface<T>* fetcher,
                                   uint64_t hertz,uint64_t validity,std::string dev_id,
                                   uint64_t bufferLenght): AbstractDeviceTracker<U,T>( fetcher, hertz,validity,dev_id){
                
                this->bufferLenght=bufferLenght;
                buffer=new std::vector<DataElement<T>* >();
                dataAggregation=aggregator;
            
            }
            
            //ConcreteDeviceTracker(caching_system::DataFetcherInterface<T>* fetcher): AbstractDeviceTracker<T>(fetcher){}
            
            void doTracking(DataElement<T>* newElement){
                
                //std::cout<<"Arrivato\n";
                this->buffer->push_back(newElement);
                if(this->buffer->size()==bufferLenght){
                    //std::cout<<"svuoto\n";

                    U* newToAdd= this->dataAggregation->aggregateData(this->buffer);
                    uint64_t timestamp= my_time::getMillisTimestamp();
                    DataElement<U>* modifiedData=new DataElement<U>(newToAdd,timestamp);
                    
                    SmartPointer<U> * mySmartData=this->highResQueue->enqueue(modifiedData,true);
                    
                    this->insertInSubBuffers(mySmartData,modifiedData);
            
                    delete mySmartData;
                    clearBuffer();
                    //svuota il buffer

                
                }
                
                
            }
            
            
            
            
            
        };
        
    }
}
}


#endif
