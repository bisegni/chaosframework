/*
 *	SetInstanceDescription.h
 *	!CHAOS
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
#ifndef __CHAOSFramework__SetInstanceDescription__
#define __CHAOSFramework__SetInstanceDescription__

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <boost/ptr_container/ptr_vector.hpp>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace control_unit {
                
                typedef boost::ptr_vector<common::data::CDataWrapper>           CDWList;
                typedef boost::ptr_vector<common::data::CDataWrapper>::iterator CDWListIterator;
                
                class SetInstanceDescription;
                
                //api helper
                /*!
                 helper class for collecting the data needes by the api
                 */
                class SetInstanceDescriptionHelper {
                    friend class SetInstanceDescription;
                    //!list for all ddriver description added to the api
                    CDWList driver_descriptions;
                    //!list all the attribute value description added to the api
                    CDWList attribute_value_descriptions;
                public:
                    SetInstanceDescriptionHelper();
                    ~SetInstanceDescriptionHelper();
                    
                    //! the unique id of the control unit instance
                    std::string control_unit_uid;
                    //! the unit server that host the instance
                    std::string unit_server_uid;
                    //!the control unit implementaiton of the instance
                    std::string control_unit_implementation;
                    //!set the instance autoload flag
                    
                    /*!
                     specify if the control unit, hosted by an unit server, need to be loaded after 
                     his parent has been successfully registered.
                     */
                    bool auto_load;
                    
                    //!specify if the control unit, hosted by an unit server, need to be initialized after his load phase
                    bool auto_init;
                    
                    //!specify if the control unit, hosted by an unit server, need to be started after his init phase
                    bool auto_start;
                    
                    //!define the default value for the scheduler step's delay within the control unit
                    uint64_t default_schedule_delay;
                    
                    //!define the storage type as in @DataServiceNodeDefinitionType::DSStorageType
                    DataServiceNodeDefinitionType::DSStorageType storage_type;
                    
                    /*!
                     seconds for wich the data need to remain archive within the !CHAOS 
                     storage system. A value of 0 mean infinit persistence
                     */
                    uint32_t history_ageing;
                    
                    /*!
                     the time that need to pass before store another datapack into the history system
                     */
                    uint64_t history_time;
                    
                    //! the string is passed to the control unit for the load phase
                    std::string load_parameter;
                    
                    //!add a new driver description
                    void addDriverDescription(const std::string& driver_name,
                                              const std::string& driver_version,
                                              const std::string& driver_init_parameter);
                    
                    //! clear all previously added driver descriptions
                    void clearAllDriverDescriptions();
                    
                    //! add an attribute range value description for the default value and range
                    void addAttributeConfig(const std::string& attribute_name,
                                            const std::string& attribute_default_value,
                                            const std::string& attribute_max_range = std::string(),
                                            const std::string& attribute_min_range = std::string());
                    //! remove all previously added attribute range value description
                    void clearAllAttributeConfig();
                };
                
                //api alcass
                class SetInstanceDescription:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(SetInstanceDescription)
                protected:
                    API_PROXY_CD_DECLARATION(SetInstanceDescription)
                public:
                    std::auto_ptr<SetInstanceDescriptionHelper> getNewHelper();
                    /*!
                     Set the isntance
                     */
                    ApiProxyResult execute(SetInstanceDescriptionHelper& api_data);
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__SetInstanceDescription__) */
