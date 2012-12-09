/*	
 *	KeyDataStorage.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *	
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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

#ifndef KeyDataStorage_H
#define KeyDataStorage_H

#include <string>
#include <chaos/cu_toolkit/DataManager/MultiBufferDataStorage.h>
#include <chaos/common/utility/ArrayPointer.h>

namespace chaos{
    using namespace std;
    
    class KeyDataStorage : public MultiBufferDataStorage {
        string dataSetKey;
        IODataDriver *liveIODriver;
        auto_ptr<CDataWrapper> keyData;
    public:
        KeyDataStorage(const char*);
        KeyDataStorage(string&);
        virtual ~KeyDataStorage();
        
        void init(CDataWrapper*);
        
        /*
         Return a new instace for the CSDatawrapped
         */
        CDataWrapper* getNewDataWrapper();

        /*
         Retrive the data from Live Storage
         */
        ArrayPointer<CDataWrapper>* getLastDataSet();
        
        /*
         Retrive the data from History Storage
         */
         ArrayPointer<CDataWrapper>*  getHistoricalDataSet(CDataWrapper*);
        
        /*
         Permit to be live configurable
         */
        CDataWrapper* updateConfiguration(CDataWrapper *);

    };
}
#endif
