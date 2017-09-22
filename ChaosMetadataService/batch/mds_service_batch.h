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
#ifndef CHAOSFramework_mds_service_batch_h
#define CHAOSFramework_mds_service_batch_h

#include "MDSBatchExecutor.h"
#include "MDSBatchCommand.h"

#define DECLARE_MDS_COMMAND_ALIAS \
public:\
static const std::string command_alias;\
private:


#define DEFINE_MDS_COMAMND_ALIAS(class)\
const std::string class::command_alias = #class;

#define GET_MDS_COMMAND_ALIAS(class)\
class::command_alias

#endif
