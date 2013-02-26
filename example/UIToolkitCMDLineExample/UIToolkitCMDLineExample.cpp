/*	
 *	UIToolkitCMDLineExample.cpp
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
#include <iostream>
#include <string>
#include <vector>
#include <chaos/common/global.h>
#include <chaos/common/cconstants.h>
#include <chaos/common/network/CNodeNetworkAddress.h>
#include <chaos/ui_toolkit/ChaosUIToolkit.h>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/ui_toolkit/HighLevelApi/HLDataApi.h>
#include <stdio.h>
#include <chaos/common/bson/bson.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/regex.hpp>

/*! \page page_example_uiex1 ChaosUIToolkit Example
 \section page_example_uiex1_sec A basic usage for the ChaosUIToolkit package
 
 \subsection page_example_uiex1_sec_sub1 Toolkit usage
 As in the CUToolkit, a developer can add a custom startup param
 \snippet example/UIToolkitCMDLineExample/UIToolkitCMDLineExample.cpp UIToolkit Init
 
 this example show how initialize the UIToolkit, acquire a basic channel, used it and deinit the toolkit.
 The UIToolkit is create around singleton pattern and the channel object are self managed by toolkit. So the first thing
 is to initialize the toolkit internal engine:
 
 \snippet example/UIToolkitCMDLineExample/UIToolkitCMDLineExample.cpp UIToolkit Init
 
 the argc, argv parameter are the common c and cpp main param.
 
 For comunicate with the chaos resurces, as example the metadataserver, a channel is needed. To instantiate a mds channel
 must be used the LLRpcApi api package. To achieve channel creation can be used the singleton instance for calling the method "getNewMetadataServerChannel"
 \snippet example/UIToolkitCMDLineExample/UIToolkitCMDLineExample.cpp UIToolkit ChannelCreation
 The returned pointer to MDSMessageChannel is interally managed, there is no need to deallocate it by and. When a channel is no more neede, the
 deallocation api must be called.
 
 At this point a request can be sent to metadata server and we must wait for the answer. For get all active devices we can use the method "getAllDeviceID"
 that get as param a vector of string, that wil be filled with all device id found
 \snippet example/UIToolkitCMDLineExample/UIToolkitCMDLineExample.cpp Datapack sent
 
 When all operation are finisched the UIToolkit ca be deinitialized, this operation will clean all pendi operation and hannel deallocation
 \snippet example/UIToolkitCMDLineExample/UIToolkitCMDLineExample.cpp UIToolkit Deinit
 */
using namespace std;
using namespace chaos;
using namespace chaos::ui;
using namespace bson;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::date_time;


#define OPT_ITERATION "iteration"
#define OPT_SLEEP_TIME "dac_sleep_time"
#define OPT_DEVICE_ID "device_id"
/* syntax: -set-attribute=<name>:<value> */
#define OPT_SET_ATTRIBUTE_ID "set-attribute"
#define OPT_GET_ATTRIBUTE_ID "get-attribute"


inline ptime utcToLocalPTime(ptime utcPTime){
	c_local_adjustor<ptime> utcToLocalAdjustor;
	ptime t11 = utcToLocalAdjustor.utc_to_local(utcPTime);
	return t11;
}

