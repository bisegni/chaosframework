//
//  ControlUnit.cpp
//  ChaosFramework
//
//  Created by Claudio Bisegni on 09/03/11.
//  Copyright 2011 INFN. All rights reserved.
//


#include <chaos/common/global.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/cu_toolkit/ControlManager/AbstractControlUnit.h>
#include <chaos/cu_toolkit/DataManager/DataManager.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

using namespace chaos;
using namespace std;
using namespace boost;
using namespace boost::uuids;

#pragma mark constructor

AbstractControlUnit::AbstractControlUnit(){
    _sharedInit();
}

AbstractControlUnit::AbstractControlUnit(const char *descJsonPath){
     _sharedInit();
    initWithJsonFilePath(descJsonPath);
}

AbstractControlUnit::~AbstractControlUnit() {
}

/*
 thi is the shared intiialization phase of the
 abstract control unit
 */
void AbstractControlUnit::_sharedInit() {
    cuInstance = UUIDUtil::generateUUIDLite();
}

/*
 return the CU name
 */
const char * AbstractControlUnit::getCUName(){
    return cuName.size()?cuName.c_str():"Name no yet configured";
};

/*
 return the CU name
 */
const char * AbstractControlUnit::getCUInstance(){
    return cuInstance.size()?cuInstance.c_str():"Instance no yet configured";
};

/*
 Add a new KeyDataStorage for a specific key
 */
void AbstractControlUnit::addKeyDataStorage(const char *key, KeyDataStorage* keyDatStorafePointer) {
    keyDataStorageMap.insert(make_pair(key, keyDatStorafePointer));
}

/*
 load the json file setupped into jsonSetupFilePath class attributed
 */
void AbstractControlUnit::loadCDataWrapperForJsonFile(CDataWrapper& setupConfiguration)  throw (CException)  {
    int tmpInt;
    string tmpStr;
    auto_ptr<CDataWrapper> csDWFromJson;
        //CHAOS_ASSERT(setupConfiguration)
        //check if the path is filled
    if(!jsonSetupFilePath.size()) return;
        
    FILE *cfgFile = fopen(jsonSetupFilePath.c_str(), "r");
    //inFile.open(jsonSetupFilePath.c_str(), ios::binary );
    if(cfgFile){
            //check if the shared pointer if allcoated
        fseek (cfgFile, 0, SEEK_END	 );
        long length = ftell(cfgFile);
        rewind (cfgFile);
        
            //need to make 1 byte larger for simualte c string
        char *buffer = new char[length];
        memset(buffer, 0, length);
        fread(buffer, 1, length, cfgFile);
        fclose (cfgFile);

        while(buffer[length-1]=='\r' || buffer[length-1]=='\n'){
            buffer[(length--)-1]=(char)0;
        }

            //create the csdatafrapper and append all elemento to the input setupConfiguration
        csDWFromJson.reset(new CDataWrapper(buffer, false));
            //delete the buffer
        delete[] buffer;

        
        //add default value from jfg file
        if(csDWFromJson->hasKey(CUDefinitionKey::CS_CM_CU_NAME)){
        	tmpStr = csDWFromJson->getStringValue(CUDefinitionKey::CS_CM_CU_NAME);
        	setupConfiguration.addStringValue(CUDefinitionKey::CS_CM_CU_NAME, tmpStr);
        }
        
        if(csDWFromJson->hasKey(CUDefinitionKey::CS_CM_CU_DESCRIPTION)){
        	tmpStr = csDWFromJson->getStringValue(CUDefinitionKey::CS_CM_CU_DESCRIPTION);
        	setupConfiguration.addStringValue(CUDefinitionKey::CS_CM_CU_DESCRIPTION, tmpStr);
        }
        if(csDWFromJson->hasKey(CUDefinitionKey::CS_CM_CU_CLASS)){
                	tmpStr = csDWFromJson->getStringValue(CUDefinitionKey::CS_CM_CU_CLASS);
                	setupConfiguration.addStringValue(CUDefinitionKey::CS_CM_CU_CLASS, tmpStr);
                }

        if(csDWFromJson->hasKey(CUDefinitionKey::CS_CM_CU_AUTOSTART)){
                       	tmpInt = csDWFromJson->getInt32Value(CUDefinitionKey::CS_CM_CU_AUTOSTART);
                       	setupConfiguration.addInt32Value(CUDefinitionKey::CS_CM_CU_AUTOSTART, tmpInt);
                       }
        if(csDWFromJson->hasKey(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY)){
                       	tmpStr = csDWFromJson->getStringValue(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY);
                       	setupConfiguration.addStringValue(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY, tmpStr);
                       }


        //auto_add all possible dataset attribute from the configuration file
        if(csDWFromJson->hasKey(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION)) {
        	//in the configuration file there are some attribute for the dataset we need to register it
        	auto_ptr<CMultiTypeDataArrayWrapper> datasetAttributes (csDWFromJson->getVectorValue(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION));
        	for (unsigned int i = 0;  i < datasetAttributes->size() ; i++) {
        		addAttributeToDataSetFromDataWrapper(*datasetAttributes->getCDataWrapperElementAtIndex(i));
			}
        }
    }
}


