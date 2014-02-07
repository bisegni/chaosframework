/*
 *	FastSlotArray.h
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
#ifndef __CHAOSFramework__FastSlotArray__
#define __CHAOSFramework__FastSlotArray__

#include <chaos/common/global.h>
#include <chaos/common/utility/InizializableService.h>

#include <boost/lockfree/queue.hpp>

namespace chaos {
	namespace common {
		namespace utility {
			
#define FastSlotArray_LOG_HEAD "[FastSlotArray] - "
			
#define FSA_LAPP_ LAPP_ << FastSlotArray_LOG_HEAD
#define FSA_LDBG_ LDBG_ << FastSlotArray_LOG_HEAD
#define FSA_LERR_ LERR_ << FastSlotArray_LOG_HEAD
			
			template<typename T>
			struct SlotElement{
				unsigned int slot_index;
				T * slot_element;
			};
			
			template<typename T>
			class FastSlotArray : public chaos::utility::InizializableService {
				//! available endpoint slotc
				SlotElement<T> * * slot_array;
				
				const unsigned int max_slot;
				
				//!available index queue
				boost::lockfree::queue<unsigned int> available_slot;
			public:
				
				FastSlotArray(unsigned int _max_slot):max_slot(_max_slot) {}
				
				// Initialize instance
				void init(void *init_data) throw(chaos::CException) {
					//allocate memory for the endpoint array
					FSA_LAPP_<< "Allocating all memory for endpoint slot array";
					max_slot = (SlotElement<T>**)malloc(max_slot);
					if(!max_slot)  throw chaos::CException(-1, "Error allocating memory for slot", __FUNCTION__);
					
					FSA_LAPP_ << "Allocating all slot";
					//allocate all endpoint slot
					for (int idx = 0; idx < max_slot; idx++) {
						max_slot[idx] = new SlotElement<T>();
						max_slot[idx]->slot_index = idx;
						//add this endpoint to free slot queue
						available_slot.push(idx);
					}
				}
					
					
				// Deinit the implementation
					void deinit() throw(chaos::CException) {
						if(max_slot) {
							FSA_LAPP_ << "Deallocating all endpoint slot";
							//allocate all endpoint slot
							for (int idx = 0; idx < max_slot; idx++) {
								//delete andpoint slot
								delete max_slot[idx];
							}
							FSA_LAPP_ << "Free slot array memory";
							free(max_slot);
						}
					}
					
					//allocate new endpoint
					SlotElement<T> *getAvailableSlot() {
						unsigned int next_available_slot = -1;
						if(!available_slot.pop(next_available_slot)) return NULL;
						
						max_slot[next_available_slot]->slot_element = new T();
						if(!max_slot[next_available_slot]->slot_element) {
							available_slot.push(next_available_slot);
							return NULL;
						}
						return max_slot[next_available_slot];
					}
					
					//! relase an endpoint
					void releaseSlot(SlotElement<T> *slot_to_release) {
						if(!slot_to_release) return;
						// get slot index
						unsigned int slot_idx = slot_to_release->slot_index;
						
						//delete endpoint
						if(slot_to_release->slot_element)
							delete(slot_to_release->slot_element);
						
						//reuse the index
						available_slot.push(slot_idx);
					}
					
			};
		}
	}
}
					
#endif /* defined(__CHAOSFramework__FastSlotArray__) */
