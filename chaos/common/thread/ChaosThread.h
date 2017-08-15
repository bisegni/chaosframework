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
