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
#include <chaos_metadata_service_client/ChaosMetadataServiceClient.h>

#include <chaos_metadata_service_client/node_controller/CUController.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/data/CDataVariant.h>

#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/io/IODirectIODriver.h>
#include <boost/lexical_cast.hpp>
#include <chaos_metadata_service_client/api_proxy/unit_server/NewUS.h>
#include <chaos_metadata_service_client/api_proxy/unit_server/DeleteUS.h>
#include <chaos_metadata_service_client/api_proxy/unit_server/GetSetFullUnitServer.h>

#include <chaos_metadata_service_client/api_proxy/unit_server/ManageCUType.h>
#include <chaos_metadata_service_client/api_proxy/service/SetSnapshotDatasetsForNode.h>

#include <chaos_metadata_service_client/api_proxy/control_unit/SetInstanceDescription.h>
#include <chaos_metadata_service_client/api_proxy/control_unit/Delete.h>
#include <chaos_metadata_service_client/api_proxy/control_unit/DeleteInstance.h>
#include <chaos_metadata_service_client/api_proxy/agent/agent.h>
#include <chaos_metadata_service_client/api_proxy/logging/logging.h>

using namespace std;
using namespace chaos;
using namespace chaos::metadata_service_client::node_controller;
using namespace chaos::common::io;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::network;
using namespace chaos::common::message;
using namespace chaos::common::batch_command;
using namespace chaos::cu::data_manager;
using namespace chaos::metadata_service_client;
#define MDS_TIMEOUT 5000
#define DBGET LDBG_<<"["<<__PRETTY_FUNCTION__<<"]"
#define EXECUTE_CHAOS_API(api_name,time_out,...) \
DBGET<<" " <<" Executing Api:\""<< # api_name<<"\"" ;\
chaos::metadata_service_client::api_proxy::ApiProxyResult apires=  GET_CHAOS_API_PTR(api_name)->execute( __VA_ARGS__ );\
apires->setTimeout(time_out);\
apires->wait();\
if(apires->getError()){\
std::stringstream ss;\
ss<<" error in :"<<__FUNCTION__<<"|"<<__LINE__<<"|"<< # api_name <<" " <<apires->getErrorMessage();\
throw CException(-2,ss.str(),__PRETTY_FUNCTION__);}

CUController::CUController(const std::string& _deviceID,
                           chaos::common::io::IODataDriverShrdPtr _ioLiveDataDriver):
datasetDB(true),
mdsChannel(NULL),
deviceChannel(NULL),devId(_deviceID) {
    millisecToWait = RpcConfigurationKey::GlobalRPCTimeoutinMSec;
    
    ioLiveDataDriver=_ioLiveDataDriver;
    mdsChannel = NetworkBroker::getInstance()->getMetadataserverMessageChannel();
    if(!mdsChannel) throw CException(-1,
                                     "No MDS Channel created",
                                     __PRETTY_FUNCTION__);
    
    //get a new message channel in a self manage way
    deviceChannel = NetworkBroker::getInstance()->getDeviceMessageChannelFromAddress(new CDeviceNetworkAddress(_deviceID),
                                                                                     true,
                                                                                     true);
    if(!deviceChannel) throw CException(-2,
                                        "Invalid device channel created",
                                        __PRETTY_FUNCTION__);
    //add me as listener
    deviceChannel->addListener(this);
    //update live data driver
    
    if(!ioLiveDataDriver.get()) throw CException(-3,
                                                 "Invalid data io driver found",
                                                 __PRETTY_FUNCTION__);
    CDWUniquePtr tmp_data_handler;
    if(!mdsChannel->getDataDriverBestConfiguration(tmp_data_handler, millisecToWait)){
        ioLiveDataDriver->updateConfiguration(tmp_data_handler.get());
    }
    
    channel_keys.resize(DPCK_LAST_DATASET_INDEX + 1);
    channel_keys[DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT]=(_deviceID + DataPackPrefixID::OUTPUT_DATASET_POSTFIX);
    channel_keys[DataPackCommonKey::DPCK_DATASET_TYPE_INPUT]=(_deviceID + DataPackPrefixID::INPUT_DATASET_POSTFIX);
    channel_keys[DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM]=(_deviceID + DataPackPrefixID::CUSTOM_DATASET_POSTFIX);
    channel_keys[DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM]=(_deviceID + DataPackPrefixID::SYSTEM_DATASET_POSTFIX);
    channel_keys[DataPackCommonKey::DPCK_DATASET_TYPE_DEV_ALARM]=(_deviceID + DataPackPrefixID::DEV_ALARM_DATASET_POSTFIX);
    channel_keys[DataPackCommonKey::DPCK_DATASET_TYPE_CU_ALARM]=(_deviceID + DataPackPrefixID::CU_ALARM_DATASET_POSTFIX);
    channel_keys[DataPackCommonKey::DPCK_DATASET_TYPE_HEALTH]=(_deviceID + DataPackPrefixID::HEALTH_DATASET_POSTFIX);
    //  current_dataset.push_back(d);
    for(int cnt=0;cnt<channel_keys.size();cnt++){
        ChaosSharedPtr<chaos::common::data::CDataWrapper> ch;
        ch.reset(new chaos::common::data::CDataWrapper());
        current_dataset.push_back(ch);
    }
    updateChannel();
}

CUController::~CUController() {
    boost::mutex::scoped_lock lock(trackMutext);

    LDBG_<<"["<<__PRETTY_FUNCTION__<<"] remove Device Controller:"<<devId;
    stopTracking();
    
    if(deviceChannel){
        deviceChannel->removeListener(this);
        NetworkBroker::getInstance()->disposeMessageChannel(deviceChannel);
    }
    
    if(mdsChannel){
        NetworkBroker::getInstance()->disposeMessageChannel(mdsChannel);
    }
    
    if(ioLiveDataDriver.get()){
        ioLiveDataDriver->deinit();
        ioLiveDataDriver.reset();
    }
}


void CUController::setRequestTimeWaith(uint32_t newMillisecToWait){
    millisecToWait = newMillisecToWait;
}

uint32_t CUController::getRequestTimeWaith(){
    return millisecToWait;
}

void CUController::getDeviceId(string& dId) {
    dId = devId;
}