/*
 fill the CDataWrapper with AbstractCU system configuration, this method
 is called after getStartConfiguration directly by sandbox. in this method
 are defined the action for the input element of the dataset
 */
void AbstractControlUnit::_defineActionAndDataset(CDataWrapper& setupConfiguration)  throw(CException) {
    vector<string> tempStringVector;
    //global var initialization
    scheduleDelay = 0;
    
        //add the CU isntance, this can be redefinide by user in the defineActionAndDataset method
        //for let the CU have the same instance at every run
    setupConfiguration.addStringValue(CUDefinitionKey::CS_CM_CU_INSTANCE, cuInstance);
        //check if as been setuped a file for configuration
    LAPP_ << "Check if as been setup a json file path to configura CU:" << CU_IDENTIFIER_C_STREAM;
    loadCDataWrapperForJsonFile(setupConfiguration);
    
        //first call the setup abstract method used by the implementing CU to define action, dataset and other
        //usefull value
    LAPP_ << "Define Actions and Dataset for:" << CU_IDENTIFIER_C_STREAM;
    defineActionAndDataset(setupConfiguration);    
    
    
    //add the scekdule dalay for the sandbox
    if(scheduleDelay){
    	//in this case ovverrride the config file
    	setupConfiguration.addInt32Value(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY , scheduleDelay);
    }

   /* tempStringVector.clear();
    CUSchemaDB::getAllDeviceId(tempStringVector);
    if(tempStringVector.size()){  
        LAPP_ << "Configure the DeviceID for the CU:" << CU_IDENTIFIER_C_STREAM;
        for (vector<string>::iterator devIdIter = tempStringVector.begin();
             devIdIter != tempStringVector.end();
             devIdIter++) {
            //add managed device id
            setupConfiguration.appendStringToArray(*devIdIter);   
        }
        //finalize array
        setupConfiguration.finalizeArrayForKey(CUDefinitionKey::ControlUnitSandBoxConstant::CS_CM_CU_MANAGED_DEVICE_ID);
    }*/
    
    //for now we need only to add custom action for expose to rpc
    //input element of the dataset
    LAPP_ << "Define the base action for map the input attribute of the dataset of the CU:" << CU_IDENTIFIER_C_STREAM;
    AbstActionDescShrPtr 
    actionDescription = addActionDescritionInstance<AbstractControlUnit>(this, 
                                                              &AbstractControlUnit::_setDatasetAttribute, 
                                                              "setDatasetElement", 
                                                              "method for set the input element for the dataset");
    
    shared_ptr<CDataWrapper> elementDatasetDescription(new CDataWrapper());
    
    tempStringVector.clear();
    vector< shared_ptr<CDataWrapper> > domainAttributeList; 
    CUSchemaDB::getAllDeviceId(tempStringVector);
    
 
    for (vector<string>::iterator datasetIter = tempStringVector.begin();
         datasetIter != tempStringVector.end();
         datasetIter++) {
    
        string ndName = *datasetIter;
        domainAttributeList.clear();
        CUSchemaDB::getAttributeForDirection(ndName, DataType::Input, domainAttributeList);

        
        //add param to second action
        for (vector< shared_ptr<CDataWrapper> >::iterator datasetAttributeIter = domainAttributeList.begin();
             datasetAttributeIter != domainAttributeList.end();
             datasetAttributeIter++) {
            
            string attributeName = ndName;
            shared_ptr<CDataWrapper> attrDesc = *datasetAttributeIter;
            attributeName.append("_");attributeName.append(attrDesc->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME));
            LAPP_ << "Define the setDatasetAttribute parameter '" << attributeName << "' for the CU:"<< CU_IDENTIFIER_C_STREAM;
            actionDescription->addParam(attributeName.c_str(), 
                                        (DataType::DataType)attrDesc->getInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE),
                                        attrDesc->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DESCRIPTION).c_str());
        }
    }

    
    LAPP_ << "Get Action Description for Control Unit:" << CU_IDENTIFIER_C_STREAM;
        //grab dataset description
    CUSchemaDB::fillDataWrpperWithDataSetDescirption(setupConfiguration);
    
    
        //grab action description
    LAPP_ << "Get Dataset Description for Control Unit:" << CU_IDENTIFIER_C_STREAM;
    DeclareAction::getActionDescrionsInDataWrapper(setupConfiguration);

    auto_ptr<SerializationBuffer> ser(setupConfiguration.getBSONData());
    //copy configuration for internal use
    _internalSetupConfiguration.reset(new CDataWrapper(ser->getBufferPtr()));
}

