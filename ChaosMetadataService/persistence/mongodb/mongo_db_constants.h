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

//----------------------------------collection naming-------------------------------------------
static const char * const MONGODB_COLLECTION_NODES                      = "mds_nodes";
static const char * const MONGODB_COLLECTION_NODES_COMMAND              = "mds_nodes_command";
static const char * const MONGODB_COLLECTION_NODES_COMMAND_TEMPLATE     = "mds_nodes_command_template";
static const char * const MONGODB_COLLECTION_SEQUENCES                  = "mds_sequences";

#endif
