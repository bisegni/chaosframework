/*
 *	DSAttributeHandlerExecutionEngine.cpp
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

#include <chaos/common/global.h>
#include <chaos/cu_toolkit/ControlManager/DSAttributeHandlerExecutionEngine.h>

#define LDSAHEE_ LAPP_ << "[DSAttributeHandlerExecutionEngine] - "
using namespace chaos;
using namespace chaos::cu;
using namespace chaos::cu::control_manager;
using namespace chaos::common::data;

/*!
 Default constructor with std string
 */
DSAttributeHandlerExecutionEngine::DSAttributeHandlerExecutionEngine(){}
/*!
 Default constructor with std string
 */
DSAttributeHandlerExecutionEngine::DSAttributeHandlerExecutionEngine(chaos::common::data::DatasetDB *_schemaDB): referenceDeviceSchemaDB(_schemaDB) {}

/*!
 Default destructor
 */
DSAttributeHandlerExecutionEngine::~DSAttributeHandlerExecutionEngine() {
    deinit();
}

void DSAttributeHandlerExecutionEngine::setDeviceSchemaDB(chaos::common::data::DatasetDB *_referenceDeviceSchemeDB) {
    referenceDeviceSchemaDB = _referenceDeviceSchemeDB;
}

//! Initialize instance
void DSAttributeHandlerExecutionEngine::init(void *initData) throw(chaos::CException) {
}

//! Start the implementation
void DSAttributeHandlerExecutionEngine::start() throw(chaos::CException) {
}

//! Deinit the implementation
void DSAttributeHandlerExecutionEngine::deinit() throw(chaos::CException) {
    //lock all engine
    boost::shared_lock< boost::shared_mutex >(engineManagmentLock);
    
    //delete all handler
    for (std::map<std::string, std::vector< handler::DSAttributeHandler* > * >::iterator iter = attrNameHandlerMap.begin();
         iter != attrNameHandlerMap.end();
         iter++) {
        
        std::vector< handler::DSAttributeHandler* > *tmpVec = iter->second;
        
        for (std::vector< handler::DSAttributeHandler* >::iterator iter = tmpVec->begin();
             iter != tmpVec->end();
             iter++) {
            handler::DSAttributeHandler *handlerPtr = *iter;
            delete(handlerPtr);
        }
        tmpVec->clear();
        delete(tmpVec);
    }
    
    attrNameHandlerMap.clear();
}

/*!
 Default destructor
 */
void DSAttributeHandlerExecutionEngine::addHandlerForDSAttribute(handler::DSAttributeHandler* classHandler)  throw (CException) {
    if(!classHandler) throw CException(0, "The class handler need to be a valid pointer", "DSAttributeHandlerExecutionEngine::addHandlerForDSAttribute");
    
    if(serviceState != ::chaos::utility::service_state_machine::InizializableServiceType::IS_DEINTIATED) throw CException(1, "Engine need to be in deinitialized state to accept new hander", "DSAttributeHandlerExecutionEngine::addHandlerForDSAttribute");
    
    std::string& attrName = classHandler->getAttributeName();
    
    LDSAHEE_ << "Add new handler for attribute '" << attrName << "'";
    
    //add the new handler
    if(attrNameHandlerMap.count(attrName) == 0){
        std::vector< handler::DSAttributeHandler* >  *tmp = new std::vector< handler::DSAttributeHandler* >();
        attrNameHandlerMap.insert(make_pair(attrName, tmp));
        LDSAHEE_ << "Add new list fo handler for attribute '" << attrName << "'";
    }
    attrNameHandlerMap[attrName]->push_back(classHandler);
    
    // check the attribute list
    if(managedAttributesName.count(attrName)==0){
        //insert the new attribute
        managedAttributesName.insert(attrName);
        LDSAHEE_ << "Register new attribute '" << attrName << "' to be managed";
    }
}

/*!
 Default destructor
 */
bool DSAttributeHandlerExecutionEngine::executeHandler(CDataWrapper *message) throw (CException)  {
    boost::shared_lock< boost::shared_mutex >(engineManagmentLock);
    bool result = false;
    CHAOS_ASSERT(message)
    bool bv = false;
    int32_t i32v = 0;
    int64_t i64v = 0;
    double  dv = 0.0F;
    std::string  sv;
    const char * binv = NULL;
    int binv_dim;
    void *abstractValuePtr;
    CDataWrapper *cdatv;
    RangeValueInfo attributeInfo;
    
    //if(deviceID.compare(referenceDeviceSchemaDB->getDeviceID())) return false;
    
    if((result = message->hasKey(DatasetDefinitionkey::DEVICE_ID))) {
        std::string _messageDeviceID = message->getStringValue(DatasetDefinitionkey::DEVICE_ID);
        
        //compare the message device id and the local
        for (std::set<std::string>::iterator iter = managedAttributesName.begin();
             iter != managedAttributesName.end();
             iter++) {
            //execute attribute handler
            const char * cAttrName = iter->c_str();
            if(message->hasKey(cAttrName)) {
                //get attribute info
                referenceDeviceSchemaDB->getAttributeRangeValueInfo(*iter, attributeInfo);
                //call handler
                switch (attributeInfo.valueType) {
                    case DataType::TYPE_BOOLEAN: {
                        bv = message->getBoolValue(cAttrName);
                        abstractValuePtr = &bv;
                        break;
                    }
                    case DataType::TYPE_INT32: {
                        i32v = message->getInt32Value(cAttrName);
                        abstractValuePtr = &i32v;
                        break;
                    }
                    case DataType::TYPE_INT64: {
                        i64v = message->getInt64Value(cAttrName);
                        abstractValuePtr = &i64v;
                        break;
                    }
                    case DataType::TYPE_STRING: {
                        sv = message->getStringValue(cAttrName);
                        abstractValuePtr = &sv;
                        break;
                    }
                    case DataType::TYPE_DOUBLE: {
                        dv = message->getDoubleValue(cAttrName);
                        abstractValuePtr = &dv;
                        break;
                    }
                    case DataType::TYPE_BYTEARRAY: {
                        binv = message->getBinaryValue(cAttrName, binv_dim);
                        abstractValuePtr = &binv;
                        break;
                    }
                }
                
                std::vector< handler::DSAttributeHandler* > *tmpVector = attrNameHandlerMap[*iter];
                for (std::vector< handler::DSAttributeHandler* >::iterator iter = tmpVector->begin();
                     iter != tmpVector->end();
                     iter++) {
                    handler::DSAttributeHandler *handlerPtr = *iter;
                    handlerPtr->handle(abstractValuePtr);
                }
            }
        }
    }
    return result;
}