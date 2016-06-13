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

//-------snapshot data------
static const char * const MONGODB_COLLECTION_SNAPSHOT                   = "snapshot";
static const char * const MONGODB_COLLECTION_SNAPSHOT_DATA              = "snapshot_data";

//-------tree group---------
static const char * const MONGODB_COLLECTION_TREE_GROUP                 = "mds_tree_group";

//---------logging----------
static const char * const MONGODB_COLLECTION_LOGGING                    = "mds_logging";

//---------script----------
static const char * const MONGODB_COLLECTION_SCRIPT                     = "mds_script";

#endif
