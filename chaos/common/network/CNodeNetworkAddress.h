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

#ifndef ChaosFramework_NetworkAddress_h
#define ChaosFramework_NetworkAddress_h

#include <string>

#include <chaos/common/chaos_types.h>
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
			struct CDeviceNetworkAddress : public chaos::common::network::CNodeNetworkAddress {
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
            
            
            CHAOS_DEFINE_VECTOR_FOR_TYPE(chaos::common::network::CNetworkAddress, VectorNetworkAddress);
		}
	}
}
#endif
