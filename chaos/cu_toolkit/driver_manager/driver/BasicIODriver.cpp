/*
 *	BasicIODriver.cpp
 *	!CHAOS
 *	Created by Andrea Michelotti
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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
#include <chaos/cu_toolkit/driver_manager/driver/BasicIODriver.h>
#include <stdlib.h>
#include <string>

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#include <boost/lexical_cast.hpp>


#define BasicIODriverLAPP_		INFO_LOG(BasicIODriver)
#define BasicIODriverLDBG_		DBG_LOG(BasicIODriver)
#define BasicIODriverLERR_		ERR_LOG(BasicIODriver)
using namespace chaos::cu::driver_manager::driver;


//GET_PLUGIN_CLASS_DEFINITION
//we need to define the driver with alias version and a class that implement it
//default constructor definition
DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR(BasicIODriver) {
}


//default descrutcor
BasicIODriver::~BasicIODriver() {

}

void BasicIODriver::driverInit(const char *initParameter) throw(chaos::CException) {
  int ret;
  BasicIODriverLAPP_ << "Init driver:"<<initParameter;
  if ( (ret=initIO((void*)initParameter,(int)strlen(initParameter)))!=0){
    std::stringstream ss;
    ss<<"Cannot initialize driver,initialization string:\""<<initParameter<<"\"";
    throw chaos::CException(ret, ss.str(), __FUNCTION__);
  }
	
}

void BasicIODriver::driverDeinit() throw(chaos::CException) {
	BasicIODriverLAPP_ << "Deinit driver";
    deinitIO();

}

//! Execute a command
cu_driver::MsgManagmentResultType::MsgManagmentResult BasicIODriver::execOpcode(cu_driver::DrvMsgPtr cmd) {
	cu_driver::MsgManagmentResultType::MsgManagmentResult result = cu_driver::MsgManagmentResultType::MMR_EXECUTED;
	switch(cmd->opcode) {
		case BasicIODriverOpcode_READ:{
                    int ch=cmd->parm[0];
                    int sizeb=cmd->parm[1];
                    cmd->resultDataLength= sizeb;
                    BasicIODriverLDBG_<<"Read add:"<<ch<<" size:"<<sizeb;
                    cmd->ret=read(cmd->resultData,ch,sizeb);
                    break;
                }

		case BasicIODriverOpcode_WRITE:{
                    int ch=cmd->parm[0];
                    int sizeb=cmd->parm[1];

                    cmd->inputDataLength= sizeb;
                    BasicIODriverLDBG_<<"Write add:"<<ch<<" size:"<<sizeb;
                    cmd->ret=write(cmd->inputData,ch,sizeb);
                    break;
                }
                
                case BasicIODriverOpcode_IOP:{
                    int op=cmd->parm[0];
                    int sizeb=cmd->parm[1];

                    cmd->inputDataLength= sizeb;
                    BasicIODriverLDBG_<<"iop :"<<op<<" size:"<<sizeb;
                    cmd->ret=iop(op,cmd->inputData,sizeb);
                    break;
                }
                
                case BasicIODriverOpcode_INIT:{
                    BasicIODriverLDBG_<<"Init";
                    int sizeb=cmd->parm[0];
                    cmd->inputDataLength=sizeb;
                    cmd->ret=initIO(cmd->inputData,sizeb);
                    break;
                }
                case BasicIODriverOpcode_DEINIT:{
                    BasicIODriverLDBG_<<"DeInit";

                    cmd->ret=deinitIO();
                    break;
                }
                case BasicIODriverOpcode_GET_DATASET:{
                    ddDataSet_t*dataset=(ddDataSet_t*)cmd->resultData;

                    cmd->ret = getDataset(dataset,cmd->resultDataLength);
                }
                break;
                case BasicIODriverOpcode_GET_DATASETSIZE:{
                    cmd->ret = getDatasetSize();

                }
                break;
	}
	return result;
}

int BasicIODriver::getDatasetSize(){
  return datasetSize;
}
int BasicIODriver::getDataset(ddDataSet_t*data,int sizeb){
    int ret=sizeb<datasetSize?sizeb:getDatasetSize();
    memcpy(data,dataset,ret);
    return ret;

}
void BasicIODriver::setDataSet(ddDataSet_t*data,int sizeb){
    dataset = data;
    datasetSize=sizeb;
}
