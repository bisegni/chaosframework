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
#ifndef CHAOSFramework_api_proxy_h
#define CHAOSFramework_api_proxy_h

#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>
#include <chaos_metadata_service_client/api_proxy/ApiProxyManager.h>

//---include all api header
#include <chaos_metadata_service_client/api_proxy/agent/agent.h>
#include <chaos_metadata_service_client/api_proxy/node/node.h>
#include <chaos_metadata_service_client/api_proxy/groups/groups.h>
#include <chaos_metadata_service_client/api_proxy/script/script.h>
#include <chaos_metadata_service_client/api_proxy/logging/logging.h>
#include <chaos_metadata_service_client/api_proxy/service/service.h>
#include <chaos_metadata_service_client/api_proxy/data_service/data_service.h>
#include <chaos_metadata_service_client/api_proxy/control_unit/control_unit.h>
#include <chaos_metadata_service_client/api_proxy/unit_server/unit_server.h>

//add all service common data
#include <chaos_service_common/data/data.h>

#endif
