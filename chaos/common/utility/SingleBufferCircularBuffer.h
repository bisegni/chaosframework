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

#ifndef CHAOSFramework_SingleBufferCircularBuffer_h
#define CHAOSFramework_SingleBufferCircularBuffer_h
//#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <cstring>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

namespace chaos {
	namespace common {
		namespace utility {
			class UIDataBuffer {
			public:
				virtual int64_t getWriteBufferPosition() = 0;
				
				virtual int64_t getDimension() = 0;
				
				virtual void * const getBasePointer() = 0;
				
				virtual void * const getWritePointer() = 0;
			};
			
			//! SingleBufferCircularBuffer templated chaos circular buffer
			/*!
			 This special circular buffer expose two pointer that can be used to use
			 memcopy to get al data. HIstory data is located form writePOinter unteil the end
			 of buffer given by it's dimension. The most recet data is available from basePointer
			 to bufferDImnation - 1. the last data is the element pointed by bufferDImnation - 1
			 position.
			 */
			template<typename T>
			class SingleBufferCircularBuffer : public UIDataBuffer {
				
				//! the pointer to the memory for the buffer
				int64_t currentPostion;
				int64_t bufferDimension;
				T *basePointer;
				T *writePointer;
			public:
				
				//!Default constructor
				/*!
				 Allocate a new circular buffer with a determinate dimension
				 */
				SingleBufferCircularBuffer(int64_t dim):bufferDimension(dim){
					basePointer = writePointer = (T*)malloc(dim*sizeof(T));
					if(basePointer){
						std::memset(basePointer, 0, dim*sizeof(T));
					}
					currentPostion = 0;
				}
				
				//!Default destructor
				virtual ~SingleBufferCircularBuffer(){
					if(basePointer){
						free(basePointer);
					}
				}
				//!Add a new value to the circular buffer
				/*!
				 \param newValue the value that is insert into the circular buffer
				 */
				void addValue(T newValue) {
					(*writePointer) = newValue;
					currentPostion = (++currentPostion) % bufferDimension;
					writePointer = basePointer + currentPostion;
				}
				//!Retur the write buffer position
				int64_t getWriteBufferPosition(){
					return currentPostion;
				}
				//!Return the circular buffer dimension
				int64_t getDimension(){
					return bufferDimension;
				}
				//!Return the pointer to the base of the buffer
				void * const getBasePointer(){
					return basePointer;
				}
				//!Return the pointer to write position
				void * const getWritePointer(){
					return writePointer;
				}
				
				T getLastValue() {
					return (T)*(basePointer+((currentPostion - 1) % bufferDimension));
				}
			};
			
			//! PointerBuffer is utility class that contain pointer to some data
			/*!
			 This class is used to share some contaier of data poitner that cha be changed. When the
			 pointer is requested with getPtr method, a shared point is given. This permit to manage different allocation
			 of memory in the same container.
			 */
			class PointerBuffer {
				ChaosSharedPtr<char> ptr;
				int32_t bufferDimension;
				boost::mutex mux;
			public:
				//! Default constructor
				PointerBuffer(){
					bufferDimension = 0;
				}
				//! Default destructor
				~PointerBuffer(){
				}
				
				//!Update the data in buffer
				/*!
				 This method update the memory buffer copyng data from the source pointer.
				 If data dimension is different from the one contained int the this instance,
				 the memory is reallcoated to ensure that data will fit
				 */
				void updateData(const char * srcPtr, int32_t srcDataLen) {
					boost::mutex::scoped_lock lock(mux);
					if(srcPtr == NULL || srcDataLen <=0) return;
					if(ptr){
						if(bufferDimension!=srcDataLen){
							ptr.reset((char*)std::malloc(srcDataLen));
						}
					}else{
						ptr.reset((char*)std::malloc(srcDataLen));
					}
					bufferDimension = srcDataLen;
					std::memcpy(ptr.get(), srcPtr, srcDataLen);
				}
				
				//! Return the pointer contained into this instance
				/*!
				 the pointer is a shared ptr, so it can be changed also if someone,
				 is useing  memory allocated into this instance
				 */
				ChaosSharedPtr<char> getPtr(int32_t& bufDim){
					bufDim = bufferDimension;
					return ptr;
				}
				//! Return the typed ptr to the internal buffer
				template<typename T>
				ChaosSharedPtr<T> const getTypedPtr(int32_t& bufDim){
					bufDim = bufferDimension/sizeof(T);
					return boost::static_pointer_cast<T>(ChaosSharedPtr<void>(ptr));
				}
			};
		}
	}
}
#endif
