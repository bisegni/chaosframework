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
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chaos_metadata_service_client/ChaosMetadataServiceClient.h>
#include <chaos_metadata_service_client/api_proxy/unit_server/NewUS.h>
#include <chaos_metadata_service_client/api_proxy/unit_server/DeleteUS.h>

#include <chaos_metadata_service_client/api_proxy/unit_server/ManageCUType.h>

#include <chaos_metadata_service_client/api_proxy/control_unit/SetInstanceDescription.h>
#include <chaos_metadata_service_client/api_proxy/control_unit/Delete.h>
#include <chaos_metadata_service_client/api_proxy/control_unit/DeleteInstance.h>

#include <chaos/common/global.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/network/CNodeNetworkAddress.h>
#include <stdio.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

using namespace std;
using namespace chaos;
using namespace chaos::common::data;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::date_time;
using namespace chaos::metadata_service_client;

namespace chaos_batch = chaos::common::batch_command;

#define OPT_MDS_FILE "mds-conf,c"
#define OPT_RESET_CONFIG "reset-conf,r"


#define EXECUTE_CHAOS_API(api_name,time_out,...) {\
		chaos::metadata_service_client::api_proxy::ApiProxyResult r=  GET_CHAOS_API_PTR(api_name)->execute( __VA_ARGS__ );\
		r->setTimeout(time_out);\
		r->wait();\
		if(r->getError()){\
			std::stringstream ss;\
			ss<<" error in :"<<__FUNCTION__<<"|"<<__LINE__<<"|"<< # api_name <<" " <<r->getErrorMessage()<<std::endl;\
			throw CException(r->getError(),"EXECUTE_CHAOS_API",ss.str());\
		}}
