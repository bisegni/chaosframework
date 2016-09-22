/*
 *	ControlUnitDataAccess.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
#include "ControlUnitDataAccess.h"

#include <chaos/common/chaos_constants.h>
#include <chaos/common/io/IODirectIODriver.h>
#include <chaos/common/network/NetworkBroker.h>

using namespace chaos::common::io;
using namespace chaos::common::utility;
using namespace chaos::common::network;
using namespace chaos::metadata_service::persistence::data_access;

DEFINE_DA_NAME(ControlUnitDataAccess)

CHAOS_DEFINE_VECTOR_FOR_TYPE(boost::shared_ptr<chaos::common::data::CDataWrapper>, CDSList)

//! default constructor
ControlUnitDataAccess::ControlUnitDataAccess(DataServiceDataAccess *_data_service_da):
AbstractDataAccess("ControlUnitDataAccess"),
data_service_da(_data_service_da){}

//!default destructor
ControlUnitDataAccess::~ControlUnitDataAccess() {}

//!Remove all data befor the unit_ts timestamp
/*!
 \param control_unit_id the unique id of the control unit
 \param unit_ts is the timestamp befor wich we want to erase all data(included it)
 */
int ControlUnitDataAccess::eraseControlUnitDataBeforeTS(const std::string& control_unit_id,
                                                        uint64_t unit_ts) {
    std::auto_ptr<IODataDriver> data_driver(ObjectFactoryRegister<IODataDriver>::getInstance()->getNewInstanceByName("IODirectIODriver"));
    if(data_driver.get() == NULL) return -1;
    
    int err = 0;
    CDSList best_cds;
    //set the information
    IODirectIODriverInitParam init_param;
    std::memset(&init_param, 0, sizeof(IODirectIODriverInitParam));
    //get client and endpoint
    init_param.network_broker = NetworkBroker::getInstance();
    init_param.client_instance = NULL;
    init_param.endpoint_instance = NULL;
    ((IODirectIODriver*)data_driver.get())->setDirectIOParam(init_param);
    try{
        data_driver->init(NULL);
        
        data_service_da->getBestNDataService(best_cds, 3);
        
        for(CDSListIterator it = best_cds.begin();
            it != best_cds.end();
            it++) {
            if(!(*it)->hasKey(chaos::NodeDefinitionKey::NODE_DIRECT_IO_ADDR) || !(*it)->hasKey(chaos::DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT)) continue;
            
            //add endpoint
            const std::string endpoint_addr =  boost::str(boost::format("%1%|%2%")%
                                                          (*it)->getStringValue(chaos::NodeDefinitionKey::NODE_DIRECT_IO_ADDR)%
                                                          (*it)->getInt32Value(chaos::DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT));
            ((IODirectIODriver*)data_driver.get())->addServerURL(endpoint_addr);
        }
        
        const std::string output_key	= control_unit_id + DataPackPrefixID::OUTPUT_DATASE_PREFIX;
        const std::string input_key     = control_unit_id + DataPackPrefixID::INPUT_DATASE_PREFIX;
        const std::string system_key	= control_unit_id + DataPackPrefixID::SYSTEM_DATASE_PREFIX;
        const std::string custom_key	= control_unit_id + DataPackPrefixID::CUSTOM_DATASE_PREFIX;
        
        err = data_driver->removeData(output_key,
                                      0,
                                      unit_ts);
        err = data_driver->removeData(input_key,
                                      0,
                                      unit_ts);
        err = data_driver->removeData(system_key,
                                      0,
                                      unit_ts);
        err = data_driver->removeData(custom_key,
                                      0,
                                      unit_ts);
    } catch(CException& ex) {
        err = ex.errorCode;
    } catch(...) {
        err = -10000;
    }
    data_driver->deinit();
    
    return err;
}
