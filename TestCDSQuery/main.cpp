/*
 *	main.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
        
        QueryTest::getInstance()->doQuery("rt_sin_a", "2015-02-17T10","2015-02-17T11");

        QueryTest::getInstance()->stop();
    }catch(...) {
        
    }
    try{
        QueryTest::getInstance()->deinit();
    }catch(...) {
        
    }
    return 0;
}
