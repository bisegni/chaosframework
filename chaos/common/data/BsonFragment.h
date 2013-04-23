//
//  BsonFragment.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 4/23/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

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
