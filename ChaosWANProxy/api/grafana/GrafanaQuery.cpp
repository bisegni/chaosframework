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
    std::stringstream ss;

    if(!input_data.empty() && !input_data["targets"].empty()){
        PID_LDBG << "PROCESSING:"<<json_writer.write(input_data).c_str();

        std::map<std::string,std::vector<std::string> > accesses;
        boost::regex expr("(.*)/(.*)$");
        std::string start=input_data["range"]["from"].asString();
        std::string end=input_data["range"]["to"].asString();
        const Json::Value targets = input_data["targets"];

        int maxDataPoints=input_data["maxDataPoints"].asInt();
        if(maxDataPoints<=0){
            maxDataPoints=1000;
        }
        std::vector<std::string> metrics;
        // reduce accesses
        for ( int index = 0; index < targets.size(); ++index ){
            std::string tname=targets[index]["target"].asString();
            metrics.push_back(tname);
            PID_LDBG << "Target:"<<tname;
        }
        chaos::wan_proxy::persistence::metrics_results_t res;
        if(persistence_driver->queryMetrics(start,end,metrics,res,maxDataPoints)==0){
            ss<<"[";
            for(chaos::wan_proxy::persistence::metrics_results_t::iterator i=res.begin();i!=res.end();){
                ss<<"{\"target\":\""<<i->first<<"\",\"datapoints\":[";
                for(std::vector<chaos::wan_proxy::persistence::metric_t>::iterator j=i->second.begin();j!=i->second.end();j++){
                    ss<<"["<<j->value<<","<<j->milli_ts+j->idx<<"]";
                    if((j+1)!=i->second.end()){
                        ss<<",";
                    }
                }
                ss<<"]}";
                if((++i)!=res.end()){
                    ss<<",";
                }
            }
            ss<<"]";
        }
        uint64_t diff_from_now_s=chaos::common::utility::TimingUtil::getTimeStamp()-chaos::common::utility::TimingUtil::getTimestampFromString(start,true);
        uint64_t diff_from_now_e=chaos::common::utility::TimingUtil::getTimeStamp()-chaos::common::utility::TimingUtil::getTimestampFromString(end,true);

        PID_LDBG << "ANSWER "<<start<<"("<< chaos::common::utility::TimingUtil::getTimestampFromString(start)<<" diff from now:"<<diff_from_now_s<<") - "<<end<<"("<<chaos::common::utility::TimingUtil::getTimestampFromString(end)<<" diff from now:"<<diff_from_now_e<<"):"<<ss.str();

    } else{
        ss<<"[]";
    }

    json_reader.parse(ss.str(),output_data);

    } catch(std::exception e){
        PID_LERR << "Exception:"<<e.what();
        err=-1;
    } catch(...){
        PID_LERR << "Exception during process";
        err=-2;
    }
    return err;
}
