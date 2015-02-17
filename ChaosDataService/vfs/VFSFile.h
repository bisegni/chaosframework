/*
 *	VFSFile.h
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
#ifndef __CHAOSFramework__VFSFile__
#define __CHAOSFramework__VFSFile__

#include "VFSTypes.h"
#include "DataBlock.h"
#include "../db_system/DBDriver.h"
#include "storage_system/StorageDriver.h"


namespace chaos {
	namespace data_service {
		namespace db_system {
			class DBDriver;
		}
		namespace vfs {
		
			namespace chaos_index = chaos::data_service::db_system;
			
			
			class VFSManager;
			struct DataBlock;
			
			typedef enum VFSFileOpenMode {
				VFSFileOpenModeRead = 0,
				VFSFileOpenModeWrite = 1
			} VFSFileOpenMode;
						
			//! VFS Logical file
			class VFSFile {
				friend class VFSManager;
				friend class DBDriver;
				//! operational setting for the virtual file
				VFSFileInfo vfs_file_info;
				
				//record last read or write operation timestamp
				uint64_t last_wr_ts;
				
			protected:
				//! track the opening mode
				int open_mode;
				
				//!tag if the file is in good state
				bool good;

				//! Current available data block where read or write
				DataBlock *current_data_block;
				
				//!current journal datablock
				DataBlock *current_journal_data_block;
				
				//!index driver pointer
				chaos_index::DBDriver *db_driver_ptr;
				
				//!storage driver pointer
				storage_system::StorageDriver *storage_driver_ptr;
			
				//! return new datablock where write into
				int getNewDataBlock(DataBlock **new_data_block_handler);
				
				//! change Datablock state
				int updateDataBlockState(data_block_state::DataBlockState state);
				
				//! release a datablock adn set the current work position
				/*!
				 perform some operation befor phisically close the data block setting
				 also the state. 
				 \param data_block_ptr data block to release
				 \param closed_state is the state ater the block is processed
				 */
				virtual int releaseDataBlock(DataBlock *data_block_ptr, int closed_state = data_block_state::DataBlockStateNone);
				
				//! check the validity of the datablock(usefull only on write version of the write)
				bool isDataBlockValid(DataBlock *data_block_ptr);
				
				//default consturctor or destructor
				VFSFile(storage_system::StorageDriver *_storage_driver_ptr,
						db_system::DBDriver *_db_driver_ptr,
						std::string area,
						std::string vfs_fpath,
						int _open_mode);
				
				//!destructore
				~VFSFile();
				
				//   journal file managment api
				//! open a journa file for the datablock
				int openJournalForDatablock(DataBlock *datablock, DataBlock **current_journal_data_block);
				
				//! close the journal file for the datablock
				int closeJournalDatablock(DataBlock *current_journal_data_block);
				
				//! sync location of master datablock on journal
				int syncWorkPositionFromDatablockAndJournal(DataBlock *datablock, DataBlock *current_journal_data_block);
				
				//! check if the journl is present
				int journalIsPresent(DataBlock *datablock,  bool &presence);
				
			public:
				//! Get the VFS information for file
				const VFSFileInfo *getVFSFileInfo() const;
				
				//! return the curent location pointed by writeable file
				FileLocationPointer getCurrentFileLocation();
				
				//! Check if the file exists
				bool exist();
				
				//! Return the goodness of the file
				bool isGood();
				
				//! seek on block
				/*!
				 \param gp false = seekg true = seekp
				 */
				virtual int seekOnCurrentBlock(block_seek_base::BlockSeekBase base_direction, int64_t offset, bool gp);
				
				//! write data on the current data block
				virtual int write(void *data, uint32_t data_len);
				
				//! read data from the current data block
				/*!
				 Read the data for the current loaded databloc
				 \param data pointer where put the readed byte
				 \param byte to read
				 \return negative number is something is not gone well,
				 otherwise the number of byte readed
				 */
				virtual int read(void *buffer, uint32_t buffer_len);
				
				//! give heartbeat on current datablock
				int giveHeartbeat(uint64_t hb_time = 0);
				
				//!Synck the journal with current block state
				virtual int syncCurrentOffsetToJournal();
				
				//! write text key value to the journal file
				int writeKeyValueToJournal(std::string journal_key, std::string journal_value);

				//! write text key value to the journal file
				int openKeyToJournal(std::string journal_key);
				
				//! write text key value to the journal file
				int appendValueToJournal(std::string journal_value);
				
				//! write the clouse information for the key
				int closeKeyToJournal(std::string journal_key);
				
				//!close the current data block setting the state
				int closeCurrentDataBlock(int closed_state = data_block_state::DataBlockStateNone);
				
				//! perform the matainance of the virtual file
				/*!
				 virtual file has many block that can be opened one at time
				 or many at time(depending on sublcass). Any vfile type
				 as some logic for keep datablock open. With this method a check is
				 done on validity and it is false the current datablock is closed 
                 using a state
                 \param closed_state state to set the closed datablock
				 */
				int mantain(int closed_state = data_block_state::DataBlockStateNone);
				
				//! prefetch needed datablock
				/*!
				  subclass implement this to permit the prefetch of needed datablock
				 */
				virtual int prefetchData();
				
				//! return the presen of a datablock
				virtual int hasData(bool& has_data);
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__VFSFile__) */