void CUController::deviceAvailabilityChanged(const std::string& device_id,
                                             const OnlineState availability) {
    if(availability == OnlineStateOnline) {
        updateChannel();
    }
}

void CUController::updateChannel() throw(CException) {
    int err = ErrorCode::EC_NO_ERROR;
    CDWUniquePtr tmp_data_handler;
    CHAOS_ASSERT(deviceChannel)
    LDBG_<<"UPDATING \""<<devId<<"\"";
    err = mdsChannel->getLastDatasetForDevice(devId, tmp_data_handler, millisecToWait);
    if(err!=ErrorCode::EC_NO_ERROR || !tmp_data_handler.get()) return;
    datasetDB.addAttributeToDataSetFromDataWrapper(*tmp_data_handler);
}

int CUController::setScheduleDelay(uint64_t microseconds) {
    
    chaos::common::property::PropertyGroup pg(chaos::ControlUnitPropertyKey::GROUP_NAME);
    pg.addProperty(chaos::ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY, CDataVariant(static_cast<uint64_t>(microseconds)));
    DBGET<<chaos::ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY<<":"<<microseconds;
    EXECUTE_CHAOS_API(chaos::metadata_service_client::api_proxy::node::UpdateProperty,millisecToWait,devId,pg);
    return 0;
}

int CUController::setBypass(bool onoff){
    chaos::common::property::PropertyGroup pg(chaos::ControlUnitPropertyKey::GROUP_NAME);
    pg.addProperty(chaos::ControlUnitDatapackSystemKey::BYPASS_STATE, CDataVariant(static_cast<bool>(onoff)));
    
    EXECUTE_CHAOS_API(chaos::metadata_service_client::api_proxy::node::UpdateProperty,millisecToWait,devId,pg);
    return 0;
}

void CUController::getDeviceDatasetAttributesName(vector<string>& attributesName) {
    datasetDB.getDatasetAttributesName(attributesName);
}

void CUController::getAttributeDescription(const string& attributesName, string& attributeDescription) {
    datasetDB.getAttributeDescription(attributesName, attributeDescription);
}

void CUController::getDeviceDatasetAttributesName(vector<string>& attributesName, DataType::DataSetAttributeIOAttribute directionType) {
    datasetDB.getDatasetAttributesName(directionType, attributesName);
}

void CUController::getDeviceAttributeRangeValueInfo(const string& attributesName, chaos::common::data::RangeValueInfo& rangeInfo) {
    datasetDB.getAttributeRangeValueInfo(attributesName, rangeInfo);
}
int CUController::getDeviceAttributeDirection(const string& attributesName, DataType::DataSetAttributeIOAttribute& directionType) {
    return datasetDB.getAttributeDirection(attributesName, directionType);
}

int CUController::getAttributeStrValue(string attribute_name, string& attribute_value) {
    int err = 0;
    CDataWrapper *  dataWrapper = getLiveCDataWrapperPtr();
    if(dataWrapper) {
        DataType::DataType attributeType;
        err = getDeviceAttributeType(attribute_name, attributeType);
        if(err == 0){
            switch (attributeType) {
                case DataType::TYPE_INT64:
                    attribute_value = boost::lexical_cast<string>(dataWrapper->getInt64Value(attribute_name.c_str()));
                    break;
                    
                case DataType::TYPE_INT32:
                    attribute_value = boost::lexical_cast<string>(dataWrapper->getInt64Value(attribute_name.c_str()));
                    break;
                    
                case DataType::TYPE_DOUBLE:
                    attribute_value = boost::lexical_cast<string>(dataWrapper->getDoubleValue(attribute_name.c_str()));
                    break;
                    
                case DataType::TYPE_CLUSTER:
                case DataType::TYPE_STRING:
                    attribute_value = boost::lexical_cast<string>(dataWrapper->getStringValue(attribute_name.c_str()));
                    break;
                    
                case DataType::TYPE_BOOLEAN:
                    attribute_value = boost::lexical_cast<string>(dataWrapper->getBoolValue(attribute_name.c_str()));
                    break;
                    
                case DataType::TYPE_BYTEARRAY: {
                    const char *buffer = NULL;
                    uint32_t size;
                    buffer = dataWrapper->getBinaryValue(attribute_name.c_str(), size);
                    if(buffer) attribute_value.assign(buffer, size);
                    break;
                }
                    
                default:
                    err = -1;
                    break;
            }
        }
        
    }
    return err;
}
int CUController::getDeviceAttributeType(const string& attr, DataType::DataType& type) {
    int err = 0;
    if(attributeValueMap.count(attr)){
        type = attributeValueMap[attr].valueType;
    } else {
        err = -1;
    }
    return err;
}

int CUController::getType(std::string& control_unit_type) {
    int err = 0;
    if(cu_type.empty()) {
        CDataWrapper*tmp=fetchCurrentDatatasetFromDomain(KeyDataStorageDomainSystem).get();
        if(tmp && tmp->hasKey(DataPackSystemKey::DP_SYS_UNIT_TYPE)){
            std::string t=tmp->getCStringValue(DataPackSystemKey::DP_SYS_UNIT_TYPE);
            cu_type = t;
        } else {
            return -1;
        }
        
    }
    control_unit_type = cu_type;
    
    return err;
}

int CUController::initDevice() {
    CHAOS_ASSERT(mdsChannel && deviceChannel)
    int err = 0;
    CDWUniquePtr init_conf(new CDataWrapper());
    datasetDB.fillDataWrapperWithDataSetDescription(*init_conf);
    
    //initialize the devica with the metadataserver data
    err = deviceChannel->initDevice(MOVE(init_conf), millisecToWait);
    //configure the live data with the same server where the device write
    return err;
}

int CUController::startDevice() {
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->startDevice(millisecToWait);
}

int CUController::stopDevice() {
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->stopDevice(millisecToWait);
}

int CUController::deinitDevice() {
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->deinitDevice(millisecToWait);
}

