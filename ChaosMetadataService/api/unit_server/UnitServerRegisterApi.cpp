/*
 *	UnitServerRegisterApi.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyrigh 2015 INFN, National Institute of Nuclear Physics
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
#include "UnitServerRegisterApi.h"
#include "../../batch/unit_server/UnitServerAckBatchCommand.h"
#define USRA_INFO INFO_LOG(UnitServerRegisterApi)
#define USRA_DBG  DBG_LOG(UnitServerRegisterApi)
#define USRA_ERR  ERR_LOG(UnitServerRegisterApi)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::unit_server;

UnitServerRegisterApi::UnitServerRegisterApi():
AbstractApi("registerUnitServer"){
    
}

UnitServerRegisterApi::~UnitServerRegisterApi() {
    
}

chaos::common::data::CDataWrapper *UnitServerRegisterApi::execute(chaos::common::data::CDataWrapper *api_data,
                                                                  bool& detach_data) throw (chaos::CException){
    USRA_INFO << api_data->getJSONString();
    int err = 0;
    uint64_t command_id;
    bool is_present = false;
    persistence::data_access::UnitServerDataAccess *us_da = getPersistenceDriver()->getUnitServerDataAccess();
    if(!api_data->hasKey(ChaosSystemDomainAndActionLabel::MDS_REGISTER_UNIT_SERVER_ALIAS)) {
        throw CException(-1, "Unit server alias not found", __PRETTY_FUNCTION__);
    }
    //fetch the unit server alias
    detach_data = true;
    const std::string unit_server_alias = api_data->getStringValue(ChaosSystemDomainAndActionLabel::MDS_REGISTER_UNIT_SERVER_ALIAS);
    try {
        if((err = us_da->checkUnitServerPresence(unit_server_alias, is_present))) {
            //err
            api_data->addInt32Value(ChaosSystemDomainAndActionLabel::MDS_REGISTER_UNIT_SERVER_RESULT,
                                    ErrorCode::EC_MDS_UNIT_SERV_REGISTRATION_FAILURE_INVALID_ALIAS);
            LOG_AND_TROW(USRA_ERR, -2, "error checking the unit server presence")
        }if(is_present) {
            //present
            if((err = us_da->updateUnitServer(*api_data))) {
                api_data->addInt32Value(ChaosSystemDomainAndActionLabel::MDS_REGISTER_UNIT_SERVER_RESULT,
                                        ErrorCode::EC_MDS_UNIT_SERV_REGISTRATION_FAILURE_INVALID_ALIAS);
                LOG_AND_TROW(USRA_ERR, -3, "error updating the unit server information")
            }
        }else {
            //unit server not found
            if((err = us_da->insertNewUnitServer(*api_data))) {
                api_data->addInt32Value(ChaosSystemDomainAndActionLabel::MDS_REGISTER_UNIT_SERVER_RESULT,
                                        ErrorCode::EC_MDS_UNIT_SERV_REGISTRATION_FAILURE_INVALID_ALIAS);
                LOG_AND_TROW(USRA_ERR, -4, "error saving the new unit server information")
            }
        }
        //now we can send back the received message with the ack result
        api_data->addInt32Value(ChaosSystemDomainAndActionLabel::MDS_REGISTER_UNIT_SERVER_RESULT, ErrorCode::EC_MDS_UNIT_SERV_REGISTRATION_OK);
    } catch (chaos::CException& ex) {
        api_data->addInt32Value(ChaosSystemDomainAndActionLabel::MDS_REGISTER_UNIT_SERVER_RESULT, ErrorCode::EC_MDS_UNIT_SERV_REGISTRATION_OK);
        getBatchExecutor()->submitCommand(std::string(GET_MDS_COMMAND_ASLIAS(batch::unit_server::UnitServerAckCommand)),
                                          api_data,
                                          command_id);
        USRA_ERR << "Sent ack for registration denied to the unit server " << unit_server_alias;
        throw ex;
    } catch (...) {
        getBatchExecutor()->submitCommand(std::string(GET_MDS_COMMAND_ASLIAS(batch::unit_server::UnitServerAckCommand)),
                                          api_data,
                                          command_id);
        LOG_AND_TROW(USRA_ERR, -5, "Unknown exception")
        USRA_ERR << "Sent ack for registration denied to the unit server " << unit_server_alias;
   }

    //all is gone weel
    USRA_DBG << "Send ack for registration ok to the unit server " << unit_server_alias;
    getBatchExecutor()->submitCommand(std::string(GET_MDS_COMMAND_ASLIAS(batch::unit_server::UnitServerAckCommand)),
                                      api_data,
                                      command_id);
    
    return NULL;
}