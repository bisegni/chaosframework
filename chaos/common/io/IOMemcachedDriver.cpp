/*	
 *	IOMemcachedDriver.cpp
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

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "IOMemcachedDriver.h"
#include "../cconstants.h"
#include "../global.h"

#define LMEMDRIVER_ LAPP_ << "[Memcached IO Driver] - "

namespace chaos{ 
    using namespace std;
    using namespace boost;
    using namespace boost::algorithm;
    //using namespace memcache;
    
    /*
     * Driver constructor
     */
    IOMemcachedDriver::IOMemcachedDriver() {
        memClient = NULL;
    }
    
    /*
     * Driver distructor
     */
    IOMemcachedDriver::~IOMemcachedDriver() {
        //delete memcache client instance
        if(memClient) {
            memcached_free(memClient);
        }
    }
    
    /*
     * Init method, the has map has all received value for configuration
     * every implemented driver need to get all needed configuration param
     */
    void IOMemcachedDriver::init() throw(CException) {
        LMEMDRIVER_ << "Initializing Driver with libmemcache: " << LIBMEMCACHED_VERSION_STRING;
        
        //memcached_return_t configResult = MEMCACHED_SUCCESS;
        
        memClient = memcached("", 0);
    }
    
    /*
     * Deinitialization of memcached driver
     */
    void IOMemcachedDriver::deinit() throw(CException) {
        if(memClient){
            memcached_free(memClient);
            memClient = NULL;
        }
    }
    
    /*
     * This method will cache all object passed to driver
     */
    void IOMemcachedDriver::storeData(CDataWrapper *dataToStore) throw(CException) {
        memcached_return_t mcSetResult = MEMCACHED_SUCCESS;
        if(!dataToStore) return;
        
        //get the key to store data on the memcached
        string key = dataToStore->getStringValue(DataPackKey::CS_CSV_DEVICE_ID);
        SerializationBuffer* serialization = dataToStore->getBSONData();
        if(!serialization) {
            return;
        }
        
        boost::mutex::scoped_lock lock(useMCMutex);
        mcSetResult = memcached_set(memClient, key.c_str(), key.length(), serialization->getBufferPtr(), serialization->getBufferLen(), 0, 0);
        //for debug
        if(mcSetResult!=MEMCACHED_SUCCESS) {
#if DEBUG
            LMEMDRIVER_ << "cache data submition error";
#endif
        }
        delete(serialization);
    }
    
    /*
     * This method retrive the cached object by his key
     */
    ArrayPointer<CDataWrapper>*  IOMemcachedDriver::retriveData(CDataWrapper * const keyData)  throw(CException) {
        //check for key length
        string key = keyData->getStringValue(DataPackKey::CS_CSV_DEVICE_ID);
        return retriveData(key);
    }
    
    /*
     * This method retrive the cached object by CSDawrapperUsed as query key and
     * return a pointer to the class ArrayPointer of CDataWrapper type
     */
    ArrayPointer<CDataWrapper>* IOMemcachedDriver::retriveData(string& key)  throw(CException) {
        ArrayPointer<CDataWrapper> *result = new ArrayPointer<CDataWrapper>();
        
        char *value = retriveRawData(key);
        if (value) {
            //some value has been received
            //allocate the data wrapper object with serialization got from memcached
            //CDataWrapper *dataWrapper = 
            result->add(new CDataWrapper(value));
            free(value);
        }
        return result;
    }
    
    /*
     * This method retrive the cached object by CSDawrapperUsed as query key and
     * return a pointer to the class ArrayPointer of CDataWrapper type
     */
    char* IOMemcachedDriver::retriveRawData(string& key, size_t *dim)  throw(CException) {
        uint32_t flags= 0;
        size_t value_length= 0;
        memcached_return_t mcSetResult = MEMCACHED_SUCCESS;
        boost::mutex::scoped_lock lock(useMCMutex);
        char* result =  memcached_get(memClient, key.c_str(), key.length(), &value_length, &flags,  &mcSetResult);
        if(dim) *dim = value_length;
        return result;
    }    
    
    /*
     Update the driver configuration
     */
    CDataWrapper* IOMemcachedDriver::updateConfiguration(CDataWrapper* newConfigration) {
        memcached_return_t configResult = MEMCACHED_SUCCESS;
        //boost::mutex::scoped_lock lock(usageMutex);
        LMEMDRIVER_ << "Update Configuration";
        
        if(!memClient) throw CException(0, "Write memcached structure not allocated", "IOMemcachedDriver::updateConfiguration");
        
        if(newConfigration->hasKey(LiveHistoryMDSConfiguration::CS_DM_LD_SERVER_ADDRESS) && memClient){
            LMEMDRIVER_ << "Get the DataManager LiveData address value";
            auto_ptr<CMultiTypeDataArrayWrapper> liveMemAddrConfig(newConfigration->getVectorValue(LiveHistoryMDSConfiguration::CS_DM_LD_SERVER_ADDRESS));
            //update the live data address
            
            //we need forst to reset all the server list 
            memcached_servers_reset(memClient);
            
            
            //const char *config_string= "--SERVER=192.168.1.2";
            //configResult = memcached_parse_configuration(memClient, config_string, strlen(config_string));
            //iterate the server name
            size_t numerbOfserverAddressConfigured = liveMemAddrConfig->size();
            for ( int idx = 0; idx < numerbOfserverAddressConfigured; idx++ ){
                vector<string> serverTokens;
                string serverDesc = liveMemAddrConfig->getStringElementAtIndex(idx);
                algorithm::split(serverTokens, serverDesc, is_any_of(":"));
                if(serverTokens.size()==2){
                    in_port_t port = lexical_cast<in_port_t>(serverTokens[1]);
                    LMEMDRIVER_ << "trye to configure " << serverTokens[0] << "on port "<<port;
                    configResult = memcached_server_add(memClient, serverTokens[0].c_str(), port);
                    if(configResult != MEMCACHED_SUCCESS) 
                        LMEMDRIVER_ << "Error Configuration server '" << liveMemAddrConfig->getStringElementAtIndex(idx) << "' with error: " << memcached_strerror(NULL, configResult);
                    else
                        LMEMDRIVER_ << "The server '" << liveMemAddrConfig->getStringElementAtIndex(idx) << "' has been configurated";
                }else{
                    LMEMDRIVER_ << "Wrong Server Description '" << liveMemAddrConfig->getStringElementAtIndex(idx) << "'";
                }
            }
            LMEMDRIVER_ << "write param";
            configResult = memcached_behavior_set(memClient, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, (uint64_t)1);
            configResult = memcached_behavior_set(memClient, MEMCACHED_BEHAVIOR_NO_BLOCK, (uint64_t)1);
            configResult = memcached_behavior_set(memClient, MEMCACHED_BEHAVIOR_DISTRIBUTION, MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA);
            configResult = memcached_behavior_set(memClient, MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT, 1);
            configResult = memcached_behavior_set(memClient, MEMCACHED_BEHAVIOR_AUTO_EJECT_HOSTS, 1);
            configResult = memcached_behavior_set(memClient, MEMCACHED_BEHAVIOR_RETRY_TIMEOUT,1);
        }
        return NULL; 
    }
}
