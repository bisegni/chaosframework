//
//  log.cpp
//  ChaosFramework
//
//  Created by Bisegni Claudio on 19/12/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#include "global.h"
#include "ChaosCommon.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <boost/logging/format.hpp>
#include <boost/logging/writer/ts_write.hpp>


using namespace chaos;
using namespace boost::logging;


BOOST_DEFINE_LOG(g_l, log_type)
BOOST_DEFINE_LOG_FILTER(g_l_filter, boost::logging::level::holder)
