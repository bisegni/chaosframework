/*
 *	VFSFile.cpp
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

#include "VFSFile.h"

#include <string>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/utility/TimingUtil.h>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#define VFSFile_LOG_HEAD "[VFSFile] - " << vfs_file_info.vfs_fpath << " - "
#define VFSF_LAPP_ LAPP_ << VFSFile_LOG_HEAD
#define VFSF_LDBG_ LDBG_ << VFSFile_LOG_HEAD << __FUNCTION__ << " - "
#define VFSF_LERR_ LERR_ << VFSFile_LOG_HEAD << __FUNCTION__ << " - "


using namespace chaos::data_service::vfs;

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
VFSFile::VFSFile(storage_system::StorageDriver *_storage_driver_ptr,
				 chaos_index::DBDriver *_db_driver_ptr,
				 std::string area,
				 std::string vfs_fpath,
				 int _open_mode):
open_mode(_open_mode),
last_wr_ts(0),
storage_driver_ptr(_storage_driver_ptr),
db_driver_ptr(_db_driver_ptr),
current_data_block(NULL),
current_journal_data_block(NULL) {
	//check the path if his prefix is not equal to area (omit duplcieted name in path for prefix)
	if(vfs_fpath.substr(0, area.size()).find(area, 0) == std::string::npos) {
		vfs_file_info.vfs_fpath = area + "/" +vfs_fpath;
	} else {
		//the path in the prefix has the area
		vfs_file_info.vfs_fpath = vfs_fpath;
	}
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
VFSFile::~VFSFile() {
	if(current_data_block) {
		releaseDataBlock(current_data_block);
		current_data_block = NULL;
	}
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int VFSFile::getNewDataBlock(DataBlock **new_data_block_handler) {
	DataBlock *new_data_block_ptr = NULL;
	std::string block_unique_path = boost::str(boost::format("%1%/%2%") % vfs_file_info.vfs_fpath % chaos::UUIDUtil::generateUUID());
	
	//open new block has writeable
	if(storage_driver_ptr->openBlock(block_unique_path, block_flag::BlockFlagWriteble, &new_data_block_ptr) || !new_data_block_ptr) {
		return -1;
	}
	
	//compute data block limits for this vfs configuraiton
	new_data_block_ptr->invalidation_timestamp = TimingUtil::getTimeStamp() + vfs_file_info.max_block_lifetime;
	new_data_block_ptr->max_reacheable_size = vfs_file_info.max_block_size;
	
	//ask to db wat is the next datablock
	if(db_driver_ptr->vfsAddNewDataBlock(this, new_data_block_ptr, data_block_state::DataBlockStateAquiringData)) {
		storage_driver_ptr->closeBlock(new_data_block_ptr);
		return -2;
	}
	*new_data_block_handler = new_data_block_ptr;
	DEBUG_CODE(VFSF_LDBG_ << "Opened datablock of path " << new_data_block_ptr->vfs_path);
	return 0;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int VFSFile::updateDataBlockState(data_block_state::DataBlockState state) {
	int err = 0;
	if((err = db_driver_ptr->vfsSetStateOnDataBlock(this, current_data_block, state))) {
		VFSF_LDBG_ << "Error setting state on datablock with error " << err;
	}
	return 0;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int VFSFile::releaseDataBlock(DataBlock *data_block_ptr, int closed_state) {
	if(!data_block_ptr) return 0;
	int err = 0;
	DEBUG_CODE(VFSF_LDBG_ << "Release datablock of path " << data_block_ptr->vfs_path);
	//write on index for free of work block
	if((err = db_driver_ptr->vfsSetStateOnDataBlock(this, data_block_ptr, closed_state))) {
		VFSF_LERR_ << "Error setting state on datablock with error " << err;
	} else if ((err = db_driver_ptr->vfsUpdateDatablockCurrentWorkPosition(this, data_block_ptr))) {
		VFSF_LERR_ << "Error setting work location on datablock with error " << err;
	} else if ((err = db_driver_ptr->vfsSetHeartbeatOnDatablock(this, data_block_ptr, chaos::TimingUtil::getTimeStamp()))) {
		VFSF_LERR_ << "Error setting heartbeat on datablock with error " << err;
	}
	//close the block
	return storage_driver_ptr->closeBlock(data_block_ptr);
}


/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int VFSFile::giveHeartbeat(uint64_t hb_time) {
	int err = 0;
	if(current_data_block && (err = db_driver_ptr->vfsSetHeartbeatOnDatablock(this, current_data_block, hb_time))) {
		VFSF_LERR_ << "Error setting heartbeaat wit error " << err << " on datablock " << current_data_block->vfs_path;
	}
	return err;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
bool VFSFile::isDataBlockValid(DataBlock *data_block_ptr) {
	if(!data_block_ptr) return false;
	
	//check operational value
	bool is_valid = data_block_ptr->invalidation_timestamp > chaos::TimingUtil::getTimeStamp();
	is_valid = is_valid && data_block_ptr->current_work_position < data_block_ptr->max_reacheable_size;
	return is_valid;
}


/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
const VFSFileInfo *VFSFile::getVFSFileInfo() const {
	return &vfs_file_info;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
FileLocationPointer VFSFile::getCurrentFileLocation() {
	return FileLocationPointer(current_data_block, current_data_block == NULL ?-1:current_data_block->current_work_position);
}


/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
bool VFSFile::isGood() {
	return good;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
bool VFSFile::exist() {
	int err = 0;
	bool exist_flag = false;
	if((err = db_driver_ptr->vfsFileExist(this, exist_flag))) {
		VFSF_LERR_ << "Error checking file existance with code " << err;
		return false;
	}
	return exist_flag;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int VFSFile::seekOnCurrentBlock(block_seek_base::BlockSeekBase base_direction, int64_t offset, bool gp) {
	if(!current_data_block) return -1;
	if(gp) {
		return storage_driver_ptr->seekp(current_data_block, offset, base_direction);
	} else {
		return storage_driver_ptr->seekg(current_data_block, offset, base_direction);
	}
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int VFSFile::write(void *data, uint32_t data_len) {
	int err = 0;
	if(open_mode != VFSFileOpenModeWrite  ||
	   (err = storage_driver_ptr->write(current_data_block, data, data_len))) {
		VFSF_LERR_ << "Error writing with error " << err << " on datablock " << current_data_block->vfs_path;
	}else {
		//update the current work position
		current_data_block->current_work_position += data_len;
	}
	return err;
};

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int VFSFile::read(void *buffer, uint32_t buffer_len) {
	if(!current_data_block) return -1;
	int err = 0;
	uint32_t readed_byte = 0;
	//read the requested byte size
	if(open_mode != VFSFileOpenModeRead  ||
	   (err = storage_driver_ptr->read(current_data_block, buffer, buffer_len, readed_byte))){
		VFSF_LERR_ << "Error reading with error " << err << " on datablock " << current_data_block->vfs_path;
	} else {
		//update current size field
		current_data_block->current_work_position += readed_byte;
	}
	return readed_byte;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int VFSFile::closeCurrentDataBlock(int closed_state) {
	if(!current_data_block) return 0;
	int err = 0;
	
	//we need also to delete the journal
	if(current_journal_data_block) {
		if((err = closeJournalDatablock(current_journal_data_block))) {
			VFSF_LERR_ << "Error closing journal file " << current_data_block->vfs_path << " with error" << err;
		}else{
			current_journal_data_block = NULL;
		}
	}
	
	if((err = releaseDataBlock(current_data_block, closed_state))) {
		VFSF_LERR_ << "Error closing datablock " << current_data_block->vfs_path << " with error" << err;
	} else {
		current_data_block = NULL;
	}
	return err;
}


int VFSFile::writeKeyValueToJournal(std::string journal_key, std::string journal_value) {
	int err = 0;
	if((err = openKeyToJournal(journal_key))) {
	} else if((err = appendValueToJournal(journal_value))) {
	} else if((err = closeKeyToJournal(journal_key))) {
	}
	return  err;
}

//! write text key value to the journal file
int VFSFile::openKeyToJournal(std::string journal_key) {
	int err = 0;
	if(!current_journal_data_block) return err;
	std::string journal_row = boost::str(boost::format(":%1%:") % journal_key);
	if((err = storage_driver_ptr->write(current_journal_data_block, (void*)journal_row.c_str(), (uint32_t)journal_row.size()))) {
		VFSF_LERR_ << "Error writing on journal file " << err;
	}
	return err;

}

//! write text key value to the journal file
int VFSFile::appendValueToJournal(std::string journal_value) {
	int err = 0;
	if(!current_journal_data_block) return err;
	std::string journal_row = boost::str(boost::format("%1%:") % journal_value);
	if((err = storage_driver_ptr->write(current_journal_data_block, (void*)journal_row.c_str(), (uint32_t)journal_row.size()))) {
		VFSF_LERR_ << "Error writing on journal file " << err;
	}
	return err;

}

//! write the clouse information for the key
int VFSFile::closeKeyToJournal(std::string journal_key) {
	int err = 0;
	if(!current_journal_data_block) return err;
	std::string journal_row = boost::str(boost::format("%1%:\n") % journal_key);
	if((err = storage_driver_ptr->write(current_journal_data_block, (void*)journal_row.c_str(), (uint32_t)journal_row.size()))) {
		VFSF_LERR_ << "Error writing on journal file " << err;
	} else if((err = storage_driver_ptr->flush(current_journal_data_block))) {
		VFSF_LERR_ << "Error flushing journal " << err;
	}
	return err;

}

int VFSFile::prefetchData() {
	return 0;
}

//! return the presen of a datablock
int VFSFile::hasData(bool& has_data) {
	has_data = current_data_block != NULL;
	return 0;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int VFSFile::syncCurrentOffsetToJournal() {
	int err = 0;
	if(current_data_block && current_journal_data_block) {
		if((err = syncWorkPositionFromDatablockAndJournal(current_data_block, current_journal_data_block))) {
			//error creating journal
			VFSF_LERR_ << "Error syncronizing journal file " << err;
		}
	}
	return err;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int VFSFile::mantain(int closing_state) {
	int err = 0;
	if(!isDataBlockValid(current_data_block)) {
		if((err = closeCurrentDataBlock(closing_state))) {
		VFSF_LERR_ << "Error closing datablock " << err;
		}
	}
	return err;
}

//--------------------------------------journal api --------------------------------
//! open a journal file for the datablock
int VFSFile::openJournalForDatablock(DataBlock *datablock, DataBlock **current_journal_data_block) {
	int err = 0;
	CHAOS_ASSERT(datablock)
	
	std::string jblock_apth = boost::str(boost::format("%1%.journal") % datablock->vfs_path);
	
	//open new block has writeable
	if((err = storage_driver_ptr->openBlock(jblock_apth, block_flag::BlockFlagWriteble, current_journal_data_block))|| !*current_journal_data_block) {
		VFSF_LERR_ << "Error Opening journal file " << err;
	}
	return err;
}

//! close the journal file for the datablock
int VFSFile::closeJournalDatablock(DataBlock *current_journal_data_block) {
	int err = 0;
	if(!current_journal_data_block) return err;
	std::string jblock_apth = current_journal_data_block->vfs_path;
	//open new block has writeable
	if((err = storage_driver_ptr->closeBlock(current_journal_data_block))) {
		VFSF_LERR_ << "Error closing journal file " << err;
	} else  if((err = storage_driver_ptr->deletePath(jblock_apth))) {
		VFSF_LERR_ << "Error deleting journal file at " << jblock_apth << " with error " << err;
	}
	return err;
}

//! sync location of master datablock on journal
int VFSFile::syncWorkPositionFromDatablockAndJournal(DataBlock *datablock, DataBlock *current_journal_data_block) {
	int err = 0;
	CHAOS_ASSERT(datablock)
	if(!current_journal_data_block) return err;
	if((err = writeKeyValueToJournal("offset", boost::lexical_cast<std::string>(datablock->current_work_position)))){
		VFSF_LERR_ << "Error writing information on journal " << err;
	} else if((err = storage_driver_ptr->flush(current_journal_data_block))) {
		VFSF_LERR_ << "Error flushing journal " << err;
	}
	return 0;
}

//! check if the journl is present
int VFSFile::journalIsPresent(DataBlock *current_journal_data_block, bool &presence) {
	int err = 0;
	if(!current_journal_data_block) return err;
	if((err = storage_driver_ptr->blockIsPresent(current_journal_data_block, presence))) {
		VFSF_LERR_ << "Error checking file existance with code " << err;
		
	}
	return err;
}
