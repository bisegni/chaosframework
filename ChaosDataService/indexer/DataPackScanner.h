/*
 *	DataPackScanner.h
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
#ifndef __CHAOSFramework__DataPackScanner__
#define __CHAOSFramework__DataPackScanner__

#include "../vfs/VFSFile.h"
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
			
			
			//! data pack scanner class
			/*!

			 */
			class DataPackScanner {
				friend class chaos::data_service::StageDataConsumer;
				
				void *data_buffer;
				uint32_t curret_data_buffer_len;

				void grow(uint32_t new_size);
				
			protected:
				
				//file managment class
				vfs::VFSManager *vfs_manager;
				//index driver
				db_system::DBDriver *db_driver;
				
				//!working file (stage or data)
				vfs::VFSFile *working_data_file;
				
				//! abstract method for prcoessing one datapack at time
				virtual int processDataPack(const bson::BSONObj& data_pack) = 0;

				//! handler called befor the scan task
				virtual int startScanHandler() = 0;
				
				//! handler called after the termination of the scan task
				virtual int endScanHandler(int end_scan_error) = 0;
			public:
				DataPackScanner(vfs::VFSManager *_vfs_manager,
								db_system::DBDriver *_db_driver,
								vfs::VFSFile *_working_data_file);
				
				~DataPackScanner();
				
				std::string getScannedVFSPath();
				
				//! scan an entire block of the stage file
				int scan();
				
				//! matains stage
				virtual int mantains() = 0;
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__DataPackScanner__) */
