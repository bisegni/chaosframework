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

#include <chaos/common/state_flag/DeclareStateFlag.h>

using namespace chaos::common::state_flag;

DeclareStateFlag::DeclareStateFlag(const std::string& _catalog_name):
StateFlagCatalog(_catalog_name){}

DeclareStateFlag::~DeclareStateFlag(){}

void DeclareStateFlag::addBoolFlag(const std::string flag_name,
                                    const std::string flag_description,
                                    const StateLevel& off_level,
                                    const StateLevel& on_level) {
    ChaosSharedPtr<StateFlagBoolState> bool_flag(new StateFlagBoolState(flag_name,
                                                                             flag_description,
                                                                             off_level,
                                                                             on_level));
    StateFlagCatalog::addFlag(bool_flag);
}

void DeclareStateFlag::addLevelFlagWithStates(const std::string flag_name,
                                               const std::string flag_description,
                                               const StateLevelContainer& map_code_level_state) {
    ChaosSharedPtr<StateFlag> level_flag(new StateFlag(flag_name, flag_description));
    level_flag->addLevelsFromSet(map_code_level_state);
    StateFlagCatalog::addFlag(level_flag);
}

void DeclareStateFlag::addFlag(const std::string flag_name,
                                const std::string flag_description) {
    StateFlagCatalog::addFlag(ChaosSharedPtr<StateFlag>(new StateFlag(flag_name,
                                                                            flag_description)));
}

void DeclareStateFlag::addStateToFlag(const std::string flag_name,
                                       const StateLevel& new_state) {
    ChaosSharedPtr<StateFlag> flag = getFlagByName(flag_name);
    if(flag.get() == NULL) return;
    flag->addLevel(new_state);
}

void DeclareStateFlag::setFlagState(const std::string flag_name,
                                     const int8_t set_state) {
    ChaosSharedPtr<StateFlag> flag = getFlagByName(flag_name);
    if(flag.get() == NULL) return;
    flag->setCurrentLevel(set_state);
}
