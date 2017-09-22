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
                    
                    
                    
                    //!Return editor for the specified dataset
                    ChaosUniquePtr<DatasetEditor> getDatasetEditorFor(const std::string& ds_name);
                    
                    //!Return wrapper for the specified dataset cache
                    ChaosUniquePtr<DatasetCacheWrapper> getDatasetCacheWrapperFor(const std::string& ds_name);
                    
                    //! return a CDataWrapper pointr with the dataset serialization
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize();
                    
                    //! fille curren databrocker with serialization
                    void deserialize(chaos::common::data::CDataWrapper& serialization);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__5E877DE_5866_4F13_9762_9D4BBF1143EE_DataBrokerEditor_h */
