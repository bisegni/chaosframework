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

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <chaos/common/additional_lib/base64.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/network/CNodeNetworkAddress.h>
#include <chaos_metadata_service_client/ChaosMetadataServiceClient.h>



#include <chaos/common/bson/bson.h>

#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

using namespace std;
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::utility;

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::node_controller;
using namespace boost;

#define OPT_CU_ID           "device-id"
#define OPT_TIMEOUT         "timeout"
#define OPT_DST_TARGET      "dest-target"
#define OPT_DST_FILE        "dest-file"
#define OPT_DST_TYPE        "dest-type"
#define OPT_PAGE_LENGHT     "page-lenght"
#define OPT_START_TIME      "start-time"
#define OPT_END_TIME		"end-time"
#define OPT_TAGS            "tags"


void sendBackOnRow() {
    std::cout << "\r" <<std::flush;
}

void sendBackForNPos(int position = 1) {
    for(int i = 0; i < position; i++) {
        std::cout << "\b";
    }
    std::cout << std::flush;
}

void printNumberOfExportedElement(uint32_t done) {
    std::cout << CHAOS_FORMAT("\rExported %1% record", %done) << std::flush;
}

void printStep() {
    std::cout << "." <<std::flush;
}

int computePercent(uint64_t done, uint64_t all) {
    int result = ((double)done/(double)all)*100;
    return result;
}

chaos::common::data::SerializationBuffer *getCSVDecoding(CUController& controller,
                                                         const std::vector<std::string>& output_element_name,
                                                         CDataWrapper& data_pack) {
    chaos::common::data::SerializationBuffer *result = NULL;
    std::stringstream csv_lin;
    chaos::common::data::RangeValueInfo attribute_info;
    
    //write timetamp before all field
    csv_lin << data_pack.getInt64Value(chaos::DataPackCommonKey::DPCK_TIMESTAMP) << ",";
    csv_lin << data_pack.getInt64Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_RUN_ID) << ",";
    csv_lin << data_pack.getInt64Value(chaos::DataPackCommonKey::DPCK_SEQ_ID) << ",";
    int idx = 0;
    for(std::vector<std::string>::const_iterator it = output_element_name.begin();
        it < output_element_name.end();
        it++){
        //fetch the type
        controller.getDeviceAttributeRangeValueInfo(*it, attribute_info);
        
        //write the data in csv way
        switch(attribute_info.valueType){
            case DataType::TYPE_BOOLEAN:{
                if(data_pack.hasKey((*it).c_str())){
                    csv_lin << data_pack.getBoolValue((*it).c_str());
                }
                break;
            }
                
            case DataType::TYPE_BYTEARRAY:{
                uint32_t len;
                std::string binary_field;
                const char * base_addr = data_pack.getBinaryValue((*it).c_str(), len);
                if(base_addr) {
                    binary_field = base64_encode( (unsigned char const* ) base_addr , len );
                    csv_lin << binary_field;
                }
                break;
            }
                
            case DataType::TYPE_DOUBLE:
                if(data_pack.hasKey((*it).c_str())){
                    csv_lin << data_pack.getDoubleValue((*it).c_str());
                }
                break;
            case DataType::TYPE_INT32:{
                if(data_pack.hasKey((*it).c_str())){
                    csv_lin << data_pack.getInt32Value((*it).c_str());
                }
                break;
            }
                
            case DataType::TYPE_INT64:{
                if(data_pack.hasKey((*it).c_str())){
                    csv_lin << data_pack.getInt64Value((*it).c_str());
                }
                break;
            }
                
            case DataType::TYPE_STRING:{
                if(data_pack.hasKey((*it).c_str())){
                    csv_lin << data_pack.getStringValue((*it).c_str());
                }
                break;
            }
                
            default:
                break;
        }
        if(++idx < output_element_name.size()) {
            csv_lin << ",";
        }else{
            csv_lin << std::endl;
        }
    }
    std::string line = csv_lin.str();
    if(line.size()>0) {
        result = new chaos::common::data::SerializationBuffer(line.c_str(), line.size());
    }
    return result;
}

