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

#ifndef __CHAOSFrameworkTests_D40485EA_AE3A_461F_818A_4272F4DC9097_TestStateFlagCatalog_h
#define __CHAOSFrameworkTests_D40485EA_AE3A_461F_818A_4272F4DC9097_TestStateFlagCatalog_h
#include <chaos/common/alarm/AlarmCatalog.h>
#include <chaos/common/state_flag/StateFlagCatalog.h>

#include <gtest/gtest.h>

class TestStateFlagCatalog:
public testing::Test{
public:
    TestStateFlagCatalog();
protected:
    chaos::common::alarm::AlarmCatalog alarm_catalog;
    chaos::common::state_flag::StateFlagCatalog catalog_a;
    chaos::common::state_flag::StateFlagCatalog catalog_b;
    virtual void SetUp();
};


#endif /* __CHAOSFrameworkTests_D40485EA_AE3A_461F_818A_4272F4DC9097_TestStateFlagCatalog_h */
