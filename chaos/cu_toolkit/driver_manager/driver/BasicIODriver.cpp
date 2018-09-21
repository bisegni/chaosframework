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

void BasicIODriver::driverInit(const char *initParameter)  {
  int ret;
  BasicIODriverLAPP_ << "Init driver:"<<initParameter;
  if ( (ret=initIO((void*)initParameter,(int)strlen(initParameter)))!=0){
    std::stringstream ss;
    ss<<"Cannot initialize driver,initialization string:\""<<initParameter<<"\"";
    throw chaos::CException(ret, ss.str(), __FUNCTION__);
  }
	
}

void BasicIODriver::driverDeinit()  {
	BasicIODriverLAPP_ << "Deinit driver";
    deinitIO();

}

//! Execute a command
MsgManagmentResultType::MsgManagmentResult BasicIODriver::execOpcode(DrvMsgPtr cmd) {
	MsgManagmentResultType::MsgManagmentResult result = MsgManagmentResultType::MMR_EXECUTED;
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
