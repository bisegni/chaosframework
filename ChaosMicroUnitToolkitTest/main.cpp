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

#include <iostream>
#include <chaos_micro_unit_toolkit/micro_unit_toolkit.h>

using namespace chaos::micro_unit_toolkit;
using namespace chaos::micro_unit_toolkit::data;
using namespace chaos::micro_unit_toolkit::connection;
using namespace chaos::micro_unit_toolkit::connection::unit_proxy;

int main(int argc, const char * argv[]) {
    DataPack dp;
    DataPack dp_parent;
    dp.addBool("b", true);
    dp.addInt32("ti32", 32);
    dp.addInt64("ti64", 32);
    dp.addDouble("double", 90.5);
    dp_parent.addString("skey", "strings");
    dp_parent.addDataPack("dpkey", dp);
    dp_parent.createArrayForKey("array");
    dp_parent.appendDataPack("array", dp);
    dp_parent.appendDataPack("array", dp);
    dp_parent.appendDataPack("array", dp);

    std::vector<int> arr;
    arr.push_back(1);
    arr.push_back(2);
    arr.push_back(3);
    arr.push_back(4);
    arr.push_back(5);
    dp_parent.addArray("iarr", arr);
    std::cout << dp_parent.toUnformattedString() << std::endl;
    return 0;
}
