/*
 *	VFSDataWriteableFile.cpp
 *	!CHOAS
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

#include "VFSDataWriteableFile.h"
#include <chaos/common/utility/TimingUtil.h>
using namespace chaos::data_service::vfs;

#define VFSDataWriteableFile_LOG_HEAD "[VFSDataWriteableFile] - " << getVFSFileInfo()->vfs_fpath << " - "
#define VFSWF_LAPP_ LAPP_ << VFSDataWriteableFile_LOG_HEAD
#define VFSWF_LDBG_ LDBG_ << VFSDataWriteableFile_LOG_HEAD
#define VFSWF_LERR_ LERR_ << VFSDataWriteableFile_LOG_HEAD <<__LINE__<< " - "


/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
VFSDataWriteableFile::VFSDataWriteableFile(storage_system::StorageDriver *_storage_driver_ptr,
                                           db_system::DBDriver *_db_driver_ptr,
                                           std::string data_vfs_relative_path):
VFSDataFile(_storage_driver_ptr,
            _db_driver_ptr,
            data_vfs_relative_path,
            VFSDataFileOpenModeWrite) {
    
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int VFSDataWriteableFile::releaseDataBlock(DataBlock *data_block_ptr,
                                           int closed_state) {
    int err = 0;
    std::string db_vfs_domain = data_block_ptr->vfs_domain;
    std::string db_vfs_path = data_block_ptr->vfs_path;
    if((err = VFSDataFile::releaseDataBlock(data_block_ptr, closed_state))){
        VFSWF_LERR_ << "Error closing processed datafile state on all datablock possible index " << err;
    }
    
    if((err = db_driver_ptr->idxSetDataPackIndexStateByDataBlock(db_vfs_domain,
                                                                 db_vfs_path,
                                                                 db_system::DataPackIndexQueryStateQuerable))) {
        VFSWF_LERR_ << "Error setting querable state on all datablock possible index " << err;
    }
    return err;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
bool VFSDataWriteableFile::isDataBlockValid(DataBlock *new_data_block_ptr) {
    //check operational value
    return VFSDataFile::isDataBlockValid(new_data_block_ptr);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int VFSDataWriteableFile::switchDataBlock(bool close_only) {
    int err = 0;
    if(current_data_block) {
        //close data block setting his state to querable because
        if((err = releaseDataBlock(current_data_block, data_block_state::DataBlockStateQuerable))){
            VFSWF_LERR_ << "Error releaseing datablock " << err;
        }
    }
    if(!close_only) {
        //! we need also to allocate new one datablock
        if((err = getNewDataBlock(&current_data_block))) {
            VFSWF_LERR_ << "Error creating datablock " << err;
        }
    }
    return err;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int VFSDataWriteableFile::ensureDatablock(bool force_switch) {
    if(!current_data_block ||
       force_switch) {
        return switchDataBlock();
    }
    return 0;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int VFSDataWriteableFile::write(void *data, uint32_t data_len) {
    //check validation
    int err = 0;
    
    //write data before fiel validation
    //this is needed because data writeable file is used into index and
    //the write is done after got the file location for indexing purphose
    //For this reason, some times, the datapack is wrote in a new chunk
    //different for one that as been used for get the future datapack position
    if((err = VFSDataFile::write(data, data_len))) {
        VFSWF_LERR_ << "Error writing on datablock " << err;
        return err;
    }
    
    //check if datablock is still valid
    if(!isDataBlockValid(current_data_block)) {
        //datablock is not more valid so switch a new block
        if((err = switchDataBlock())) {
            VFSWF_LERR_ << "Error switching datablock " << err;
        }
    }
    return err;
}