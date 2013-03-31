/*
 *	FastHash.cpp
 *	!CHOAS
 *	Modified by Bisegni Claudio.
 *
 * The hash function used here is by Bob Jenkins, 1996:
 *    <http://burtleburtle.net/bob/hash/doobs.html>
 *       "By Bob Jenkins, 1996.  bob_jenkins@burtleburtle.net.
 *       You may use this code any way you wish, private, educational,
 *       or commercial.  It's free."
 */

#ifndef __CHAOSFramework__FastHash__
#define __CHAOSFramework__FastHash__
#include <stdint.h>
#include <unistd.h>

namespace chaos {
    namespace data {
        namespace cache {
            
            class FastHash {
            public:
                uint32_t hash(const void *key, size_t length, const uint32_t initval);
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__FastHash__) */
