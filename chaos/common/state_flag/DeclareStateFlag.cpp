/*
 *	DeclareStateFlag.cpp
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

#include <chaos/common/state_flag/DeclareStateFlag.h>

using namespace chaos::common::state_flag;

DeclareStateFlag::DeclareStateFlag(const std::string& _catalog_name):
StateFlagCatalog(_catalog_name){}

DeclareStateFlag::~DeclareStateFlag(){}

void DeclareStateFlag::addBoolFlag(const std::string flag_name,
                                    const std::string flag_description,
                                    const StateLevel& off_level,
                                    const StateLevel& on_level) {
    boost::shared_ptr<StateFlagBoolState> bool_flag(new StateFlagBoolState(flag_name,
                                                                             flag_description,
                                                                             off_level,
                                                                             on_level));
    StateFlagCatalog::addFlag(bool_flag);
}

void DeclareStateFlag::addLevelFlagWithStates(const std::string flag_name,
                                               const std::string flag_description,
                                               const StateLevelContainer& map_code_level_state) {
    boost::shared_ptr<StateFlag> level_flag(new StateFlag(flag_name, flag_description));
    level_flag->addLevelsFromSet(map_code_level_state);
    StateFlagCatalog::addFlag(level_flag);
}

void DeclareStateFlag::addFlag(const std::string flag_name,
                                const std::string flag_description) {
    StateFlagCatalog::addFlag(boost::shared_ptr<StateFlag>(new StateFlag(flag_name,
                                                                            flag_description)));
}

void DeclareStateFlag::addStateToFlag(const std::string flag_name,
                                       const StateLevel& new_state) {
    boost::shared_ptr<StateFlag> flag = getFlagByName(flag_name);
    if(flag.get() == NULL) return;
    flag->addLevel(new_state);
}

void DeclareStateFlag::setFlagState(const std::string flag_name,
                                     const int8_t set_state) {
    boost::shared_ptr<StateFlag> flag = getFlagByName(flag_name);
    if(flag.get() == NULL) return;
    flag->setCurrentLevel(set_state);
}
