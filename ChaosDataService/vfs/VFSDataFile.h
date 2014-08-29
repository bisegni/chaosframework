/*
 *	VFSDataFile.h
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

#ifndef __CHAOSFramework__VFSDataFile__
#define __CHAOSFramework__VFSDataFile__

#include "VFSFile.h"

namespace chaos {
	namespace data_service {
		namespace vfs {
			//forward declaration
			class VFSManager;
			
			typedef enum VFSDataFileOpenMode {
				VFSDataFileOpenModeRead = VFSFileOpenModeRead,
				VFSDataFileOpenModeWrite = VFSFileOpenModeWrite
			} VFSDataFileOpenMode;
			
			//! Abstract the managment of the data file
			/*!
			 The data file is the virtual file, identified by different phisical blocks[files], where
			 data pack are stored. A virtual datafile is asosciated to a unique instrument identification
			 code.
			 */
			class VFSDataFile : public VFSFile {
				friend class VFSManager;
				
			protected:
				//! default ocnstructor for vfs file
				/*!
				 Allcoata a new file class for the absraction of the data bloc managment
				 */
				VFSDataFile(storage_system::StorageDriver *_storage_driver_ptr,
							chaos_index::IndexDriver *_index_driver_ptr,
							std::string data_vfs_relative_path,
							VFSDataFileOpenMode _open_mode);
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__VFSStageFile__) */