int CUController::restoreDeviceToTag(const std::string& restore_tag) {
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->restoreDeviceToTag(restore_tag,
                                             millisecToWait);
}
int CUController::recoverDeviceFromError() {
    CHAOS_ASSERT(deviceChannel)
    return deviceChannel->recoverDeviceFromError(millisecToWait);
}
uint64_t CUController::getState(CUStateKey::ControlUnitState& deviceState) {
    uint64_t ret=0;
    CDataWrapper*tmp=fetchCurrentDatatasetFromDomain(KeyDataStorageDomainHealth).get();
    deviceState=CUStateKey::UNDEFINED;
    if(tmp && tmp->hasKey(NodeHealtDefinitionKey::NODE_HEALT_STATUS)){
        std::string state=tmp->getCStringValue(NodeHealtDefinitionKey::NODE_HEALT_STATUS);
        if((state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_START) || (state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_STARTING))
            deviceState=CUStateKey::START;
        else if((state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_STOP) || (state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_STOPING))
            deviceState= CUStateKey::STOP;
        else if((state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_INIT) || (state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_INITING))
            deviceState= CUStateKey::INIT;
        else if((state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_DEINIT) || (state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_DEINITING)|| (state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_LOAD))
            deviceState= CUStateKey::DEINIT;
        else if((state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_RERROR))
            deviceState= CUStateKey::RECOVERABLE_ERROR;
        else if((state== NodeHealtDefinitionValue::NODE_HEALT_STATUS_FERROR))
            deviceState= CUStateKey::FATAL_ERROR;
        
        if(tmp->hasKey(NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP)){
            ret = tmp->getInt64Value(NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP);
        }
    }
    return ret;
}

int CUController::getChannelsNum(){
    return (int)channel_keys.size();
}
int CUController::setAttributeValue(string& attributeName, int32_t attributeValue) {
    return setAttributeValue(attributeName.c_str(), attributeValue);
}

int CUController::setAttributeValue(const char *attributeName, int32_t attributeValue) {
    CDWUniquePtr attr_value_pack(new CDataWrapper());
    attr_value_pack->addInt32Value(attributeName, attributeValue);
    return deviceChannel->setAttributeValue(MOVE(attr_value_pack), millisecToWait);
}

int CUController::setAttributeValue(string& attributeName, double attributeValue) {
    return setAttributeValue(attributeName.c_str(), attributeValue);
}

int CUController::setAttributeValue(const char *attributeName, double attributeValue) {
    CDWUniquePtr attr_value_pack(new CDataWrapper());
    attr_value_pack->addDoubleValue(attributeName, attributeValue);
    return deviceChannel->setAttributeValue(MOVE(attr_value_pack), millisecToWait);
}

int CUController::setAttributeToValue(const char *attributeName, const char *attributeValue, bool noWait) {
    int err = ErrorCode::EC_NO_ERROR;
    CDWUniquePtr attributeValuePack(new CDataWrapper());
    RangeValueInfo range_info;
    
    //get type for the value
    if((err = datasetDB.getAttributeRangeValueInfo(attributeName, range_info)))
        return err;
    
    switch (range_info.valueType) {
        case DataType::TYPE_BOOLEAN: {
            bool boolValuePtr = lexical_cast<bool>(attributeValue);
            attributeValuePack->addBoolValue(attributeName, boolValuePtr);
            break;
        }
        case DataType::TYPE_CLUSTER:
            attributeValuePack->addJsonValue(attributeName, attributeValue);
            break;
            
            
        case DataType::TYPE_STRING:{
            attributeValuePack->addStringValue(attributeName, attributeValue);
            break;
        }
        case DataType::TYPE_DOUBLE:{
            double doubleValuePtr = lexical_cast<double>(attributeValue);
            attributeValuePack->addDoubleValue(attributeName, doubleValuePtr);
            break;
        }
        case DataType::TYPE_INT32:{
            int32_t i32ValuePtr = lexical_cast<int32_t>(attributeValue);
            attributeValuePack->addInt32Value(attributeName, i32ValuePtr);
            break;
        }
        case DataType::TYPE_INT64:{
            int64_t i64ValuePtr = lexical_cast<int64_t>(attributeValue);
            attributeValuePack->addInt64Value(attributeName, i64ValuePtr);
            break;
        }
        case DataType::TYPE_BYTEARRAY:{
            //const char *byteArrayValuePtr = static_cast<const char*>(attributeValue);
            //attributeValuePack.addBinaryValue(attributeName, byteArrayValuePtr, bufferValuedDim);
            throw CException(1, "The byte array set is not managed", "CUController::setAttributeToValue");
            break;
        }
            
        default:break;
    }
    LDBG_<<"["<<__PRETTY_FUNCTION__<<"] Sending attribute '"<<attributeName<<"'='"<<attributeValuePack->getJSONString()<<"'";
    
    return deviceChannel->setAttributeValue(MOVE(attributeValuePack), noWait, millisecToWait);
}

int CUController::setAttributeToValue(const char *attributeName, void *attributeValue, bool noWait, int32_t bufferValuedDim) {
    RangeValueInfo range_info;
    
    //get type for the value
    datasetDB.getAttributeRangeValueInfo(attributeName, range_info);
    
    //call default API
    return setAttributeToValue(attributeName, range_info.valueType, attributeValue, noWait, bufferValuedDim);
}

int CUController::setAttributeToValue(const char *attributeName, DataType::DataType attributeType, void *attributeValue, bool noWait, int32_t bufferValuedDim) {
    CDWUniquePtr attributeValuePack(new CDataWrapper());
    switch (attributeType) {
        case DataType::TYPE_BOOLEAN: {
            bool *boolValuePtr = static_cast<bool *>(attributeValue);
            attributeValuePack->addBoolValue(attributeName, *boolValuePtr);
            break;
        }
        case DataType::TYPE_CLUSTER:{
            
            attributeValuePack->addJsonValue(attributeName,static_cast<const char *>(attributeValue));
            break;
        }
        case DataType::TYPE_STRING:{
            const char *strValuePtr = static_cast<const char *>(attributeValue);
            attributeValuePack->addStringValue(attributeName, strValuePtr);
            break;
        }
        case DataType::TYPE_DOUBLE:{
            double *doubleValuePtr = static_cast<double*>(attributeValue);
            attributeValuePack->addDoubleValue(attributeName, *doubleValuePtr);
            break;
        }
        case DataType::TYPE_INT32:{
            int32_t *i32ValuePtr = static_cast<int32_t*>(attributeValue);
            attributeValuePack->addInt32Value(attributeName, *i32ValuePtr);
            break;
        }
        case DataType::TYPE_INT64:{
            int64_t *i64ValuePtr = static_cast<int64_t*>(attributeValue);
            attributeValuePack->addInt64Value(attributeName, *i64ValuePtr);
            break;
        }
        case DataType::TYPE_BYTEARRAY:{
            const char *byteArrayValuePtr = static_cast<const char*>(attributeValue);
            attributeValuePack->addBinaryValue(attributeName, byteArrayValuePtr, bufferValuedDim);
            break;
        }
    }
    return deviceChannel->setAttributeValue(MOVE(attributeValuePack), noWait, millisecToWait);
}

