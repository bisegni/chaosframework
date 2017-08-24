/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
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
        public:
            const edb::KeyIdAndValue& getKeyInfo(){return _keyValueInfo;};
        };
        
    }
    
}

#endif
