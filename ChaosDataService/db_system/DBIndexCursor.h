/*
 *	DBIndexCursor.h
 *	!CHOAS
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

#ifndef __CHAOSFramework__DBCursor__
#define __CHAOSFramework__DBCursor__

#include "db_system_types.h"

namespace chaos {
	namespace data_service {
		namespace db_system {
			
			class DBDriver;
			
			//! database cursor abstraction
			/*!
			 This class represent the abstraction for the result
			 of query on chaos file offset indexes
			 */
			class DBIndexCursor {
				friend class DBDriver;
			protected:
				//! the total number of ellement that are found by qury
				uint64_t element_found;
				
				//! the number of element fetched from the database
				uint64_t number_of_element_fetched;
				
				//!
				uint32_t result_page_dimension;
				
				//!
				uint32_t total_page_number;
				
				//!
				uint32_t current_page;

				
				//!
				DBDriver *driver_ptr;
				
				//! the query to pperform
				DataPackIndexQuery query;
			public:
				//! private constructor
				DBIndexCursor(DBDriver *_driver_ptr,
							  const DataPackIndexQuery& _query);
				
				virtual ~DBIndexCursor();
				
				//!
				uint32_t getResultPageDimension();
			
				//!
				uint64_t getNumberOfElementFound();
				
				//!
				uint64_t getNumberOfElementFetched();
				
				//!
				uint32_t getTotalPage();

				//!
				uint32_t getCurrentPage();
				
				//!
				void setResultPageDimension(uint32_t);
				
				//! return true if there are othere index to fetch on current page
				/*!
				 \return true is there are other result to fetch
				 */
				virtual bool hasNext() = 0;
				
				//! perform the next page query
				virtual int performNextPagedQuery() = 0;
				
				//! return next index
				/*!
				 \return the location on virtual filesystem of the found data pack
				 */
				virtual DataPackIndexQueryResult *getIndex() = 0;
			};
			
		}
	}
}
#endif /* defined(__CHAOSFramework__DBCursor__) */