int CUController::submitSlowControlCommand(string commandAlias,
                                           SubmissionRuleType::SubmissionRule submissionRule,
                                           uint32_t priority,
                                           uint64_t& command_id,
                                           uint32_t execution_channel,
                                           uint64_t scheduler_steps_delay,
                                           uint32_t submission_checker_steps_delay,
                                           CDataWrapper *slow_command_data) {
    CDWUniquePtr local_command_pack(new CDataWrapper());
    CDWUniquePtr result_data;
    int err = ErrorCode::EC_NO_ERROR;
    
    if(slow_command_data) {
        local_command_pack->appendAllElement(*slow_command_data);
    }
    
    // set the default slow command information
    local_command_pack->addStringValue(BatchCommandAndParameterDescriptionkey::BC_ALIAS, commandAlias);
    local_command_pack->addInt32Value(BatchCommandSubmissionKey::SUBMISSION_RULE_UI32, (uint32_t) submissionRule);
    local_command_pack->addInt32Value(BatchCommandSubmissionKey::SUBMISSION_PRIORITY_UI32, (uint32_t) priority);
    
    if(execution_channel) local_command_pack->addInt32Value(BatchCommandSubmissionKey::COMMAND_EXECUTION_CHANNEL, (uint32_t) execution_channel);
    if(scheduler_steps_delay) local_command_pack->addInt64Value(BatchCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL_UI64, scheduler_steps_delay);
    if(submission_checker_steps_delay) local_command_pack->addInt32Value(BatchCommandSubmissionKey::SUBMISSION_RETRY_DELAY_UI32, submission_checker_steps_delay);
    
    //err = deviceChannel->setAttributeValue(local_command_pack, false, millisecToWait);
    local_command_pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, devId);
    err = deviceChannel->sendCustomRequest(ControlUnitNodeDomainAndActionRPC::CONTROL_UNIT_APPLY_INPUT_DATASET_ATTRIBUTE_CHANGE_SET,
                                           MOVE(local_command_pack),
                                           result_data,
                                           millisecToWait);
    if(err == ErrorCode::EC_NO_ERROR &&
       result_data.get() &&
       result_data->hasKey(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64)) {
        //fill the command id
        command_id = result_data->getUInt64Value(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64);
    } else {
        if(result_data)
            LERR_<<"missing key:"<<BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64<<" :"<<result_data->getJSONString();
    }
    return err;
}

int CUController::submitSlowControlCommand(string commandAlias,
                                           SubmissionRuleType::SubmissionRule submissionRule,
                                           uint64_t& command_id,
                                           uint32_t execution_channel,
                                           uint64_t scheduler_steps_delay,
                                           uint32_t submission_checker_steps_delay,
                                           CDataWrapper *slow_command_data) {
    CDWUniquePtr local_command_pack(new CDataWrapper());
    CDWUniquePtr result_data;
    int err = ErrorCode::EC_NO_ERROR;
    
    if(slow_command_data) {
        local_command_pack->appendAllElement(*slow_command_data);
    }
    // set the default slow command information
    local_command_pack->addStringValue(BatchCommandAndParameterDescriptionkey::BC_ALIAS, commandAlias);
    local_command_pack->addInt32Value(BatchCommandSubmissionKey::SUBMISSION_RULE_UI32, (uint32_t) submissionRule);
    
    if(execution_channel) local_command_pack->addInt32Value(BatchCommandSubmissionKey::COMMAND_EXECUTION_CHANNEL, (uint32_t) execution_channel);
    if(scheduler_steps_delay) local_command_pack->addInt64Value(BatchCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL_UI64, scheduler_steps_delay);
    if(submission_checker_steps_delay) local_command_pack->addInt32Value(BatchCommandSubmissionKey::SUBMISSION_RETRY_DELAY_UI32, (uint32_t) submission_checker_steps_delay);
    
    //forward the request
    local_command_pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, devId);
    err = deviceChannel->sendCustomRequest(ControlUnitNodeDomainAndActionRPC::CONTROL_UNIT_APPLY_INPUT_DATASET_ATTRIBUTE_CHANGE_SET,
                                           MOVE(local_command_pack),
                                           result_data,
                                           millisecToWait);
    if(err == ErrorCode::EC_NO_ERROR
       && result_data.get()
       && result_data->hasKey(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64)) {
        //fill the command id
        command_id = result_data->getUInt64Value(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64);
    }
    return err;
}

int CUController::setSlowCommandFeatures(features::Features& features, bool lock_features, uint32_t execution_channel) {
    CDWUniquePtr local_command_pack(new CDataWrapper());
    CDWUniquePtr result;
    if(features.featuresFlag & features::FeaturesFlagTypes::FF_LOCK_USER_MOD) {
        local_command_pack->addBoolValue(BatchCommandExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_LOCK_BOOL, lock_features);
    }
    
    if(features.featuresFlag & features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY) {
        local_command_pack->addInt64Value(BatchCommandExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_SCHEDULER_STEP_WAITH_UI64, features.featureSchedulerStepsDelay);
    }
    
    if(execution_channel) local_command_pack->addInt32Value(BatchCommandSubmissionKey::COMMAND_EXECUTION_CHANNEL, (uint32_t) execution_channel);
    
    return deviceChannel->sendCustomRequest(BatchCommandExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES,
                                            MOVE(local_command_pack),
                                            result,
                                            millisecToWait);
}

int CUController::setSlowCommandLockOnFeatures(bool lock_features) {
    CDWUniquePtr local_command_pack(new CDataWrapper());
    CDWUniquePtr result;
    local_command_pack->addBoolValue(BatchCommandExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES_LOCK_BOOL, lock_features);
    return deviceChannel->sendCustomRequest(BatchCommandExecutorRpcActionKey::RPC_SET_COMMAND_FEATURES,
                                            MOVE(local_command_pack),
                                            result,
                                            millisecToWait);
}

