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
#define MONGODB_COLLECTION_UNIT_SERVER  "unit_server"
#define MONGODB_KEY_UNIT_SERVER_ALIAS       ChaosSystemDomainAndActionLabel::MDS_REGISTER_UNIT_SERVER_ALIAS
#define MONGODB_KEY_UNIT_SERVER_CU_TYPE     ChaosSystemDomainAndActionLabel::MDS_REGISTER_UNIT_SERVER_CONTROL_UNIT_ALIAS
#define MONGODB_KEY_UNIT_SERVER_KEY         ChaosSystemDomainAndActionLabel::MDS_REGISTER_UNIT_SERVER_KEY
#define MONGODB_KEY_UNIT_SERVER_RPC_ADDR    CUDefinitionKey::CU_INSTANCE_NET_ADDRESS

#endif
