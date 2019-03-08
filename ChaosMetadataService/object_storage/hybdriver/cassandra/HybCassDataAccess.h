/*
 * Copyright 2012, 25/05/2018 INFN
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

#ifndef HybCassDataAccess_hpp
#define HybCassDataAccess_hpp

#include "../HybBaseDataAccess.h"
#include "HybCassandraDataType.h"

namespace chaos {
    namespace metadata_service {
        namespace object_storage {
            namespace hybdriver {
                namespace cassandra {
                    
                    class HybCassDataAccess:
                    public HybBaseDataAccess {
                        CassSessionShrdPtr session_shrd_ptr;
                        CassPreparedShrdPtr insert_daq_prepared;
                        CassPreparedShrdPtr get_daq_prepared;
                    protected:
                        int storeData(const std::string& key,
                                      const int64_t& shard_value,
                                      const CInt64& run_id,
                                      const CInt64& seq_id,
                                      const common::data::CDataWrapper& object);
                        
                        int retrieveData(const std::string& key,
                                         const int64_t& shard_value,
                                         const CInt64& run_id,
                                         const CInt64& seq_id,
                                         chaos::common::data::CDWUniquePtr& object);
                    public:
                        HybCassDataAccess(CassSessionShrdPtr& _cass_sess_shrd_ptr);
                    };
                }
            }
        }
    }
}

#endif /* HybCassDataAccess_hpp */