int CUController::getCommandState(CommandState& command_state) {
    CDWUniquePtr local_command_pack(new CDataWrapper());
    CDWUniquePtr result_data;
    int err = ErrorCode::EC_NO_ERROR;
    local_command_pack->addInt64Value(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_CMD_ID_UI64, command_state.command_id);
    err = deviceChannel->sendCustomRequest(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE,
                                           MOVE(local_command_pack),
                                           result_data,
                                           millisecToWait);
    if(err == ErrorCode::EC_NO_ERROR &&
       result_data.get()) {
        //fill the command state
        command_state.last_event = static_cast<BatchCommandEventType::BatchCommandEventType>(result_data->getUInt32Value(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_LAST_EVENT_UI32));
        if(command_state.last_event == BatchCommandEventType::EVT_FAULT) {
            command_state.fault_description.code = result_data->getUInt32Value(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_ERROR_CODE_UI32);
            command_state.fault_description.description = result_data->getStringValue(BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_ERROR_DESC_STR);
            command_state.fault_description.domain = BatchCommandExecutorRpcActionKey::RPC_GET_COMMAND_STATE_ERROR_DOMAIN_STR;
        } else {
            command_state.fault_description.code = 0;
            command_state.fault_description.description.clear();
            command_state.fault_description.domain.clear();
        }
    }
    return err;
}

int CUController::killCurrentCommand() {
    CDWUniquePtr result;
    return deviceChannel->sendCustomRequest(BatchCommandExecutorRpcActionKey::RPC_KILL_CURRENT_COMMAND, CDWUniquePtr(), result, millisecToWait);
}

int CUController::flushCommandStateHistory() {
    return 0;/*deviceChannel->sendCustomRequest(BatchCommandExecutorRpcActionKey::RPC_FLUSH_COMMAND_HISTORY, NULL, NULL, millisecToWait);*/
}

int CUController::sendCustomRequest(const std::string& action,
                                    CDWUniquePtr param,
                                    CDWUniquePtr& result) {
    return deviceChannel->sendCustomRequest(action,
                                            MOVE(param),
                                            result,
                                            millisecToWait);
}

void CUController::sendCustomMessage(const std::string& action,
                                     CDWUniquePtr param) {
    deviceChannel->sendCustomMessage(action,
                                     MOVE(param));
}

int CUController::checkRPCInformation(CDWUniquePtr& result_information,
                                      uint32_t timeout) {
    int err = -1;
    ChaosUniquePtr<MessageRequestFuture> result = deviceChannel->checkRPCInformation();
    if(result.get() == NULL) return -1;
    if(result->wait(timeout)) {
        err = result->getError();
        if(err == 0) {
            result_information = result->detachResult();
        }
    }else{
        err = -2;
    }
    return err;
}

int CUController::echoTest(CDWUniquePtr echo_data,
                           CDWUniquePtr& echo_data_result,
                           uint32_t timeout) {
    int err = -1;
    ChaosUniquePtr<MessageRequestFuture> result = deviceChannel->echoTest(MOVE(echo_data));
    if(result.get() == NULL) return err;
    if(result->wait(timeout)) {
        err = result->getError();
        if(err == 0) {
            echo_data_result = result->detachResult();
        }
    }else{
        err = -2;
    }
    return err;
}

ChaosUniquePtr<MessageRequestFuture> CUController::sendCustomRequestWithFuture(const std::string& action_name,
                                                                               common::data::CDWUniquePtr request_date) {
    return deviceChannel->sendCustomRequestWithFuture(action_name,
                                                      MOVE(request_date));
}

int CUController::setAttributeValue(string& attributeName, string& attributeValue) {
    return setAttributeValue(attributeName, attributeValue.c_str(),(uint32_t)attributeValue.size());
}

int CUController::setAttributeValue(string& attributeName, const char* attributeValue) {
    return setAttributeValue(attributeName, attributeValue,(uint32_t)strlen(attributeValue));
}

std::vector<chaos::common::data::RangeValueInfo> CUController::getDeviceValuesInfo(){
    std::vector<chaos::common::data::RangeValueInfo> ret;
    for(std::map<std::string,chaos::common::data::RangeValueInfo>::iterator i= attributeValueMap.begin();i!=attributeValueMap.end();i++)
        ret.push_back(i->second);
    
    return ret;
}
int CUController::setAttributeValue(string& attributeName, const char* attributeValue, uint32_t size) {
    CDWUniquePtr attributeValuePack(new CDataWrapper());
    const char *attrname=attributeName.c_str();
    
    if(attributeValueMap.find(attributeName) == attributeValueMap.end() )
        return ErrorCode::EC_ATTRIBUTE_NOT_FOUND;
    
    if(attributeValueMap[attributeName].dir==DataType::Output)
        return ErrorCode::EC_ATTRIBUTE_BAD_DIR;
    
    switch (attributeValueMap[attributeName].valueType) {
            
        case DataType::TYPE_INT64:
            attributeValuePack->addInt64Value(attrname, boost::lexical_cast<int64_t>(attributeValue));
            return deviceChannel->setAttributeValue(MOVE(attributeValuePack),millisecToWait);
            
        case DataType::TYPE_INT32:
            attributeValuePack->addInt32Value(attrname, boost::lexical_cast<int32_t>(attributeValue));
            return deviceChannel->setAttributeValue(MOVE(attributeValuePack),millisecToWait);
            
        case DataType::TYPE_DOUBLE:
            attributeValuePack->addDoubleValue(attrname, boost::lexical_cast<double>(attributeValue));
            return deviceChannel->setAttributeValue(MOVE(attributeValuePack),millisecToWait);
            
        case DataType::TYPE_BYTEARRAY:
            attributeValuePack->addBinaryValue(attrname,attributeValue,size);
            return deviceChannel->setAttributeValue(MOVE(attributeValuePack),millisecToWait);
        case DataType::TYPE_CLUSTER:{
            attributeValuePack->addJsonValue(attrname,attributeValue);
            return deviceChannel->setAttributeValue(MOVE(attributeValuePack),millisecToWait);
        }
        case DataType::TYPE_STRING:
            attributeValuePack->addStringValue(attrname,attributeValue);
            return deviceChannel->setAttributeValue(MOVE(attributeValuePack),millisecToWait);
        default:
            break;
    };
    return ErrorCode::EC_ATTRIBUTE_TYPE_NOT_SUPPORTED;
}

