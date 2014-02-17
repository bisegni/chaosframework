/*
 *	DirectIOCDataWrapperClientChannel.h
 *	!CHOAS
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

#ifndef __CHAOSFramework__DirectIOCDataWrapperClientChannel__
#define __CHAOSFramework__DirectIOCDataWrapperClientChannel__

#include <string>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualClientChannel.h>

namespace chaos_data = chaos::common::data;
namespace chaos {
	namespace common {
		namespace direct_io {
			namespace channel {
                
                 REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(DirectIOCDataWrapperClientChannel, DirectIOVirtualClientChannel) {
                     REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(DirectIOCDataWrapperClientChannel)
					 DirectIODataPack dio_data_pack;
                 protected:
                     DirectIOCDataWrapperClientChannel(std::string alias);
                 public:
					~DirectIOCDataWrapperClientChannel();
                    uint32_t pushCDataWrapperSerializationBuffer(uint16_t endpoint_idx, uint8_t opcode, chaos_data::SerializationBuffer *data_pack);
                 };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__DirectIOCDataWrapperClientChannel__) */
