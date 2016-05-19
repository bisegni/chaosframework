/*
 *	NetworkAddress.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
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

#ifndef ChaosFramework_NetworkAddress_h
#define ChaosFramework_NetworkAddress_h

#include <string>
namespace chaos {
	namespace common {
		namespace network {
			//! Represent the abstraction of network address of the physical node
			/*! \class DeviceAddress
			 this permit to abtsrac the chaos address of single node througt rpc system
			 */
			struct CNetworkAddress {
				//the ip and port for the host that run the control unit
				std::string ip_port;
                CNetworkAddress(){};
                CNetworkAddress(const CNetworkAddress& cna):
                ip_port(cna.ip_port){};
                
                CNetworkAddress(const std::string& _ip_port):
                ip_port(_ip_port){};
			};
			
			//! Represent the abstraction of chaos node id of the chaos virtual node
			/*! \class DeviceAddress
			 this permit to abtsrac the chaos address of single node througt rpc system
			 */
			struct CNodeNetworkAddress : public CNetworkAddress {
				//the instance of control unit that contain the deviceid
				std::string node_id;
                CNodeNetworkAddress(){};
                CNodeNetworkAddress(const CNodeNetworkAddress& cnna):
                CNetworkAddress(cnna.ip_port),
                node_id(cnna.node_id){};

                CNodeNetworkAddress(const std::string& _node_id,
                                    const std::string& _ip_port):
                CNetworkAddress(_ip_port),
                node_id(_node_id){};
			};
			
			//! Represent the abstraction of chaos address of the node
			/*! \class DeviceAddress
			 this permit to abtsrac the chaos address of single node througt rpc system
			 */
			struct CDeviceNetworkAddress : public CNodeNetworkAddress {
				//the ipo for the host that run the control unit
				std::string device_id;
                CDeviceNetworkAddress(){};
                CDeviceNetworkAddress(const CDeviceNetworkAddress& cdna):
                CNodeNetworkAddress(cdna.node_id,
                                    cdna.ip_port),
                device_id(cdna.device_id){};

                CDeviceNetworkAddress(const std::string _device_id,
                                      const std::string& _node_id,
                                      const std::string& _ip_port):
                CNodeNetworkAddress(_node_id,
                                    _ip_port),
                device_id(_device_id){};

                CDeviceNetworkAddress(const std::string _device_id):
                CNodeNetworkAddress(std::string(),
                                    std::string()),
                device_id(_device_id){};
			};
		}
	}
}
#endif
