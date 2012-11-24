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


#include "DSAttributeHandlerExecutionEngine.h"

#define LDSAHEE_ LAPP_ << "[DSAttributeHandlerExecutionEngine] - "
using namespace chaos;
using namespace chaos::cu;
/*!
 Default constructor with std string
 */
DSAttributeHandlerExecutionEngine::DSAttributeHandlerExecutionEngine(std::string& _deviceID,
                                                                     chaos::CUSchemaDB *_schemaDB): deviceID(_deviceID),
                                                                                                    referenceCUSchemeDB(_schemaDB) {
    
}

/*!
 Default constructor with c string
 */
DSAttributeHandlerExecutionEngine::DSAttributeHandlerExecutionEngine(const char * _deviceID,
                                                                     chaos::CUSchemaDB *_schemaDB): deviceID(std::string(_deviceID)),
                                                                                                    referenceCUSchemeDB(_schemaDB) {
    
}

/*!
 Default destructor
 */
DSAttributeHandlerExecutionEngine::~DSAttributeHandlerExecutionEngine() {
    
}

    //! Initialize instance
void DSAttributeHandlerExecutionEngine::init(CDataWrapper*) throw(chaos::CException) {
    LDSAHEE_ << "Init for device id:" << deviceID;
}

    //! Start the implementation
void DSAttributeHandlerExecutionEngine::start() throw(chaos::CException) {
    

}

    //! Deinit the implementation
void DSAttributeHandlerExecutionEngine::deinit() throw(chaos::CException) {
    if(isdState != utility::ISDInterface::ISD_INITIATED) throw CException(0, "Engine need to be initialized to be deinitilized", "DSAttributeHandlerExecutionEngine::deinit");
        //lock all engine
    boost::shared_lock< boost::shared_mutex >(engineManagmentLock);
    LDSAHEE_ << "Deinit for device id:" << deviceID;
   
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
        
        delete(tmpVec);
    }
}

/*!
 Default destructor
 */
void DSAttributeHandlerExecutionEngine::addHandlerForDSAttribute(handler::DSAttributeHandler* classHandler)  throw (CException) {
    if(!classHandler) throw CException(0, "The class handler need to be a valid pointer", "DSAttributeHandlerExecutionEngine::addHandlerForDSAttribute");

    if(isdState != utility::ISDInterface::ISD_DEINTIATED) throw CException(1, "Engine need to be in deinitialized state to accept new hander", "DSAttributeHandlerExecutionEngine::addHandlerForDSAttribute");
    
    std::string& attrName = classHandler->getAttributeName();
    
    LDSAHEE_ << "Add new handler for device id '"<< deviceID <<"' and attribute '" << attrName << "'";

        //add the new handler
    if(attrNameHandlerMap.count(attrName) == 0){
        std::vector< handler::DSAttributeHandler* >  *tmp = new std::vector< handler::DSAttributeHandler* >();
        attrNameHandlerMap.insert(make_pair(attrName, tmp));
        LDSAHEE_ << "Add new list fo handler for device id '"<< deviceID <<"' and attribute '" << attrName << "'";
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
    int32_t i32v = 0;
    int64_t i64v = 0;
    double  dv = 0.0F;
    std::string  sv;
    const char * binv = NULL;
    int binv_dim;
    void *abstractValuePtr;
    CDataWrapper *cdatv;
    CUSchemaDB::RangeValueInfo attributeInfo;
    if((result = message->hasKey(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID))) {
        std::string _messageDeiveID = message->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID);
        
            //compare the message device id and the local
        if(!_messageDeiveID.compare(deviceID)) {
            LDSAHEE_ << "the message is for this device->" << deviceID;
            for (std::set<std::string>::iterator iter = managedAttributesName.begin();
                 iter != managedAttributesName.end();
                 iter++) {
                    //execute attribute handler
                const char * cAttrName = iter->c_str();
                LDSAHEE_ << "get the type of the attribute " << cAttrName;

                if(message->hasKey(cAttrName)) {
                    LDSAHEE_ << "The attribute " << cAttrName << " has benn found in message";
                        //get attribute info
                    referenceCUSchemeDB->getDeviceAttributeRangeValueInfo(deviceID, *iter, attributeInfo);
                    
                        //call handler
                    switch (attributeInfo.valueType) {
                        case DataType::TYPE_INT32: {
                            i32v = message->getInt32Value(cAttrName);
                            LDSAHEE_ << "The attribute " << cAttrName << " is an int32 =" << i32v;
                            abstractValuePtr = &i32v;
                            break;
                        }
                        case DataType::TYPE_INT64: {
                            i64v = message->getInt64Value(cAttrName);
                            LDSAHEE_ << "The attribute " << cAttrName << " is an int64 =" << i64v;
                            abstractValuePtr = &i64v;
                            break;
                        }
                        case DataType::TYPE_STRING: {
                            sv = message->getStringValue(cAttrName);
                            LDSAHEE_ << "The attribute " << cAttrName << " is a string =" << sv;
                            abstractValuePtr = &sv;
                            break;
                        }
                        case DataType::TYPE_DOUBLE: {
                            dv = message->getDoubleValue(cAttrName);
                            LDSAHEE_ << "The attribute " << cAttrName << " is an double =" << dv;
                            abstractValuePtr = &dv;
                            break;
                        }
                        case DataType::TYPE_BYTEARRAY: {
                            binv = message->getBinaryValue(cAttrName, binv_dim);
                            LDSAHEE_ << "The attribute " << cAttrName << " is a binary";
                            abstractValuePtr = &binv;
                            break;
                        }
                        case DataType::TYPE_STRUCT: {
                            cdatv = message->getCSDataValue(cAttrName);
                            LDSAHEE_ << "The attribute " << cAttrName << " is a struct";
                            abstractValuePtr = &cdatv;
                            break;
                        }
                    }
                    
                        //call every handler for the current attribute

                    LDSAHEE_ << "Pre call all handler";


                    std::vector< handler::DSAttributeHandler* > *tmpVector = attrNameHandlerMap[*iter];
                    for (std::vector< handler::DSAttributeHandler* >::iterator iter = tmpVector->begin();
                         iter != tmpVector->end();
                         iter++) {
                        handler::DSAttributeHandler *handlerPtr = *iter;
                        handlerPtr->handle(abstractValuePtr);
                    }
                    LDSAHEE_ << "Post call all handler";
                } else {
                    LDSAHEE_ << "The attribute " << cAttrName << " is NOT FOUND in message";
                }
            }
        }
    }
    return result;
}