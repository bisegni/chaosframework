/*
 *	DBIndexCursor.cpp
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

#include "DBIndexCursor.h"

using namespace chaos::data_service::db_system;

#define RESULT_PAGE_DIMENSION 100

//! private constructor
DBIndexCursor::DBIndexCursor(DBDriver *_driver_ptr,
							 const DataPackIndexQuery& _query):
element_found(0),
number_of_element_fetched(0),
result_page_dimension(RESULT_PAGE_DIMENSION),
total_page_number(0),
current_page(0),
driver_ptr(_driver_ptr),
query(_query){
	
}

DBIndexCursor::~DBIndexCursor() {
	
}

uint32_t DBIndexCursor::getResultPageDimension() {
	return result_page_dimension;
}

uint64_t DBIndexCursor::getNumberOfElementFound() {
	return element_found;
}

uint64_t DBIndexCursor::getNumberOfElementFetched() {
	return number_of_element_fetched;
}

void DBIndexCursor::setResultPageDimension(uint32_t _result_page_dimension) {
	result_page_dimension = _result_page_dimension;
}

//!
uint32_t DBIndexCursor::getTotalPage() {
	return total_page_number;
}

//!
uint32_t DBIndexCursor::getCurrentPage() {
	return current_page;
}