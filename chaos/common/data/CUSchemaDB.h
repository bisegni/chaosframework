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
     define the direction of dataset element
 */
typedef enum {
			Input = 0,
			Output,
			Bidirectional,
} DataSetAttributeIOAttribute;

/*
     struct for describe an element for the dataset
 */
struct DataSetAttribute {
	//the domain of the attribute(usually the deviceid)
	string attributeTag;
	//the name of the attribute
	string attributeName;
	//the description of the attribute
	string attributeDescription;
	//the type
	CommandManagerConstant::ActionParamType attributeType;
	//the direction (I|O|IO)
	DataSetAttributeIOAttribute attributeDirection;
};

/*
     Class for contain all field for the CU Dataset
 */
class CUSchemaDB {
        //map<string, shared_ptr<DataSetAttribute> > datasetAttributeMap;
        //map containing the domain and the attribute list for that domain
    map<string, ptr_vector<DataSetAttribute> > datasetAttributeMap;
    
protected:
    /*
     Fill the CDataWrapper with the valu eof the attribute of the dataset
     */
    void fillCDataWrapperWithAttributeValues(DataSetAttribute*,CDataWrapper*);


public:

    
    /*
     return the vector containing the atrtibute list for a domain
     */
    ptr_vector<DataSetAttribute>& getDSAttributeListForDeviceID(string&);
  
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
			CommandManagerConstant::ActionParamType,
			DataSetAttributeIOAttribute);

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
	void getAttributeForDirection(string& domainName, DataSetAttributeIOAttribute, vector< DataSetAttribute* >&);
    
    /*
     REturn all the setupped device id
     */
    void getAllDeviceId(vector<string>& domainNames);
};

}

#endif
