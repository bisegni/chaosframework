/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
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
