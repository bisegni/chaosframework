/*
 *	TemplatedConcurrentQueue.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__TemplatedConcurrentQueue__
#define __CHAOSFramework__TemplatedConcurrentQueue__

#include <boost/thread.hpp>
namespace chaos {
	template<typename T>
	class TemplatedConcurrentQueue
	{
	private:
		std::queue<T> the_queue;
		mutable boost::mutex the_mutex;
		boost::condition_variable the_condition_variable;
	public:
		void push(T const& data)
		{
			boost::mutex::scoped_lock lock(the_mutex);
			the_queue.push(data);
			lock.unlock();
			the_condition_variable.notify_one();
		}
		
		bool empty() const
		{
			boost::mutex::scoped_lock lock(the_mutex);
			return the_queue.empty();
		}
		
		bool try_pop(T& popped_value)
		{
			boost::mutex::scoped_lock lock(the_mutex);
			if(the_queue.empty())
			{
				return false;
			}
			
			popped_value=the_queue.front();
			the_queue.pop();
			return true;
		}
		
		void wait_and_pop(T& popped_value)
		{
			boost::mutex::scoped_lock lock(the_mutex);
			while(the_queue.empty())
			{
				the_condition_variable.wait(lock);
			}
			
			popped_value=the_queue.front();
			the_queue.pop();
		}
		
	};
}
#endif /* defined(__CHAOSFramework__TemplatedConcurrentQueue__) */
