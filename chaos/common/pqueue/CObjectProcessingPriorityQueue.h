/*	
 *	CObjectProcessingPriorityQueue.h
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
#ifndef Common_CObjectProcessingPriorityQueue_h
#define Common_CObjectProcessingPriorityQueue_h

#include "CObjectProcessingQueue.h"
#include <queue>

namespace chaos {
    using namespace std;
    /*
     Element for the heap
     */
    template <typename T>
    class PriorityQueuedElement {
        T *element;
        bool disposeOnDestroy;
        int priority;
    public:
        PriorityQueuedElement(T *_element, int _priority = 50, bool _disposeOnDestroy = true):element(_element), priority(_priority), disposeOnDestroy(_disposeOnDestroy){}
        ~PriorityQueuedElement(){
            if (disposeOnDestroy && element) {
                dispose(element);
            }
        }
        T* operator->() { return &element; }
        int getPriority(){
            return priority;
        }
    };
    
    /*

    
    /*
     Operator for heap
     */
    bool operator<(const PriorityQueuedElement * const a, const PriorityQueuedElement * const b)
    {
    return a->getPriority() < b->getPriority();
    }
    
    /*
     
     */
    template<T>
    class CObjectProcessingPriorityQueue : public CObjectProcessingQueue< PriorityQueuedElement<T>, priority_queue<PriorityQueuedElement*> > {
        
        
    public:
        void push(T* elementToPush, int _priority = 50, bool _disposeOnDestroy = true){
            PriorityQueuedElement<T> *_element = new PriorityQueuedElement<T>(elementToPush, _priority, _disposeOnDestroy);
            CObjectProcessingQueue< PriorityQueuedElement<T>, ority_queue<PriorityQueuedElement*> >::push(_element);
        }
    };
}
#endif
