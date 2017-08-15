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
#include "QueryTest.h"

using namespace chaos;
int main(int argc, char * argv[]) {
    try{
        //preparse for blow custom option
        QueryTest::getInstance()->preparseCommandOption(argc, argv);
        
        //initilize the faramework
        QueryTest::getInstance()->init(NULL);
        
        //
        QueryTest::getInstance()->start();
        
        //test on localhost
        QueryTest::getInstance()->addURL("localhost:1672:30175|0");
        
        QueryTest::getInstance()->doQuery("rt_sin_a", "","");

        QueryTest::getInstance()->stop();
    }catch(...) {
        
    }
    try{
        QueryTest::getInstance()->deinit();
    }catch(...) {
        
    }
    return 0;
}
