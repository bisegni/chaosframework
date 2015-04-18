/*
 *	AbstractDataAccess.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyrigh 2015 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__AbstractDataAccess__
#define __CHAOSFramework__AbstractDataAccess__

#include <chaos/common/utility/NamedService.h>

#include <boost/format.hpp>

#define DECLARE_DA_NAME static const char * const name;
#define DEFINE_DA_NAME(x) const char * const x::name=#x;

namespace chaos {
    namespace service_common {
        namespace persistence {
            namespace data_access {

#define DATA_ACCESS_LOG_ENTRY(x, i)\
x ": %" #i "%"

#define DATA_ACCESS_NEXT_ENTRY()\
"\n"

#define DATA_ACCESS_LOG_VALUE(v)\
% v

#define DATA_ACCESS_LOG_1_ENTRY(x,v)\
DATA_ACCESS_LOG_ENTRIES(DATA_ACCESS_LOG_ENTRY(x,1), DATA_ACCESS_LOG_VALUE(v))

#define DATA_ACCESS_LOG_2_ENTRY(x_1, x_2, v_1, v_2)\
DATA_ACCESS_LOG_ENTRIES(DATA_ACCESS_LOG_ENTRY(x_1,1)\
                        DATA_ACCESS_NEXT_ENTRY()\
                        DATA_ACCESS_LOG_ENTRY(x_2,2),\
                        DATA_ACCESS_LOG_VALUE(v_1)\
                        DATA_ACCESS_LOG_VALUE(v_2))

#define DATA_ACCESS_LOG_3_ENTRY(x_1, x_2, x_3, v_1, v_2, v_3)\
DATA_ACCESS_LOG_ENTRIES(DATA_ACCESS_LOG_ENTRY(x_1,1)\
                        DATA_ACCESS_NEXT_ENTRY()\
                        DATA_ACCESS_LOG_ENTRY(x_2,2)\
                        DATA_ACCESS_NEXT_ENTRY()\
                        DATA_ACCESS_LOG_ENTRY(x_3,3),\
                        DATA_ACCESS_LOG_VALUE(v_1)\
                        DATA_ACCESS_LOG_VALUE(v_2)\
                        DATA_ACCESS_LOG_VALUE(v_3))

#define DATA_ACCESS_LOG_ENTRIES(entries,values)\
boost::str(boost::format(entries) values)

                    //! Data Access base class
                /*!
                 The data access are those class the directly work on data.
                 Every implementation will perform the data operation
                 on different persistence engine. The sublclass of first level need to define
                 the api on the data and the second level subclass needs to define the implementation
                 */
                class AbstractDataAccess:
                public chaos::common::utility::NamedService {

                protected:
                    std::string log_message(const std::string& section,
                                            const std::string& subsection,
                                            const std::string& log_message);

                public:
                    AbstractDataAccess(const std::string& name);
                    virtual ~AbstractDataAccess();
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__AbstractDataAccess__) */