int main(int argc, const char* argv[]) {
    char buf[255];
    uint32_t timeout;
    string device_id;
    string dst_file="export";
    uint32_t dest_type;
    uint32_t page_len;
    string start_time;
    string end_time;
    ChaosStringVector meta_tags;
    std::string err_str;
    std::ostream	*destination_stream = NULL;
    std::ofstream	destination_file;
    
    uint64_t start_ts = 0;
    uint64_t end_ts = 0;
    
    int rett=0;
    int retry = 0;
    //clear buffer
    memset(buf, 0, 255);
    
    //disable buffer
    std::setvbuf(stdout, NULL, _IONBF, 0);
    
    CDeviceNetworkAddress deviceNetworkAddress;
    try{
        //std::cout << "\x1B[?25l";
        //! [UIToolkit Attribute Init]
        ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption<string>(OPT_CU_ID, "The identification string of the device", &device_id);
        ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>(OPT_TIMEOUT, "Timeout for wait the answer in milliseconds", 2000, &timeout);
        ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>(OPT_DST_TYPE, "Destination date type [binary(0), JSON(1), CSV(2)]", 0, &dest_type);
        ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption<string>(OPT_DST_FILE, "Destination file for save found datapack", &dst_file);
        ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption<string>(OPT_START_TIME, "Time for first datapack to find [format from %Y-%m-%dT%H:%M:%S.%f to %Y]", &start_time);
        ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption<string>(OPT_END_TIME, "Time for last datapack to find [format from %Y-%m-%dT%H:%M:%S.%f to %Y]", &end_time);
        ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>(OPT_PAGE_LENGHT, "query page lenght", 30, &page_len);
        ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption<ChaosStringVector>(OPT_TAGS, "Meta tags list", &meta_tags);
        //! [UIToolkit Attribute Init]
        
        //! [UIToolkit Init]
        ChaosMetadataServiceClient::getInstance()->init(argc, argv);
        //! [UIToolkit Init]
        if(!ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_CU_ID)){
            throw CException(-1, "invalid device identification string", "check param");
        }
        
        //get the timestamp for query boundary
        if(ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_START_TIME)){
            //try to check if the string is a number
            try {
                start_ts = lexical_cast<uint64_t>(start_time);
            } catch(bad_lexical_cast &) {
                if(!TimingUtil::dateWellFormat(start_time)) {
                    throw CException(-2, "Invalid star date format", "check date");
                }
                start_ts = TimingUtil::getTimestampFromString(start_time);
            }
            std::cout << "Set start data to:"<< start_time << std::endl;
        }
        
        if(ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_END_TIME)){
            //try to check if the string is a number
            try {
                end_ts = lexical_cast<uint64_t>(end_time);
            } catch(bad_lexical_cast &) {
                if(!TimingUtil::dateWellFormat(end_time)) {
                    throw CException(-3, "Invalid end date format", "check date");
                }
                end_ts = TimingUtil::getTimestampFromString(end_time);
            }
            std::cout << "Set end data to:"<< end_time << std::endl;
        }
        
        switch(dest_type) {
            case 0:
                dst_file.append(".bin");
                break;
            case 1:
                dst_file.append(".json");
                break;
            case 2:
                dst_file.append(".cvs");
                break;
        }
        
        std::basic_ios<char>::openmode dst_file_mode = ios_base::out;
        if(dest_type) {
            dst_file_mode |= ios_base::binary;
        }
        destination_file.open(dst_file.c_str(), dst_file_mode);
        if(!destination_file.good()) {
            err_str = "Error opening destination file ";
            err_str.append(buf, strlen(buf));
            throw CException(-4, err_str, string("check param"));
        }
        std::cout << "Destination file -> " << dst_file << std::endl;
        destination_stream = &destination_file;
        
        //we can allocate the channel
        std::cout << "Acquiring controller" << std::endl;
        CUController *controller = NULL;
        ChaosMetadataServiceClient::getInstance()->getNewCUController(device_id, &controller);
        if(!controller) throw CException(4, "Error allocating decive controller", "device controller creation");
        controller->setQueryOnIndex(true);
        ChaosStringSet search_tags;
        chaos::common::io::QueryCursor *query_cursor = NULL;
        for(ChaosStringVectorIterator it = meta_tags.begin(),
            end = meta_tags.end();
            it != end;
            it++) {
            search_tags.insert(*it);
        }
        controller->executeTimeIntervallQuery(chaos::cu::data_manager::KeyDataStorageDomainOutput,
                                              start_ts,
                                              end_ts,
                                              search_tags,
                                              &query_cursor,
                                              page_len);
        
        std::vector<std::string> output_element_name;
        //fetche the output element of the device
        controller->getDeviceDatasetAttributesName(output_element_name,
                                                   DataType::Output);
        
        //create header
        if(dest_type == 2) {
            //write header
            int idx = 0;
            (*destination_stream) << chaos::DataPackCommonKey::DPCK_TIMESTAMP << ",";
            (*destination_stream) << chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_RUN_ID << ",";
            (*destination_stream) << chaos::DataPackCommonKey::DPCK_SEQ_ID << ",";
            for(std::vector<std::string>::const_iterator it = output_element_name.begin();
                it < output_element_name.end();
                it++){
                (*destination_stream) << *it;
                if(++idx < output_element_name.size()) {
                    (*destination_stream) << ",";
                }
            }
            (*destination_stream) << std::endl;
        }
        
        if(query_cursor) {
            uint32_t exported = 0;
            std::cout << "Exported " << std::flush;
            while(query_cursor->hasNext()) {
                exported++;
                ChaosSharedPtr<CDataWrapper> q_result(query_cursor->next());
                if(q_result.get()) {
                    retry = 0;
                    SerializationBufferUPtr ser;
                    //get serialization buffer by type
                    switch (dest_type) {
                            //BSON
                        case 0:{
                            ser = q_result->getBSONData();
                            break;
                        }
                            //JSON
                        case 1:{
                            std::string ser_str = q_result->getJSONString();
                            //ser = ChaosMakeSharedPtr<SerializationBuffer>(ser_str.c_str(), ser_str.size()));
                            break;
                        }
                            //CSV
                        case 2:{
                            ser.reset(getCSVDecoding(*controller, output_element_name, *q_result.get()));
                            break;
                        }
                    }
                    //write the data
                    if(ser.get())destination_stream->write(ser->getBufferPtr(), ser->getBufferLen());
                    
                } else {
                    break;
                }
                printNumberOfExportedElement(exported);
            }
            printNumberOfExportedElement(exported);
            std::cout << std::endl;
            std::cout << "Releasing query" << std::endl;
            controller->releaseQuery(query_cursor);
        }
        destination_stream->flush();
        std::cout << "Releasing controller" << std::endl;
        ChaosMetadataServiceClient::getInstance()->deleteCUController(controller);
    } catch (CException& e) {
        std::cout << "\x1B[?25h";
        std::cerr << e.errorCode << " - "<< e.errorDomain << " - " << e.errorMessage << std::endl;
        return -1;
    } catch (...) {
        std::cout << "\x1B[?25h";
        std::cerr << "General error " << std::endl;
        return -2;
    }
    
    try {
        //! [UIToolkit Deinit]
        ChaosMetadataServiceClient::getInstance()->deinit();
        //! [UIToolkit Deinit]
    } catch (CException& e) {
        std::cout << "\x1B[?25h";
        std::cerr << e.errorCode << " - "<< e.errorDomain << " - " << e.errorMessage << std::endl;
        return -3;
    }
    std::cout << "\x1B[?25h";
    std::cout << std::endl << "Export done"<< std::endl;
    return rett;
}

