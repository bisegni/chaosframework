//
//  ModbusProtocol.h
//  chaosDeviceDriver
//
//  Created by andrea michelotti on 4/24/13.
//  Copyright (c) 2013 andrea michelotti. All rights reserved.
//

#ifndef __chaosDeviceDriver__ModbusProtocol__
#define __chaosDeviceDriver__ModbusProtocol__

#include <iostream>
#include "ProtocolDriver.h"
#include "stdint.h"
namespace chaos{
    namespace ModBus {
    enum  ModBusState{
        MODBUS_NONE,
        MODBUS_CONNECTED
    };
    };
    class ModbusProtocol:public ProtocolDriver{
        
        static int transaction_id;
    private:
        long CalculateCRC (unsigned char *frame,short frame_length,unsigned char *crc);
        long SendFrame (unsigned char *frame, short frame_length, long *error);
        long SendRTUFrame (unsigned char *frame, short frame_length, long *error);
        long SendTCPFrame (unsigned char *frame, short frame_length, long *error);
        long GetResponse (unsigned char *frame, short frame_length, long *error);
        long GetRTUResponse (unsigned char *frame, short frame_length, long *error);
        long GetTCPResponse (unsigned char *frame, short frame_length, long *error);
        long SendGet (unsigned char *frame,short frame_length,unsigned char *response,short response_length,long *error);
        int read_registers(int code, uint16_t add,uint16_t*buf,uint16_t len16);
        
        int write_registers(int code, uint16_t add,uint16_t*buf,uint16_t len16);
        void dumpRequestHeader(unsigned char*frame);
        ModBus::ModBusState m_state;
        int address;
    public:
        ModbusProtocol(CommunicationChannel*comm);
        ModbusProtocol(const char* name,CommunicationChannel*comm);
        ModbusProtocol(const char* name);
        char *GetErrorMessage(long code);
        
        /**
         Protocol driver interface
         */
        
        /**
         *	Command force_single_coil related method
         *	Description: Write single coil (digital I/O) state.
         *
         *	@param argin coil address,
         *  @param value 0/1
         *	@returns 
         */

        void force_single_coil(uint16_t argin,uint8_t value);
        
        /**
         *	Command ReadCoilStatus related method
         *	Description: Read coil (digital I/O) status.
         *
         *	@param argin coil address
         *  @param status returned status
         *	@returns 0 on success
         */
        int read_coil_status(uint16_t argin,uint8_t*status);

        /**
         *	Command read_input_status related method
         *	Description: Read discrete input status. Return one boolean per array element.
         *
         *	@param add input address
         *  @param buf returning buffer
         *  @param len16 no. of inputs
         *	@returns 0 on success
         */
        int read_input_status(uint16_t add, uint16_t*buf,uint16_t len16);
        
        /**
         *	Command read_holding_register related method
         *	Description: Read multiple 16bits registers.
         *
         *	@param add register address
         *  @param buf returning buffer
         *  @param len16 no. of registers
         *	@returns 0 on success
         */
        int read_holding_registers(uint16_t add, uint16_t*buf,uint16_t len16);

        /**
         *	Command read_input_registers related method
         *	Description: Read Multiple 16bits input registers.
         *
         *	@param add register address
         *  @param buf returning buffer
         *  @param len16 no. of registers
         *	@returns 0 on success
         */
        int read_input_registers(uint16_t add, uint16_t*buf,uint16_t len16);
       
        /**
         *	Command preset_single_register related method
         *	Description: Write single 16bits register.
         *
         *	@param add register address
         *  @param val value
         *	@returns 0 on success
         */
        int preset_single_register(uint16_t add, uint16_t val);
        
        /**
         *	Command read_exception_status related method
         *	Description: Read exception status (usually a predefined range of 8 bits
         *
         *	@param val returned value
         *	@returns 0 on success
         */
        int read_exception_status(uint16_t*val);
        
        /**
         *	Command fetch_comm_event_ctr related method
         *	Description: Fetch communications event counter.
         *
         *	@param status return status
          *	@param eventcnt return event counter
         *	@returns 0 on success
         */
        int fetch_comm_event_ctr(uint16_t*status,uint16_t*eventcnt);
        /**
         *	Command force_multiple_coils related method
         *	Description: Write multiple coils (digital I/O) state.
         *	@param add coil address
         *  @param buf returning buffer
         *  @param len16 no. of coils
         *	@returns 0 on success
         */
        int force_multiple_coils(uint16_t add, uint8_t*buf,uint16_t len16);
        
        /**
         *	Command read_multiple_coils related method
         *	Description: Read multiple coil (digital I/O) status.
         *	@param add register address
         *  @param buf returning buffer
         *  @param len16 no. of coils
         *	@returns 0 on success
         */
        int read_multiple_coils(uint16_t add, uint8_t*buf,uint16_t len16);
     
        /**
         *	Command PresetMultipleRegisters related method
         *	Description: Write multiple 16bits registers.
         *	@param add register address
         *  @param buf returning buffer
         *  @param len16 no. of coils
         *	@returns 0 on success
         */
        int preset_multiple_registers(uint16_t add, uint16_t*buf,uint16_t len16);
       
        /**
         *	Command mask_write_register related method
         *	Description: Mask write a 16bits register.
         *
         *	@param add register address
         *  @param andmask AND mask
         *  @param ormask OR mask
         *	@returns 0 on success
         */
        
        int mask_write_register(uint16_t add,uint16_t andmask,uint16_t ormask);
        /**
         *	Command read_write_register related method
         *	Description: Read and Write multiple 16bits registers.
         *
         *	@param radd register read address
         *  @param rbuf return read buffer
         *  @param rlen16 registers to read
         *	@param wadd register write address
         *  @param wbuf write buffer
         *  @param wlen16 registers to write
         *	@returns 0 on success
         */
        int read_write_register(uint16_t radd, uint16_t*rbuf,uint16_t rlen16,uint16_t wadd, uint16_t*wbuf,uint16_t wlen16);
        
        
        size_t read(basichw*,uint32_t add,char*buf, uint32_t sizeb,int timeo =0,uint32_t flags=0);

        size_t write(basichw*,uint32_t add,char*buf, uint32_t sizeb,int timeo=0,uint32_t flags=0);

        
        /**
            HW/SW inerithed
         */
        
        int init();
        int deinit();
        int reset();

    };
};
#endif /* defined(__chaosDeviceDriver__ModbusProtocol__) */
