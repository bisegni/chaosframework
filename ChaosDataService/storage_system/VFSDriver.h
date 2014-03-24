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

#include <chaos/common/utility/NamedService.h>

#include <boost/ptr_container/ptr_vector.hpp>

namespace chaos {
	namespace data_service {
		namespace storage_system {
			
			//forward declaration
			class VFSDriver;
			
			/*!
			 define the different type of block of data
			 withing !CHAOS VFS
			 */
			typedef enum BlockType {
				BlockTypeStage,
				BlockTypeData
			} BlockType;

			/*!
			 define the different type of block of data
			 withing !CHAOS VFS
			 */
			typedef enum BlockFlag {
				BlockFlagNone		= 0,
				BlockFlagWriteble	= 1,
				BlockFlagReadeable	= 2,
			} BlockFlag;
			
			//! define a block of data into !CHAOS VFS
			class DataBlock {
				friend class VFSDriver;
				//! the type of the block
				BlockType	type;
				
				//! the flag for this bloc
				unsigned int flags;
				
				//! is a precalculated timestamp after which the
				//! block became read only
				uint64_t	invalidation_timestamp;
				
				//! the precalculated max reacheable size for the block
				uint64_t	max_reacheable_size;
				
				//! block path
				std::string path;
			};
			
			
			//! Abstraction of the vfs driver
			/*!
			 This class represent the abstraction of the
			 work to do on chaos VFS
			 */
			class VFSDriver : public chaos::NamedService {
			protected:
				//protected constructor
				VFSDriver(std::string alias);

				//! perit to the subclass to set the flags on data block
				int _setFlag(DataBlock *data_block, unsigned int flags);
				
            public:
				
				//! public destructor
				virtual ~VFSDriver();
				
				//! open a block of a determinated type with
				/*!
				 The returned block is a struct that desbribe a data block
				 can we can write or read within.
				 \param type the type of the block we want to open
				 \param path the path relative to the type of the block
				 \flag the flag for determinate the mode if the open operation
				 \
				 */
				virtual int openBlock(BlockType type, std::string path, unsigned int flag, DataBlock *data_block) = 0;

				//! set the limits for the block
				/*!
				 For every block, size and time limit can be set. According to
				 max size and live time, when one of these are reached the block
				 is tagged as read only.
				 */
				virtual int setBlockLimits(DataBlock *data_block,  uint32_t max_size, uint32_t max_live_time) = 0;
				
				//! return all block of data found on the path, acocrding to the type
				virtual int listBlock(BlockType type, std::string path, boost::ptr_vector<DataBlock>& bloks_found) = 0;
				
				//! close the block of data
				virtual int closeBlock(DataBlock *data_block) = 0;

				//! write an amount of data into a DataBlock
                virtual int write(DataBlock *data_block, void * data, uint32_t data_len) = 0;
				
				//! read an amount of data from a DataBlock
                virtual int read(DataBlock *data_block, uint64_t offset, void * * data, uint32_t *data_len) = 0;
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__VFSDriver__) */
