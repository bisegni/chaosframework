/*
 *	VFSStageFile.cpp
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

#include "VFSStageFile.h"

#include <boost/format.hpp>
using namespace chaos::data_service::vfs;

VFSStageFile::VFSStageFile(storage_system::StorageDriver *_storage_driver_ptr,
						   chaos_index::DBDriver *_db_driver_ptr,
						   std::string stage_vfs_relative_path,
						   VFSStageFileOpenMode _open_mode):
VFSFile(_storage_driver_ptr,
		_db_driver_ptr,
		VFS_STAGE_AREA,
		stage_vfs_relative_path,
		(int)_open_mode) //superclass constructor
{
	//allocate all thepath for this file
	good = (storage_driver_ptr->createPath(getVFSFileInfo()->vfs_fpath) == 0);
}