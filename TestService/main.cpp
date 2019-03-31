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

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/message/MultiAddressMessageChannel.h>
#include <chaos_metadata_service_client/ChaosMetadataServiceClient.h>

#include <gtest/gtest.h>

#define MSCT_INFO   INFO_LOG(ServiceTest)
#define MSCT_DBG    INFO_LOG(ServiceTest)
#define MSCT_ERR    INFO_LOG(ServiceTest)

int main(int argc, const char * argv[]) {
    chaos::GlobalConfiguration::getInstance()->preParseStartupParameters();
    chaos::GlobalConfiguration::getInstance()->parseStartupParametersAllowingUnregistered(argc, argv);
    
    chaos::common::log::LogManager::getInstance()->init();
    ::testing::InitGoogleTest(&argc, const_cast<char **>(argv));
    
    return RUN_ALL_TESTS();
}
