/*
 *	main.cpp
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

#include <iostream>

#include "ChaosWANProxy.h"
#include "global_constant.h"

using namespace chaos::wan_proxy;

int main(int argc, char * argv[]) {
	try {
		//custom start parameter
		//cache parameter
		ChaosWANProxy::getInstance()->getGlobalConfigurationInstance()->addOption< SettingStringList >(setting_options::OPT_CDS_ADDR,
																									   "CDS server lists [host:p_port:s_port|endpoint]",
																									   &ChaosWANProxy::getInstance()->setting.list_cds_server,
																									   true);
		
		ChaosWANProxy::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(setting_options::OPT_WAN_INTERFACE_PARAM,
																								 "a json block of only strings that that contain the parameter for wan interfaces",
																								 &ChaosWANProxy::getInstance()->setting.parameter_wan_interfaces);
		
		ChaosWANProxy::getInstance()->getGlobalConfigurationInstance()->addOption< SettingStringList >(setting_options::OPT_INTERFACE_TO_ACTIVATE,
																									   "Specify wich interface to enable",
																									   &ChaosWANProxy::getInstance()->setting.list_wan_interface_to_enable,
																									   true);
		//Init the Node
		chaos::wan_proxy::ChaosWANProxy::getInstance()->init(argc, argv);
		//!Start the node
		chaos::wan_proxy::ChaosWANProxy::getInstance()->start();
	} catch (chaos::CException& ex) {
		DECODE_CHAOS_EXCEPTION(ex)
	}
	return 0;
}

