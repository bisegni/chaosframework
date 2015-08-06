/*	
 *	ChaosThread.h
 *	!CHAOS
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
#ifndef ChaosFramework_ChaosThread_h
#define ChaosFramework_ChaosThread_h
#include <boost/thread.hpp>
namespace chaos {
	namespace common {
		namespace thread {
			typedef boost::upgrade_lock<boost::shared_mutex>			UpgradeableLock;
			typedef boost::upgrade_to_unique_lock<boost::shared_mutex>	UpgradeReadToWriteLock;
			typedef boost::shared_lock<boost::shared_mutex>				ReadLock;
			typedef boost::unique_lock<boost::shared_mutex>				WriteLock;
			typedef boost::shared_mutex									SharedMutex;			
		}
	}
}
#endif