int main (int argc, char* argv[] )
{
    try {
        int err = 0;
        int iteration = 10;
        uint64_t sleep = 1000000;
        istringstream optionStream;
        string devID;
        vector<string> setAttrsOpt;
        vector<string> getAttrs;
        map<string,string> setAttributes;
        map<string,PointerBuffer *> OutBufs;
        vector<string> allDevice;
        posix_time::time_duration currentTime;
        //DataBuffer *intValueBuff;
        DataBuffer *tsBuffer;
        //! [UIToolkit Attribute Init]
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_ITERATION, po::value<int>()->default_value(10), "Set the number of acquiring iteration");
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_SLEEP_TIME, po::value<uint64_t>()->default_value(1000000), "Set the number of microsecond between an acquisition and th eother");
    
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_SET_ATTRIBUTE_ID, po::value<vector<string> > (), "Set the initial attributes of a device");
      
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_GET_ATTRIBUTE_ID, po::value<vector<string> >(), "Trace the specified attributes of a device");
        
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_DEVICE_ID, po::value<string>(), "Specify the device ID");
        
        //! [UIToolkit Attribute Init]
        
        //! [UIToolkit Init]
        ChaosUIToolkit::getInstance()->init(argc, argv);
        if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_SET_ATTRIBUTE_ID))
            setAttrsOpt=ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<vector<string> >(OPT_SET_ATTRIBUTE_ID);
        
        if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_GET_ATTRIBUTE_ID))
            getAttrs=ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<vector<string> >(OPT_GET_ATTRIBUTE_ID);
       
        if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_DEVICE_ID))
            devID=ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<string>(OPT_DEVICE_ID);
        else {
            cerr<< "must specify a device id, for help:"<<argv[0]<<" --help"<<endl;
            return -1;
        }
        
        if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_ITERATION)){
            iteration = ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<int>(OPT_ITERATION);
        }
        
        if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_SLEEP_TIME)){
            sleep = ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<uint64_t>(OPT_SLEEP_TIME);
        }
        
        cout << "Query for:"<<devID<<endl;
        //! [UIToolkit ChannelCreation]
        CDeviceNetworkAddress deviceNetworkAddress;
        CUStateKey::ControlUnitState deviceState;
        //! [Datapack sent]
        DeviceController *controller = HLDataApi::getInstance()->getControllerForDeviceID(devID);
        if(!controller) {
             std::cout << "Error creating controller for:" << devID << std::endl;
        }
        
        regex expr("(\\w+):(\\w+)");
        vector<string> allInAttrName;
        controller->getDeviceDatasetAttributesName(allInAttrName, chaos::DataType::Input);
        
        for (vector<string>::iterator i=setAttrsOpt.begin(); i!=setAttrsOpt.end(); i++) {
            smatch m;
            if(regex_match(*i,m,expr)){
                string attribute=m[1];
                setAttributes[attribute]=m[2];
            }
        }
        
        
        //------------------------------------------------------------------------------------
        //List all attribute of dataset without use BSON
        vector<string> allOutAttrName;
        chaos::RangeValueInfo rangeInfo;
        // check the type of attribute
        chaos::DataType::DataSetAttributeIOAttribute direction;
        controller->getDeviceDatasetAttributesName(allOutAttrName, chaos::DataType::Output);
        for(vector<string>::iterator i=allOutAttrName.begin();i!=allOutAttrName.end();i++){
            controller->getDeviceAttributeDirection(*i, direction);
            controller->getDeviceAttributeRangeValueInfo(*i, rangeInfo);
            cout<<" attributes to track:\""<<*i<<"\""<<endl;
        }
       // intValue1Buff = controller->getBufferForAttribute(key);
        
      
       
        //------------------------------------------------------------------------------------
        /*
         get the state
         */
        err = controller->getState(deviceState);
        
        /*
         initialization of thedevice isntead form MDS
         */
        err = controller->initDevice();
        
        //---------------------------------------------------------------------------------------------------------
        //this step is to use internal UIToolkit buffer logic
        //it initializes the direction and type of attributes
        //it initializes and allocates the timestamp attribute
        controller->setupTracking();
        //
        
        tsBuffer = controller->getPtrBufferForTimestamp();
        if(tsBuffer==NULL){
            cerr<<"cannot allocate memory for timestamp"<<endl;
            //return -1;
        }
        std::cout << "Print all output attribute for the device to read:" << std::endl;
        for (int idx = 0; idx < allOutAttrName.size(); idx++) {
            if(std::find(getAttrs.begin(),getAttrs.end(),allOutAttrName[idx])!=getAttrs.end()){
                // if the direction and type are defined, it allocates buffers
                controller->addAttributeToTrack(allOutAttrName[idx]);
                // get the buffers
                PointerBuffer*tmp=controller->getPtrBufferForAttribute(allOutAttrName[idx]);
                if(tmp==NULL){
                    cerr<<"cannot allocate memory for attribute \""<<allOutAttrName[idx]<<"\""<<endl;
                    return -2;
                }
                
                std::cout << "OUT:"<<allOutAttrName[idx] << "(Tracking)"<<std::endl;
                OutBufs[allOutAttrName[idx]]=tmp;
                
            } else {
                std::cout << "OUT:"<<allOutAttrName[idx] <<std::endl;
                
            }
        }

        /*
         set the run schedule delay for the CU
         */
        err = controller->setScheduleDelay(1000000);

        //------------------------------------------------------------------------------------
        //send command for set attribute of dataset without use BSON
        
      
        for (int idx = 0; idx < allInAttrName.size(); idx++) {
            if(setAttributes.find(allInAttrName[idx])!=setAttributes.end()){
                string attributeValue=setAttributes[allInAttrName[idx]];
                err = controller->setAttributeValue(allInAttrName[idx], attributeValue);
                if(err!=ErrorCode::EC_NO_ERROR){
                    cerr<<"# Bad attribute \""<<allInAttrName[idx]<<"\" or value \""<<attributeValue<<"\", err="<<err<<endl;
                    return -1;
                }
                cout<<"IN:"<<allInAttrName[idx]<<"="<<attributeValue<<endl;
            } else {
                cout<<"IN:"<<allInAttrName[idx]<<endl;
            }
        }
        //------------------------------------------------------------------------------------
        
        
        /*
         Start the control unit
         */
        err = controller->startDevice();
        
        
        for (int idx = 0; idx < iteration; idx++) {
            controller->fetchCurrentDeviceValue();
            
            if(tsBuffer){
                int64_t *bPtr = static_cast<int64_t*>(tsBuffer->getBasePointer());
                int64_t *wPtr = static_cast<int64_t*>(tsBuffer->getWritePointer());
                int64_t *lastTimestamp = bPtr==wPtr?bPtr+tsBuffer->getDimension()-1:wPtr-1;
                if(lastTimestamp){
                    currentTime = boost::posix_time::milliseconds(*lastTimestamp);
                    std::cout << "Buffer received ts:" << utcToLocalPTime(EPOCH + currentTime) << std::endl;
                }
            }
            for(map<string,PointerBuffer*>::iterator i=OutBufs.begin();i!=OutBufs.end();i++){
            if(i->second){
                int32_t bufLen = 0;
                
                boost::shared_ptr<double> doublePtr = i->second->getTypedPtr<double>(bufLen);
                if(doublePtr.get()){
                    std::cout << "Buffer received len:" << bufLen << std::endl;
                    for(int32_t idx = 0; idx < bufLen; idx++){
                        std::cout << doublePtr.get()[idx];
                    }
                    
                    std::cout << std::endl;
                }
            }
            usleep(sleep);
        }
        }
        controller->stopTracking();
        //---------------------------------------------------------------------------------------------------------
        
        /*
         stopping of the device instead form MDS
         */
        controller->stopDevice();
        
        controller->getState(deviceState);
        std::cout << "state " << deviceState << std::endl;
        
        /*
         deinit of the device instead form MDS
         */
        controller->deinitDevice();
        
        controller->getState(deviceState);
        std::cout << "state " << deviceState << std::endl;
        
        //! [UIToolkit Deinit]
        ChaosUIToolkit::getInstance()->deinit();
        //! [UIToolkit Deinit]
    } catch (CException& e) {
        cerr<<"Exception::"<<endl;
        std::cerr<< "in:"<<e.errorDomain << std::endl;
        std::cerr<< "cause:"<<e.errorMessage << std::endl;
    } catch (program_options::error &e){
        cerr << "Unable to parse command line: " << e.what() << endl; 
    } catch (...){
        cerr << "unexpected exception caught.. " << endl;
    }
    return 0;
}
