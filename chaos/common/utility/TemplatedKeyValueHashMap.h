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
#ifndef __CHAOSFramework__TemplatedHash__
#define __CHAOSFramework__TemplatedHash__

#include <chaos/common/data/cache/FastHash.h>

#include <boost/thread.hpp>

namespace chaos {
	namespace common {
		namespace utility {
			
			template<typename T>
			struct KeyValueHashElement {
				//! hashed element
				T element;
				
				//! key valueof the lement
				void *key;
				
				//! key length of the value
				uint32_t key_len;
				
				//! Next element having the same hash value
				KeyValueHashElement *next;
				
				//! Previous element having the same hash value
				KeyValueHashElement *prev;
			};
			
			//! list head for every hash slot
			template<typename T>
			struct HashHeadElementList {
				//!list mutext
				boost::mutex	mutex_list;
				
				//! list point to the head element
				KeyValueHashElement<T> *head;
			};
			
#define hashsize(n) ((uint32_t)1<<(n))
#define hashmask(n) (hashsize(n)-1)
			/*!
			 Templated fast key value hash container, made on the desing of
			 the caching model adopted by Memcached. The collision are managed
			 by a bidirectional list of element having same hash. This is a revised
			 hashing model found on memcached source code
			 */
			template<typename T,
			typename H = chaos::common::data::cache::FastHash>
			class TemplatedKeyValueHashMap {
				//symplify the code read
				
				//! List head for hash slot
				typedef HashHeadElementList<T> HashList;
				
				//! hashed struct for encapsulate the element
				typedef KeyValueHashElement<T> HashedStruct;
				
				uint32_t		hash_hashpower;
				uint32_t		hash_mask;
				HashList		**hash_vector;
				
				//! flag for memory the allcoation state of the memory
				bool			allocation_success;
				
				boost::mutex	mutex_hash;
                
				//! get the has for the key
				inline uint32_t getHashForKey(const void * key, const uint32_t key_len) {
					return H::hash(key, key_len, 0);
				}
				
				//! return the headl of the list identified by the has value
				/*!
				 The head is create dinamically when is used for the first time
				 */
				inline HashList *getListByHash(uint32_t hash_value) {
					HashList *_list_head = hash_vector[hash_value & hash_mask];
					if(!_list_head) {
                            hash_vector[hash_value & hash_mask] = _list_head = new HashList();
                            _list_head->head = NULL;
					}
					return _list_head;
				}
				
				//! memory allocation for the vector hash
				int _allocateHashMemory() {
					//get real hash vector dimension
					uint64_t hash_size = hashsize(hash_hashpower);
					
					//allcoate hash vector memory
					hash_vector = (HashList**)calloc(hash_size, sizeof(HashList**));
					
					//in case zero all memory
					if(hash_vector) std::memset(hash_vector, 0, hash_size*sizeof(HashList**));
					else return -1;
					
					//flag success on allcoation
					allocation_success = true;
					
					return 0;
				}
				
				//! relase all hash structures memory
				void _releaseHashMemory() {
					if(hash_vector) {
						for(int idx = 0; idx < hashsize(hash_hashpower); idx++) {
							if(hash_vector[idx]) delete(hash_vector[idx]);
						}
						free(hash_vector);
					}
				}
				
			protected:
				//! template element deallocation user method (to override)
				virtual void clearHashTableElement(const void *key, uint32_t key_len, T element){};
				
			public:
				//! default contructor
				TemplatedKeyValueHashMap():
				hash_hashpower(16),
				hash_mask(hashmask(hash_hashpower)),
				hash_vector(NULL),
				allocation_success(false)  {
					_allocateHashMemory();
				}
				
				//! parametrized
				TemplatedKeyValueHashMap(uint32_t _hash_power):
				hash_hashpower(_hash_power),
				hash_mask(hashmask(hash_hashpower)),
				hash_vector(NULL),
				allocation_success(false) {
					_allocateHashMemory();
				}
				
				//! default destructor
				~TemplatedKeyValueHashMap() {
					if(hash_vector) {
						//clear all element
						clear();
						
						//free hash vector
						_releaseHashMemory();
					}
				}
				
				//! check if hashtable memory is valid
				bool isValid(){
					return hash_vector!=NULL;
				}
				
