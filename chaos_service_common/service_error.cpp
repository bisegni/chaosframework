/*
 *	service_error.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 07/07/16 INFN, National Institute of Nuclear Physics
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

#include <chaos_service_common/service_error.h>

using namespace chaos::service_common::ErrorMDSCode;

CHAOS_DEFINE_ERROR_CODE_MAPPING(EC_MDS_NODE_BAD_NAME,               -10000, "Bad name format for node");
CHAOS_DEFINE_ERROR_CODE_MAPPING(EC_MDS_NODE_BAD_PATH_EXTRACTION,    -10001, "Node name cannot be extract from parent path");
CHAOS_DEFINE_ERROR_CODE_MAPPING(EC_MDS_PARENT_NODE_NOT_IN_DOMAIN,   -10002, "The parent path has not been found on domain");
CHAOS_DEFINE_ERROR_CODE_MAPPING(EC_MDS_NODE_DUPLICATION,            -10002,  "There another node with same name");

