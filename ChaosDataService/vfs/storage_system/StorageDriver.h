/*
 *	StorageDriver.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__StorageDriver__
#define __CHAOSFramework__StorageDriver__

#include <string>
#include <vector>
#include "../VFSTypes.h"
#include "../DataBlock.h"
#include <map>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/InizializableService.h>

namespace chaos_vfs = chaos::data_service::vfs;

namespace chaos {
    namespace data_service {
        namespace vfs {
            namespace storage_system {
                //! storage driver setting
                typedef struct StorageDriverSetting {
                    //!driver domain name
                    vfs::VFSDomain domain;
                    
                    //! kv param for the implementations of the driver
                    std::map<std::string,std::string> key_value_custom_param;
                    
                    bool log_metric;
                    
                    uint32_t log_metric_update_interval;
                } StorageDriverSetting;
                
                //forward declaration
                class StorageDriverMetricCollector;
                
                //! Abstraction of the vfs driver
                /*!
                 This class represent the abstraction of the
                 work to do on chaos VFS
                 */
                class StorageDriver:
                public common::utility::NamedService,
                public common::utility::InizializableService {
                    friend class StorageDriverMetricCollector;
                protected:
                    
                    //! Driver sepcific configration
                    StorageDriverSetting *setting;
                    
                    //! Protected constructor
                    StorageDriver(std::string alias);
                    
                protected:
                    //! domain initialization
                    /*!
                     Subclass need to implement his domain creation strategy
                     to permit that the same drivers on same storage share the same
                     domain
                     */
                    virtual void initDomain() throw (chaos::CException) = 0;
                    
                public:
                    
                    //! public destructor
                    virtual ~StorageDriver();
                    
                    //! init
                    /*!
                     This implementation need a point to the StorageDriverSetting struct
                     for his configuration;
                     */
                    void init(void *init_data) throw (chaos::CException);
                    
                    //! deinit
                    void deinit() throw (chaos::CException);
                    
                    //! return the storage domain for this driver
                    virtual const std::string getStorageDomain() const;
                    
                    //! Check if the datablock is present
                    /*!
                     Check if a block fisically present on database
                     \param data block to chek
                     */
                    virtual int blockIsPresent(chaos_vfs::DataBlock *data_block, bool &presence) = 0;
                    
                    //! Open a block
                    /*!
                     Open a block filled eith all information to be opened
                     \param type the type of the block we want to open
                     \return error code
                     */
                    virtual int openBlock(chaos_vfs::DataBlock *data_block, unsigned int flags) = 0;
                    
                    //! open a block of a determinated type
                    /*!
                     The returned block is a struct that desbribe a data block
                     can we can write or read within.
                     \param type the type of the block we want to open
                     \param path the path relative to the type of the block
                     \flag the flag for determinate the mode if the open operation it can be a concatenation of chaos::data_service::chaos_vfs::block_flag::DataBlockFlag
                     \
                     */
                    virtual int openBlock(const std::string& vfs_path, unsigned int flags, chaos_vfs::DataBlock **data_block) = 0;
                    
                    //! close the block of data
                    virtual int closeBlock(chaos_vfs::DataBlock *data_block) = 0;
                    
                    //! return the block current size
                    virtual int getBlockSize(chaos_vfs::DataBlock *data_block) = 0;
                    
                    //! write an amount of data into a DataBlock
                    virtual int write(chaos_vfs::DataBlock *data_block, void * data, uint32_t data_len) = 0;
                    
                    //! read an amount of data from a DataBlock
                    virtual int read(chaos_vfs::DataBlock *data_block, void * buffer, uint32_t buffer_len, uint32_t& readed_byte) = 0;
                    
                    //! change the block pointer for read or write
                    virtual int seekp(chaos_vfs::DataBlock *data_block, uint64_t offset, chaos_vfs::block_seek_base::BlockSeekBase base_direction) = 0;
                    
                    //! change the block pointer for read or write
                    virtual int seekg(chaos_vfs::DataBlock *data_block, uint64_t offset, chaos_vfs::block_seek_base::BlockSeekBase base_direction) = 0;
                    
                    //! get the current block data pointer position
                    virtual int tellp(chaos_vfs::DataBlock *data_block, uint64_t& offset) = 0;
                    
                    //! get the current block data pointer position
                    virtual int tellg(chaos_vfs::DataBlock *data_block, uint64_t& offset) = 0;
                    
                    //! truncate the file to the new size
                    virtual int resize(chaos_vfs::DataBlock *data_block, uint64_t new_size) = 0;
                    
                    //! flush data on file
                    virtual int flush(chaos_vfs::DataBlock *data_block) = 0;
                    
                    //! create a directory
                    virtual int createDirectory(const std::string& vfs_path) = 0;
                    
                    //! create a directory
                    virtual int createPath(const std::string& vfs_path) = 0;
                    
                    //! delete a directory
                    virtual int deletePath(const std::string& vfs_path, bool all = false) = 0;
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__StorageDriver__) */
