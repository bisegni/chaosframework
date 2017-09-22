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

#ifndef __CHAOSFramework__TemplatedConcurrentQueue__
#define __CHAOSFramework__TemplatedConcurrentQueue__
#include <queue>

#include <chaos/common/pqueue/CObjectProcessingPriorityQueue.h>

#include <boost/thread.hpp>
#include <boost/heap/priority_queue.hpp>

namespace chaos {
    namespace common  {
        namespace thread {
            
            
            template <typename T>
            struct TemplatedConcurrentQueueElement {
                T element;
                int priority;
                
                TemplatedConcurrentQueueElement(T _element, int _priority = 50):element(_element), priority(_priority) {}
                
                /*
                 Operator for heap
                 */
                bool operator < (const TemplatedConcurrentQueueElement& b) const {
                    return priority < b.priority;
                }
                
                inline int getPriority() {
                    return priority;
                }
            };
            
            template<typename Type, typename Compare = std::less<Type> >
            struct pless : public std::binary_function<Type *, Type *, bool> {
                bool operator()(const Type *x, const Type *y) const {
                    return Compare()(*x, *y);
                }
            };
            
            template<typename T>
            class TemplatedConcurrentQueue
            {
            private:
                //std::queue<T> the_queue;
                boost::heap::priority_queue< TemplatedConcurrentQueueElement<T> > element_queue;
                mutable boost::mutex the_mutex;
                
                boost::condition_variable the_condition_variable;
            public:
                void push(T const& data, uint32_t priority = 50) {
                    boost::mutex::scoped_lock lock(the_mutex);
                    element_queue.push(TemplatedConcurrentQueueElement<T>(data, priority));
                    
                    //free the mutex
                    lock.unlock();
                    
                    //notify the withing thread
                    the_condition_variable.notify_one();
                }
                
                bool empty() const {
                    boost::mutex::scoped_lock lock(the_mutex);
                    return element_queue.empty();
                }
                
                bool try_pop(T& popped_value) {
                    boost::mutex::scoped_lock lock(the_mutex);
                    if(element_queue.empty()) {
                        return false;
                    }
                    TemplatedConcurrentQueueElement<T> popped_element = element_queue.top();
                    element_queue.pop();
                    popped_value=popped_element.element;
		    //                    delete(popped_element);
                    return true;
                }
                
                void wait_and_pop(T& popped_value)
                {
                    boost::mutex::scoped_lock lock(the_mutex);
                    while(element_queue.empty())
                    {
                        the_condition_variable.wait(lock);
                    }
                    
                    TemplatedConcurrentQueueElement<T> popped_element = element_queue.top();
                    element_queue.pop();
                    popped_value=popped_element.element;
		    //                    delete(popped_element);
                }
                
            };
        }
    }
}
#endif /* defined(__CHAOSFramework__TemplatedConcurrentQueue__) */
