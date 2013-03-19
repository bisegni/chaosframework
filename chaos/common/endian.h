//
//  endian.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 3/25/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef CHAOSFramework_endian_h
#define CHAOSFramework_endian_h


#include <stdint.h>

#define __CHAOS_BIG_ENDIAN__ ((1 >> 1 == 0) ? 0 : 1)
#define __CHAOS_LITTLE_ENDIAN__ !__CHAOS_BIG_ENDIAN__

#endif
