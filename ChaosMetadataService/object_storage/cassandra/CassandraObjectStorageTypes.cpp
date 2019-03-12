/*
 * Copyright 2012, 30/11/2018 INFN
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

#if USE_CASSANDRA_DRIVER
#include "CassandraObjectStorageTypes.h"
using namespace chaos::metadata_service::object_storage::cassandra;
//static void cassandraClusterDeallocator(CassCluster* cluster) {cass_cluster_free(cluster);}
//static void cassandraSessionDeallocator(CassSession* session) {cass_session_free(session);}
//static void cassandraFutureDeallocator(CassFuture* future) {cass_future_free(future);}
//static void cassandraPreparedDeallocator(CassPrepared* prepared){cass_prepared_free(prepared);};
#endif