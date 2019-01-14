/*
 * Copyright 2012, 29/11/2018 INFN
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

#ifndef CassandraObjectStorageTypes_h
#define CassandraObjectStorageTypes_h
#if USE_CASSANDRA_DRIVER
#include <cassandra.h>
#include <chaos/common/chaos_types.h>
namespace chaos {
    namespace metadata_service {
        namespace object_storage {
            namespace cassandra {
                typedef ChaosSharedPtr<CassCluster> CassClusterShrdPtr;
                typedef ChaosSharedPtr<CassSession> CassSessionShrdPtr;
                typedef ChaosSharedPtr<CassFuture> CassFutureShrdPtr;
                typedef ChaosSharedPtr<CassPrepared> CassPreparedShrdPtr;
                
                static void cassandraClusterDeallocator(CassCluster* cluster) {cass_cluster_free(cluster);}
                static void cassandraSessionDeallocator(CassSession* session) {cass_session_free(session);}
                static void cassandraFutureDeallocator(CassFuture* future) {cass_future_free(future);}
                static void cassandraPreparedDeallocator(CassPrepared* prepared){cass_prepared_free(prepared);};
                
#define ALLOCATE_CLUSTER() CassClusterShrdPtr(cass_cluster_new(), &cassandraClusterDeallocator)
#define ALLOCATE_SESSION() CassSessionShrdPtr(cass_session_new(), &cassandraSessionDeallocator)
#define MAKE_MANAGED_FUTURE(n,x) CassFutureShrdPtr n = CassFutureShrdPtr(x, &cassandraFutureDeallocator)
#define MAKE_MANAGED_PREPARED(n,x) CassPreparedShrdPtr n = CassPreparedShrdPtr(x, &cassandraPreparedDeallocator)
                
            }
        }
    }
}
#endif
#endif /* CassandraObjectStorageTypes_h */
