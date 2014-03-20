/*
 *	VFSDriver.h
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
#ifndef __CHAOSFramework__VFSDriver__
#define __CHAOSFramework__VFSDriver__

#include <string>

namespace chaos {
	namespace data_service {
		namespace storage_system {
			
			
			class BlockInfo {
				
			};
			
			typedef enum BlockType {
				BlockTypeStage,
				BlockTypeData
			} BlockType;
			
			//! Abstraction of the vfs driver
			/*!
			 This class represent the abstraction of the
			 work to do on chaos VFS
			 */
			class VFSDriver {
			public:
				/*!
				 
				*/
				virtual BlockInfo *getNewBlock(BlockType type, std::string path, uint32_t max_size, uint32_t max_live_time) = 0;
				
				virtual BlockInfo *releaseBlock(BlockType type, std::string path) = 0;

                virtual int write(BlockInfo *, void * data, uint32_t data_len) = 0;
				
                virtual int read(BlockInfo *, uint64_t offset, void * * data, uint32_t *data_len) = 0;
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__VFSDriver__) */