#define GET_CONFIG_STRING_DEFAULT(what,attr,def) \
		std::string attr=def;\
		if(what->hasKey( # attr)){attr=what->getStringValue( # attr);}

#define GET_CONFIG_STRING(what,attr) \
		std::string attr=what->getStringValue( # attr);
//std::cout<<" :" # attr <<" ="<<attr<<std::endl;

#define GET_CONFIG_INT(what,attr) \
		int32_t attr=what->getInt32Value(# attr);

#define GET_CONFIG_BOOL(what,attr) \
		bool attr=what->getBoolValue( # attr);

#define GET_CONFIG_DEFAULT_BOOL(what,attr,def) \
		bool attr=def;\
		if(what->hasKey( # attr)){attr=what->getBoolValue( # attr);}

#define GET_CONFIG_DEFAULT_INT(what,attr,def) \
		int32_t attr=def;\
		if(what->hasKey( # attr)){attr=what->getInt32Value( # attr);}

int initialize_from_old_mds(std::string conf){
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
	//  std::cout<<"json:"<<data.getJSONString()<<std::endl;
	//  std::auto_ptr<CMultiTypeDataArrayWrapper> data_servers(mdsconf.getVectorValue("data_servers"));
	/*  if(data_servers.get()){
         for(int cnt=0;cnt<data_servers->size();cnt++){
             std::basic_string<char>::iterator  pnt;
             int chan=0;
            std::stringstream ss;
            ss<<"data_server"<<cnt;
            std::auto_ptr<CDataWrapper> ele(data_servers->getCDataWrapperElementAtIndex(cnt));
            GET_CONFIG_STRING(ele.get(),hostname);
          //  GET_CONFIG_INT(data_servers->getCDataWrapperElementAtIndex(cnt),id_server);
            pnt=hostname.begin()+hostname.rfind('|');
            if(pnt!=hostname.end()){
                std::string schan;
                schan.assign(pnt+1,hostname.end());
                chan=atoi(schan.c_str());
                hostname.erase(pnt,hostname.end());
            }

            std::cout<<"* found dataserver["<<cnt<<"]:"<<hostname<<" channel:"<<chan<<std::endl;
            EXECUTE_CHAOS_API(api_proxy::data_service::DeleteDS,3000,ss.str());
            EXECUTE_CHAOS_API(api_proxy::data_service::NewDS,3000,ss.str(),hostname,chan);

        }
    }
	 */
    ChaosSharedPtr<CMultiTypeDataArrayWrapper> us=mdsconf.getVectorValue("us");
	if(us.get()){
		for(int cnt=0;(us.get()!=NULL)&&(cnt<us->size());cnt++){
			std::auto_ptr<CDataWrapper> usw(us->getCDataWrapperElementAtIndex(cnt));
			GET_CONFIG_STRING(usw,unit_server_alias);
			std::cout<<"* found us["<<cnt<<"]:"<<unit_server_alias<<std::endl;
			//GET_CHAOS_API_PTR(api_proxy::unit_server::NewUS)->execute(usname.c_str());
			//  EXECUTE_CHAOS_API(api_proxy::unit_server::DeleteUS,3000,unit_server_alias);
			try {
				EXECUTE_CHAOS_API(api_proxy::unit_server::NewUS,3000,unit_server_alias);
			} catch(CException e){
				std::cout<<" warning:"<<" cannot create a new US:"<<unit_server_alias<<std::endl;
			}
            ChaosSharedPtr<CMultiTypeDataArrayWrapper> cu_l=usw->getVectorValue("cu_desc");
			for(int cui=0;(cu_l.get() !=NULL) && (cui<cu_l->size());cui++){
				api_proxy::control_unit::SetInstanceDescriptionHelper cud;
				std::auto_ptr<CDataWrapper> cuw(cu_l->getCDataWrapperElementAtIndex(cui));
				GET_CONFIG_STRING(cuw,cu_id);
				GET_CONFIG_STRING(cuw,cu_type);
				GET_CONFIG_STRING_DEFAULT(cuw,cu_param,"");
				GET_CONFIG_DEFAULT_BOOL(cuw,auto_load,true);
				GET_CONFIG_DEFAULT_BOOL(cuw,auto_init,true);
				GET_CONFIG_DEFAULT_BOOL(cuw,auto_start,true);
				GET_CONFIG_DEFAULT_INT(cuw,storage_type,1);
				GET_CONFIG_DEFAULT_INT(cuw,storage_ageing,3600*24);
				GET_CONFIG_DEFAULT_INT(cuw,default_schedule_delay,1000000);
				std::cout<<"\t =="<<((auto_load)?"L":"=")<<((auto_init)?"I":"=")<<((auto_start)?"S":"=")<<"== \""<< cu_id<<"\",\""<<cu_type<<"\" schedule:"<<default_schedule_delay<<std::endl;
				cud.auto_load=auto_load;
				cud.auto_init=auto_init;
				cud.auto_start=auto_start;
				cud.load_parameter = cu_param;
				cud.control_unit_uid=cu_id;
				cud.default_schedule_delay=default_schedule_delay;
				cud.unit_server_uid=unit_server_alias;
				cud.control_unit_implementation=cu_type;
				cud.history_ageing=storage_ageing;
				cud.storage_type=(chaos::DataServiceNodeDefinitionType::DSStorageType)storage_type;
				//EXECUTE_CHAOS_API(api_proxy::unit_server::ManageCUType,3000,unit_server_alias,cu_type,1);
				//   EXECUTE_CHAOS_API(api_proxy::control_unit::DeleteInstance,3000,unit_server_alias,cu_id);
				//   EXECUTE_CHAOS_API(api_proxy::control_unit::Delete,3000,cu_id);
				EXECUTE_CHAOS_API(api_proxy::unit_server::ManageCUType,3000,unit_server_alias,cu_type,0);

				// drivers
                ChaosSharedPtr<CMultiTypeDataArrayWrapper> drv_l=cuw->getVectorValue("DriverDescription");
				for(int drv=0;(drv_l.get() !=NULL) && (drv<drv_l->size());drv++){
					std::auto_ptr<CDataWrapper> drv_w(drv_l->getCDataWrapperElementAtIndex(drv));

					GET_CONFIG_STRING(drv_w,DriverDescriptionName);
					GET_CONFIG_STRING(drv_w,DriverDescriptionVersion);
					GET_CONFIG_STRING(drv_w,DriverDescriptionInitParam);
					cud.addDriverDescription(DriverDescriptionName,DriverDescriptionVersion,DriverDescriptionInitParam);
				}
				//attributes
                ChaosSharedPtr<CMultiTypeDataArrayWrapper> attr_l=cuw->getVectorValue("AttrDesc");
				for(int attr=0;(attr_l.get() !=NULL) && (attr<attr_l->size());attr++){
					std::auto_ptr<CDataWrapper> attr_w(attr_l->getCDataWrapperElementAtIndex(attr));

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

int main (int argc, const char* argv[] )
{
	std::string conf_file;
	std::string mds;
	bool operation_defined=false;
	bool reset_config=false;

	try {
		ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption<std::string>(OPT_MDS_FILE, "MDS configuration file (initialize the MDS with the given json configuration [old style])",&conf_file);
		ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_RESET_CONFIG, po::value<bool>(&reset_config)->default_value(false),"reset MDS configuration");
		ChaosMetadataServiceClient::getInstance()->init(argc, argv);



		chaos::common::data::CDataWrapper *conf=ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->getConfiguration();
	//	std::cout<<conf->getJSONString()<<std::endl;
		if(conf->hasKey("metadata-server")){
			mds  = conf->getVectorValue("metadata-server")->getStringElementAtIndex(0);
		} else {
			std::cerr<< "# you must define a valid MDS server 'metadata-server'"<<std::endl;
			return -4;
		}
		ChaosMetadataServiceClient::getInstance()->start();

		//! [UIToolkit Attribute Init]
		std::cout <<"* MDS:"<<mds<<std::endl;

	//	ChaosMetadataServiceClient::getInstance()->addServerAddress(mds);
		if(reset_config){
			std::cout<<"* resetting MDS configuration"<<std::endl;
			EXECUTE_CHAOS_API(api_proxy::service::ResetAll,3000);
		}

		if(!conf_file.empty()){
			std::cout<<"* Initializing mds:"<< mds<<" with:"<<conf_file<<endl;

			operation_defined=true;
			initialize_from_old_mds(conf_file);


		}
		ChaosMetadataServiceClient::getInstance()->stop();
		ChaosMetadataServiceClient::getInstance()->deinit();
		std::cout<<"* done"<<std::endl;

		exit(0);


	}catch (CException& e) {
		std::cerr << e.errorCode << " - "<< e.errorDomain << " - " << e.errorMessage << std::endl;
		return -3;
	}
	return 0;

}
