/*
 *	Element.h
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

#ifndef CachingSystem_Element_h
#define CachingSystem_Element_h
#include <iostream>
#include <boost/interprocess/detail/atomic.hpp>
#include <chaos/common/caching_system/common_buffer/helper/MillisTime.h>
#include <chaos/common/caching_system/common_buffer/IteratorBase.h>
#include <chaos/common/caching_system/common_buffer/CommonBuffer.h>
namespace chaos {
    
    namespace caching_system {
        
        template <typename T>
        
        /*!
         * This class represents a node of the queue.
         */
        class Element{
            
            template <typename TT> friend class CommonBuffer;
            template <typename TT> friend class IteratorBase;
            template <typename TT> friend class IteratorReader;
            template <typename TT> friend class IteratorGarbage;
            
            template <typename TT> friend class SmartPointer;
            template <typename TT> friend class SmartElement;
            
            
        private:
            int elementNumber;
            
            Element<T>* next;//!< pointer to next element in queue.
            volatile boost::uint32_t readerCount;//!< counter of acutal reading threads.
            volatile boost::uint32_t referenceCount;//!< counter of references to this element.
        protected:
            T* data;//!<content of element.
            uint64_t timedOut;
            
            /*!
             * Increase readers count.
             */
            void inline increment(){
                boost::interprocess::ipcdetail::atomic_inc32(&readerCount);
                
            }
            
            /*!
             * decrease readers count.
             */
            void inline decrementCounter(){
                boost::interprocess::ipcdetail::atomic_dec32(&readerCount);
                
            }
            
            /*!
             * Increase references count
             */
            void inline incrementReferenceCount(){
                
                boost::interprocess::ipcdetail::atomic_inc32(&referenceCount);
                
            }
            
            /*!
             * Decrease references count
             */
            void inline decrementReferenceCount(){
                
                boost::interprocess::ipcdetail::atomic_dec32(&referenceCount);
                
            }
            
            
        public:
            
            
            /*!
             * It constructs an empty element and set readers count and references count to zero
             */
            Element(){
                next=NULL;
                readerCount=0;
                referenceCount=0;
                data=NULL;
                timedOut=0;
                
            }
            
            /*!
             * Destroy this element
             */
            ~Element(){
                delete data;
                
            }
            
            /*!
             * Set data content in current element
             */
            void setData(T* data,uint64_t timeout){
                
                this->data=data;
                this->timedOut=timeout;
                
            }
            
            /*!
             * Returns a pointer to data content of current element
             */
            T* getData(){
                
                return data->getData();
                
            }
            
            
            /*!
             * It returns true if an element is out-of-date.
             */
            bool inline isValid(){
                long actualTime=my_time::getMillisTimestamp();
                
                if(data!=NULL && (timedOut<actualTime)){
                    /*    if(data!=NULL){
                     std::cout<<"tempo attuale: "<<actualTime<<"\n";
                     std::cout<<"timestamp: "<<this->data->getTimestamp()<<"\n";
                     std::cout<<"timeout: "<<this->data->getTimeout()<<"\n";
                     }*/
                    return false;
                    
                }else{
                    
                    return true;
                }
            }
            
            
        };
        
    }
}
#endif