				/*!
				 Get element by his key
				 */
				int getElement(const void * key, const uint32_t key_len, T *element_ptr) {
                    boost::unique_lock<boost::mutex>  wl(mutex_hash);
					int err = -3;
					//get the list for the has value
					HashList *_list_head = getListByHash(getHashForKey(key, key_len));
					
					// che if the key has an attacched list
                    if(!_list_head) {
                        return -1;
                    }
                    
					//lock the list in read
					//boost::shared_lock<boost::mutex> readLock(_list_head->mutex_list);
                    if(!_list_head->head) {
                        return -2;
                    }
					//appo var for cicling element
					HashedStruct *_cur_hash_element = _list_head->head;
					
					//search need element with key
					while (_cur_hash_element) {
						//!compare the length of the key and the memory
						if ((key_len == _cur_hash_element->key_len) &&
							(memcmp(key, _cur_hash_element->key, key_len) == 0)) {
							*element_ptr = _cur_hash_element->element;
							err = 0;
							break;
						}
						_cur_hash_element = _cur_hash_element->next;
					}
					return err;
				}
				
				/*!
				 Add element by key value
				 */
				int addElement(const void *key, const uint32_t key_len, T element) {
					T *element_ptr = NULL;
					//we can't add two times the same key to hash
					if(!getElement(key, key_len, element_ptr)) {
						//another element with the same key is preset
						return -1;
					}
                    boost::unique_lock<boost::mutex>  wl(mutex_hash);
					//get the list for the has value
					HashList *_list_head = getListByHash(getHashForKey(key, key_len));
					
					// check if we have got some trouble to allcoate retrieve head list
					if(!_list_head) return -1;
					
					//get the write lock
					boost::unique_lock<boost::mutex> writeLock(_list_head->mutex_list);
					
					//allocate new struct for the element
					HashedStruct *new_element = new HashedStruct();
					
					//check if we have got the memory
					if(!new_element) return -2;
					
					//clear all field of the element
					std::memset(new_element, 0, sizeof(HashedStruct));
					
					//associate the element and key
					new_element->element = element;
					new_element->key = malloc(key_len);
					std::memcpy(new_element->key, key, (new_element->key_len = key_len));
					
					//get last slot with same hash
					HashedStruct *_cur_hash_element = _list_head->head;
					if(_cur_hash_element) {
						//incremente the list of collided element
						_cur_hash_element->prev = new_element;
						new_element->next = _cur_hash_element;
					}
					//add new elemento as first element of collide hash's slot
					_list_head->head = new_element;
					
					//return success
					return 0;
				}
				
				/*!
				 Remove the element identified by key
				 */
				void removeElement(const void *key, const uint32_t key_len) {
                    boost::unique_lock<boost::mutex>  wl(mutex_hash);
					//get the list for the has value
					HashList *_list_head = getListByHash(getHashForKey(key, key_len));
					
					//! if the key has no list we return
					if(!_list_head) return;
					
					//get the write lock
					//boost::unique_lock<boost::shared_mutex> writeLock(_list_head->mutex_list);
					
					HashedStruct *element_to_delete = NULL;
					HashedStruct *_cur_hash_element = _list_head->head;
					while (_cur_hash_element) {
						if ((key_len == _cur_hash_element->key_len) &&
                            (memcmp(key, _cur_hash_element->key, key_len) == 0)) {
							element_to_delete = _cur_hash_element;
							if(_cur_hash_element->prev &&
                               _cur_hash_element->next) {
								_cur_hash_element->next->prev = _cur_hash_element->prev;
							} else {
								//is the first of the list so set the next as first
								_list_head->head = _cur_hash_element->next;
							}
							//call user overloaded (possible) clear element
							clearHashTableElement(element_to_delete->key, element_to_delete->key_len, element_to_delete->element);
							//free key memory and slot
							free(element_to_delete->key);
							delete(element_to_delete);
							break;
						} else {
							//go ahead
							_cur_hash_element = _cur_hash_element->next;
						}
					}
				}
				
				//!clear all hash table
				void clear() {
                    boost::unique_lock<boost::mutex>  wl(mutex_hash);
					//clear all element(not the hash vector)
					for(int idx = 0; idx < hashsize(hash_hashpower); idx++) {
						
						//chec if the current slot as an attacche list
						if(!hash_vector[idx]) continue;
						
						HashedStruct *to_delete_slot = NULL;
						HashedStruct *cur_slot = hash_vector[idx]->head;
						
						//achieve a write lock for access list
						//boost::unique_lock<boost::shared_mutex> readLock(hash_vector[idx]->mutex_list);
						
						while(cur_slot) {
							//get reference to slot that need to be deleted
							to_delete_slot = cur_slot;
							
							//go ahead in list
							cur_slot = cur_slot->next;
							
							//call user overloaded (possible) clear element
							clearHashTableElement(to_delete_slot->key, to_delete_slot->key_len, to_delete_slot->element);
							
							//free key memory and slot
							free(to_delete_slot->key);
							delete(to_delete_slot);
							
							//go ahead
						}
						
						//all element has been deleted
						hash_vector[idx]->head = NULL;
					}
				}
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__TemplatedHash__) */
