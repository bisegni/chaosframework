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

#ifndef __CHAOSFrameworkTests__01071BC_B99C_407B_B3F0_721B6EE771A4_TestURLFeeder_h
#define __CHAOSFrameworkTests__01071BC_B99C_407B_B3F0_721B6EE771A4_TestURLFeeder_h

#include <chaos/common/network/URLServiceFeeder.h>
#include <gtest/gtest.h>

class ServiceForURL {
public:
    std::string url;
};

class TestURLFeeder :
public testing::Test,
public chaos::common::network::URLServiceFeederHandler {
protected:
    uint32_t number_of_cycle;
    chaos::common::network::URLServiceFeeder feeder_engine;
    void disposeService(void *service_ptr);
    void* serviceForURL(const chaos::common::network::URL& url,
                        uint32_t service_index);
    virtual void SetUp();
public:
    TestURLFeeder();
};

#endif /* __CHAOSFrameworkTests__01071BC_B99C_407B_B3F0_721B6EE771A4_TestURLFeeder_h */
