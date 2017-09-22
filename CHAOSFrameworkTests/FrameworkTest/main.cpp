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

#include <gtest/gtest.h>
#include <chaos/common/log/LogManager.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

int main(int argc,  char ** argv) {
    chaos::GlobalConfiguration::getInstance()->preParseStartupParameters();
    chaos::GlobalConfiguration::getInstance()->parseStartupParameters(argc, argv);
    chaos::common::log::LogManager::getInstance()->init();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
