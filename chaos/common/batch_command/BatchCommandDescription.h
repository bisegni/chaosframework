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
#include <chaos/common/utility/ObjectInstancer.h>

#include <boost/shared_ptr.hpp>

#include <map>

//macro fro helping the creation of batch command description
#define BATCH_COMMAND_DESCRIPTION(n)\
BatchCommandDescription ## n

#define BATCH_COMMAND_FUNCTION_GET_DESCRIPTION(n)\
Get ## BatchCommandDescription ## n

#define BATCH_COMMAND_DECLARE_DESCRIPTION(n)\
boost::shared_ptr<chaos::common::batch_command::BatchCommandDescription> BATCH_COMMAND_FUNCTION_GET_DESCRIPTION(n)();

#define BATCH_COMMAND_OPEN_DESCRIPTION(n, d)\
boost::shared_ptr<chaos::common::batch_command::BatchCommandDescription> BATCH_COMMAND_FUNCTION_GET_DESCRIPTION(n)(){\
boost::shared_ptr<chaos::common::batch_command::BatchCommandDescription> result(new BatchCommandDescription(#n, d));

#define BATCH_COMMAND_ADD_BOOL_PARAMTER(p, d)\
result->addParameter(p, d, chaos::DataType::TYPE_BOOLEAN);
#define BATCH_COMMAND_ADD_INT32_PARAMTER(p, d)\
result->addParameter(p, d, chaos::DataType::TYPE_INT32);
#define BATCH_COMMAND_ADD_INT64_PARAMTER(p, d)\
result->addParameter(p, d, chaos::DataType::TYPE_INT64);
#define BATCH_COMMAND_ADD_DOUBLE_PARAMTER(p, d)\
result->addParameter(p, d, chaos::DataType::TYPE_DOUBLE);
#define BATCH_COMMAND_ADD_STRING_PARAMTER(p, d)\
result->addParameter(p, d, chaos::DataType::TYPE_STRING);

#define BATCH_COMMAND_CLOSE_DESCRIPTION()\
return result;\
}

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
            
            typedef std::map<std::string,
            boost::shared_ptr<chaos::common::data::CDataWrapper> > MapParamter;
            
            typedef std::map<std::string,
            boost::shared_ptr<chaos::common::data::CDataWrapper> >::iterator MapParamterIterator;
            
            
            
            //! provide a set of method that permit to declare a batch command
            class BatchCommandDescription {
                friend class BatchCommandExecutor;
                std::string alias;
                std::string description;

                MapParamter map_parameter;
                
                chaos::common::utility::ObjectInstancer<BatchCommand> *instancer;
            public:
                //! default constructor
                BatchCommandDescription();
                //! default constructor with the alias of the command
                BatchCommandDescription(const std::string& _command_alias,
                                        const std::string& _command_description);
                
                //! default constructor with the alias of the command
                BatchCommandDescription(const std::string& _command_alias,
                                        const std::string& _command_description,
                                        chaos::common::utility::ObjectInstancer<BatchCommand> *_instancer);
                
                //!copy constructor
                BatchCommandDescription(const std::string& _command_alias,
                                        const std::string& _command_description,
                                        const MapParamter& _map_paramter);
                
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
                                  chaos::DataType::DataType type);
                
                boost::shared_ptr<chaos::common::data::CDataWrapper>
                getFullDescription();
            };
            
            

        }
    }
}

#endif /* defined(__CHAOSFramework__BatchCommandDescription__) */
