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

#ifndef CHAOSFramework_ObjectSlot_h
#define CHAOSFramework_ObjectSlot_h

#include <string.h>
#include <stdint.h>
#include <boost/atomic.hpp>

namespace chaos {
	namespace common {
		namespace utility {
			
#define OBJECT_SLOT_MAX_AVAILABLE_SLOT	1000
			
			template <typename T>
			class ObjectSlot {
				
				T						*slot_array;
				const uint32_t			slot_dimension;
				boost::atomic<uint32_t> current_available_slot;
				boost::atomic<uint32_t> using_slot;
                
				void initMemory() {
                    if(slot_array) free(slot_array);
					slot_array = (T*)malloc(sizeof(T)*slot_dimension);
					clearSlots();
				}
			public:
				ObjectSlot():slot_array(NULL), slot_dimension(OBJECT_SLOT_MAX_AVAILABLE_SLOT) {
					initMemory();
				};
				
				ObjectSlot(uint32_t dimension):slot_dimension(dimension) {
					initMemory();
				};
				
				~ObjectSlot() {
					if(slot_array) free(slot_array);
				};
				
				const uint32_t getNumberOfSlot() {
					return current_available_slot;
				}
				
				T accessSlotByIndex(uint32_t index) {
					return slot_array[index];
				}
				
				void clearSlots() {
					using_slot = 0;
					current_available_slot = 0;
					std::memset(slot_array, 0, sizeof(T)*slot_dimension);
				}
				
				inline T accessSlot() {
					uint32_t slot_to_use;
					return accessSlot(slot_to_use);
				}
				
				inline T accessSlot(uint32_t& return_index) {
					return_index = using_slot++ % current_available_slot;
					return slot_array[return_index];
				}
				
				bool addSlot(T element) {
					uint32_t new_slot_index = current_available_slot++;
					if(new_slot_index >= slot_dimension) return false;
					
					slot_array[new_slot_index] = element;
					return true;
				}
			};
		}
	}
}

#endif
