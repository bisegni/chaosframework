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
#ifndef __CHAOSFramework__MongoDBUnitServerDataAccess__
#define __CHAOSFramework__MongoDBUnitServerDataAccess__


#include "MongoDBNodeDataAccess.h"
#include "../data_access/UnitServerDataAccess.h"

#include <chaos/common/utility/ObjectInstancer.h>

#include <chaos_service_common/persistence/mongodb/MongoDBAccessor.h>

#include <boost/shared_ptr.hpp>

namespace chaos {
	namespace metadata_service {
		namespace persistence {
			namespace mongodb {

				//forward declaration
				class MongoDBPersistenceDriver;

                //! Data Access for producer manipulation data
				class MongoDBUnitServerDataAccess:
				public data_access::UnitServerDataAccess,
                protected service_common::persistence::mongodb::MongoDBAccessor {
                    friend class MongoDBPersistenceDriver;

                    MongoDBNodeDataAccess *node_data_access;
				protected:
                    MongoDBUnitServerDataAccess(const ChaosSharedPtr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
					~MongoDBUnitServerDataAccess();
				public:
                    //inherited method
                    int insertNewUS(chaos::common::data::CDataWrapper& unit_server_description, bool check_for_cu_type = true);

                    //inherited method
                    int addCUType(const std::string& unit_server_uid, const std::string& cu_type);

                    //inherited method
                    int removeCUType(const std::string& unit_server_uid, const std::string& cu_type);

                    //inherited method
                    int updateUS(chaos::common::data::CDataWrapper& unit_server_description);

                    //inherited method
                    int checkPresence(const std::string& unit_server_unique_id,
                                      bool& presence);

                    //inherited method
                    int deleteUS(const std::string& unit_server_unique_id);

                    //inherited method
                    int getDescription(const std::string& unit_server_uid,
                                       chaos::common::data::CDataWrapper **unit_server_description);


                    int getUnitserverForControlUnitID(const std::string& control_unit_id,
                                                              std::string& unit_server_host);
				};
			}
		}
	}
}
#endif /* defined(__CHAOSFramework__MongoDBProducerDataAccess__) */
