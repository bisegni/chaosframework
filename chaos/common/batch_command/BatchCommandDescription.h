/*
 *	BatchCommandDescription.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__BatchCommandDescription__
#define __CHAOSFramework__BatchCommandDescription__

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataWrapper.h>

#include <boost/shared_ptr.hpp>

#include <map>

//macro fro helping the creation of batch command description
#define BATCH_COMMAND_DESCRIPTION(name)\
BatchCommandDescription ## name

#define BATCH_COMMAND_FUNCTION_GET_DESCRIPTION(name)\
Get ## BatchCommandDescription ## name

#define BATCH_COMMAND_DECLARE_DESCRIPTION(name)\
boost::shared_ptr<chaos::common::data::CDataWrapper> BATCH_COMMAND_FUNCTION_GET_DESCRIPTION(name)();

#define BATCH_COMMAND_OPEN_DESCRIPTION(name, description)\
boost::shared_ptr<chaos::common::data::CDataWrapper> BATCH_COMMAND_FUNCTION_GET_DESCRIPTION(name)(){\
BatchCommandDescription BATCH_COMMAND_DESCRIPTION(name)(#name, description);

#define BATCH_COMMAND_ADD_BOOL_PARAMTER(name, paramter, description)\
BATCH_COMMAND_DESCRIPTION(name).addParameter(paramter, description, chaos::DataType::TYPE_BOOLEAN);
#define BATCH_COMMAND_ADD_INT32_PARAMTER(name, paramter, description)\
BATCH_COMMAND_DESCRIPTION(name).addParameter(paramter, description, chaos::DataType::TYPE_INT32);
#define BATCH_COMMAND_ADD_INT64_PARAMTER(name, paramter, description)\
BATCH_COMMAND_DESCRIPTION(name).addParameter(paramter, description, chaos::DataType::TYPE_INT64);
#define BATCH_COMMAND_ADD_DOUBLE_PARAMTER(name, paramter, description)\
BATCH_COMMAND_DESCRIPTION(name).addParameter(paramter, description, chaos::DataType::TYPE_DOUBLE);
#define BATCH_COMMAND_ADD_STRING_PARAMTER(name, paramter, description)\
BATCH_COMMAND_DESCRIPTION(name).addParameter(paramter, description, chaos::DataType::TYPE_STRING);

#define BATCH_COMMAND_CLOSE_DESCRIPTION(name)\
return BATCH_COMMAND_DESCRIPTION(name).getDescription();\
}

//macro that will return the shared pointer to cdatawrapper that contains the description
#define BATCH_COMMAND_GET_DESCRIPTION(name)\
BATCH_COMMAND_FUNCTION_GET_DESCRIPTION(name)()

namespace chaos {
    namespace common {
        namespace batch_command {
            
            typedef std::map<std::string,
            boost::shared_ptr<chaos::common::data::CDataWrapper> > MapParamter;
            
            typedef std::map<std::string,
            boost::shared_ptr<chaos::common::data::CDataWrapper> >::iterator MapParamterIterator;
            
            
            
            //! provide a set of method that permit to declare a batch command
            class BatchCommandDescription {
                const std::string alias;
                const std::string description;

                MapParamter map_parameter;
                static std::map<std::string, BatchCommandDescription* > global_description_instances;
            public:
                //! default constructor with the alias of the command
                BatchCommandDescription(const std::string& batch_command_alias,
                                        const std::string& batch_command_description);
                ~BatchCommandDescription();
                
                //! return the alias of the command
                const std::string& getAlias();
                
                //! add a parameter to the command
                void addParameter(const std::string& parameter_name,
                                  const std::string& parameter_description,
                                  chaos::DataType::DataType type);
                
                boost::shared_ptr<chaos::common::data::CDataWrapper>
                getDescription();
            };
            
            

        }
    }
}

#endif /* defined(__CHAOSFramework__BatchCommandDescription__) */
