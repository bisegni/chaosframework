/*
 *	MongoDBIndexCursor.cpp
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

#include "MongoDBIndexCursor.h"

using namespace chaos::data_service::db_system;

//! private constructor
MongoDBIndexCursor::MongoDBIndexCursor(DBDriver *_driver_ptr,
									   std::auto_ptr<mongo::DBClientCursor> _cursor):
DBIndexCursor(_driver_ptr),
cursor(_cursor) {

}

//! return true if there are othere index to fetch
bool MongoDBIndexCursor::hasNext() {
	return cursor->more();
}

//! return next index
chaos::data_service::vfs::PathFileLocation *MongoDBIndexCursor::getIndex() {
	mongo::BSONObj next_element = cursor->next();
	std::string block_path = next_element.getField(MONGO_DB_IDX_DATA_PACK_DATA_BLOCK_DST_PATH).String();
	uint64_t block_offset = (uint64_t)next_element.getField(MONGO_DB_IDX_DATA_PACK_DATA_BLOCK_DST_OFFSET).Long();
	return new chaos::data_service::vfs::PathFileLocation(block_path, block_offset);
}