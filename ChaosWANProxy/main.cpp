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

