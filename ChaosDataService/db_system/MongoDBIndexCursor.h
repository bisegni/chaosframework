/*
 *	MongoDBIndexCursor.h
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
#ifndef __CHAOSFramework__MongoDBIndexCursor__
#define __CHAOSFramework__MongoDBIndexCursor__

#include "DBIndexCursor.h"
#include "mongo_db_types.h"

#include <mongo/client/dbclient.h>

namespace chaos {
	namespace data_service {
		namespace db_system {
			
			class MongoDBDriver;
			
			/*!
			 Implement the index cursor on mongodb
			 */
			class MongoDBIndexCursor : public DBIndexCursor {
				friend class MongoDBDriver;
				
				std::auto_ptr<mongo::DBClientCursor> cursor;
				
				//! private constructor
				MongoDBIndexCursor(DBDriver *_driver_ptr,
								   std::auto_ptr<mongo::DBClientCursor> _cursor);
			public:
				
				//! return true if there are othere index to fetch
				/*!
				 \return true is there are other result to fetch
				 */
				bool hasNext();
				
				//! return next index
				/*!
				 \return the location on virtual filesystem of the found data pack
				 */
				chaos::data_service::vfs::PathFileLocation *getIndex();
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__MongoDBIndexCursor__) */
