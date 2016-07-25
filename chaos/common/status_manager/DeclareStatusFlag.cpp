/*
 *	DeclareStatusFlag.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 12/07/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/status_manager/DeclareStatusFlag.h>

using namespace chaos::common::status_manager;

DeclareStatusFlag::DeclareStatusFlag(const std::string& _catalog_name):
StatusFlagCatalog(_catalog_name){}

DeclareStatusFlag::~DeclareStatusFlag(){}

void DeclareStatusFlag::addBoolFlag(const std::string flag_name,
                                    const std::string flag_description,
                                    const StateLevel& off_level,
                                    const StateLevel& on_level) {
    boost::shared_ptr<StatusFlagBoolState> bool_flag(new StatusFlagBoolState(flag_name,
                                                                             flag_description,
                                                                             off_level,
                                                                             on_level));
    StatusFlagCatalog::addFlag(bool_flag);
}

void DeclareStatusFlag::addLevelFlagWithStates(const std::string flag_name,
                                               const std::string flag_description,
                                               const StateLevelContainer& map_code_level_state) {
    boost::shared_ptr<StatusFlag> level_flag(new StatusFlag(flag_name, flag_description));
    level_flag->addLevelsFromSet(map_code_level_state);
    StatusFlagCatalog::addFlag(level_flag);
}

void DeclareStatusFlag::addFlag(const std::string flag_name,
                                const std::string flag_description) {
    StatusFlagCatalog::addFlag(boost::shared_ptr<StatusFlag>(new StatusFlag(flag_name,
                                                                            flag_description)));
}

void DeclareStatusFlag::addStateToFlag(const std::string flag_name,
                                       const StateLevel& new_state) {
    boost::shared_ptr<StatusFlag> flag = getFlagByName(flag_name);
    if(flag.get() == NULL) return;
    flag->addLevel(new_state);
}

void DeclareStatusFlag::setFlagState(const std::string flag_name,
                                     const int8_t set_state) {
    boost::shared_ptr<StatusFlag> flag = getFlagByName(flag_name);
    if(flag.get() == NULL) return;
    flag->setCurrentLevel(set_state);
}
