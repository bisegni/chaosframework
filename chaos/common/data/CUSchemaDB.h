/*	
 *	CUSchemaDB.h
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
#ifndef ChaosFramework_CUSchemaDB_h
#define ChaosFramework_CUSchemaDB_h
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <vector>

#include "CDataWrapper.h"
#include <chaos/common/cconstants.h>

namespace chaos {
    using namespace std;
    using namespace boost;
    
    /*
     Class for contain all field for the CU Dataset
     */
    class CUSchemaDB {
        map<string, vector< CDataWrapper* > >deviceIDDataset;
        
        void clearAllDatasetsVectors();
        void clearDatasetForDeviceID(string&);
        
    public:
            //!Describe the range of the value for an attribute of the dataset
        typedef struct {
                //!max value of the range
            string maxRange;
                //!minimum value of the range
            string minRange;
                //!defaultValue
            string defaultValue;
                //!Type of the value
            DataType::DataType valueType;
        } RangeValueInfo;
        
        CUSchemaDB();
        virtual ~CUSchemaDB();
        
        /*
         return the vector containing the atrtibute list for a domain
         */
        vector< CDataWrapper* >& getDatasetForDeviceID(string& deviceID);
        
        /*
         add a new device id
         */
        void addDeviceId(string);
        
        /*
         Add the new field at the CU dataset
         */
        void addAttributeToDataSet(const char*const,
                                   const char*const,
                                   const char*const,
                                   DataType::DataType,
                                   DataType::DataSetAttributeIOAttribute);
        
        /*
         Add the new field at the CU dataset from the CDataWrapper
         */
        void addAttributeToDataSetFromDataWrapper(CDataWrapper&);
        
        /*
         fill a CDataWrapper with the dataset decode
         */
        void fillDataWrpperWithDataSetDescirption(CDataWrapper&);
        
        /*
         * Return the attribute array for a specified direction
         */
        void getAttributeForDirection(string& deviceID, DataType::DataSetAttributeIOAttribute, vector< boost::shared_ptr<CDataWrapper> >&);
        
        /*
         REturn all the setupped device id
         */
        void getAllDeviceId(vector<string>& domainNames);
        
        /*!
         return true if the device id is found
         */
        bool deviceIsPresent(string& deviceID);
        
        void getDeviceDatasetAttributesName(string& deviceID, vector<string>& attributesName);
        
        void getDeviceDatasetAttributesName(string& deviceID, vector<string>& attributesName, DataType::DataSetAttributeIOAttribute directionType);
        
        void getDeviceAttributeDescription(string& deviceID, string& attributesName, string& attributeDescription);
        
        void getDeviceAttributeRangeValueInfo(string& deviceID, string& attributesName, RangeValueInfo& rangeInfo);
        
        int getDeviceAttributeDirection(string& deviceID, string& attributesName, DataType::DataSetAttributeIOAttribute& directionType);
    };
}
#endif
