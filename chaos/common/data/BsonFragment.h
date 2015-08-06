/*
 *	BsonFragment.h
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

#ifndef __CHAOSFramework__BsonFragment__
#define __CHAOSFramework__BsonFragment__

#include <chaos/common/bson/bsonobjbuilder.h>

namespace chaos {
    
    namespace data {
     
        class  BsonFragment : public bson::BSONObjBuilder {
          
        public:
            
            /**
             * 
             */
            BsonFragment();
            
            /*!
             Return the pointer of the fragment and prepare the buffer for the next fragment
             \param fragmetDimension the dimension of the fragment
             \return the pointer to the fragment it is anyway managed by BsonFragment Class
             */
            const char *getFragmentPtr(uint32_t& fragmetDimension);
        };
        
    }
    
}


#endif /* defined(__CHAOSFramework__BsonFragment__) */
