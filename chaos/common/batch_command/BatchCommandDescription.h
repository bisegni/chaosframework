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

#include <chaos/common/chaos_types.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/ObjectInstancer.h>

#include <boost/shared_ptr.hpp>

#include <map>

//macro fro helping the creation of batch command description
#define BATCH_COMMAND_DESCRIPTION(n,c)\
n::BatchCommandDescription ## c

#define BATCH_COMMAND_FUNCTION_GET_DESCRIPTION(c)\
Get ## BatchCommandDescription ## c

#define BATCH_COMMAND_DECLARE_DESCRIPTION(c)\
boost::shared_ptr<chaos::common::batch_command::BatchCommandDescription> BATCH_COMMAND_FUNCTION_GET_DESCRIPTION(c)();

#define BATCH_COMMAND_OPEN_DESC(n, c, alias, d, uid)\
boost::shared_ptr<chaos::common::batch_command::BatchCommandDescription> n GetBatchCommandDescription ## c (){\
boost::shared_ptr<chaos::common::batch_command::BatchCommandDescription> result(new chaos::common::batch_command::BatchCommandDescription( alias, d, uid));

#define BATCH_COMMAND_ADD_BOOL_PARAM(p, d, flag)\
result->addParameter(p, d, chaos::DataType::TYPE_BOOLEAN, flag);
#define BATCH_COMMAND_ADD_INT32_PARAM(p, d, flag)\
result->addParameter(p, d, chaos::DataType::TYPE_INT32, flag);
#define BATCH_COMMAND_ADD_INT64_PARAM(p, d, flag)\
result->addParameter(p, d, chaos::DataType::TYPE_INT64, flag);
#define BATCH_COMMAND_ADD_DOUBLE_PARAM(p, d, flag)\
result->addParameter(p, d, chaos::DataType::TYPE_DOUBLE, flag);
#define BATCH_COMMAND_ADD_STRING_PARAM(p, d, flag)\
result->addParameter(p, d, chaos::DataType::TYPE_STRING, flag);
#define BATCH_COMMAND_ADD_JSON_PARAM(p, d, flag)\
result->addParameter(p, d, chaos::DataType::TYPE_JSONOBJ, flag);
#define BATCH_COMMAND_CLOSE_DESCRIPTION()\
return result;}


//macro that will return the shared pointer to cdatawrapper that contains the description
#define BATCH_COMMAND_GET_DESCRIPTION(name)\
BATCH_COMMAND_FUNCTION_GET_DESCRIPTION(name)()

#define DEFINE_BATCH_COMMAND_CLASS(n,subclass)\
BATCH_COMMAND_DECLARE_DESCRIPTION(n)\
class n:public subclass

namespace chaos {
    namespace common {
        namespace batch_command {
            
            //!forward declaration
            class BatchCommandExecutor;
            class BatchCommand;
            
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, boost::shared_ptr<chaos::common::data::CDataWrapper>, BatchCommandMapParamter);
            CHAOS_DEFINE_VECTOR_FOR_TYPE(std::string, BatchCommandParameterNameList);
            
            //! provide a set of method that permit to declare a batch command
            /*!
             A batch command need to be declare to permit to other chaos node
             to know how call and configure it.
             */
            class BatchCommandDescription {
                friend class BatchCommandExecutor;
                std::string alias;
                std::string description;
                std::string unique_identifier;
                BatchCommandMapParamter map_parameter;
                
                chaos::common::utility::ObjectInstancer<BatchCommand> *instancer;
            public:
                //! default constructor
                BatchCommandDescription();
                //! default constructor with the alias of the command
                BatchCommandDescription(const std::string& _command_alias,
                                        const std::string& _command_description,
                                        const std::string& _unique_identifier = std::string("00000000-0000-0000-0000-00000000000"));

                
                ~BatchCommandDescription();
                
                void setAlias(const std::string& _alias);
                //! return the alias of the command
                const std::string& getAlias();
                
                void setDescription(const std::string& _description);
                //! return the alias of the command
                const std::string& getDescription();
                
                void setInstancer(chaos::common::utility::ObjectInstancer<BatchCommand> *_instancer);
                                        
                //! add a parameter to the command
                void addParameter(const std::string& parameter_name,
                                  const std::string& parameter_description,
                                  chaos::DataType::DataType type,
                                  int32_t flag);
                
                void getParameters(BatchCommandParameterNameList& parameter_name_list);
                
                bool getParameterType(const std::string& parameter_name,
                                      chaos::DataType::DataType& type);
                
                bool getParameterFlag(const std::string& parameter_name,
                                      int32_t& flag);
                
                bool getParameterDescription(const std::string& parameter_name,
                                             std::string& parameter_description);
                
                boost::shared_ptr<chaos::common::data::CDataWrapper>
                getParameterDescription(const std::string& parameter_name);
                
                
                boost::shared_ptr<chaos::common::data::CDataWrapper>
                getFullDescription();
            };
            
            

        }
    }
}

#endif /* defined(__CHAOSFramework__BatchCommandDescription__) */
