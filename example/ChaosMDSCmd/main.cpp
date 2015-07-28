/*
 *	ChaosMDSCmd
 *	!CHAOS
 *	Created by Andrea Michelotti
 *      Command Line Interface to MDS services
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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
#include <iostream>
#include <string>
#include <vector>
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>
#include <ChaosMetadataServiceClient/api_proxy/unit_server/NewUS.h>
#include <ChaosMetadataServiceClient/api_proxy/unit_server/ManageCUType.h>

#include <ChaosMetadataServiceClient/api_proxy/control_unit/SetInstanceDescription.h>

#include <chaos/common/global.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/network/CNodeNetworkAddress.h>
#include <chaos/ui_toolkit/ChaosUIToolkit.h>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/ui_toolkit/HighLevelApi/HLDataApi.h>
#include <stdio.h>
#include <chaos/common/bson/bson.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

using namespace std;
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::ui;
using namespace bson;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::date_time;
using namespace chaos::metadata_service_client;

namespace chaos_batch = chaos::common::batch_command;

#define OPT_MDS_FILE "mds-conf"
#define OPT_TIMEOUT         "timeout"
#define OPT_CU_ID			"deviceid"
#define OPT_SCHEDULE_TIME   "stime"
#define OPT_PRINT_STATE     "print-state"
#define OPT_PRINT_TYPE		"print-type"
#define OPT_PRINT_DATASET	"print-dataset"
#define OPT_GET_DS_VALUE	"get_ds_value"
//--------------slow contorol option----------------------------------------------------
#define OPT_SL_ALIAS									"sc-alias"
#define OPT_SL_EXEC_CHANNEL								"sc-exec-channel"
#define OPT_SL_PRIORITY									"sc-priority"
#define OPT_SL_SUBMISSION_RULE							"sc-sub-rule"
#define OPT_SL_COMMAND_DATA								"sc-cmd-data"
#define OPT_SL_COMMAND_ID								"sc-cmd-id"
#define OPT_SL_COMMAND_SCHEDULE_DELAY					"sc-cmd-sched-wait"
#define OPT_SL_COMMAND_SUBMISSION_RETRY_DELAY			"sc-cmd-submission-retry-delay"
#define OPT_SL_COMMAND_SET_FEATURES_LOCK				"sc-cmd-features-lock"
#define OPT_SL_COMMAND_SET_FEATURES_SCHEDULER_WAIT		"sc-cmd-features-sched-wait"
//--------------rt control unit option--------------------------------------------------
#define OPT_RT_ATTRIBUTE_VALUE							"rt-attr-val"

    //throw CException(r->getError(),__FUNCTION__,r->getErrorMessage());

#define EXECUTE_CHAOS_API(api_name,time_out,...) {\
chaos::metadata_service_client::api_proxy::ApiProxyResult r=  GET_CHAOS_API_PTR(api_name)->execute( __VA_ARGS__ );\
r->setTimeout(time_out);\
r->wait();\
if(r->getError()){\
    std::cerr<<" error in :"<<__FUNCTION__<<"|"<<__LINE__<<"|"<< # api_name <<" " <<r->getErrorMessage()<<std::endl;\
}}

#define GET_CONFIG_STRING(what,attr) \
std::string attr=(what)->getStringValue( # attr);
//std::cout<<" :" # attr <<" ="<<attr<<std::endl;

#define GET_CONFIG_INT(what,attr) \
int32_t attr=(what)->getInt32Value(# attr);

#define GET_CONFIG_BOOL(what,attr) \
bool attr=(what)->getBoolValue( # attr);

inline ptime utcToLocalPTime(ptime utcPTime){
  c_local_adjustor<ptime> utcToLocalAdjustor;
  ptime t11 = utcToLocalAdjustor.utc_to_local(utcPTime);
  return t11;
}

void print_state(CUStateKey::ControlUnitState state) {
  switch (state) {
  case CUStateKey::INIT:
    std::cout << "Initialized:"<<state;
    break;
  case CUStateKey::START:
    std::cout << "Started:"<<state;
    break;
  case CUStateKey::STOP:
    std::cout << "Stopped:"<<state;
    break;
  case CUStateKey::DEINIT:
    std::cout << "Deinitilized:"<<state;
    break;
  default:
    std::cout << "Uknown:"<<state;

  }
  std::cout<<std::endl;
}


int initialize_mds(std::string conf){
    std::stringstream stringa;
    std::ifstream f(conf.c_str());
    if(f.fail()){
       throw CException(-3,__FUNCTION__,"Cannot open file:"+ conf);
    }
    std::cout<<"* reading:"<<conf<<std::endl;
    stringa << f.rdbuf();
    //std::cout<<"read:"<<stringa.str()<<std::endl;
   // BSONObj bson(" \"data_servers\" : [ { \"hostname\" : \"192.168.150.21:1672:30175|0\" , \"id_server\" : 5 , \"is_live\" : true} , { \"hostname\" : \"192.168.150.22:1672:30175|0\" , \"id_server\" : 6 , \"is_live\" : true} , { \"hostname\" : \"192.168.150.23:1672:30175|0\" , \"id_server\" : 7 , \"is_live\" : true}]");
    //BSONObj bson(strdup(stringa.str().c_str()));
    //std::cout<<"BJSON:"<<bson.jsonString();
    CDataWrapper mdsconf;
    mdsconf.setSerializedJsonData(stringa.str().c_str());

    //! rest ALL
    EXECUTE_CHAOS_API(api_proxy::service::ResetAll,3000);
  //  std::cout<<"json:"<<data.getJSONString()<<std::endl;
    CMultiTypeDataArrayWrapper* data_servers=mdsconf.getVectorValue("data_servers");
    if(data_servers){
         for(int cnt=0;cnt<data_servers->size();cnt++){
            GET_CONFIG_STRING(data_servers->getCDataWrapperElementAtIndex(cnt),hostname);
            GET_CONFIG_INT(data_servers->getCDataWrapperElementAtIndex(cnt),id_server);
            std::cout<<"* found dataserver["<<cnt<<"]:"<<hostname<<" id:"<<id_server<<std::endl;
            EXECUTE_CHAOS_API(api_proxy::data_service::UpdateDS,3000,hostname,hostname,id_server);
            EXECUTE_CHAOS_API(api_proxy::data_service::NewDS,3000,hostname,hostname,id_server);

        }
    }

    CMultiTypeDataArrayWrapper* us=mdsconf.getVectorValue("us");
    if(us){
        for(int cnt=0;(us!=NULL)&&(cnt<us->size());cnt++){
            CDataWrapper* usw=us->getCDataWrapperElementAtIndex(cnt);
            GET_CONFIG_STRING(usw,unit_server_alias);
            std::cout<<"* found us["<<cnt<<"]:"<<unit_server_alias<<std::endl;
            //GET_CHAOS_API_PTR(api_proxy::unit_server::NewUS)->execute(usname.c_str());
             EXECUTE_CHAOS_API(api_proxy::unit_server::NewUS,3000,unit_server_alias);

             CMultiTypeDataArrayWrapper* cu_l=usw->getVectorValue("cu_desc");
             for(int cui=0;(cu_l !=NULL) && (cui<cu_l->size());cui++){
                 api_proxy::control_unit::SetInstanceDescriptionHelper cud;
                 CDataWrapper* cuw=cu_l->getCDataWrapperElementAtIndex(cui);
                 GET_CONFIG_STRING(cuw,cu_id);
                 GET_CONFIG_STRING(cuw,cu_type);
                 GET_CONFIG_STRING(cuw,cu_param);
                 GET_CONFIG_BOOL(cuw,auto_load);
                 std::cout<<"\t"<<cu_id<<","<<cu_type<<std::endl;
                 cud.auto_load=auto_load;
                 cud.load_parameter = cu_param;
                 cud.control_unit_uid=cu_id;
                 cud.unit_server_uid=unit_server_alias;
                 cud.control_unit_implementation=cu_type;
                 //EXECUTE_CHAOS_API(api_proxy::unit_server::ManageCUType,3000,unit_server_alias,cu_type,1);

                 EXECUTE_CHAOS_API(api_proxy::unit_server::ManageCUType,3000,unit_server_alias,cu_type,0);

                 // drivers
                 CMultiTypeDataArrayWrapper* drv_l=cuw->getVectorValue("DriverDescription");
                 for(int drv=0;(drv_l !=NULL) && (drv<drv_l->size());drv++){
                    CDataWrapper* drv_w=drv_l->getCDataWrapperElementAtIndex(drv);

                    GET_CONFIG_STRING(drv_w,DriverDescriptionName);
                    GET_CONFIG_STRING(drv_w,DriverDescriptionVersion);
                    GET_CONFIG_STRING(drv_w,DriverDescriptionInitParam);
                    cud.addDriverDescription(DriverDescriptionName,DriverDescriptionVersion,DriverDescriptionInitParam);
                 }
                 //attributes
                 CMultiTypeDataArrayWrapper* attr_l=cuw->getVectorValue("AttrDesc");
                 for(int attr=0;(attr_l !=NULL) && (attr<attr_l->size());attr++){
                    CDataWrapper* attr_w=attr_l->getCDataWrapperElementAtIndex(attr);

                    GET_CONFIG_STRING(attr_w,ds_attr_name);
                    GET_CONFIG_STRING(attr_w,ds_default_value);
                    GET_CONFIG_STRING(attr_w,ds_max_range);
                    GET_CONFIG_STRING(attr_w,ds_min_range);

                    cud.addAttributeConfig(ds_attr_name,ds_default_value,ds_max_range,ds_min_range);
                 }

                 EXECUTE_CHAOS_API(api_proxy::control_unit::SetInstanceDescription,3000,cud);
             }
        }
    }


    return 0;
}
int checkSubmissionRule(std::string scSubmissionRule) {
  if( scSubmissionRule.compare("normal") == 0) {
    return chaos_batch::SubmissionRuleType::SUBMIT_NORMAL;
  } else if( scSubmissionRule.compare("stack")  == 0) {
    return chaos_batch::SubmissionRuleType::SUBMIT_AND_Stack;
  } else if( scSubmissionRule.compare("kill")  == 0) {
    return chaos_batch::SubmissionRuleType::SUBMIT_AND_Kill;
  } else return -1;
}

int main (int argc, char* argv[] )
{
    std::string conf_file;
    std::string mds;
    bool operation_defined=false;

  try {
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption<string>(OPT_MDS_FILE, "MDS configuration file (initialize the MDS with the given configuration)",&conf_file);

    ChaosMetadataServiceClient::getInstance()->init(argc, argv);



    mds  = ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->getConfiguration()->getStringValue("metadata-server");
    if (mds.empty()){
        std::cerr<< "# you must define a valid MDS server 'metadata-server'"<<std::endl;
        return -4;
    }
    //! [UIToolkit Attribute Init]
    std::cout <<"* MDS:"<<mds<<std::endl;
    if(!conf_file.empty()){
        std::cout<<"* Initializing mds:"<< mds<<" with:"<<conf_file<<endl;
        ChaosMetadataServiceClient::getInstance()->addServerAddress(mds);
        operation_defined=true;
        initialize_mds(conf_file);
        return 0;

    }
    if(operation_defined){
            ChaosMetadataServiceClient::getInstance()->enableMonitoring();

        ChaosMetadataServiceClient::getInstance()->start();
    }

  }catch (CException& e) {
    std::cerr << e.errorCode << " - "<< e.errorDomain << " - " << e.errorMessage << std::endl;
    return -3;
  }

  return 0;
}
