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
static const char * const MONGODB_COLLECTION_NODES       = "nodes";
static const char * const MONGODB_COLLECTION_DATASETS    = "datasets";
static const char * const MONGODB_COLLECTION_SEQUENCES   = "sequences";

#endif
