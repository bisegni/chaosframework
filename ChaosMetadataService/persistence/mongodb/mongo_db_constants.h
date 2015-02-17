//
//  mongo_db_constants.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 30/01/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

#include <chaos/common/chaos_constants.h>

#include <boost/format.hpp>

#ifndef CHAOSFramework_mongo_db_constants_h
#define CHAOSFramework_mongo_db_constants_h

#define MONGO_DB_COLLECTION_NAME(db,coll)	boost::str(boost::format("%1%.%2%") % db % coll)

//-------unit server------
#define MONGODB_COLLECTION_NODES            "nodes"
#define MONGODB_COLLECTION_DATASET          "dataset"

#endif
