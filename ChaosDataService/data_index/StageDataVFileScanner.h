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

#include "../vfs/VFSStageReadableFile.h"

#include <string>

namespace chaos{
    namespace data_service {
		
		//forward declaration
		class StageDataConsumer;
		
		namespace data_index {
			
			class StageDataVFileScanner {
				friend class StageDataConsumer;
				
				vfs::VFSStageReadableFile *stage_file;
			public:
				StageDataVFileScanner(vfs::VFSStageReadableFile *_stage_file);
				~StageDataVFileScanner();
				
				std::string getScannedVFSPath();
				//! scan an entire block of the stage file
				int scan();
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__StageDataVFileScanner__) */
