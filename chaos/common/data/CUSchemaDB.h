/*
 *	CUSchemaDB.h
 *	!CHAOS
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
#include <boost/ptr_container/ptr_map.hpp>
#include <vector>
#include <chaos/common/data/entity_db/EntityDB.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/TimingUtil.h>
namespace chaos {
	namespace common {
		namespace data {
			using namespace std;
			using namespace boost;
            //map<string, entity::Entity*> deviceEntityMap;
			typedef map<string, entity::Entity*> EntityPtrMap;
			typedef EntityPtrMap::iterator EntityPtrMapIterator;
			
			//!Describe the range of the value for an attribute of the dataset
			/*!
			 \ingroup Control_Unit_User_Api
			 This structure contains the additiona ilnformation  about a 
			 dataset attribute
			 */
			typedef struct RangeValueInfo {
				//!max value of the range
				uint32_t maxSize;
				//!max value of the range
				string maxRange;
				//!minimum value of the range
				string minRange;
				//!defaultValue
				string defaultValue;
				//!Type of the value
				DataType::DataType valueType;
				//! reset all filed
				void reset();
			} RangeValueInfo;
			
			class CDataWrapper;
			
			//!Control Unit Device Database
			/*!
			 This is a database for the management of the dataset and property, of a device, implemented using EntityDB
			 */
			class CUSchemaDB {
				
				//! Entity Database pointer
				edb::EntityDB *entityDB;
				
				//! Timing utils
				utility::TimingUtil timingUtils;
				
				//! Map that contains all device entity
				EntityPtrMap deviceEntityMap;
				
				//! entity poitner for all device defined withing the control unit
				std::map<const char *, uint32_t> mapDatasetKeyForID;
				
				//! fill a CDatawrapper with the device description
				void fillCDataWrapperDSAtribute(CDataWrapper *dsAttribute,  entity::Entity *deviceIDEntity, entity::Entity *attrEntity, ptr_vector<edb::KeyIdAndValue>& attrProperty);
				
				void clearAllAttributeForProperty(entity::Entity *attributeEntity, uint32_t keyIDToAdd);
				
				//! add an attribute in unique mode
				/*!
				 Before add and attribute, is checked if it is present.
				 \param attributeEntity the entity where attacch the attribute
				 \param attributeValue the string attribute value
				 \param checkValueForUnicity check also the value for identify the presence
				 */
				void addUniqueAttributeProperty(entity::Entity *attributeEntity, uint32_t keyIDToAdd, const char * attributeValue, bool checkValueForUnicity = true);
				
				//! add an attribute in unique mode
				/*!
				 Before add and attribute, is checked if it is present.
				 \param attributeEntity the entity where attacch the attribute
				 \param attributeValue the string attribute value
				 \param checkValueForUnicity check also the value for identify the presence
				 */
				void addUniqueAttributeProperty(entity::Entity *attributeEntity, uint32_t keyIDToAdd, string& attributeValue, bool checkValueForUnicity = true);
				
				//! add an attribute in unique mode
				/*!
				 Before add and attribute, is checked if it is present.
				 \param attributeEntity the entity where attacch the attribute
				 \param attributeValue the int64_t attribute value
				 \param checkValueForUnicity check also the value for identify the presence
				 */
				void addUniqueAttributeProperty(entity::Entity *attributeEntity, uint32_t keyIDToAdd, int64_t attributeValue, bool checkValueForUnicity = true);
				
				//! add an attribute in unique mode
				/*!
				 Before add and attribute, is checked if it is present.
				 \param attributeEntity the entity where attacch the attribute
				 \param attributeValue the double attribute value
				 \param checkValueForUnicity check also the value for identify the presence
				 */
				void addUniqueAttributeProperty(entity::Entity *attributeEntity, uint32_t keyIDToAdd, double attributeValue, bool checkValueForUnicity = true);
				
				//! Get the entity reporesenting the dataset element
				/*!
				 Before add and attribute, is checked if it is present.
				 \param device the entity represent the device
				 \param attributeName the name of the attribute
				 */
				entity::Entity *getDatasetElement(entity::Entity *device, string& attributeName);
				
				//! Get the entity representing the dataset element
				/*!
				 Before add and attribute, is checked if it is present.
				 \param device the entity represent the device
				 \param attributeName the name of the attribute
				 */
				entity::Entity *getDatasetElement(entity::Entity *device, const char * attributeName);
				
				//! Get the entity representing the device
				/*!
				 Return entity object of a device
				 */
				entity::Entity* getDeviceEntity(const string& deviceID);
				
				void initDB(const char* name, bool onMemory);
				
				//!Fill a CDataWrapper with the dataset decode
				void fillDataWrapperWithDataSetDescription(entity::Entity*, CDataWrapper&);
				
				//!Compose the name of the attribute
				inline void composeAttributeName(const string& deviceID,
												 const string& attributeName,
												 string& composedName);
				
				//!Decompose the attribute name
				inline void decomposeAttributeName(const string& deviceID,
												   const string& attributeName,
												   std::string& decomposed);
			public:
				//!Default Contructor
				/*!
				 The database is created with the default parameter
				 */
				CUSchemaDB();
				
				//!Contructor for specify the onMemory flag
				/*!
				 The database is create with onMemory option enable.This
				 permit to use only the computer memory to store the data
				 and the index.
				 
				 \param onMemory flag for specify the use of only memory
				 */
				CUSchemaDB(bool onMemory);
				
				//!Contructor for specify the name of the database and onMemory flag
				/*!
				 The database is create with onMemory option enable and with
				 the name specified. This permit to use only the computer memory
				 to store the data and the index.
				 
				 \param databaseName the name of the database
				 \param onMemory flag for specify the use of only memory
				 */
				CUSchemaDB(const char *databaseName, bool onMemory);
				
				//!Default Destructor
				virtual ~CUSchemaDB();
				
				//! Add device dataset definitio by serialized form
				/*!
				 Fill the database with the devices and their dataset with the content
				 of the serialized data
				 
				 \param serializedDataset serialze dataset
				 */
				void addAttributeToDataSetFromDataWrapper(CDataWrapper& serializedDataset);
				
				//! Fill a CDataWrapper with all device information
				/*!
				 fill a CDataWrapper with the dataset decode
				 */
				void fillDataWrapperWithDataSetDescription(CDataWrapper&);
				
				//! Fill a CDataWrapper with a single device information
				/*!
				 fill a CDataWrapper with the dataset decode
				 */
				void fillDataWrapperWithDataSetDescription(string&, CDataWrapper&);
				
				//! New device creation
				/*!
				 Add a new device to the database with a specified deviceID
				 
				 \param deviceID the the device identification string
				 */
				void addDeviceId(const string& deviceID);
				
				//! Add dataset attribute
				/*!
				 Add the new attribute to the deviceID dataset specifing
				 the default parameter
				 \ingroup Control_Unit_User_Api
				 \param deviceID the id of the device
				 \param attributeName the name of the new attribute
				 \param attributeDescription the description of the attribute
				 \param attributeType the type of the new attribute
				 \param attributeDirection the direction of the new attribute
				 \param maxSize maximum size for the type that are not raw (ex string, buffert, etc...)
				 */
				void addAttributeToDataSet(const char*const deviceID,
										   const char*const attributeName,
										   const char*const attributeDescription,
										   DataType::DataType attributeType,
										   DataType::DataSetAttributeIOAttribute attributeDirection,
										   uint32_t maxSize = 0);
				//!Get al device id
				/*!
				 Return all the setupped device id
				 */
				void getAllDeviceId(vector<string>& deviceNames);
				
				//!Check if a device is present
				/*!
				 Return true if the device id is found
				 */
				bool deviceIsPresent(const string& deviceID);
				
				//!Get device dataset attribute names
				/*!
				 \ingroup Control_Unit_User_Api
				 Return all dataset attribute name
				 \param deviceID the identification of the device
				 \param attributesName the array that will be filled with the name
				 */
				void getDeviceDatasetAttributesName(const string& deviceID,
													vector<string>& attributesName);
				
				//!Get device attribute name that has a specified direction
				/*!
				 \ingroup Control_Unit_User_Api
				 Return all dataset attribute name
				 \param deviceID the identification of the device
				 \param directionType the direction for attribute filtering
				 \param attributesName the array that will be filled with the name
				 */
				void getDeviceDatasetAttributesName(const string& deviceID,
													DataType::DataSetAttributeIOAttribute directionType,
													vector<string>& attributesName);
				
				//!Get device attribute description
				/*!
				 Return the dataset description
				 \param deviceID the identification of the device
				 \param attributesName the name of the attribute
				 \param attributeDescription the returned description
				 */
				void getDeviceAttributeDescription(const string& deviceID,
												   const string& attributesName,
												   string& attributeDescription);
				
				//!Get the value information for a specified attribute name
				/*!
				 Return the range value for the attribute
				 \param deviceID the identification of the device
				 \param attributesName the name of the attribute
				 \param rangeInfo the range and default value of the attribute, the fields
				 of the struct are not cleaned, so if an attrbute doesn't has
				 some finromation, relative field are not touched.
				 */
				int getDeviceAttributeRangeValueInfo(const string& deviceID,
													  const string& attributesName,
													  RangeValueInfo& rangeInfo);
				
				//!Set the range values for an attribute
				/*!
				 set the range value for the attribute
				 \param deviceID the identification of the device
				 \param attributesName the name of the attribute
				 \param rangeInfo the range and default value of the attribute, the fields
				 of the struct are not cleaned, so if an attrbute doesn't has
				 some finromation, relative field are not touched.
				 */
				void setDeviceAttributeRangeValueInfo(const string& deviceID,
													  const string& attributesName,
													  RangeValueInfo& rangeInfo);
				
				//!Get the direction of an attribute
				/*!
				 Return the direcion of the attribute
				 \param deviceID the identification of the device
				 \param attributesName the name of the attribute
				 \param directionType the direction of the attribute
				 */
				int getDeviceAttributeDirection(const string& deviceID,
												const string& attributesName,
												DataType::DataSetAttributeIOAttribute& directionType);
			};
		}
	}
}
#endif
