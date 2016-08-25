/*
 *	DataBrokerEditor.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 25/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__5E877DE_5866_4F13_9762_9D4BBF1143EE_DataBrokerEditor_h
#define __CHAOSFramework__5E877DE_5866_4F13_9762_9D4BBF1143EE_DataBrokerEditor_h

#include <chaos/common/utility/LockableObject.h>

#include <chaos/cu_toolkit/data_manager/data_manager_types.h>
#include <chaos/cu_toolkit/data_manager/manipulation/DatasetEditor.h>
#include <chaos/cu_toolkit/data_manager/manipulation/DatasetCacheWrapper.h>

namespace chaos {
    namespace cu {
        namespace data_manager {
            
            //!forward decalration
            class DataBroker;
            
            namespace manipulation {
                
                //! root class for the editing of the dataset
                class DataBrokerEditor {
                    friend class chaos::cu::data_manager::DataBroker;
                    
                    //reference to root dataset element
                    chaos::common::utility::LockableObject<DatasetElementContainer> container_dataset;
                    
                    DECNameIndex&        ds_index_name;
                    DECOrderedIndex&     ds_index_ordered;
                    DECTypeNameIndex&    ds_index_type_name;
                    
                    //add new dataset
                    /*!
                     New dataset is added checking if no other dataset has the same name and type.
                     In case dataset already exists it attribute will be updated with new values
                     */
                    int addNewDataset(chaos::common::data::structured::Dataset& new_dataset);
                    
                    void updateAttributePorertyForDataset(chaos::common::data::structured::DatasetAttribute attr_src,
                                                          chaos::common::data::structured::DatasetAttribute attr_dst);
                protected:
                    DataBrokerEditor();
                    ~DataBrokerEditor();
                    
                    //! add new dataset
                    /*!
                     Create a new empty dataset
                     \param name is the name of the newly create dataset
                     \param shared_key is the key used to publish the dataset
                     \param type is the type of the newly created dataset
                     \ingroup Dataset_Editor_Api
                     */
                    int addNewDataset(const std::string& name,
                                      const chaos::DataType::DatasetType type,
                                      const std::string& shared_key);
                    
                    //!check if a dataset is present by his name
                    bool hasDataset(const std::string& dataset_name);
                    
                    //!Return editor for the specified dataset
                    std::auto_ptr<DatasetEditor> getDatasetEditorFor(const std::string& ds_name);
                    
                    //!Return wrapper for the specified dataset cache
                    std::auto_ptr<DatasetCacheWrapper> getDatasetCacheWrapperFor(const std::string& ds_name);
                    
                    //! return a CDataWrapper pointr with the dataset serialization
                    std::auto_ptr<chaos::common::data::CDataWrapper> serialize();
                    
                    //! fille curren databrocker with serialization
                    void deserialize(chaos::common::data::CDataWrapper& serialization);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__5E877DE_5866_4F13_9762_9D4BBF1143EE_DataBrokerEditor_h */
