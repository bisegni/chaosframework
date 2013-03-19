/*
 *	PublisherDataService.h
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

#ifndef __CHAOSFramework__PublishDataService__
#define __CHAOSFramework__PublishDataService__

#include <string>

namespace chaos {
    namespace databroker {
        
        class DataBroker;
        
        namespace publisher {
            
            //! Publisher Service
            /*!
                This class represent
             */
            class PublisherDataService {
                friend class chaos::databroker::DataBroker;
                
                std::string publisServiceName;
                
                PublisherDataService(const char *keyName);
                PublisherDataService(std::string& keyName);
                ~PublisherDataService();
            public:
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__PublisherDataService__) */
