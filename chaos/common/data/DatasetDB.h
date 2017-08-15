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

#ifndef __CHAOSFramework__DatasetDB__
#define __CHAOSFramework__DatasetDB__

#include <string>

#include <chaos/common/data/CUSchemaDB.h>

#include <boost/lexical_cast.hpp>

namespace chaos_data = chaos::common::data;

namespace chaos {
    namespace common {
        namespace data {
			
			class DatasetDB : private chaos_data::CUSchemaDB {
				
				std::string device_id;
				
			protected:
				void setDeviceID(const std::string & _deviceID);
			public:
				DatasetDB(bool onMemory);
				~DatasetDB();
				
				
				const string & getDeviceID();
				
				//! Add device dataset definitio by serialized form
				/*!
				 Fill the database with the devices and their dataset with the content
				 of the serialized data
				 
				 \param serializedDataset serialze dataset
				 */
				void addAttributeToDataSetFromDataWrapper(chaos_data::CDataWrapper& serializedDataset);
				
				//! Fill a CDataWrapper with a single device information
				/*!
				 fill a CDataWrapper with the dataset decode
				 */
				void fillDataWrapperWithDataSetDescription(chaos_data::CDataWrapper&);
				
				//! Add dataset attribute
				/*!
				 Add the new attribute to the deviceID dataset specifing
				 the default parameter
				 \ingroup Control_Unit_User_Api
				 \ingroup Control_Unit_Definition_Api
				 \param attributeName the name of the new attribute
				 \param attributeDescription the description of the attribute
				 \param attributeType the type of the new attribute
				 \param attributeDirection the direction of the new attribute
				 */
                void addAttributeToDataSet(const std::string& attribute_name,
										   const std::string& attribute_description,
										   DataType::DataType attribute_type,
										   DataType::DataSetAttributeIOAttribute attribute_direction,
										   uint32_t maxSize = 0);
                /*!
                 \ingroup Control_Unit_User_Api
                 \ingroup Control_Unit_Definition_Api
                 */
                void addBinaryAttributeAsSubtypeToDataSet(const std::string& attribute_name,
                                                          const std::string& attribute_description,
                                                          DataType::BinarySubtype               subtype,
                                                          int32_t    cardinality,
                                                          DataType::DataSetAttributeIOAttribute attribute_direction);
                
                /*!
                 \ingroup Control_Unit_User_Api
                 \ingroup Control_Unit_Definition_Api
                 */
                void addBinaryAttributeAsSubtypeToDataSet(const std::string&            attribute_name,
                                                          const std::string&            attribute_description,
                                                          const std::vector<int32_t>&   subtype_list,
                                                          int32_t                       cardinality,
                                                          DataType::DataSetAttributeIOAttribute attribute_direction);
                
                /*!
                 \ingroup Control_Unit_User_Api
                 \ingroup Control_Unit_Definition_Api
                 */
                void addBinaryAttributeAsMIMETypeToDataSet(const std::string& attribute_name,
                                                           const std::string& attribute_description,
                                                           std::string mime_type,
                                                           DataType::DataSetAttributeIOAttribute attribute_direction);
				
				//!Get dataset attribute names
				/*!
				 Return all dataset attribute name
				 \ingroup Control_Unit_User_Api
				 \param attributesName the array that will be filled with the name
				 */
				void getDatasetAttributesName(vector<string>& attributesName);
				
				//!Get device attribute name that has a specified direction
				/*!
				 Return all dataset attribute name
				 \ingroup Control_Unit_User_Api
				 \param directionType the direction for attribute filtering
				 \param attributesName the array that will be filled with the name
				 */
				void getDatasetAttributesName(DataType::DataSetAttributeIOAttribute directionType,
											  vector<string>& attributesName);
				
				//!Get  attribute description
				/*!
				 Return the dataset description
				 \ingroup Control_Unit_User_Api
				 \param attributesName the name of the attribute
				 \param attributeDescription the returned description
				 */
				void getAttributeDescription(const string& attributesName,
											 string& attributeDescription);
				
				//!Get the value information for a specified attribute name
				/*!
				 Return the range value for the attribute
				 \ingroup Control_Unit_User_Api
				 \param attributesName the name of the attribute
				 \param rangeInfo the range and default value of the attribute
				 */
				int getAttributeRangeValueInfo(const string& attributesName,
											   chaos_data::RangeValueInfo& rangeInfo);
				
				//!Set the range values for an attribute of the device
				/*!
				 set the range value for the attribute of the device
				 \ingroup Control_Unit_User_Api
				 \param attributesName the name of the attribute
				 \param rangeInfo the range and default value of the attribute, the fields
				 of the struct are not cleaned, so if an attrbute doesn't has
				 some finromation, relative field are not touched.
				 */
				void setAttributeRangeValueInfo(const string& attributesName,
												chaos_data::RangeValueInfo& rangeInfo);
				
				//!Get the direction of an attribute
				/*!
				 Return the direcion of the attribute
				 \ingroup Control_Unit_User_Api
				 \param attributesName the name of the attribute
				 \param directionType the direction of the attribute
				 */
				int getAttributeDirection(const string& attributesName,
										  DataType::DataSetAttributeIOAttribute& directionType);
				
			};
		}
    }
}
#endif /* defined(__CHAOSFramework__DatasetDB__) */
