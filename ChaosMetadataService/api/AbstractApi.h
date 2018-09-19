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
#ifndef CHAOSFramework_AbstractApi_h
#define CHAOSFramework_AbstractApi_h
#include "../mds_types.h"
#include "../persistence/persistence.h"
#include "../batch/batch_impl.h"

#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/global.h>
namespace chaos {
    namespace metadata_service {
        namespace api {
            
#define GET_DATA_ACCESS(x,v, err)\
x *v = getPersistenceDriver()->getDataAccess<x>();\
if(v == NULL) throw CException(err, "Error allocating " #x, __PRETTY_FUNCTION__);
            
#define MOVE_STRING_VALUE(k, src, dst)\
if(src->hasKey(k)) {\
dst->addStringValue(k, src->getStringValue(k));\
}
            
#define MERGE_STRING_VALUE(k, src, src2, dst)\
if(src2->hasKey(k)) {\
dst->addStringValue(k, src2->getStringValue(k));\
} else {\
MOVE_STRING_VALUE(k, src, dst)\
}
            
#define MOVE_INT32_VALUE(k, src, dst)\
if(src->hasKey(k)) {\
dst->addInt32Value(k, src->getInt32Value(k));\
}
            
#define CHAOS_MDS_DEFINE_API_CLASS(name)\
class name:\
public AbstractApi {\
protected:\
public:\
    name();\
    ~name();\
    chaos::common::data::CDWUniquePtr execute(chaos::common::data::CDWUniquePtr api_data);\
};
            
#define CHAOS_MDS_DEFINE_API_CLASS_CD(n, a)\
n::n():\
AbstractApi(a){}\
n::~n(){}
            
            
            class AbstractApiGroup;
            
            //! Api abstraction
            /*!
             This class define the rule for the api development
             */
            class AbstractApi:
            public chaos::common::utility::NamedService,
            public chaos::common::utility::InizializableService {
                friend class AbstractApiGroup;
                
                AbstractApiGroup *parent_group;
            protected:
                service_common::persistence::data_access::AbstractPersistenceDriver *getPersistenceDriver();
                batch::MDSBatchExecutor *getBatchExecutor();
                chaos::common::network::NetworkBroker *getNetworkBroker();
            public:
                //! defaukt constructor with the alias of the api
                AbstractApi(const std::string& name);
                
                //d! efault destructor
                virtual ~AbstractApi();
                
                void init(void *init_data);
                
                void deinit();
                
                //! execute the api
                virtual chaos::common::data::CDWUniquePtr execute(chaos::common::data::CDWUniquePtr api_data) = 0;
            };

        }
    }
}

#endif
