//
//  BasicIODriverInterface.cpp
//
//  Created by andrea michelotti 
//  Copyright (c) 2014 andrea michelotti. All rights reserved.
//

#include <chaos/cu_toolkit/driver_manager/driver/BasicIODriverInterface.h>


#include <chaos/common/exception/exception.h>


using namespace chaos::cu::driver_manager::driver;
#define BasicIODriverLAPP_		INFO_LOG(BasicIODriverInterface)
#define BasicIODriverLDBG_		DBG_LOG(BasicIODriverInterface)
#define BasicIODriverLERR_		ERR_LOG(BasicIODriverInterface)

int BasicIODriverInterface::read(void *buffer,int addr,int bcount){
    
    int ret,ret2;
    message.parm[0] = addr;
    message.parm[1] = bcount;
    message.opcode =BasicIODriverOpcode_READ;
    message.resultDataLength=bcount;
    message.resultData = (void*)buffer;
    ret2=accessor->send(&message);
    ret=message.ret;
    BasicIODriverLDBG_<<"readChannel addr:"<<addr<<", count:"<<bcount<<",func ret:"<<ret<<",accessor ret "<<ret2;
    return ret;
    
}

int BasicIODriverInterface::iop(int operation,void*data,int sizeb){
    int ret,ret2;
    message.parm[0] = operation;
    message.parm[1] = sizeb;
    message.opcode =BasicIODriverOpcode_IOP;
    message.inputDataLength=sizeb;
    message.inputData = (void*)data;
    ret2=accessor->send(&message);
    ret=message.ret;
    BasicIODriverLDBG_<<"iop op:"<<operation<<", size:"<<sizeb<<",func ret:"<<ret<<",accessor ret "<<ret2;
    return ret;

}

int BasicIODriverInterface::write(void *buffer,int addr,int bcount){
    
    int ret,ret2;
    message.parm[0] = addr;
    message.parm[1] = bcount;
    message.opcode =BasicIODriverOpcode_WRITE;
    message.inputDataLength=bcount;
    message.inputData = (void*)buffer;
    ret2=accessor->send(&message);
    ret=message.ret;
    BasicIODriverLDBG_<<"writeChannel addr:"<<addr<<", count:"<<bcount<<",func ret:"<<ret<<",accessor ret "<<ret2;
    return ret;

}

int BasicIODriverInterface::initIO(void *buffer,int sizeb){
    int ret,ret2;
    message.parm[0] = sizeb;
    message.opcode =BasicIODriverOpcode_INIT;
    message.inputDataLength=sizeb;
    ret2=accessor->send(&message);
    ret=message.ret;
    BasicIODriverLDBG_<<"Init,func ret:"<<ret<<",accessor ret "<<ret2;
    return ret;


}

int BasicIODriverInterface::deinitIO(){
    int ret,ret2;
    
    message.opcode =BasicIODriverOpcode_DEINIT;
    message.inputDataLength=0;
    ret2=accessor->send(&message);
    ret=message.ret;
    BasicIODriverLDBG_<<"DeInit,func ret:"<<ret<<",accessor ret "<<ret2;
    return ret;
}

int BasicIODriverInterface::getDataset(ddDataSet_t*data,int sizen){
    int ret2;
    int size=0;
    message.resultData = (void*)data;
    message.resultDataLength = sizeof(ddDataSet_t)*sizen;
    message.opcode =BasicIODriverOpcode_GET_DATASET;
    message.inputDataLength=0;
    ret2=accessor->send(&message);
    size=message.ret;

    BasicIODriverLDBG_<<"getDataset,func ret:"<<size<<",accessor ret "<<ret2;
    return size;
   
}


int BasicIODriverInterface::getDatasetSize(){
  int size,ret2;
    message.resultData = 0;
    message.resultDataLength = 0;
    message.opcode =BasicIODriverOpcode_GET_DATASETSIZE;
    message.inputDataLength=0;
    ret2=accessor->send(&message);
    size=message.ret;

    BasicIODriverLDBG_<<"getDatasetSize,func ret:"<<size<<",accessor ret "<<ret2;
    return size;
    

}
