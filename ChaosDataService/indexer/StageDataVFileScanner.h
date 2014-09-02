/*
 *	StageDataVFileScanner.h
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
#ifndef __CHAOSFramework__StageDataVFileScanner__
#define __CHAOSFramework__StageDataVFileScanner__

#include "DataPackScanner.h"
#include "../vfs/VFSStageReadableFile.h"
#include "../vfs/VFSDataWriteableFile.h"
#include "../db_system/DBDriver.h"

#include <chaos/common/bson/bson.h>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
#include <string>

namespace chaos{
    namespace data_service {
		
		//forward declaration
		class StageDataConsumer;
		
		namespace indexer {
			
			typedef struct DataFileInfo {
				//!  pointer to writeable data file
				vfs::VFSDataWriteableFile *data_file_ptr;
				
				//! mantains track of the latest timestamp stored into datafile
				uint64_t last_wrote_ts;
			} DataFileInfo;
			
			//! Stage data scanner
			/*!
			 this class scan a stage file consuming interanlly storede
			 data pack (BSON) one at one. The default index has the following structure
			 (exposed in json format):
			 {	did:string,
				dts:uint64,
				cpt:{
					chunk_key:chunk id(string)
					chunk_offset:uint64_t
				}
			 }
			 For every datapack it makes:
			 1 - retrive the field necessary to crete a default index
			 2 - save pack into the corresponding data file for the device id
			 3 - store the dafault index into the database using the driver
			 4 - step torward
			 */
			class StageDataVFileScanner : public DataPackScanner {
				friend class chaos::data_service::StageDataConsumer;

				uint64_t last_hb_on_vfile;
				
				//!association between did and his data file
				boost::shared_mutex mutext_did_data_file;
				std::map<std::string, shared_ptr<DataFileInfo> > map_did_data_file;

				
				int startScanHandler();
				
				int processDataPack(const bson::BSONObj& data_pack);
				
				int endScanHandler(int end_scan_error);
				
				boost::shared_ptr<DataFileInfo> getWriteableFileForDID(const std::string& did);
				
				int closeAllDatafile();
			public:
				StageDataVFileScanner(vfs::VFSManager *_vfs_manager,
									  db_system::DBDriver *_db_driver,
									  vfs::VFSStageReadableFile *_working_stage_file);
				
				~StageDataVFileScanner();
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__StageDataVFileScanner__) */
