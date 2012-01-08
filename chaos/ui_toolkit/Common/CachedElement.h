//
//  CachedElement.h
//  ChaosFramework
//
//  Created by Claudio Bisegni on 02/10/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#ifndef ChaosFramework_CachedElement_h
#define ChaosFramework_CachedElement_h

#include <chaos/common/global.h>
#include <boost/interprocess/managed_shared_memory.hpp>

namespace chaos {
    namespace ui{
        using namespace boost::interprocess;
        /*
         Class that represent a cached element, it's mapp apiece of shared memory
         into limited mem size mapppped object
         */
        template<typename T>
        class CachedElement {
            T *castedElement;
            mapped_region *mappedRegion;
        public:
            CachedElement(mapped_region *_mappedRegion):mappedRegion(_mappedRegion){
                if(mappedRegion)
                    castedElement = static_cast<T*>(mappedRegion->get_address());
            }
            
            ~CachedElement(){
                DELETE_OBJ_POINTER(mappedRegion)
            }
            
            T *getElement(){
                return castedElement;
            }
        };       
    }
}

#endif
