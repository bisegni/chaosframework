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
#include "GrafanaSearch.h"

#include <cstring>
#include <json/reader.h>

#include <json/writer.h>
#include <chaos/common/data/CDataWrapper.h>

#include <chaos/common/chaos_constants.h>
#include <chaos/common/ChaosCommon.h>
#include <boost/algorithm/string.hpp>

using namespace chaos::common::data;
using namespace chaos::wan_proxy::api::grafana;

#define PRODUCER_INSERT_ERR(where, err, msg)\
    MAKE_API_ERR(where, "producer_insert_err", err, "producer_insert_err_msg", msg)

#define PID_LAPP LAPP_ << "[GrafanaSearch] - "
#define PID_LDBG LDBG_ << "[GrafanaSearch] - "
#define PID_LERR LERR_ << "[GrafanaSearch] - " << __PRETTY_FUNCTION__ << "(" << __LINE__ << ") - "
static boost::posix_time::ptime const time_epoch(boost::gregorian::date(1970, 1, 1));

//! default constructor
//!


GrafanaSearch::GrafanaSearch(persistence::AbstractPersistenceDriver *_persistence_driver):
    AbstractApi("search",
                _persistence_driver){
    
}


//! default destructor
GrafanaSearch::~GrafanaSearch() {
    
}

//! execute the api
int GrafanaSearch::execute(std::vector<std::string>& api_tokens,
                           const Json::Value& input_data,
                           std::map<std::string, std::string>& output_header,
                           Json::Value& output_data) {
    CHAOS_ASSERT(persistence_driver);

    int err = 0;
    std::string err_msg;
    std::string producer_name;
    int cnt;
    uint64_t ts;
    try {

        Json::StyledWriter				json_writer;


        CDataWrapper dataset_pack;


        std::string target;

        // add the node unique id
        if(!input_data.empty()){
            std::string json_str=json_writer.write(input_data);
            const char* json_string_p=json_str.c_str();
            PID_LDBG << "PROCESSING:"<<json_str.c_str();//json_writer.write(input_data).c_str();

            if(json_string_p){
                PID_LDBG << "TO DECODE:"<<json_string_p<<" string size:"<<json_str.size();

                dataset_pack.setSerializedJsonData(json_string_p);
                if(dataset_pack.hasKey("target")){
                    target=dataset_pack.getStringValue("target");
                }

            }
        }
        std::stringstream ss;
        Json::Reader json_reader;


#if 0
        chaos::common::data::CDWShrdPtr cd =persistence_driver->searchMetrics(target,false);
        chaos::common::data::CDataWrapper w;
        std::vector<std::string> keys;
        cd->getAllKey(keys);
        ss<<"[";
        for(std::vector<std::string>::iterator i=keys.begin();i!=keys.end();i++){
            ChaosUniquePtr<chaos::common::data::CMultiTypeDataArrayWrapper> w(cd->getVectorValue(*i));
            for(int idx=0;idx<w->size();idx++){
                chaos::common::data::CDWUniquePtr ws(w->getCDataWrapperElementAtIndex(idx));
                if(ws->hasKey("cudk_ds_attr_name")){
                    std::string name=ws->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME);
                    if(ws->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION)){
                        int val=ws->getInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION);
                        if(val == chaos::DataType::DataSetAttributeIOAttribute::Input){
                            ss<<"\""<<*i<<"/"<<"input/"<<name<<"\"";
                        }
                        if(val == chaos::DataType::DataSetAttributeIOAttribute::Output){
                            ss<<"\""<<*i<<"/"<<"output/"<<name<<"\"";
                        }
                        if(val == chaos::DataType::DataSetAttributeIOAttribute::Bidirectional){
                            ss<<"\""<<*i<<"/"<<"output/"<<name<<"\","<<"\""<<*i<<"/"<<"input/"<<name<<"\"";
                        }
                    }
                    if((i+1) !=keys.end()){
                        ss<<",";
                    } else {
                        if((idx+1)<w->size()){
                            ss<<",";
                        }
                    }
                }
            }

        }
        ss<<"]";
#else
        ChaosStringVector metrics;
        persistence_driver->searchMetrics(target,metrics,false);
        ss<<"[";
        for(ChaosStringVector::iterator i=metrics.begin();i!=metrics.end();++i){
            ss<<"\""<<*i<<"\"";
            if((i+1)!=metrics.end()){
                ss<<",";
            }
        }
        ss<<"]";


#endif
        PID_LDBG << "RETURNING:"<<ss.str();

        json_reader.parse(ss.str(),output_data);
    } catch(std::exception e){
        PID_LERR << "Exception GRAFANA SEARCH: "<<e.what();
        err=-1;
    } catch(...){
        PID_LERR << "Exception during process GRAFANA SEARCH";
        err=-2;
    }
    return err;
}
