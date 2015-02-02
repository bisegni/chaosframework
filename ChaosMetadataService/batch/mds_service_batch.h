/*
 *	mds_service_batch.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
#ifndef CHAOSFramework_mds_service_batch_h
#define CHAOSFramework_mds_service_batch_h

#include "MDSBatchExecutor.h"
#include "MDSBatchCommand.h"

#define DECLARE_MDS_COMMAND_ALIAS \
public:\
static const char * const command_alias;\
private:


#define DEFINE_MDS_COMAMND_ALIAS(class)\
const char * const class::command_alias = #class;

#define GET_MDS_COMMAND_ASLIAS(class)\
class::command_alias

#endif
