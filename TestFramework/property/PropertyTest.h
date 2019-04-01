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

#ifndef PropertyTest_hpp
#define PropertyTest_hpp

#include <gtest/gtest.h>
#include <chaos/common/property/property.h>
class PropertyTest:
public testing::Test {
protected:
    unsigned int ch_count;
    unsigned int uh_count;
    virtual void SetUp();
public:
    PropertyTest();
    bool changeHandler(const std::string& group_name,
                       const std::string& property_name,
                       const chaos::common::data::CDataVariant& property_value);
    
    bool updateHandler(const std::string& group_name,
                       const std::string& property_name,
                       const chaos::common::data::CDataVariant& old_value,
                       const chaos::common::data::CDataVariant& property_value);
};
#endif /* PropertyTest_hpp */
