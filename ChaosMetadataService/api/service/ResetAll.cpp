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

#include "ResetAll.h"

#define S_RA_INFO INFO_LOG(ResetAll)
#define S_RA_DBG  DBG_LOG(ResetAll)
#define S_RA_ERR  ERR_LOG(ResetAll)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::service;
using namespace chaos::metadata_service::persistence::data_access;

typedef std::vector< ChaosSharedPtr<CDataWrapper> > ResultVector;

ResetAll::ResetAll():
AbstractApi("resetAll"){

}

ResetAll::~ResetAll() {

}

chaos::common::data::CDataWrapper *ResetAll::execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) {
    int err = 0;
    GET_DATA_ACCESS(UtilityDataAccess, u_da, -1);
    if((err = u_da->resetAllData())) {
        LOG_AND_TROW(S_RA_ERR, err, "Error resetting all data");
    }
    return NULL;
}
