/*
 *	SnapshotDataAccess.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#include "SnapshotDataAccess.h"
#include "DataServiceDataAccess.h"

#include <chaos/common/io/IODirectIODriver.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/chaos_types.h>

#include <boost/format.hpp>

using namespace chaos::common::io;
using namespace chaos::common::utility;
using namespace chaos::common::network;
using namespace chaos::metadata_service::persistence::data_access;

#define SDA_INFO INFO_LOG(SnapshotDataAccess)
#define SDA_DBG  DBG_LOG(SnapshotDataAccess)
#define SDA_ERR  ERR_LOG(SnapshotDataAccess)

CHAOS_DEFINE_VECTOR_FOR_TYPE(ChaosSharedPtr<chaos::common::data::CDataWrapper>, CDSList)

DEFINE_DA_NAME(SnapshotDataAccess)

SnapshotDataAccess::SnapshotDataAccess(DataServiceDataAccess *_data_service_da):
AbstractDataAccess("SnapshotDataAccess"),
data_service_da(_data_service_da){}

SnapshotDataAccess::~SnapshotDataAccess() {}
