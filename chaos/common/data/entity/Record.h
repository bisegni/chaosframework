/*
 *	Record.h
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

#ifndef CHAOSFramework_Record_h
#define CHAOSFramework_Record_h

#include <chaos/common/data/entity_db/EntityDB.h>

namespace chaos{
    
    //forward declaration
    class CException;
    
    /*!
     The namespace entity group all code for the managment of a the abstract and specified chaos entity.
     */
    namespace entity {
        
        /*!
         Base class for all Record that need to be persisted on EntityDB Implementation
         */
        class Record {
            friend class chaos::edb::EntityDB;
        protected:
            
            edb::EntityDB *database;
            
            //! primary key of the record
            int32_t _id;
            
            //! information about the key of the current entity db record
            edb::KeyIdAndValue _keyValueInfo;
            
            Record(edb::EntityDB *_database):database(_database){};
            
            virtual ~Record(){};
        };
        
    }
    
}

#endif
