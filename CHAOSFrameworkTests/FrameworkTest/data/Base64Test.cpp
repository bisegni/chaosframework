/*
 * Copyright 2012, 25/07/2018 INFN
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

#include <chaos/common/utility/Base64Util.h>

using namespace chaos::common::utility;

TEST(Base64Test, BinTxtBin) {
    char buffer[256];
    const std::string src(buffer, buffer+256);
    const std::string b64 = Base64Util::encode(buffer, 256);
    chaos::common::data::CDBufferUniquePtr buf = Base64Util::decode(b64);
    ASSERT_EQ(buf->getBufferSize(), 256);
    ASSERT_EQ(std::memcmp(buf->getBuffer(), buffer, 256), 0);
}
