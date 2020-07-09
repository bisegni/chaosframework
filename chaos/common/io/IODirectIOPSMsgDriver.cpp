/*
 * Copyright 2020 INFN
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


#include "IODirectIOPSMsgDriver.h"
#include <chaos/common/configuration/GlobalConfiguration.h>
#define IODirectIOPSMsgDriver_LINFO_ INFO_LOG(IODirectIOPSMsgDriver)
#define IODirectIOPSMsgDriver_DLDBG_ DBG_LOG(IODirectIOPSMsgDriver)
#define IODirectIOPSMsgDriver_LERR_ ERR_LOG(IODirectIOPSMsgDriver)

using namespace chaos;
using namespace chaos::common::io;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::network;

using namespace std;
using namespace boost;

namespace chaos_data = chaos::common::data;
namespace chaos_dio = chaos::common::direct_io;
namespace chaos_dio_channel = chaos::common::direct_io::channel;

DEFINE_CLASS_FACTORY(IODirectIOPSMsgDriver, IODataDriver);

//using namespace memcache;
IODirectIOPSMsgDriver::IODirectIOPSMsgDriver(const std::string& alias):
IODirectIODriver(alias){
    msgbrokerdrv="kafka-rdk";
    msgbrokerdrv = GlobalConfiguration::getInstance()->getOption<std::string>(InitOption::OPT_MSG_BROKER_DRIVER);
    
    prod = chaos::common::message::MessagePSDriver::getProducerDriver(msgbrokerdrv);
    std::string gid=GlobalConfiguration::getInstance()->getOption<std::string>(InitOption::CONTROL_MANAGER_UNIT_SERVER_ALIAS);
    if(gid==""){
        gid="IODirectIODriver";
    }
    cons = chaos::common::message::MessagePSDriver::getConsumerDriver(msgbrokerdrv,gid);
    cons->addHandler(chaos::common::message::MessagePublishSubscribeBase::ONARRIVE, boost::bind(&IODirectIOPSMsgDriver::defaultHandler, this, _1));

}

IODirectIOPSMsgDriver::~IODirectIOPSMsgDriver() {
    // SO that if used as shared pointer will be called once the object is destroyed
}


void IODirectIOPSMsgDriver::init(void *_init_parameter) {
    IODirectIODriver::init(_init_parameter);
    if (GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_MSG_BROKER_SERVER)) {
        msgbroker = GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(InitOption::OPT_MSG_BROKER_SERVER);
        prod->addServer(msgbroker);

        if(prod->applyConfiguration()!=0){
            throw chaos::CException(-1,"cannot initialize Publish Subscribe Producer:"+prod->getLastError(),__PRETTY_FUNCTION__);
        }
        prod->start();
   
    }

    
}
void IODirectIOPSMsgDriver::defaultHandler(const chaos::common::message::ele_t& data){
    IODirectIOPSMsgDriver_DLDBG_<<" message from:"<<data.key;

    return;
}

int IODirectIOPSMsgDriver::subscribe(const std::string&key){
    int ret=-1;
    if(cons.get()!=NULL){
        ret=cons->subscribe(key);
        if(ret==0){
            cons->start();
        }

  
    }

    return ret;

}
int IODirectIOPSMsgDriver::addHandler(chaos::common::message::msgHandler cb){
    if(cons.get()){
        return cons->addHandler(chaos::common::message::MessagePublishSubscribeBase::ONARRIVE, cb);
    }
    IODirectIOPSMsgDriver_LERR_<<" Consumer not yet created, broker server:"<<msgbrokerdrv;

    return -1;
}

void IODirectIOPSMsgDriver::deinit() {
    IODirectIODriver::deinit();
    prod->stop();
    if(cons.get()){
        cons->stop();
    }
    IODirectIOPSMsgDriver_DLDBG_<<"Deinitialized";
}

int IODirectIOPSMsgDriver::storeData(const std::string& key,
                                CDWShrdPtr data_to_store,
                                DataServiceNodeDefinitionType::DSStorageType storage_type,
                                const ChaosStringSet& tag_set)  {
    int err = 0;
    if(data_to_store.get()==NULL){
        IODirectIOPSMsgDriver_LERR_ << "Packet not allocated";
        return -100;
    }
    
    if(!data_to_store->hasKey(DataServiceNodeDefinitionKey::DS_STORAGE_TYPE)){
        data_to_store->addInt32Value(DataServiceNodeDefinitionKey::DS_STORAGE_TYPE,storage_type);
    } else {
        data_to_store->setValue(DataServiceNodeDefinitionKey::DS_STORAGE_TYPE,storage_type);
    }
    if(tag_set.size()){
        if(!data_to_store->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_TAG)){
            data_to_store->addStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_TAG,*tag_set.begin());
        } else {
            data_to_store->setValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_TAG,*tag_set.begin());

        }
    }
    if((err=prod->pushMsgAsync(*data_to_store.get(), key))!=0){
        DEBUG_CODE(IODirectIOPSMsgDriver_LERR_ << "Error pushing "<<prod->getLastError());

    }
   
    return err;
}

chaos::common::data::CDataWrapper* IODirectIOPSMsgDriver::updateConfiguration(chaos::common::data::CDataWrapper* newConfigration) {
    //lock the feeder access
    //checkif someone has passed us the device indetification
    if(newConfigration->hasKey(DataServiceNodeDefinitionKey::DS_BROKER_ADDRESS_LIST)){
        chaos_data::CMultiTypeDataArrayWrapperSPtr liveMemAddrConfig = newConfigration->getVectorValue(DataServiceNodeDefinitionKey::DS_BROKER_ADDRESS_LIST);
        size_t numerbOfserverAddressConfigured = liveMemAddrConfig->size();
        for ( int idx = 0; idx < numerbOfserverAddressConfigured; idx++ ){

            string serverDesc = liveMemAddrConfig->getStringElementAtIndex(idx);
            IODirectIOPSMsgDriver_DLDBG_ << CHAOS_FORMAT("Adding broker %1% to IODirectIOPSMsgDriver", %serverDesc);

            if(msgbroker.size()==0){
                prod->addServer(serverDesc);
                
                if(prod->applyConfiguration()!=0){
                    throw chaos::CException(-1,"cannot initialize Publish Subscribe Producer:"+prod->getLastError(),__PRETTY_FUNCTION__);
                }
                prod->start();
                msgbroker=serverDesc;
                if(cons.get()!=NULL){
                    cons->addServer(msgbroker);
                    if(cons->applyConfiguration()!=0){
                        throw chaos::CException(-1,"cannot initialize Publish Subscribe Consumer:"+prod->getLastError(),__PRETTY_FUNCTION__);

                    }

                }

            }
            IODirectIOPSMsgDriver_DLDBG_ << CHAOS_FORMAT("Added broker %1% to IODirectIOPSMsgDriver", %serverDesc);
            
            //add new url to connection feeder, thi method in case of failure to allocate service will throw an eception
        }
       
    }
    if(newConfigration->hasKey(DataServiceNodeDefinitionKey::DS_SUBSCRIBE_KEY_LIST)){

        if(cons.get()){
         chaos_data::CMultiTypeDataArrayWrapperSPtr liveMemAddrConfig = newConfigration->getVectorValue(DataServiceNodeDefinitionKey::DS_SUBSCRIBE_KEY_LIST);
        size_t numerbOfserverAddressConfigured = liveMemAddrConfig->size();
        for ( int idx = 0; idx < numerbOfserverAddressConfigured; idx++ ){
            string key = liveMemAddrConfig->getStringElementAtIndex(idx);
            cons->subscribe(key);

        }

        }

    }
    chaos::common::data::CDataWrapper* ret=IODirectIODriver::updateConfiguration(newConfigration);

    return ret;
}
