/*
 *	DatasetEditor.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 29/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__BC70697_B7F9_4F7F_B4AF_FEDFCC0758D9_DatasetEditor_h
#define __CHAOSFramework__BC70697_B7F9_4F7F_B4AF_FEDFCC0758D9_DatasetEditor_h

#include <chaos/cu_toolkit/data_manager/data_manager_types.h>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace manipulation {
                class DatasetEditor {
                    DatasetElement::DatasetElementPtr dataset_element;
                public:
                    DatasetEditor(DatasetElement::DatasetElementPtr& _dataset_element);
                    ~DatasetEditor();
                    
                    //!
                    const chaos::DataType::DatasetType getType() const;
                    
                    //!Add a new attribute
                    /*!
                     Add the new attribute to a dataset
                     \ingroup Dataset_Editor_Api
                     \ingroup Control_Unit_Definition_Api
                     \param ds_name the name of the dataset that is parent of the attribute
                     \param attr_name the name of the new attribute
                     \param attribute_description the description of the attribute
                     \param attr_type the type of the new attribute
                     */
                    int addAttributeToDataset(const std::string& attr_name,
                                              const std::string& attr_description,
                                              const chaos::DataType::DataType attr_type,
                                              uint32_t maxSize = 0);
                    
                    /*!
                     \ingroup Dataset_Editor_Api
                     \ingroup Control_Unit_Definition_Api
                     */
                    int addBinaryAttributeAsSubtypeToDataSet(const std::string&        attr_name,
                                                             const std::string&        attr_description,
                                                             DataType::BinarySubtype   attr_subtype,
                                                             int32_t                   attr_cardinality);
                    
                    /*!
                     \ingroup Dataset_Editor_Api
                     \ingroup Control_Unit_Definition_Api
                     */
                    int addBinaryAttributeAsSubtypeToDataSet(const std::string&                                     attr_name,
                                                             const std::string&                                     attr_description,
                                                             const common::data::structured::DatasetSubtypeList&    attr_subtype_list,
                                                             int32_t                                                attr_cardinality);
                    
                    /*!
                     \ingroup Dataset_Editor_Api
                     \ingroup Control_Unit_Definition_Api
                     */
                    int addBinaryAttributeAsMIMETypeToDataSet(const std::string& attr_name,
                                                              const std::string& attr_description,
                                                              const std::string& attr_mime_type);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__BC70697_B7F9_4F7F_B4AF_FEDFCC0758D9_DatasetEditor_h */
