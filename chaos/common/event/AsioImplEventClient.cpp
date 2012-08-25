/*
 *	AsioImplEventClient.cpp
 *	CHAOSFramework
 *	Created by Claudio Bisegni on 25/08/12.
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

#include <chaos/common/event/AsioImplEventClient.h>

using namespace chaos;
using namespace chaos::event;

AsioImplEventClient::AsioImplEventClient(string *alias):EventClient(alias) {
    threadNumber = 0;
}

/*
 init the event adapter
 */
void AsioImplEventClient::init(CDataWrapper*) throw(CException) {
    threadNumber = 4;
}

/*
 start the event adapter
 */
void AsioImplEventClient::start() throw(CException) {
    
        //register forall event
        //create the services
    
    for (int idx = 0; idx < threadNumber; idx++) {
            //create the handler
        shared_ptr<thread> thread(new boost::thread(bind(&asio::io_service::run, &io_service)));
        serviceThread.push_back(thread);
    }
}

/*
 deinit the event adapter
 */
void AsioImplEventClient::deinit() throw(CException) {
    io_service.stop();
        // Wait for all threads in the pool to exit.
    for (std::size_t i = 0; i < serviceThread.size(); ++i)
        serviceThread[i]->join();
    
}
