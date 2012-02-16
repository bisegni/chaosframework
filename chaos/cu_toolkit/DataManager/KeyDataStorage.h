    //
    //  KeyDataStorage.h
    //  ChaosFramework
    //
    //  Created by Claudio Bisegni on 12/03/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

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
        ~KeyDataStorage();
        
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