#pragma mark protected initi/deinit method
/*
 Initialize the Custom Contro Unit and return the configuration
 */
void AbstractControlUnit::_init(CDataWrapper *newConfiguration) throw(CException) {
    LAPP_ << "Deinitializating AbstractControlUnit";
    
        //load all keyDataStorageMap for the registered devices
    KeyDataStorage *tmpKDS = 0L;
    vector<string> deviceIdVector;
    
    CUSchemaDB::getAllDeviceId(deviceIdVector);
    vector<string>::iterator devIDIter =  deviceIdVector.begin();
    for (  ; devIDIter != deviceIdVector.end(); devIDIter++ ) {
        tmpKDS = DataManager::getInstance()->getKeyDataStorageNewInstanceForKey(*devIDIter);
        tmpKDS->init(newConfiguration);
        keyDataStorageMap.insert(make_pair(*devIDIter, tmpKDS));
    }
        //initializing the control unit
    init(newConfiguration);
}

/*
 deinit all datastorage
 */
void AbstractControlUnit::_deinit() throw(CException) {
    LAPP_ << "Deinitializating AbstractControlUnit";
    
        //deinit the control unit 
    deinit();
    
    map<string, KeyDataStorage*>::iterator iter = keyDataStorageMap.begin();
    
    KeyDataStorage *tmpKDS = 0L;
    
    for (  ; iter != keyDataStorageMap.end(); iter++ ) {
        LAPP_ << "Deinitializating KeyDataStorage for key:" << (*iter).first;
        tmpKDS = (*iter).second;
        tmpKDS->deinit();
        delete(tmpKDS);
        tmpKDS = 0L;
    }
    //remove all
    keyDataStorageMap.clear();
}

/*
 Receive the evento for set the dataset input element
 */
CDataWrapper* AbstractControlUnit::_setDatasetAttribute(CDataWrapper *datasetAttributeValues) throw (CException) {
    return setDatasetAttribute(datasetAttributeValues);
}

/*
 Update the configuration for all descendand tree in the Control Uniti class struccture
 */
CDataWrapper*  AbstractControlUnit::updateConfiguration(CDataWrapper* newConfiguration) throw (CException) {
    map<string, KeyDataStorage*>::iterator iter = keyDataStorageMap.begin();
    KeyDataStorage *tmpKDS = 0L;
    //update the Contorl Unit's KeyDataStorage
    for (  ; iter != keyDataStorageMap.end(); iter++ ) {
        LAPP_ << "Update configuraiton for KeyDataStorage for key:" << (*iter).first;
        tmpKDS = (*iter).second;
        tmpKDS->updateConfiguration(newConfiguration);
    }
    return NULL;
}

#pragma mark protected API


/*
 Send device data to output buffer
 */
void AbstractControlUnit::pushDataSetForKey(const char *key, CDataWrapper *acquiredData) {
        //send data to related buffer
    keyDataStorageMap[key]->pushDataSet(acquiredData);
}

/*
 get last dataset for a specified key
 */
ArrayPointer<CDataWrapper> *AbstractControlUnit::getLastDataSetForKey(const char *key) {
        //use keydatastorage from map
    return keyDataStorageMap[key]->getLastDataSet();
}

/*
 Return a new instance of CDataWrapper filled with a mandatory data
 according to key
 */
CDataWrapper *AbstractControlUnit::getNewDataWrapperForKey(const char *key) {
    return keyDataStorageMap[key]->getNewDataWrapper();
}
