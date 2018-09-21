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

#include "IOCU.h"


using namespace chaos;
using namespace chaos::common::data::cache;
using namespace chaos::cu::driver_manager::driver;

PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(IOCU)

#define IOCULAPP_		LAPP_ << "[IOCU] "
#define IOCULDBG_		LDBG_ << "[IOCU] " << __PRETTY_FUNCTION__ << " "
#define IOCULERR_		LERR_ << "[IOCU] " << __PRETTY_FUNCTION__ << "("<<__LINE__<<") "

/*
 Construct
 */
IOCU::IOCU(const string& _control_unit_id, const string& _control_unit_param, const ControlUnitDriverList& _control_unit_drivers):
RTAbstractControlUnit(_control_unit_id, _control_unit_param, _control_unit_drivers) {
    
    driver=new BasicIODriverInterface(getAccessoInstanceByIndex(0));
    
    assert(driver);
    driver_dataset_size=driver->getDatasetSize();
    driver_dataset=0;
    if(driver_dataset_size>0){
        driver_dataset = (ddDataSet_t *)malloc( driver_dataset_size);
        assert(driver_dataset);
    }
}

/*
 Destructor
 */
IOCU::~IOCU() {
    if(driver){
        driver->deinitIO();
        delete driver;
        driver = NULL;
    }
    if(driver_dataset){
        free(driver_dataset);
        driver_dataset=0;
    }
    
}

//!Return the definition of the control unit
/*!
 The api that can be called withi this method are listed into
 "Control Unit Definition Public API" module into html documentation
 (chaosframework/Documentation/html/group___control___unit___definition___api.html)
 */
void IOCU::unitDefineActionAndDataset()  {
    int ret;
    IOCULAPP_ << "UnitDefine";
    ret=driver->getDataset(driver_dataset,driver_dataset_size);
    
    for(int cnt=0;cnt<ret/sizeof(ddDataSet_t);cnt++){
        int max_size=driver_dataset[cnt].maxsize;
        int type=(int)driver_dataset[cnt].type ;
        if(driver_dataset[cnt].type  == chaos::DataType::TYPE_CLUSTER){
            type = chaos::DataType::TYPE_BYTEARRAY;
            
        }
        if(type & chaos::DataType::TYPE_ACCESS_ARRAY){
            type&=~ (int)chaos::DataType::TYPE_ACCESS_ARRAY;
            switch(driver_dataset[cnt].type){
                case chaos::DataType::TYPE_INT32:
                case chaos::DataType::TYPE_BOOLEAN:
                    max_size = std::max(sizeof(int),sizeof(bool))*driver_dataset[cnt].maxsize;
                    break;
                case chaos::DataType::TYPE_INT64:
                case chaos::DataType::TYPE_DOUBLE:
                    max_size = std::max(sizeof(double),sizeof(int64_t))*driver_dataset[cnt].maxsize;
                    break;
                default:
                    max_size=driver_dataset[cnt].maxsize;
                    break;
            }
            type = (int)chaos::DataType::TYPE_BYTEARRAY;
        }
        
        
        // transform vectors in byte array
        addAttributeToDataSet(driver_dataset[cnt].name,
                              driver_dataset[cnt].desc,
                              (chaos::DataType::DataType)type,
                              driver_dataset[cnt].dir,
                              max_size
                              );
        
        if(driver_dataset[cnt].dir==chaos::DataType::Input){
            input_size.push_back(max_size);
        } else if(driver_dataset[cnt].dir==chaos::DataType::Output){
            output_size.push_back(max_size);
            
        }
        IOCULDBG_<<"adding attribute:"<<driver_dataset[cnt].name<<","<<driver_dataset[cnt].desc<<","<<driver_dataset[cnt].type<<","<<driver_dataset[cnt].dir<<","<<max_size;
    }
    
}


//!Define custom control unit attribute
void IOCU::unitDefineCustomAttribute() {
    
}

//!Initialize the Custom Control Unit
void IOCU::unitInit()  {
    getAttributeCache()->resetChangedInputIndex();
    
}

//!Execute the work, this is called with a determinated delay, it must be as fast as possible
void IOCU::unitStart()  {
    
}

//!Execute the Control Unit work
void IOCU::unitRun()  {
    //get the output attribute pointer form the internal cache
    std::vector<int>::iterator i;
    int cnt,ret,changed=0;
    //IOCULAPP_<<"UnitRun";
    for(i=output_size.begin(),cnt=0;i!=output_size.end();i++,cnt++){
        char buffer[*i];
        if((ret=driver->read(buffer,cnt,*i))){
            changed++;
            IOCULDBG_<<"Reading output channel "<<cnt<<", size :"<<*i <<" ret:"<<ret;
            getAttributeCache()->setOutputAttributeValue(cnt, (void*)buffer, *i);
        }
    }
    
    //! set output dataset as changed
    if(changed) {
        getAttributeCache()->setOutputDomainAsChanged();
    }
}

//!Execute the Control Unit work
void IOCU::unitStop()  {
    
}

//!Deinit the Control Unit
void IOCU::unitDeinit()  {
    
}

//! pre imput attribute change
void IOCU::unitInputAttributePreChangeHandler()  {
    
}

//! attribute changed handler
void IOCU::unitInputAttributeChangedHandler()  {
    std::vector<VariableIndexType> changed;
    std::vector<VariableIndexType>::iterator j;
    getAttributeCache()->getChangedInputAttributeIndex(changed);
    //IOCULAPP_<<"UnitRun";
    
    for(j=changed.begin();j!=changed.end();j++){
        const char** buffer;
        
        getAttributeCache()->getReadonlyCachedAttributeValue<char>(DOMAIN_INPUT, *j, &buffer);
        if(driver->write(buffer,*j,input_size[*j])){
            IOCULDBG_<<"writing output channel "<<*j<<", size :"<<input_size[*j];
        }
    }
    getAttributeCache()->resetChangedInputIndex();
}

/*
 CDataWrapper *IOCU::my_custom_action(CDataWrapper *actionParam, bool& detachParam) {
	CDataWrapper *result =  new CDataWrapper();
	return result;
 }
 */
