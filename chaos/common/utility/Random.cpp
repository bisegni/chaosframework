/*
 * Copyright 2012, 27/08/2018 INFN
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
#include <chaos/common/utility/Random.h>

using namespace chaos::common::utility;

boost::random_device Random::rd;

Random::Random():
rnd_gen_int64(){}

Random::Random(CInt64 min,
               CInt64 max):
rnd_gen_int64(min, max){}

Random::~Random() {}

CInt64 Random::rand() const {
    return rnd_gen_int64(rd);
}

Random::operator CInt64() const {
    return rand();
}
