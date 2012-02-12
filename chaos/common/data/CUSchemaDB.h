//
//  CUSchemaDB.h
//  ChaosFramework
//
//  Created by Bisegni Claudio on 14/07/11.
//  Copyright 2011 INFN. All rights reserved.
//

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
            //minimum value of the range
        string minRange;
            //defaultValue
        string defaultValue;
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
	void getAttributeForDirection(string& deviceID, DataType::DataSetAttributeIOAttribute, vector< shared_ptr<CDataWrapper> >&);
    
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
    
};

}

#endif
