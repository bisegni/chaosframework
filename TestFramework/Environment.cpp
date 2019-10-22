/*
 * Copyright 2012, 2019 INFN
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
#include <chaos/common/log/LogManager.h>
#include <chaos/common/async_central/async_central.h>
#include "Environment.h"
Environment::Environment(){}

Environment::~Environment(){}
void Environment::SetUp(){
    ASSERT_NO_THROW(chaos::common::log::LogManager::getInstance()->init(););
    ASSERT_NO_THROW(chaos::common::utility::InizializableService::initImplementation(chaos::common::async_central::AsyncCentralManager::getInstance(), NULL, "AsyncCentralManager", __PRETTY_FUNCTION__););
}

void Environment::TearDown() {
    ASSERT_NO_THROW(chaos::common::utility::InizializableService::deinitImplementation(chaos::common::async_central::AsyncCentralManager::getInstance(), "AsyncCentralManager", __PRETTY_FUNCTION__););
    ASSERT_NO_THROW(chaos::common::log::LogManager::getInstance()->deinit(););
}
