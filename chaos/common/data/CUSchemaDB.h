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
#include <chaos/common/data/entity_db/EntityDB.h>
#include <chaos/common/cconstants.h>

namespace chaos {
    using namespace std;
    using namespace boost;
    
    //!Describe the range of the value for an attribute of the dataset
    typedef struct RangeValueInfo {
        //!max value of the range
        string maxRange;
        //!minimum value of the range
        string minRange;
        //!defaultValue
        string defaultValue;
        //!Type of the value
        DataType::DataType valueType;
    } RangeValueInfo;
    
    class CDataWrapper;
    
    /*!
     Class for contain all field for the CU Dataset
     */
    class CUSchemaDB {
        map<string, entity::Entity*> deviceEntityMap;
        map<string, vector< CDataWrapper* > >deviceIDDataset;
        
        void addUniqueAttributeProperty(entity::Entity *attributeEntity, uint32_t keyIDToAdd, string& attributeValue);
        void addUniqueAttributeProperty(entity::Entity *attributeEntity, uint32_t keyIDToAdd, int64_t attributeValue);
        void addUniqueAttributeProperty(entity::Entity *attributeEntity, uint32_t keyIDToAdd, double attributeValue);
        entity::Entity *getDatasetElement(entity::Entity *device, string& attributeName);
        
        /*!
         return the vector containing the atrtibute list for a domain
         */
        entity::Entity* getDeviceEntity(const string& deviceID);
        edb::EntityDB *entityDB;
        uint32_t keyIdDevice;
        uint32_t keyIdDataset;
        uint32_t keyIdAttrName;
        uint32_t keyIdAttrDesc;
        uint32_t keyIdAttrType;
        uint32_t keyIdAttrDir;
        uint32_t keyIdAttrMaxRng;
        uint32_t keyIdAttrMinRng;
    public:
        
        /*!
         */
        CUSchemaDB();
        
        /*!
         */
        virtual ~CUSchemaDB();
        
        /*!
         add a new device id
         */
        void addDeviceId(string);
        
        /*!
         Add the new field at the CU dataset
         */
        void addAttributeToDataSet(const char*const,
                                   const char*const,
                                   const char*const,
                                   DataType::DataType,
                                   DataType::DataSetAttributeIOAttribute);
        
        /*!
         Add the new field at the CU dataset from the CDataWrapper
         */
        void addAttributeToDataSetFromDataWrapper(CDataWrapper&);
        
        /*!
         fill a CDataWrapper with the dataset decode
         */
        void fillDataWrpperWithDataSetDescirption(CDataWrapper&);
        
        /*!
         * Return the attribute array for a specified direction
         */
        void getAttributeForDirection(string& deviceID,
                                      DataType::DataSetAttributeIOAttribute,
                                      vector< boost::shared_ptr<CDataWrapper> >&);
        
        /*!
         REturn all the setupped device id
         */
        void getAllDeviceId(vector<string>& domainNames);
        
        /*!
         Return true if the device id is found
         */
        bool deviceIsPresent(const string& deviceID);
        
        /*!
         */
        void getDeviceDatasetAttributesName(const string& deviceID,
                                            vector<string>& attributesName);
        
        /*!
         */
       void getDeviceDatasetAttributesName(const string& deviceID,
                                           DataType::DataSetAttributeIOAttribute directionType, vector<string>& attributesName);
        
        /*!
         */
       void getDeviceAttributeDescription(const string& deviceID,
                                          const string& attributesName,
                                          string& attributeDescription);
        
        /*!
         */
       void getDeviceAttributeRangeValueInfo(const string& deviceID,
                                             const string& attributesName,
                                             RangeValueInfo& rangeInfo);
        
        /*!
         */
        int getDeviceAttributeDirection(const string& deviceID,
                                        const string& attributesName,
                                        DataType::DataSetAttributeIOAttribute& directionType);
    };
}
#endif