void CUController::initializeAttributeIndexMap() {
    //	boost::mutex::scoped_lock lock(trackMutext);
    vector<string> attributeNames;
    RangeValueInfo attributerangeInfo;
    
    attributeValueMap.clear();
    
    
    //get all attribute name from db
    datasetDB.getDatasetAttributesName(DataType::Input, attributeNames);
    for (vector<string>::iterator iter = attributeNames.begin();
         iter != attributeNames.end();
         iter++) {
        
        if(datasetDB.getAttributeRangeValueInfo(*iter, attributerangeInfo)!=0){
            LERR_<<"CANNOT RETRIVE attr range info of:"<<*iter;
        }
        //     LDBG_<<"IN attr:"<<attributerangeInfo.name<<" type:"<<attributerangeInfo.valueType<<" bin type:"<<attributerangeInfo.binType;
        attributeValueMap.insert(make_pair(*iter, attributerangeInfo));
        
    }
    
    attributeNames.clear();
    datasetDB.getDatasetAttributesName(DataType::Output, attributeNames);
    for (vector<string>::iterator iter = attributeNames.begin();
         iter != attributeNames.end();
         iter++) {
        
        
        if(datasetDB.getAttributeRangeValueInfo(*iter, attributerangeInfo)!=0){
            LERR_<<"CANNOT RETRIVE attr range info of:"<<*iter;
            
        }
        //    LDBG_<<"OUT attr:"<<attributerangeInfo.name<<" type:"<<attributerangeInfo.valueType<<" bin type:"<<attributerangeInfo.binType;
        attributeValueMap.insert(make_pair(*iter, attributerangeInfo));
        
    }
    
}

void CUController::allocateNewLiveBufferForAttributeAndType(string& attributeName,
                                                            DataType::DataSetAttributeIOAttribute attributeDirection,
                                                            DataType::DataType attrbiuteType) {
    //boost::mutex::scoped_lock  lock(trackMutext);
    if(attributeDirection == DataType::Output ||
       attributeDirection == DataType::Bidirectional ){
        
        switch (attributeValueMap[attributeName].valueType) {
                
                
            case DataType::TYPE_INT32:{
                SingleBufferCircularBuffer<int32_t> *newBuffer = new SingleBufferCircularBuffer<int32_t>(30);
                int32AttributeLiveBuffer.insert(make_pair(attributeName, newBuffer));
            }
                break;
                
            case DataType::TYPE_INT64:{
                SingleBufferCircularBuffer<int64_t> *newBuffer = new SingleBufferCircularBuffer<int64_t>(30);
                int64AttributeLiveBuffer.insert(make_pair(attributeName, newBuffer));
            }
                break;
                
            case DataType::TYPE_DOUBLE:{
                SingleBufferCircularBuffer<double_t> *newBuffer = new SingleBufferCircularBuffer<double_t>(30);
                doubleAttributeLiveBuffer.insert(make_pair(attributeName, newBuffer));
            }
                break;
            case DataType::TYPE_BYTEARRAY:{
                PointerBuffer *newBuffer = new PointerBuffer();
                pointerAttributeLiveBuffer.insert(make_pair(attributeName, newBuffer));
            }
                break;
        }
    } else if(attributeDirection == DataType::Input ||
              attributeDirection == DataType::Bidirectional ){
        
    }
    
}

UIDataBuffer *CUController::getBufferForAttribute(string& attributeName) {
    boost::mutex::scoped_lock lock(trackMutext);
    UIDataBuffer * result = NULL;
    //allocate attribute traccking
    
    if(attributeValueMap.count(attributeName) == 0  ) return result;
    
    switch (attributeValueMap[attributeName].valueType) {
            
        case DataType::TYPE_INT32:
            result = int32AttributeLiveBuffer[attributeName];
            break;
            
        case DataType::TYPE_INT64:
            result = int64AttributeLiveBuffer[attributeName];
            break;
            
        case DataType::TYPE_DOUBLE:
            result = doubleAttributeLiveBuffer[attributeName];
            break;
    }
    return result;
}

PointerBuffer *CUController::getPtrBufferForAttribute(string& attributeName) {
    boost::mutex::scoped_lock lock(trackMutext);
    PointerBuffer * result = NULL;
    //allocate attribute traccking
    if(attributeValueMap.count(attributeName) == 0  ) return result;
    
    switch (attributeValueMap[attributeName].valueType) {
            
        case DataType::TYPE_BYTEARRAY:
            result = pointerAttributeLiveBuffer[attributeName];
            break;
        default:
            break;
    }
    return result;
}

UIDataBuffer *CUController::getPtrBufferForTimestamp(const int initialDimension) {
    return int64AttributeLiveBuffer.count(DataPackCommonKey::DPCK_TIMESTAMP)>0? int64AttributeLiveBuffer[DataPackCommonKey::DPCK_TIMESTAMP]:NULL;
}

void CUController::deinitializeAttributeIndexMap() {
    //	boost::mutex::scoped_lock lock(trackMutext);
    //dispose circula buffer
    for (std::map<string,  SingleBufferCircularBuffer<int32_t> *>::iterator iter = int32AttributeLiveBuffer.begin();
         iter != int32AttributeLiveBuffer.end();
         iter++) {
        delete(iter->second);
    }
    int32AttributeLiveBuffer.clear();
    
    for (std::map<string,  SingleBufferCircularBuffer<int64_t> *>::iterator iter = int64AttributeLiveBuffer.begin();
         iter != int64AttributeLiveBuffer.end();
         iter++) {
        delete(iter->second);
    }
    int64AttributeLiveBuffer.clear();
    
    for (std::map<string,  SingleBufferCircularBuffer<double_t> *>::iterator iter = doubleAttributeLiveBuffer.begin();
         iter != doubleAttributeLiveBuffer.end();
         iter++) {
        delete(iter->second);
    }
    doubleAttributeLiveBuffer.clear();
    
    for (std::map<string,  PointerBuffer*>::iterator iter = pointerAttributeLiveBuffer.begin();
         iter != pointerAttributeLiveBuffer.end();
         iter++) {
        delete(iter->second);
    }
    pointerAttributeLiveBuffer.clear();
}

