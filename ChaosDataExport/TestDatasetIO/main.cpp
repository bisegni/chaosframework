/*
 * File:   testDataSetAttribute.cpp
 * Author: michelo
 * test from UI
 * Created on September 10, 2015, 11:24 AM
 */

#include <cstdlib>
#include <chaos_metadata_service_client/ChaosMetadataServiceClient.h>

#include "ChaosDatasetIO.h"
using namespace std;
using namespace ::driver::misc;
using namespace chaos::metadata_service_client;
#include <math.h>
/*
 *
 */
using namespace driver::misc;
static int checkData(ChaosDatasetIO& test, std::vector<ChaosDataSet> & res,uint64_t& pcktmissing,uint64_t&pckt,uint64_t&pcktreplicated,uint64_t&pcktmalformed,uint64_t&badid){
    int reterr=0;
    //  uint64_t end_time=chaos::common::utility::TimingUtil::getLocalTimeStampInMicroseconds();
    //   double avg=total*1000000.0/(end_time-start_time);
    //   printf("Retrived %.8d items items/s:%.4f tot us:%.10llu\r",res.size(),avg,(end_time-start_time));
    uint64_t cnt=0;
    for(std::vector<ChaosDataSet>::iterator i=res.begin();i!=res.end();i++){
        if((*i)->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_RUN_ID)){
            uint64_t p=(*i)->getUInt64Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_RUN_ID);
            if(p !=test.getRunID()){
                std::cout<<"\t ##["<<cnt<<"] error different runid found:"<<p<<" expected:"<<test.getRunID()<<std::endl;
                reterr++;
                badid++;
                continue;
            }
        }  else {
            pcktmalformed++;
        }
        if((*i)->hasKey(chaos::DataPackCommonKey::DPCK_SEQ_ID)){
            uint64_t p=(*i)->getUInt64Value(chaos::DataPackCommonKey::DPCK_SEQ_ID);
            if(p> pckt){
                uint64_t missing=(p-pckt);
                std::cout<<"\t ##["<<cnt<<"] error missing #pckts:"<<missing<<" starting from "<<chaos::DataPackCommonKey::DPCK_SEQ_ID<<"  :"<<pckt<<" to:"<<p<<std::endl;
                if(i!=res.begin()){
                    LERR_<<"["<<cnt<<"] MISSING START:"<<(*(i-1))->getCompliantJSONString()<<" END:"<<(*(i))->getCompliantJSONString();
                } else {
                    LERR_<<"["<<cnt<<"] MISSING START FOUND:"<<(*(i))->getCompliantJSONString();
                }
                
                reterr++;
                pckt=(p+1);
                pcktmissing+=missing;
            } else if(p<pckt){
                pcktreplicated++;
                std::cout<<"\t ##["<<cnt<<"] error replicated packet id:"<<p<<" expected:"<<pckt<<std::endl;
                LERR_<<"["<<cnt<<"] REPLICATED:"<<(*i)->getCompliantJSONString();
                reterr++;
            } else {
                pckt++;
            }
            
        } else {
            std::cout<<"\t ##["<<cnt<<"] missing "<<chaos::DataPackCommonKey::DPCK_SEQ_ID;
            reterr++;
        }
        cnt++;
    }
    return reterr;
}

