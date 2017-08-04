/*
 *	AbstractUnitProxy.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 02/08/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__9B333EE_BA6D_4EFC_8445_8AA63946A555_AbstractUnitProxy_h
#define __CHAOSFramework__9B333EE_BA6D_4EFC_8445_8AA63946A555_AbstractUnitProxy_h

#include <chaos_micro_unit_toolkit/data/DataPack.h>
#include <chaos_micro_unit_toolkit/micro_unit_toolkit_types.h>
#include <chaos_micro_unit_toolkit/connection/protocol_adapter/AbstractProtocolAdapter.h>
namespace chaos {
    namespace micro_unit_toolkit {
        namespace connection {
            namespace unit_proxy {

                struct RemoteMessage {
                    data::DataPackSharedPtr message;
                    const bool is_request;
                    const uint32_t message_id;
                    
                    RemoteMessage(const data::DataPackSharedPtr& _message);
                };
                
                typedef ChaosUniquePtr<RemoteMessage> RemoteMessageUniquePtr;
                
                //! Abstract base class for all unit proxy
                class AbstractUnitProxy {
                    protocol_adapter::AbstractProtocolAdapter& protocol_adapter;
                public:
                    const ProxyType type;
                    AbstractUnitProxy(const ProxyType _type,
                                      protocol_adapter::AbstractProtocolAdapter& _protocol_adapter);
                    
                    virtual ~AbstractUnitProxy();
                    
                    int sendMessage(data::DataPackUniquePtr& message_data);
                    
                    int sendAnswer(RemoteMessageUniquePtr& message,
                                   data::DataPackUniquePtr& message_data);
                    
                    bool hasMoreMessage();
                    
                    RemoteMessageUniquePtr getNextMessage();
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__9B333EE_BA6D_4EFC_8445_8AA63946A555_AbstractUnitProxy_h */
