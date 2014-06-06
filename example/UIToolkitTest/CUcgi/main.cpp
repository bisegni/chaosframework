/* 
 * File:   main.cpp
 * Author: michelo
 *
 * Created on May 27, 2014, 2:03 PM
 */

#include <iostream>
#include <vector>
#include <string>

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#include <chaos/ui_toolkit/ChaosUIToolkit.h>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/ui_toolkit/ChaosUIToolkitCWrapper.h>
#include <chaos/ui_toolkit/HighLevelApi/HLDataApi.h>
#include <chaos/ui_toolkit/HighLevelApi/DeviceController.h>

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/ui_toolkit/ChaosUIToolkitCWrapper.h>

#include <stdio.h>
#include <stdlib.h>
#include "common/debug/debug.h"
using namespace std;
using namespace cgicc;     // Or reference as cgicc::Cgicc formData; below in object instantiation.
	 
#define DEVICE "simpower"
#define MDS "localhost:5000"
using namespace std;
using namespace std;
using namespace chaos;
using namespace chaos::ui;
using namespace boost;
/*
 
 */

 DeviceController *initDevice(const char* mds,const char* name,int timeout){
    char tmpInitString[256];
     DeviceController *controller = NULL;
    int err;
    static int toolkit_initialised=0;

    if(toolkit_initialised==0){
        
      //log-on-console=true\nlog-level=debug
      sprintf(tmpInitString, "metadata-server=%s\n", mds);
      err = initToolkit(tmpInitString);
      if (err != 0) {
	DPRINT("Error initToolkit %d\n", err);
	return 0;
      }
      toolkit_initialised = 1;
      DPRINT("Toolkit initialised \"%s\"\n",tmpInitString);
    }

    try{
            controller = HLDataApi::getInstance()->getControllerForDeviceID(name);
            if(controller) {
                int err=0;
                    //activate the traking
                controller->setupTracking();
                controller->setRequestTimeWaith(timeout);
                controller->initDevice();
                controller->startDevice();
            }
        } catch (CException& e) {
            DPRINT("eccezione %s\n",e.what());
            err = e.errorCode;
            return 0;
        }
   
    return controller;
  }

  int sendCmd(DeviceController *controller ,char*cmd,char*param){
      int err;
      uint64_t cmd_id_tmp;
      std::auto_ptr<chaos::common::data::CDataWrapper> data_wrapper;
      std::string cmd_alias_str = cmd;
      if(param) {
	data_wrapper.reset(new chaos::common::data::CDataWrapper());
          if(data_wrapper.get())
               data_wrapper->setSerializedJsonData(param);
          else
               return -1001;
        }
			
            err = controller->submitSlowControlCommand(cmd_alias_str,
                    static_cast<chaos::common::batch_command::SubmissionRuleType::SubmissionRule>(0),
								50,
								cmd_id_tmp,
                                                  0,
										0,
												  0,
												  data_wrapper.get());

    return err;
  }
  int fetchDataSet(DeviceController *ctrl,char*jsondest,int size){
      CDataWrapper* data;
      if(ctrl){
          ctrl->fetchCurrentDeviceValue();
      } else {
          return -2;
      }
      data = ctrl->getCurrentData();
      if(data==NULL)
          return -3;
      
      strcpy(jsondest,data->getJSONString().c_str());
     
      return 0;
    }
    
  int deinitDevice(DeviceController *ctrl){
      return ctrl->deinitDevice();
    
  }


int main(int argc, char** argv) {
    Cgicc form;
    char result[2048];
    form_iterator name =form.getElement("ON");
      // Send HTTP header
    cout << "Content-Type: text/plain\n\n";

      // Set up the HTML document
     if(name != form.getElements().end()) {
         cout << "Your name: " << **name << endl;
    }
    DeviceController* dev =initDevice(MDS,DEVICE,1000);
    if(dev){
        fetchDataSet(dev,result,sizeof(result));
        cout<<result<<endl;
    }
    
#if 0
    cout<<"{";
    for(int cnt=0;cnt<6;cnt++){
        cout<<"\""<<dataset[cnt]<<"\":"<<(rand()*1.0/RAND_MAX)*200;
        if(cnt<5)cout<<",";
    }
    cout<<"}"<<endl;
#endif
     // Close the HTML document
  //    cout << body() << html();
    return 0;
}

