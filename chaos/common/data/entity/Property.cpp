/*
 *	Property.cpp
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

#include "Property.h"


Property::Property(int32_t entityID, edb::EntityDB::KeyIdAndValue _eyValueInfo) {
    
}

/*
 The subclasses need to implement this method for persister their information
 on entity db
 */
void Property::storeData(EntityDB& eDB) throw (CException) {
    
}

/*
 The subclasses need to implement this method for read their information
 on entity db
 */
void Property::readData(EntityDB& eDB) throw (CException) {
    
}
