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

#ifndef ChaosFramework_ArrayPointer_h
#define ChaosFramework_ArrayPointer_h

#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>
namespace chaos {
	namespace common {
		namespace utility {
			/*
			 This class represent a groupo or list of a CDataWrapper
			 Class
			 */
			template <typename T>
			class ArrayPointer{
			  std::vector<T*> pointerArray;
			public:
				
			  typedef typename std::vector<T*>::iterator ArrayPointerIterator;
				
				/*
				 Destructor
				 */
				~ArrayPointer() {
					clear();
				}
				
				/*
				 Add a new object pointer
				 */
				void add(T *objPtr){
					pointerArray.push_back(objPtr);
				}
				
				
				/*
				 Clear all the array with contained pointer
				 */
				void clear(){
					//we need to delete all remainig element
				  for (typename std::vector<T*>::iterator tIter = pointerArray.begin();
						 tIter != pointerArray.end();
						 tIter++) {
						delete *tIter;
					}
				}
				
				/*
				 return poitner with [] operatore
				 */
				T* operator[](int i) { return pointerArray[i]; }
				
				/*
				 return the number of objet pointer managed
				 */
				typename std::vector<T*>::size_type size() const { return pointerArray.size(); };
				
				ArrayPointerIterator begin(){ return pointerArray.begin(); }
				
				ArrayPointerIterator end(){ return pointerArray.end(); }
			};
		}
	}
}
#endif
