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

#define BATCH_COMMAND_START_DESCRIPTION(name, description)\
static BatchCommandDescription BatchCommandDescription ## #name(name, description);

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
                
                MapParamter map_paramter;
            public:
                //! default constructor with the alias of the command
                BatchCommandDescription(const std::string& batch_command_alias,
                                        const std::string& batch_command_description);
                ~BatchCommandDescription();
                
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
