/*
 *	DeviceSchemaDB.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__DeviceSchemaDB__
#define __CHAOSFramework__DeviceSchemaDB__

#include <string>

#include <chaos/common/data/CUSchemaDB.h>

namespace chaos {
    namespace cu {
        
        class AbstractControlUnit;
        
        class DeviceSchemaDB : private CUSchemaDB {
            friend class AbstractControlUnit;
            
            std::string deviceID;
            
        protected:
            void setDeviceID(std::string _deviceID);
        public:
            DeviceSchemaDB(bool onMemory);
            ~DeviceSchemaDB();
            
            
            const char * getDeviceID();
            
            //! Add device dataset definitio by serialized form
            /*!
             Fill the database with the devices and their dataset with the content
             of the serialized data
             
             \param serializedDataset serialze dataset
             */
            void addAttributeToDataSetFromDataWrapper(CDataWrapper& serializedDataset);
            
            //! Fill a CDataWrapper with a single device information
            /*!
             fill a CDataWrapper with the dataset decode
             */
            void fillDataWrapperWithDataSetDescription(CDataWrapper&);
            
            //! Add dataset attribute
            /*!
             Add the new attribute to the deviceID dataset specifing
             the default parameter
             
             \param attributeName the name of the new attribute
             \param attributeDescription the description of the attribute
             \param attributeType the type of the new attribute
             \param attributeDirection the direction of the new attribute
             */
            void addAttributeToDataSet(const char*const attributeName,
                                       const char*const attributeDescription,
                                       DataType::DataType attributeType,
                                       DataType::DataSetAttributeIOAttribute attributeDirection);
            
            
            
            //!Get dataset attribute names
            /*!
             Return all dataset attribute name
             \param attributesName the array that will be filled with the name
             */
            void getDatasetAttributesName(vector<string>& attributesName);
            
            //!Get device attribute name that has a specified direction
            /*!
             Return all dataset attribute name
             \param directionType the direction for attribute filtering
             \param attributesName the array that will be filled with the name
             */
            void getDatasetAttributesName(DataType::DataSetAttributeIOAttribute directionType,
                                          vector<string>& attributesName);
            
            //!Get  attribute description
            /*!
             Return the dataset description
             \param attributesName the name of the attribute
             \param attributeDescription the returned description
             */
            void getAttributeDescription(const string& attributesName,
                                         string& attributeDescription);
            
            //!Get the value information for a specified attribute name
            /*!
             Return the range value for the attribute
             \param attributesName the name of the attribute
             \param rangeInfo the range and default value of the attribute
             */
            void getAttributeRangeValueInfo(const string& attributesName,
                                            RangeValueInfo& rangeInfo);
            
            //!Get the direction of an attribute
            /*!
             Return the direcion of the attribute
             \param attributesName the name of the attribute
             \param directionType the direction of the attribute
             */
            int getAttributeDirection(const string& attributesName,
                                      DataType::DataSetAttributeIOAttribute& directionType);
            
        };
    }
}
#endif /* defined(__CHAOSFramework__DeviceSchemaDB__) */
