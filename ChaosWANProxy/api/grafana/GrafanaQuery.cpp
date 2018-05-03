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
#include "GrafanaQuery.h"

#include <cstring>

#include <chaos/common/chaos_constants.h>
#include <chaos/common/ChaosCommon.h>
#include <boost/algorithm/string.hpp>
#include <chaos/common/utility/TimingUtil.h>
using namespace chaos::common::data;
using namespace chaos::wan_proxy::api::grafana;

#define PRODUCER_INSERT_ERR(where, err, msg)\
MAKE_API_ERR(where, "producer_insert_err", err, "producer_insert_err_msg", msg)

#define PID_LAPP LAPP_ << "[GrafanaQuery] - "
#define PID_LDBG LDBG_ << "[GrafanaQuery] - "
#define PID_LERR LERR_ << "[GrafanaQuery] - " << __PRETTY_FUNCTION__ << "(" << __LINE__ << ") - "
static boost::posix_time::ptime const time_epoch(boost::gregorian::date(1970, 1, 1));

//! default constructor
//!


GrafanaQuery::GrafanaQuery(persistence::AbstractPersistenceDriver *_persistence_driver):
AbstractApi("query",
            _persistence_driver){
    
}


//! default destructor
GrafanaQuery::~GrafanaQuery() {
    
}

//! execute the api
int GrafanaQuery::execute(std::vector<std::string>& api_tokens,
                                   const Json::Value& input_data,
                                   std::map<std::string, std::string>& output_header,
                                   Json::Value& output_data) {
    CHAOS_ASSERT(persistence_driver);

    int err = 0;
    std::string err_msg;
    std::string producer_name;
    int cnt;
    uint64_t ts;
    Json::Reader json_reader;

    try {

    
    

    //we can proceed
    ChaosUniquePtr<chaos::common::data::CDataWrapper> output_dataset(new CDataWrapper());
    //	const std::string& producer_name = api_tokens[0];
    
    // add the node unique id
	Json::StyledWriter				json_writer;
    if(!input_data.empty()){
        PID_LDBG << "PROCESSING:"<<json_writer.write(input_data).c_str();

        std::map<std::string,std::vector<std::string> > accesses;
        boost::regex expr("(.*)/(.*)$");
        std::string start=input_data["range"]["from"].asString();
        std::string end=input_data["range"]["to"].asString();
        uint64_t start_t=chaos::common::utility::TimingUtil::getTimestampFromString(start);
        uint64_t end_t=chaos::common::utility::TimingUtil::getTimestampFromString(end);
        PID_LDBG << "Start:"<<start_t<<" End:"<<end_t;
        const Json::Value targets = input_data["targets"];
        // reduce accesses
        for ( int index = 0; index < targets.size(); ++index ){
            std::string tname=targets[index]["target"].asString();
            PID_LDBG << "Target:"<<tname;

            boost::cmatch what;
            if(regex_match(tname.c_str(), what, expr)){
                std::map<std::string,std::vector<std::string> >::iterator i=accesses.find(what[1]);
                if(i!=accesses.end()){
                    PID_LDBG << " variable:"<<what[2]<< " adding to access:"<<i->first;

                    i->second.push_back(what[2]);
                } else {
                    PID_LDBG << " variable:"<<what[2]<< " to new access:"<<what[1];
                    accesses[what[1]].push_back(what[2]);
                }
            }
        }

        // perform queries
        for(std::map<std::string,std::vector<std::string> >::iterator i=accesses.begin();i!=accesses.end();i++){
            boost::cmatch what;

            if(regex_match(i->first.c_str(), what, expr)){
                std::string cuname=what[1];
                std::string dir=what[2];
                PID_LDBG << " access CU:"<<cuname<<" channel:"<<dir<< " # vars:"<<i->second.size();

            }
        }



    }
    std::string ss="[]";
    json_reader.parse(ss,output_data);

    } catch(std::exception e){
        PID_LERR << "Exception:"<<e.what();
        err=-1;
    } catch(...){
        PID_LERR << "Exception during process";
        err=-2;
    }
    return err;
}