void CUController::addAttributeToTrack(string& attr) {
    boost::mutex::scoped_lock lock(trackMutext);
    
    //add attribute name to list of tracking attribute
    trackingAttribute.push_back(attr);
    
    //allocate attribute traccking
    if(attributeValueMap.count(attr) == 0  ) return;
    
    //allcoate the buffer for the new attribute to track
    allocateNewLiveBufferForAttributeAndType(attr, attributeValueMap[attr].dir, attributeValueMap[attr].valueType);
}

CDataWrapper * CUController::getLiveCDataWrapperPtr() {
    return current_dataset[KeyDataStorageDomainOutput].get();
}


ChaosSharedPtr<chaos::common::data::CDataWrapper> CUController::getCurrentDatasetForDomain(DatasetDomain domain) {
    boost::mutex::scoped_lock lock(trackMutext);
    if(domain<current_dataset.size()){
        return current_dataset[domain];
    }
    return current_dataset[0];
}
int CUController::getCurrentDatasetForDomain(DatasetDomain domain,chaos::common::data::CDataWrapper* ret){
    boost::mutex::scoped_lock lock(trackMutext);
    if(ret){
        //ret->reset();
        //ret->setSerializedData((const char*)current_dataset[domain]->getBSONData());
        current_dataset[domain]->copyAllTo(*ret);
        return 0;
    }
    return -1;
    
}
int   CUController::fetchCurrentDatatasetFromDomain(DatasetDomain domain,chaos::common::data::CDataWrapper* ret){
    CUController::fetchCurrentDatatasetFromDomain(domain);
    boost::mutex::scoped_lock lock(trackMutext);
    if(ret){
        // ret->reset();
        current_dataset[domain]->copyAllTo(*ret);
        return 0;
    }
    return 0;
}

int CUController::fetchAllDataset() {
    int err = 0;
    if(ioLiveDataDriver.get()==NULL){
            LERR_<<"NO IO DRIVER!!!!!";
            return -1;

    }
    boost::mutex::scoped_lock lock(trackMutext);
    chaos::common::data::VectorCDWShrdPtr results;
    if((err = ioLiveDataDriver->retriveMultipleData(channel_keys,
                                                    results)) == 0) {
        int counter = 0;
        for(std::vector< ChaosSharedPtr<chaos::common::data::CDataWrapper> >::iterator it = current_dataset.begin(),
            end = current_dataset.end();
            (it != end) && (counter<results.size());
            it++) {
            if(results[counter].get()){
                (*it) = results[counter++];
            }
            
            
        }
    }
    return err;
}


ChaosSharedPtr<chaos::common::data::CDataWrapper>  CUController::fetchCurrentDatatasetFromDomain(DatasetDomain domain) {
    boost::mutex::scoped_lock lock(trackMutext);
    size_t value_len = 0;
    char *value = ioLiveDataDriver->retriveRawData(channel_keys[domain],(size_t*)&value_len);
    if(value){
        chaos::common::data::CDataWrapper *tmp = new CDataWrapper(value);
        current_dataset[domain].reset(tmp);
        delete [] value;
        return current_dataset[domain];
    }
    return current_dataset[domain];
}

int CUController::getPackSeq(uint64_t& seq){
    CDataWrapper * d = current_dataset[KeyDataStorageDomainOutput].get();
    if(d){
        seq =d->getInt64Value(DataPackCommonKey::DPCK_SEQ_ID);
        return 0;
    }
    seq=-1;
    return -1;
}
int CUController::getTimeStamp(uint64_t& live,bool hr){
    CDataWrapper * d = current_dataset[KeyDataStorageDomainOutput].get();
    live =0;
    if(d){
        if(hr){
            if(d->hasKey(DataPackCommonKey::DPCK_HIGH_RESOLUTION_TIMESTAMP)) {
                live = d->getInt64Value(DataPackCommonKey::DPCK_HIGH_RESOLUTION_TIMESTAMP);
            } else if(d->hasKey(DataPackCommonKey::DPCK_TIMESTAMP)){
                live= d->getInt64Value(DataPackCommonKey::DPCK_TIMESTAMP)*1000;
            }
        } else {
            if(d->hasKey(DataPackCommonKey::DPCK_TIMESTAMP)){
                live = d->getInt64Value(DataPackCommonKey::DPCK_TIMESTAMP);
            } else if(d->hasKey(DataPackCommonKey::DPCK_HIGH_RESOLUTION_TIMESTAMP)){
                live= d->getInt64Value(DataPackCommonKey::DPCK_HIGH_RESOLUTION_TIMESTAMP)*1000;
            }
        }
        return 0;
    }
    return -1;
}
void CUController::setupTracking() {
    boost::mutex::scoped_lock lock(trackMutext);
    
    //init live buffer
    initializeAttributeIndexMap();
    
    //initialize timestamp buffer
    SingleBufferCircularBuffer<int64_t> *newBuffer = new SingleBufferCircularBuffer<int64_t>(10);
    int64AttributeLiveBuffer.insert(make_pair(DataPackCommonKey::DPCK_TIMESTAMP, newBuffer));
}

void CUController::stopTracking() {
    boost::mutex::scoped_lock lock(trackMutext);
    deinitializeAttributeIndexMap();
}

