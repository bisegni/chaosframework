/*
 *	ExternalUnitTest.cpp
 *
 *	!CHAOS [CHAOSFrameworkTests]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 13/10/2017 INFN, National Institute of Nuclear Physics
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

#include "ExternalUnitTest.h"

using namespace chaos::common::external_unit;

ExternalUnitTest::ExternalUnitTest() {}

ExternalUnitTest::~ExternalUnitTest() {}

void ExternalUnitTest::SetUp(){}

TEST_F(ExternalUnitTest, ExternalUnitTest) {
    ASSERT_NO_THROW(ExternalUnitManager::getInstance()->init(NULL));
    
    ASSERT_NO_THROW(ExternalUnitManager::getInstance()->deinit());
}
