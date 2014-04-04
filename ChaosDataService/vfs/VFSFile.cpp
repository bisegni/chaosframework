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


#include <string>
#include "VFSFile.h"

using namespace chaos::data_service::vfs;

VFSFile::VFSFile(std::string vfs_fpath) {
	vfs_file_info.vfs_fpath = vfs_fpath;
}

VFSFile::~VFSFile() {
	
}

const VFSFileInfo *VFSFile::getVFSFileInfo() const {
	return &vfs_file_info;
}

bool VFSFile::exist() {
	//index_driver_ptr->
	return true;
}

int VFSFile::write(void *data, uint32_t data_len) {
	return 0;
};

