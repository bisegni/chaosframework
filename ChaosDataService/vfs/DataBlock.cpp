/*
 *	DataBlock.cpp
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

#include "DataBlock.h"


using namespace chaos::data_service::vfs;

DataBlock::DataBlock(std::string path):fs_path(NULL) {
	if(path.size() > 0) {
		fs_path = (char*)malloc(sizeof(char) * path.size());
		std::memcpy(fs_path, path.c_str(), path.size());
	}
}

DataBlock::~DataBlock() {
	if(fs_path) free(fs_path);
}