void CUController::fetchCurrentDeviceValue() {
    boost::mutex::scoped_lock lock(trackMutext);
    
    //! fetch the output odmain
    fetchCurrentDatatasetFromDomain(KeyDataStorageDomainOutput);
    
    if(trackingAttribute.size() == 0) return;
    CDataWrapper *tmpPtr = current_dataset[KeyDataStorageDomainOutput].get();
    
    //add timestamp value
    int64_t got_ts = tmpPtr->getInt64Value(DataPackCommonKey::DPCK_TIMESTAMP);
    if(int64AttributeLiveBuffer[DataPackCommonKey::DPCK_TIMESTAMP]->getLastValue() == got_ts) return;
    
    int64AttributeLiveBuffer[DataPackCommonKey::DPCK_TIMESTAMP]->addValue(got_ts);
    
    //update buffer for tracked attribute
    for (std::vector<string>::iterator iter = trackingAttribute.begin();
         iter != trackingAttribute.end();
         iter++) {
        const char *key = (*iter).c_str();
        if(!tmpPtr->hasKey(key)) continue;
        
        switch (attributeValueMap[*iter].valueType) {
                //TODO: other cases
            case DataType::TYPE_INT32:
                int32AttributeLiveBuffer[*iter]->addValue(tmpPtr->getInt32Value(key));
                break;
                
            case DataType::TYPE_INT64:
                int64AttributeLiveBuffer[*iter]->addValue(tmpPtr->getInt64Value(key));
                break;
                
            case DataType::TYPE_DOUBLE:
                doubleAttributeLiveBuffer[*iter]->addValue(tmpPtr->getDoubleValue(key));
                break;
                
            case DataType::TYPE_BYTEARRAY:
                uint32_t ptrLen = 0;
                const char * tmpPtrAttribute = tmpPtr->getBinaryValue(key, ptrLen);
                pointerAttributeLiveBuffer[*iter]->updateData(tmpPtrAttribute, ptrLen);
                break;
        }
    }
}

CDataWrapper *CUController::getCurrentData(){
    return current_dataset[KeyDataStorageDomainOutput].get();
}

//! get profile info
cu_prof_t CUController::getProfileInfo(){
    chaos::common::data::CDataWrapper *prof=  fetchCurrentDatatasetFromDomain(KeyDataStorageDomainHealth).get();
    cu_prof_t p;
    bzero(&p,sizeof(cu_prof_t));
    if(prof){
        p.push_rate=prof->getDoubleValue(ControlUnitHealtDefinitionValue::CU_HEALT_OUTPUT_DATASET_PUSH_RATE);
        p.sys_time =prof->getDoubleValue(NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME);
        p.usr_time = prof->getDoubleValue(NodeHealtDefinitionKey::NODE_HEALT_USER_TIME);
        p.upt_time = prof->getInt64Value(NodeHealtDefinitionKey::NODE_HEALT_PROCESS_UPTIME);
        p.metric_time = prof->getInt64Value(NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP_LAST_METRIC);
    }
    return p;
}

//! get datapack between time itervall
void CUController::executeTimeIntervallQuery(DatasetDomain domain,
                                             uint64_t      start_ts,
                                             uint64_t      end_ts,
                                             QueryCursor** query_cursor,
                                             uint32_t      page) {
  executeTimeIntervallQuery(
      domain,
      start_ts,
      end_ts,
      ChaosStringSet(),
      query_cursor,
      page);
}
void CUController::executeTimeIntervallQuery(DatasetDomain domain,
                                             uint64_t start_ts,
                                             uint64_t end_ts,
                                             const ChaosStringSet& meta_tags,
                                             QueryCursor **query_cursor,
                                             uint32_t page) {
    if((domain>=0) && (domain<=DPCK_LAST_DATASET_INDEX)){
        *query_cursor = ioLiveDataDriver->performQuery(channel_keys[domain],
                                                       start_ts,
                                                       end_ts,
                                                       meta_tags,
                                                       page);
    }
}

void CUController::executeTimeIntervalQuery(const DatasetDomain domain,
                                            const uint64_t start_ts,
                                            const uint64_t end_ts,
                                            const uint64_t seqid,
                                            const uint64_t runid,
                                            chaos::common::io::QueryCursor **query_cursor,
                                            const uint32_t page_len){
executeTimeIntervalQuery(
    domain,
    start_ts,
    end_ts,
    seqid,
    runid,
    ChaosStringSet(),
    query_cursor,
    page_len);
}

void CUController::executeTimeIntervalQuery(const DatasetDomain domain,
                                            const uint64_t start_ts,
                                            const uint64_t end_ts,
                                            const uint64_t seqid,
                                            const uint64_t runid,
                                            const ChaosStringSet& meta_tags,
                                            chaos::common::io::QueryCursor **query_cursor,
                                            const uint32_t page_len){
    if((domain>=0) && (domain<=DPCK_LAST_DATASET_INDEX)){
        *query_cursor = ioLiveDataDriver->performQuery(channel_keys[domain],
                                                       start_ts,
                                                       end_ts,
                                                       seqid,
                                                       runid,
                                                       meta_tags,
                                                       page_len);
    }
    
}
//! release a query
void CUController::releaseQuery(QueryCursor *query_cursor) {
    ioLiveDataDriver->releaseQuery(query_cursor);
}

int CUController::loadDatasetTypeFromSnapshotTag(const std::string& snapshot_tag,
                                                 DatasetDomain dataset_type,
                                                 chaos_data::CDWShrdPtr& cdatawrapper_handler) {
    
    return ioLiveDataDriver->loadDatasetTypeFromSnapshotTag(snapshot_tag,
                                                            devId,
                                                            dataset_type,
                                                            cdatawrapper_handler);
}

int CUController::createNewSnapshot(const std::string& snapshot_tag,
                                    const std::vector<std::string>& other_snapped_device) {
    
     if(ioLiveDataDriver.get()==NULL){
            LERR_<<"NO IO DRIVER!!!!!";
            return -1;

    }
    std::vector<std::string> device_id_in_snap = other_snapped_device;
    device_id_in_snap.push_back(devId);
    return mdsChannel->createNewSnapshot(snapshot_tag,
                                         device_id_in_snap);
}

int CUController::deleteSnapshot(const std::string& snapshot_tag) {
if(ioLiveDataDriver.get()==NULL){
            LERR_<<"NO IO DRIVER!!!!!";
            return -1;

    }
        return mdsChannel->deleteSnapshot(snapshot_tag);
}

int CUController::getSnapshotList(ChaosStringVector& snapshot_list) {
    CHAOS_ASSERT(mdsChannel)
    return mdsChannel->searchSnapshotForNode(devId,
                                             snapshot_list,
                                             millisecToWait);
}

int CUController::searchNode(const std::string& unique_id_filter,
                             unsigned int node_type_filter,
                             bool alive_only,
                             unsigned int last_node_sequence_id,
                             unsigned int page_length,
                             ChaosStringVector& node_found) {
    
    return mdsChannel->searchNode(unique_id_filter,
                                  node_type_filter,
                                  alive_only,
                                  last_node_sequence_id,
                                  page_length,
                                  node_found,
                                  millisecToWait);
}