int main(int argc, const char** argv) {
    int reterr=0;
    uint32_t loops;
    const double PI  =3.141592653589793238463;
    uint32_t waitloops,wait_retrive;
    std::string name,group;
    uint32_t pagelen=0;
    uint32_t npoints=15000;
    double freqshift=0.001,ampshift=0.9999;
    double freq=1,phase=0,amp=1,afreq,aamp;
    double delta;
    bool binary;
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption("dsname", po::value<std::string>(&name)->default_value("PERFORMANCE_MESURE"),"name of the dataset (CU)");
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption("dsgroup", po::value<std::string>(&group)->default_value("DATASETIO"),"name of the group (US)");
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption("page", po::value<uint32_t>(&pagelen)->default_value(0),"Page len to recover data");
    
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption("loops", po::value<uint32_t>(&loops)->default_value(500),"number of push/loop");
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption("waitloop", po::value<uint32_t>(&waitloops)->default_value(0),"us waits bewteen loops");
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption("wait", po::value<uint32_t>(&wait_retrive)->default_value(5),"seconds to wait to retrive data after pushing");
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption("points", po::value<uint32_t>(&npoints)->default_value(15000),"Number of sin points, 0 = no wave");
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption("freqshift", po::value<double>(&freqshift)->default_value(0.001),"Modify freq Hz every loop, 0= no modify");
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption("ampshift", po::value<double>(&ampshift)->default_value(0.9999),"Modify amplitude every loop, 0= no modify");
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption("binary", po::value<bool>(&binary)->default_value(false),"The wave is in binary");
    
    ChaosMetadataServiceClient::getInstance()->init(argc,argv);
    ChaosMetadataServiceClient::getInstance()->start();
    
    ChaosDatasetIO test(name);
    ChaosDataSet my_ouput=test.allocateDataset(chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT);
    ChaosDataSet my_input=test.allocateDataset(chaos::DataPackCommonKey::DPCK_DATASET_TYPE_INPUT);
    delta=1.0/npoints;
    std::vector<double> val;
    double buf[npoints];
    afreq=freq;
    aamp=amp;
    my_ouput->addInt64Value("counter64",(int64_t)0);
    my_ouput->addInt32Value("counttoper32",0);
    my_ouput->addStringValue("stringa","hello dataset");
    my_ouput->addDoubleValue("doublevar",0.0);
    if(npoints){
        if(binary){
            for(int cnt=0;cnt<npoints;cnt++){
                double data=sin(2*PI*freq*(delta*cnt)+phase)*amp;
                buf[cnt]=data;
            }
            my_ouput->addBinaryValue("wave",chaos::DataType::BinarySubtype::SUB_TYPE_DOUBLE,(const char*)buf,npoints*sizeof(double));
            
        } else {
            for(int cnt=0;cnt<npoints;cnt++){
                double data=sin(2*PI*freq*(delta*cnt)+phase)*amp;
                my_ouput->appendDoubleToArray(data);
                val.push_back(data);
            }
            my_ouput->finalizeArrayForKey("wave");
        }
    }
    my_input->addInt64Value("icounter64",(int64_t)0);
    my_input->addInt32Value("icounter32",0);
    my_input->addStringValue("istringa","hello input dataset");
    my_input->addDoubleValue("idoublevar",0.0);
    int tenpercent=loops/10;
    if(test.registerDataset()==0){
        LDBG_<<" registration OK";
        my_input->setValue("icounter64",(int64_t)18);
        my_input->setValue("icounter32",(int32_t)1970);
        my_input->setValue("idoublevar",(double)3.14);
        if(test.pushDataset(chaos::DataPackCommonKey::DPCK_DATASET_TYPE_INPUT)!=0){
            LERR_<<" cannot push:"<<my_input->getJSONString();
            reterr++;
        } else {
            LDBG_<<"pushing:"<<my_input->getJSONString();
        }
        uint64_t query_time_start=chaos::common::utility::TimingUtil::getTimeStamp();
        uint64_t end_time,start_time=chaos::common::utility::TimingUtil::getLocalTimeStampInMicroseconds();
        double avg;
        for(int cnt=0;cnt<loops;cnt++){
            my_ouput->setValue("counter64",(int64_t)2*cnt);
            my_ouput->setValue("counter32",(int32_t)(2*cnt+1));
            my_ouput->setValue("doublevar",(double)cnt);
            if(freqshift!=0){
                afreq+=freqshift;
                
            }
            
            if(ampshift> 0 || freqshift>0){
                aamp=amp;
                for(int cntt=0;cntt<npoints;cntt++){
                    if(ampshift>0){
                        aamp=aamp*ampshift;
                    }
                    double data=sin(2*PI*afreq*(delta*cntt)+phase)*aamp;
                    if(binary){
                        buf[cntt]=data;
                    } else {
                        val[cntt]=data;
                    }
                }
                if(binary){
                    my_ouput->setValue("wave",(const void*)buf);
                } else {
                    my_ouput->setValue("wave",val);
                }
            }
            
            // LDBG_<<"int32 value:"<<my_ouput->getInt32Value("counter32");
            if(test.pushDataset()!=0){
                LERR_<<" cannot push:"<<my_ouput->getJSONString();
                reterr++;
            }
            if(waitloops){
                usleep(waitloops);
            }
            if((cnt%tenpercent)==0){
                int cntt=cnt+1;
                end_time=chaos::common::utility::TimingUtil::getLocalTimeStampInMicroseconds();
                avg=cntt*1000000.0/(end_time-start_time);
                std::cout<<"\t Average time for:"<<cntt<<" loops is:"<<avg<<" push/s, tot us: "<<(end_time-start_time)<<std::endl;
                
            }
        }
        end_time=chaos::common::utility::TimingUtil::getLocalTimeStampInMicroseconds();
        uint64_t query_time_end=chaos::common::utility::TimingUtil::getTimeStamp();
        
        avg=loops*1000000.0/(end_time-start_time);
        std::cout<<test.getUid()<<": Average time for:"<<loops<<" is:"<<avg<<" push/s, tot us: "<<(end_time-start_time)<<std::endl;
        if(wait_retrive){
            std::cout<<"* waiting "<<wait_retrive<<" s before retrive data"<<std::endl;
            sleep(wait_retrive);
        }
        std::cout<<"* "<<test.getUid()<<" recovering data... from:"<<query_time_start<<" to:"<<query_time_end<<" runID:"<<test.getRunID()<<" pagelen:"<<pagelen<<std::endl;
        start_time=chaos::common::utility::TimingUtil::getLocalTimeStampInMicroseconds();
        uint32_t uid=test.queryHistoryDatasets(query_time_start,query_time_end,pagelen);
        uint32_t total=0;
        
        uint64_t pckmissing=0,pcktreplicated=0,pckmalformed=0,badid=0,pckt=0;
        
        if(pagelen==0){
            std::vector<ChaosDataSet> res=test.queryHistoryDatasets(query_time_start,query_time_end);
            end_time=chaos::common::utility::TimingUtil::getLocalTimeStampInMicroseconds();
            avg=res.size()*1000000.0/(end_time-start_time);
            printf("Retrived %.8lu items items/s:%.4f tot us:%.10llu\r",res.size(),avg,(end_time-start_time));
            total=res.size();
            reterr+=checkData(test,res,pckmissing,pckt,pcktreplicated,pckmalformed,badid);
        } else {
            
            while(test.queryHasNext(uid)){
                std::vector<ChaosDataSet> res=test.getNextPage(uid);
                total+=res.size();
                end_time=chaos::common::utility::TimingUtil::getLocalTimeStampInMicroseconds();
                avg=total*1000000.0/(end_time-start_time);
                printf("Retrived %.8lu items, total items %.8llu items/s:%.4f tot us:%.10llu\r",res.size(),total,avg,(end_time-start_time));
                reterr+=checkData(test,res,pckmissing,pckt,pcktreplicated,pckmalformed,badid);
                
            }
        }
        printf("\n");
        
        if(total!=loops){
            std::cout<<"# number of data retrived "<<total<<" different from expected:"<<loops<<std::endl;
            reterr++;
        }
        if(reterr!=0){
            std::cout<<"## Total errors:"<<reterr<<" missing packets:"<<pckmissing<<" replicated:"<<pcktreplicated<<" pcktmalformed:"<<pckmalformed<<" badrunid:"<<badid<<std::endl;
            
        }else {
            std::cout<<"check ok"<<std::endl;
            
        }
        
    } else {
        LERR_<<" cannot register!:";
        reterr++;
    }
    test.deinit();
    ChaosMetadataServiceClient::getInstance()->stop();
    ChaosMetadataServiceClient::getInstance()->deinit();
    return reterr;
}
