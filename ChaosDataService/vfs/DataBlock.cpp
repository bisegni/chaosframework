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
#include <stdlib.h>
#include <string.h>

using namespace chaos::data_service::vfs;

DataBlock::DataBlock():
flags(0),invalidation_timestamp(0), max_reacheable_size(0),
current_size(0), creation_time(0), vfs_path(NULL), driver_private_data(NULL) {

}

DataBlock::~DataBlock() {
}
