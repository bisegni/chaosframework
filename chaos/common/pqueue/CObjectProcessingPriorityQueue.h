    //
    //  CObjectProcessingPriorityQueue.h
    //  ChaosFramework
    //
    //  Created by Bisegni Claudio on 01/11/11.
    //  Copyright (c) 2011 INFN. All rights reserved.
    //

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
