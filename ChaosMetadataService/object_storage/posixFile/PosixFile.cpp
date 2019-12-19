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

#include "PosixFile.h"
#include <boost/filesystem.hpp>

#define INFO INFO_LOG(PosixFile)
#define DBG  DBG_LOG(PosixFile)
#define ERR  ERR_LOG(PosixFile)

using namespace chaos::metadata_service::object_storage;
namespace chaos{
    namespace metadata_service{
        namespace object_storage{

            DEFINE_CLASS_FACTORY(PosixFile,
                     chaos::service_common::persistence::data_access::AbstractPersistenceDriver);

PosixFile::PosixFile(const std::string& name):
AbstractPersistenceDriver(name){}
PosixFile::PosixFile(){
    denied=false;
    

    

}

PosixFile::~PosixFile() {}

int PosixFile::pushObject(const std::string& key,
                                           const ChaosStringSetConstSPtr meta_tags,
                                           const chaos::common::data::CDataWrapper& stored_object){

}
                    
                    //!Retrieve an object from the object persistence layer
int PosixFile::getObject(const std::string& key,
                                          const uint64_t& timestamp,
                                          chaos::common::data::CDWShrdPtr& object_ptr_ref){

                                          }
                    
                    //!Retrieve the last inserted object from the object persistence layer
int PosixFile::getLastObject(const std::string& key,
                                              chaos::common::data::CDWShrdPtr& object_ptr_ref){

                                              }
                    
                    //!delete objects that are contained between intervall (exstreme included)
int PosixFile::deleteObject(const std::string& key,
                                             uint64_t start_timestamp,
                                             uint64_t end_timestamp) {

                                             }
                    
                    //!search object into object persistence layer
int PosixFile::findObject(const std::string& key,
                                           const ChaosStringSet& meta_tags,
                                           const ChaosStringSet& projection_keys,
                                           const uint64_t timestamp_from,
                                           const uint64_t timestamp_to,
                                           const uint32_t page_len,
                                           VectorObject& found_object_page,
                                           chaos::common::direct_io::channel::opcode_headers::SearchSequence& last_record_found_seq){
                                               return 0;
                                           }
                    
                    //!fast search object into object persistence layer
                    /*!
                     Fast search return only data index to the client, in this csae client ned to use api to return the single
                     or grouped data
                     */
int PosixFile::findObjectIndex(const DataSearch& search,
                                                VectorObject& found_object_page,
                                                chaos::common::direct_io::channel::opcode_headers::SearchSequence& last_record_found_seq){
                                                    return 0;
                                                }
                    
                    //! return the object asosciated with the index array
                    /*!
                     For every index object witl be returned the associated data object, if no data is received will be
                     insert an empty object
                     */
int PosixFile::getObjectByIndex(const chaos::common::data::CDWShrdPtr& index,
                                                 chaos::common::data::CDWShrdPtr& found_object){
                                                     return 0;
                                                 }
                    
                    //!return the number of object for a determinated key that are store for a time range
int PosixFile::countObject(const std::string& key,
                                            const uint64_t timestamp_from,
                                            const uint64_t timestamp_to,
                                            const uint64_t& object_count){

                                                return 0;

}


void PosixFile::init(void *init_data) throw (chaos::CException) {
    //call sublcass
    AbstractPersistenceDriver::init(init_data);
    ChaosStringVector url_list = ChaosMetadataService::getInstance()->setting.fsobject_storage_setting.url_list;
    if(url_list.size()){
        basedatapath=url_list[0];
    } else if(metadata_service::ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_DATA_DIR)){
        basedatapath=metadata_service::ChaosMetadataService::getInstance()->getGlobalConfigurationInstance()->getOption< std::string>(OPT_DATA_DIR));
        
    } else {
        basedatapath=boost::filesystem::current_path().string();
    }
   
   /* const std::string user = ChaosMetadataService::getInstance()->setting.fsobject_storage_setting.key_value_custom_param["user"];
    const std::string password = ChaosMetadataService::getInstance()->setting.fsobject_storage_setting.key_value_custom_param["pwd"];
    const std::string database = ChaosMetadataService::getInstance()->setting.fsobject_storage_setting.key_value_custom_param["db"];
    MapKVP& obj_stoarge_kvp = metadata_service::ChaosMetadataService::getInstance()->setting.fsobject_storage_setting.key_value_custom_param;
  */
  if ((boost::filesystem::exists(basedatapath) == false) &&
            (boost::filesystem::create_directories(basedatapath) == false)) {
         ERR<<"cannot create directory:"<<basedatapath;
        denied=true;

  }
 
    //register the data access implementations
    registerDataAccess<ObjectStorageDataAccess>(new MongoDBObjectStorageDataAccess(BaseMongoDBDiver::getPool()));
}

void PosixFile::deinit() throw (chaos::CException) {
    //call sublcass
    AbstractPersistenceDriver::deinit();
}

void PosixFile::deleteDataAccess(void *instance) {
    AbstractDataAccess *da_instance = static_cast<AbstractDataAccess*>(instance);
    if(da_instance != NULL)delete(da_instance);
}


                                                        
        }
    }
}
