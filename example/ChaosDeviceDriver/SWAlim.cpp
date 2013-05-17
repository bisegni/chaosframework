//
//  SWAlim.cpp
//  chaosDeviceDriver
//
//  Created by andrea michelotti on 4/30/13.
//  Copyright (c) 2013 andrea michelotti. All rights reserved.
//

#include "SWAlim.h"
#include "ModbusProtocol.h"

namespace chaos {

#define CMD_REGISTER 1
#define CURRENT_WRITE_REGISTER 2
#define CURRENT_READ_REGISTER 3
#define VOLTAGE_READ_REGISTER 4


    
  SWAlim::SWAlim(const char*name):device_proxy(name,""){
    //create and initialize attributes of this device
    setAttribute("address",std::string(""));
    setAttribute("port",std::string(""));
  }
  SWAlim::SWAlim(const char*name,const char* busid):device_proxy(name,busid){
        // TCP busid = <address>:port
        // RTU busid = <address>
        size_t found = m_busid.find(":");
        if(found != std::string::npos){
            // TCP
            std::string address = m_busid.substr(0,found);
            std::string port    =m_busid.substr(found+1);
            setAttribute("address",address);
            setAttribute("port",port);
            // set the type
            setUID(DeviceUid::DEVICE_SWALIM_TCP);
        } else {
            if(!m_busid.empty()){
                setAttribute("address", m_busid);
                setUID(DeviceUid::DEVICE_SWALIM_RTU);
            }
        }
    }
    int SWAlim::init(){
      std::string address,port;
      if(getAttribute("address",address)==false){
	DERR ("Address not set\n");
	return DevError::DEVICE_BAD_ADDRESS;
      }
      if(getAttribute("port",port)==false){
	setUID(DeviceUid::DEVICE_SWALIM_RTU);
      } else {
	setUID(DeviceUid::DEVICE_SWALIM_TCP);
      }
      m_busid = address + ":" + port;
      return device_proxy::init();
    }
    int SWAlim::deinit(){
        return device_proxy::deinit();
    }

    
    int SWAlim::command(int cmdid, int val){
        int reg=1;
        uint16_t v=0;
        ModbusProtocol * prt = getProtocol<ModbusProtocol>();
        
        if((cmdid == SET_ON) || (cmdid == SET_OFF)|| (cmdid == START) || (cmdid==MOVE_LVL)){
            v= cmdid;
            reg = 1;
        } else if(cmdid == SET_CURRENT){
            reg =2 ;
            v = val;
        }
        
        return prt->preset_multiple_registers(reg, &v, 1);
        
    }
    
    int SWAlim::readCurrent(int& curr){
        ModbusProtocol * prt = getProtocol<ModbusProtocol>();
        uint16_t buf;
        if(prt->read_input_registers(CURRENT_READ_REGISTER, &buf, 1)==1){
            curr = buf;
        } else {
            
            return 0;
        }
        return 1;
        
    }
    int SWAlim::readVoltage(int&volt){
        ModbusProtocol * prt = getProtocol<ModbusProtocol>();
        uint16_t buf;
        if(prt->read_input_registers(VOLTAGE_READ_REGISTER, &buf, 1)==1){
            volt = buf;
        } else {
            DERR ("Cannot read voltage\n");
            return 0;
        }
        return 1;

        
    }
    int SWAlim::on(){
        return command(SET_ON);
        
    }
    int SWAlim::off(){
        return command(SET_OFF);
    }
    int SWAlim::start(){
        return command(START);
    }
    int SWAlim::moveLevel(){
        return command(MOVE_LVL);
    }
    int SWAlim::setCurrent(int curr){
        return command(SET_CURRENT,curr);
    }


}; 
