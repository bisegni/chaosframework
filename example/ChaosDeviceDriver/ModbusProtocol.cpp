//
//  ModbusProtocol.cpp
//  chaosDeviceDriver
//
//  Created by andrea michelotti on 4/24/13.
//  Copyright (c) 2013 andrea michelotti. All rights reserved.
//

#include "ModbusProtocol.h"
#include "CommunicationChannel.h"
#define READ_COIL_STATUS                        1
#define READ_INPUT_STATUS                       2
#define READ_HOLDING_REGISTERS                  3
#define READ_INPUT_REGISTERS                    4
#define FORCE_SINGLE_COIL                       5
#define PRESET_SINGLE_REGISTER                  6
#define READ_EXCEPTION_STATUS                   7
#define FETCH_COMM_EVENT_CTR                    11
#define FETCH_COMM_EVENT_LOG                    12
#define FORCE_MULTIPLE_COILS                    15
#define PRESET_MULTIPLE_REGISTERS               16
#define REPORT_SLAVE_ID                         17
#define READ_GENERAL_REFERENCE                  20
#define WRITE_GENERAL_REFERENCE                 21
#define MASK_WRITE_REGISTER                     22
#define READ_WRITE_REGISTERS                    23
#define READ_FIFO_QUEUE                         24

#define MODBUS_ERR_SendTCPFrame                 -2
#define MODBUS_ERR_GetRTUResponse_1             -3
#define MODBUS_ERR_GetRTUResponse_2             -4
#define MODBUS_ERR_GetRTUResponse_3             -5
#define MODBUS_ERR_GetRTUResponse_4             -6
#define MODBUS_ERR_GetRTUResponse_5             -7
#define MODBUS_ERR_GetRTUResponse_6             -8
#define MODBUS_ERR_GetRTUResponse_8             -9
#define MODBUS_ERR_GetRTUResponse_9             -10
#define MODBUS_ERR_GetRTUResponse_CRC           -11
#define MODBUS_ERR_GetTCPResponse_1             -33
#define MODBUS_ERR_GetTCPResponse_2             -34
#define MODBUS_ERR_GetTCPResponse_3             -35
#define MODBUS_ERR_GetTCPResponse_4             -36
#define MODBUS_ERR_GetTCPResponse_5             -37
#define MODBUS_ERR_GetTCPResponse_6             -38
#define MODBUS_ERR_GetTCPResponse_8             -39
#define MODBUS_ERR_GetTCPResponse_9             -40
#define MODBUS_ERR_GetTCPResponse_Resp          -41
#define MODBUS_ERR_GetTCPResponse_TO            -42
#define MODBUS_ERR_GetTCPResponse_Select        -43
#define MODBUS_ERR_GetTCPResponse_Recv          -44

#define OK		0
#define NOTOK		(-1)

namespace chaos {
    int ModbusProtocol::transaction_id=1;
    
    ModbusProtocol::ModbusProtocol(CommunicationChannel*comm):ProtocolDriver("Modbus driver",ProtocolDriverUid::NO_PROTOCOL,comm){
        m_state = ModBus::MODBUS_NONE;
        address = -1;
    
    }
    ModbusProtocol::ModbusProtocol(const char* name,CommunicationChannel*comm):ProtocolDriver(name,ProtocolDriverUid::NO_PROTOCOL,comm){
        m_state = ModBus::MODBUS_NONE;
        address = -1;

    }
    
    ModbusProtocol::ModbusProtocol(const char* name):ProtocolDriver(name){
        m_state = ModBus::MODBUS_NONE;
        address = -1;

    }
    
    long ModbusProtocol::CalculateCRC (unsigned char *frame, short frame_length, unsigned char *crc){
        /* Table of CRC values for high-order byte */
        
        static unsigned char auchCRCHi[] = {
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
            0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
            0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
            0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
            0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
            0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
            0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
            0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
            0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
            0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
            0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
            0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
            0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
        } ;
        
        /* Table of CRC values for low-order byte */
        
        static unsigned char auchCRCLo[] = {
            0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
            0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
            0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
            0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
            0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
            0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
            0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
            0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
            0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
            0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
            0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
            0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
            0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
            0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
            0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
            0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
            0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
            0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
            0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
            0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
            0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
            0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
            0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
            0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
            0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
            0x43, 0x83, 0x41, 0x81, 0x80, 0x40
        } ;
        unsigned char uchCRCHi = 0xFF ; /* high CRC byte initialized */
        unsigned char uchCRCLo = 0xFF ; /* low CRC byte initialized  */
        unsigned uIndex ;               /* will index into CRC lookup*/
        /* table */
        
        while (frame_length--)             /* pass through message buffer */
        {
            uIndex = uchCRCHi ^ *frame++ ;        /* calculate the CRC */
            uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;
            uchCRCLo = auchCRCLo[uIndex] ;
        }
        
        crc[0] = uchCRCHi;
        crc[1] = uchCRCLo;
        
        return (OK) ;
    }
    
    
    long ModbusProtocol::SendFrame (unsigned char *query, short query_length, long *error){
        
        if (m_uid == ProtocolDriverUid::MODBUS_RTU_PROTOCOL)
        {
            return(SendRTUFrame(query, query_length, error));
        }
        
        if (m_uid == ProtocolDriverUid::MODBUS_TCP_PROTOCOL)
        {
            return(SendTCPFrame(query, query_length, error));
        }
        
        return(NOTOK);
    }


    long ModbusProtocol::SendRTUFrame (unsigned char *query, short query_length, long *error){
        unsigned char frame[1024], crc[2];
        int iframe, i;
        
        iframe=0;
        
        frame[iframe++] = address;
        for (i=0; i<query_length; i++)
        {
            frame[iframe++] = query[i];
        }
        CalculateCRC(frame, query_length+1, crc);
        frame[iframe++] = crc[0];
        frame[iframe++] = crc[1];
        
        
        if (m_bus->write(m_dev,(char*)frame, iframe)!=iframe)
            return(NOTOK);
        
        return(OK);
    }

    //+=====================================================================
    // Function:    ModbusCore::SendTCPFrame()
    //
    // Description:	Send a Modbus frame to a node using the TCP protocol
    //
    // Arg(s) In:	unsigned char function - function code
    //		short * data - data to send
    //		short length - number of data words to send
    //
    // Arg(s) Out:	none
    //-=====================================================================
    
    long ModbusProtocol::SendTCPFrame (unsigned char *query, short query_length, long *error)
    {
        unsigned char frame[1024];
        long i,j;
        
        
        // transaction id
        frame[0] =(transaction_id>>8)&0xff;
        frame[1] = transaction_id&0xff;
        // protocol
        frame[2] = 0;
        frame[3] = 0;
        // length
        frame[4] = ((query_length+1)>>8)&0xff;
        frame[5] = (query_length+1)&0xff;

        // slave id
        frame[6] = 0xff;
        
        for (i=0,j=7; i<query_length; i++,j++){
            frame[j] = query[i];
        }
        DPRINT("query len:%d, trx id =%d, function %d addr:%d num bytes %d\n",(frame[4]<<8) | frame[5],(frame[0]<<8) | frame[1] ,frame[7],(frame[8]<<8)|frame[9],(frame[10]<<8)|frame[11]);
#ifdef DEBUG
        dumpRequestHeader(frame);
#endif
        if(m_bus->write(m_dev, (char*)frame, query_length+7)!=query_length+7){
            *error = MODBUS_ERR_SendTCPFrame;
           
            return NOTOK;
        }
        transaction_id++;
        return(OK);
    }
    //+=====================================================================
    // Function:    ModbusCore::GetResponse()
    //
    // Description:	Get response from Modbus node via serial line (RTU/ASCII)
    //		or TCP/IP
    //
    // Arg(s) In:	short * data - data to send
    //		short length - number of data words to send
    //
    // Arg(s) Out:	none
    //-=====================================================================
    
    long ModbusProtocol::GetResponse (unsigned char *response, short response_length, long *error)
    {
        if (m_uid == ProtocolDriverUid::MODBUS_RTU_PROTOCOL)
        {
            return(GetRTUResponse(response, response_length, error));
        }
        
        if (m_uid == ProtocolDriverUid::MODBUS_TCP_PROTOCOL)
        {
            return(GetTCPResponse(response, response_length, error));
        }
        
        return(NOTOK);
    }
    
    
    void ModbusProtocol::dumpRequestHeader(unsigned char*frame){
        unsigned trx_id=(frame[0]>>8) | frame[1];
        unsigned prot_id=(frame[2]>>8) | frame[3];
        unsigned len = (frame[4]>>8) | frame[5];
        unsigned unit_id = frame[6];
        unsigned func_code = frame[7];
        unsigned start_addr = frame[8]>>8 | frame[9];
        unsigned num_regs   = frame[10]>>8 | frame[11];
        
        std::cout<<"Transaction ID:"<<trx_id<<std::endl;
        std::cout<<"Protocol ID:"<<prot_id<<std::endl;
        std::cout<<"Len:"<<len<<std::endl;
        std::cout<<"unit ID:"<<unit_id<<std::endl;
        std::cout<<"function code:"<<func_code<<std::endl;
        std::cout<<"start addr:"<<start_addr<<std::endl;
        std::cout<<"num registers:"<<num_regs<<std::endl;
        
        
    }

    //+=====================================================================
    // Function:    ModbusCore::GetRTUResponse()
    //
    // Description:	Get response from Modbus node via serial line using the
    //		RTU (binary) protocol
    //
    // Arg(s) In:	short * data - data to send
    //		short length - number of data words to send
    //
    // Arg(s) Out:	none
    //-=====================================================================
    
    long ModbusProtocol::GetRTUResponse (unsigned char *response, short response_length, long *error)
    {
        unsigned char frame[1024], crc[2];
        int ncharexp, i;
        size_t nchar;
    	ncharexp = 2;
        if((nchar=m_bus->read(m_dev,(char*)frame,2))!=2){
            return NOTOK;
        }
        
        if (frame[1] & 0x80)
        {
           
            if((nchar=m_bus->read(m_dev,(char*)frame,3))!=3){
                return NOTOK;
            }
            
            switch (frame[0])
            {
                case (1) :
                    *error = MODBUS_ERR_GetRTUResponse_1;
                    break;
                    
                case (2) :
                    *error = MODBUS_ERR_GetRTUResponse_2;
                    break;
                    
                case (3) :
                    *error = MODBUS_ERR_GetRTUResponse_3;
                    break;
                    
                case (4) :
                    *error = MODBUS_ERR_GetRTUResponse_4;
                    break;
                    
                case (5) :
                    *error = MODBUS_ERR_GetRTUResponse_5;
                    break;
                    
                case (6) :
                    *error = MODBUS_ERR_GetRTUResponse_6;
                    break;
                    
                case (8) :
                    *error = MODBUS_ERR_GetRTUResponse_8;
                    break;
                    
                default :
                    *error = MODBUS_ERR_GetRTUResponse_9;
                    break;
            }
            return(NOTOK);
            
        }
        
        response[0] = frame[1];
        /* function code echoed correctly, read rest of response */
        
        ncharexp = (response_length+1);
        if((nchar=m_bus->read(m_dev,(char*)frame,ncharexp))!=ncharexp){
            return NOTOK;
        }
        
   
        CalculateCRC(frame, nchar, crc);

        DPRINT("returned %d chars, crc 0x%x\n",nchar,crc);
        
        if ((crc[0] != frame[nchar]) && (crc[1] != frame[nchar+1]))
        {
            *error = MODBUS_ERR_GetRTUResponse_CRC;
            return(NOTOK);
        }
        
        for (i=1; i<response_length; i++)
        {
            response[i] = frame[i+1];
        }
        
        return(OK);
    }
    
    //+=====================================================================
    // Function:    ModbusCore::GetTCPResponse()
    //
    // Description:	Get response from Modbus node via ethernet using the
    //		TCP protocol
    //
    // Arg(s) In:	short * data - data to send
    //		short length - number of data words to send
    //
    // Arg(s) Out:	none
    //-=====================================================================
    
    long ModbusProtocol::GetTCPResponse (unsigned char *response, short response_length, long *error)
    {
        unsigned char frame[1024];
        long status=1, i;
        DPRINT("reading response of %d bytes\n",response_length);
        if(m_bus->read(m_dev,(char*)frame,7+response_length)!=7+response_length){
            status = 0;
        }
       
        if (status == 0)
        {
            *error = MODBUS_ERR_GetTCPResponse_Recv;
            return(NOTOK);
        }
        
        if (frame[7] & 0x80)
        {
            switch (frame[8])
            {
                case (1) :
                    *error = MODBUS_ERR_GetTCPResponse_1;
                    break;
                    
                case (2) :
                    *error = MODBUS_ERR_GetTCPResponse_2;
                    break;
                    
                case (3) :
                    *error = MODBUS_ERR_GetTCPResponse_3;
                    break;
                    
                case (4) :
                    *error = MODBUS_ERR_GetTCPResponse_4;
                    break;
                    
                case (5) :
                    *error = MODBUS_ERR_GetTCPResponse_5;
                    break;
                    
                case (6) :
                    *error = MODBUS_ERR_GetTCPResponse_6;
                    break;
                    
                case (8) :
                    *error = MODBUS_ERR_GetTCPResponse_8;
                    break;
                    
                default :
                    *error = MODBUS_ERR_GetTCPResponse_9;
                    break;
            }
            return(NOTOK);
            
        }
        
        for (i=0; i<response_length+1; i++)
        {
            response[i] = frame[i+7];
        }
        
        return(OK);
    }
    
    
    //+=====================================================================
    // Function:    ModbusCore::SendGet()
    //
    // Description:	Send a Modbus frame and get its answer
    //
    // Arg(s) In:	short * data - data to send
    //		short length - number of data words to send
    //
    // Arg(s) Out:	none
    //-=====================================================================
    
    long ModbusProtocol::SendGet (
                              unsigned char *query,
                              short         query_length,
                              unsigned char *response,
                              short         response_length,
                              long          *error)
    {
        long status;
        
        {
            // For the serial lne protocol, protect against
            // unsynchronized writing and reading
            
                 
            status = SendFrame(query, query_length, error);
            if (status != OK) {
                    return(NOTOK);
            }
            
            status = GetResponse(response, response_length, error);
            if (status != OK){
                return(NOTOK);
            }
        }
        
        return(OK);
    }
    
    
    //+=====================================================================
    // Function:    ModbusCore::GetErrorMessage()
    //
    // Description:	Returns an error string for the given error code.
    //              Note: Returns a handle to a static reference so the
    //              returned string must not be freed.
    //
    // Arg(s) In:	long code - Error code
    //
    // Arg(s) Out:	none
    //-=====================================================================
    
    char *ModbusProtocol::GetErrorMessage(long code) {
        
        static char ret_str[1024];
        sprintf(ret_str,"Unknown error code:%ld",code);
        
        switch(code)
        {
            case MODBUS_ERR_GetRTUResponse_1:
            case MODBUS_ERR_GetRTUResponse_2:
            case MODBUS_ERR_GetRTUResponse_3:
            case MODBUS_ERR_GetRTUResponse_4:
            case MODBUS_ERR_GetRTUResponse_5:
            case MODBUS_ERR_GetRTUResponse_6:
            case MODBUS_ERR_GetRTUResponse_8:
            case MODBUS_ERR_GetRTUResponse_9:
                strcpy(ret_str,"ModbusCore::GetRTUResponse(): ");
                break;
            case MODBUS_ERR_GetTCPResponse_1:
            case MODBUS_ERR_GetTCPResponse_2:
            case MODBUS_ERR_GetTCPResponse_3:
            case MODBUS_ERR_GetTCPResponse_4:
            case MODBUS_ERR_GetTCPResponse_5:
            case MODBUS_ERR_GetTCPResponse_6:
            case MODBUS_ERR_GetTCPResponse_8:
            case MODBUS_ERR_GetTCPResponse_9:
                strcpy(ret_str,"ModbusCore::GetTCPResponse(): ");
                break;
        }
        
        switch(code)
        {
            case MODBUS_ERR_SendTCPFrame:
                strcpy(ret_str,"ModbusCore::SendTCPFrame(): failed to send frame to ");
                strcat(ret_str,"node using TCP protocol ");
                strcat(ret_str,"(hint: check the network cable)");
                break;
                
                
            case MODBUS_ERR_GetTCPResponse_1:
            case MODBUS_ERR_GetRTUResponse_1:
                strcat(ret_str,"error getting response, ");
                strcat(ret_str,"illegal function (hint: contact the programmer)!");
                break;
                
            case MODBUS_ERR_GetTCPResponse_2:
            case MODBUS_ERR_GetRTUResponse_2:
                strcat(ret_str,"error getting response, ");
                strcat(ret_str,"illegal data address (hint: check the module address)");
                break;
                
            case MODBUS_ERR_GetTCPResponse_3:
            case MODBUS_ERR_GetRTUResponse_3:
                strcat(ret_str,"error getting response, ");
                strcat(ret_str,"illegal data value (hint: check the module data)");
                break;
                
            case MODBUS_ERR_GetTCPResponse_4:
            case MODBUS_ERR_GetRTUResponse_4:
                strcat(ret_str,"error getting response, ");
                strcat(ret_str,"slave device failure (hint:check the module hardware)");
                break;
                
            case MODBUS_ERR_GetTCPResponse_5:
            case MODBUS_ERR_GetRTUResponse_5:
                strcat(ret_str,"error getting response, ");
                strcat(ret_str,"answer will take some time");
                strcat(ret_str,"(hint: poll the node to get the answer)");
                break;
                
            case MODBUS_ERR_GetTCPResponse_6:
            case MODBUS_ERR_GetRTUResponse_6:
                strcat(ret_str,"slave is busy, ");
                strcat(ret_str,"cannot process request ");
                strcat(ret_str,"(hint: wait and then retransmit request)");
                break;
                
            case MODBUS_ERR_GetTCPResponse_8:
            case MODBUS_ERR_GetRTUResponse_8:
                strcat(ret_str,"memory parity error while reading ");
                strcat(ret_str,"extended memory (hint: check the node hardware)");
                break;
                
            case MODBUS_ERR_GetTCPResponse_9:
            case MODBUS_ERR_GetRTUResponse_9:
                strcat(ret_str,"unknown exception code ");
                strcat(ret_str,"(hint: check the node hardware)");
                break;
                
            case MODBUS_ERR_GetRTUResponse_CRC:
                strcpy(ret_str,"ModbusCore::GetRTUResponse(): failed to get response ");
                strcat(ret_str,"from node, error in CRC");
                break;
                
            case MODBUS_ERR_GetTCPResponse_Resp:
                strcpy(ret_str,"ModbusCore::GetTCPResponse(): failed to get TCP ");
                strcat(ret_str,"response from node ");
                break;
                
            case MODBUS_ERR_GetTCPResponse_TO:
                strcpy(ret_str,"ModbusCore::GetTCPResponse(): failed to get TCP ");
                strcat(ret_str,"response from node (timeout > ");
                
                break;
                
            case MODBUS_ERR_GetTCPResponse_Select:
                strcpy(ret_str,"ModbusCore::GetTCPResponse(): failed to get TCP ");
                strcat(ret_str,"response from node (select() returned not for us)");
                break;
                
            case MODBUS_ERR_GetTCPResponse_Recv:
                strcpy(ret_str,"ModbusCore::GetTCPResponse(): failed to get TCP ");
                strcat(ret_str,"response from node, unexpected close of connection ");
                strcat(ret_str,"at remote end");
                break;
                
        }
        
        return ret_str;
        
    }

    /*******************/
    int ModbusProtocol::read_registers(int code, uint16_t add,uint16_t*buf,uint16_t len16){
        long error;
        short register_address, no_registers, no_bytes;
        unsigned char query[5], response[1024];
        int nb_reg_to_get;  // The number of registers to be read at next call.
        
#define MAX_NB_REG 120
        
        //---------------------------------------
        
    	int index = 0;
        no_registers = len16;
        register_address = add;
        
    	do  // Do as many readouts as required.
      	{
            
			if(no_registers > MAX_NB_REG)  // Check for limit.
	  			nb_reg_to_get = MAX_NB_REG;
			else
                nb_reg_to_get = no_registers;
            
            DPRINT("register address %d nb_reg_to_get %d\n",register_address,nb_reg_to_get);
            
			query[0] = code;
			query[1] = register_address >> 8;
			query[2] = register_address & 0xff;
			query[3] = nb_reg_to_get >> 8;
			query[4] = nb_reg_to_get & 0xff;
            
			no_bytes = nb_reg_to_get * 2;
            
			if(SendGet(query,5,response,no_bytes+2,&error) != OK)
	  		{
	  			return nb_reg_to_get - no_registers;
	  		}
            
			for (int i=0; i < nb_reg_to_get; i++, index++)  // Copy received data to argout
	  			buf[index] = (response[i*2+2] << 8) + response[i*2+3];
            
			register_address += nb_reg_to_get; // Update address pointer.
			no_registers -= nb_reg_to_get;     // Update remaining data to be read.
      	}
    	while(no_registers > 0);  // Still some registers to be read?
        
	    return nb_reg_to_get - no_registers;;
    }
    /******************/
    
    /*******************/
    int ModbusProtocol::write_registers(int code, uint16_t add,uint16_t*buf,uint16_t len16){
        long error;
        short register_address, no_registers, no_bytes;
        unsigned char query[1024], response[1024];
        int nb_reg_to_get;  // The number of registers to be read at next call.
        
#define MAX_NB_REG 120
        
        //---------------------------------------
        
    	int i;
        no_registers = len16;
        register_address = add;
        
        
        if(no_registers > MAX_NB_REG)  // Check for limit.
            nb_reg_to_get = MAX_NB_REG;
        else
            nb_reg_to_get = no_registers;
        
        DPRINT("register address %d nb_reg_to_write %d\n",register_address,nb_reg_to_get);
        
        query[0] = code;
        query[1] = register_address >> 8;
        query[2] = register_address & 0xff;
        query[3] = nb_reg_to_get >> 8;
        query[4] = nb_reg_to_get & 0xff;
        query[5] = no_registers * 2;
        no_bytes = 6;
        
        
        for (i=0; i<no_registers; i++)
        {
            query[6+(i*2)]   = (buf[i]) >> 8;
            no_bytes++;
            query[6+(i*2)+1] = (buf[i]) & 0xff;
            no_bytes++;
        }
        
        if(SendGet(query,no_bytes,response,5,&error) != OK) {
            return i;
        }
        
        
	    return 0;
    }
    /******************/
    void ModbusProtocol::force_single_coil(uint16_t argin,uint8_t val){
    }
    
    /**
     *	Command ReadCoilStatus related method
     *	Description: Read coil (digital I/O) status.
     *
     *	@param argin coil address
     *  @param status returned status
     *	@returns number of elements read/write
     */
    int ModbusProtocol::read_coil_status(uint16_t argin,uint8_t*status){
        return 0;
    }
    
    /**
     *	Command read_input_status related method
     *	Description: Read discrete input status. Return one boolean per array element.
     *
     *	@param add input address
     *  @param buf returning buffer
     *  @param len16 no. of inputs
     *	@returns number of elements read/write
     */
    int ModbusProtocol::read_input_status(uint16_t add, uint16_t*buf,uint16_t len16){
        return read_registers(READ_INPUT_STATUS,add,buf,len16);
    }
    /**
     *	Command read_holding_register related method
     *	Description: Read multiple 16bits registers.
     *
     *	@param add register address
     *  @param buf returning buffer
     *  @param len16 no. of registers
     *	@returns number of elements read/write
     */
    int ModbusProtocol::read_holding_registers(uint16_t add, uint16_t*buf,uint16_t len16){
        
        return read_registers(READ_HOLDING_REGISTERS,add,buf,len16);
    }

    
    /**
     *	Command read_input_registers related method
     *	Description: Read Multiple 16bits input registers.
     *
     *	@param add register address
     *  @param buf returning buffer
     *  @param len16 no. of registers
     *	@returns number of elements read/write
     */
    int ModbusProtocol::read_input_registers(uint16_t add, uint16_t*buf,uint16_t len16){
        return read_registers(READ_INPUT_REGISTERS,add,buf,len16);
    }

    
    /**
     *	Command preset_single_register related method
     *	Description: Write single 16bits register.
     *
     *	@param add register address
     *  @param val value
     *	@returns number of elements read/write
     */
    int ModbusProtocol::preset_single_register(uint16_t add, uint16_t val){
        return 0;
    }

    
    /**
     *	Command read_exception_status related method
     *	Description: Read exception status (usually a predefined range of 8 bits
     *
     *	@param val returned value
     *	@returns number of elements read/write
     */
    int ModbusProtocol::read_exception_status(uint16_t*val){
        return 0;
    }

    
    /**
     *	Command fetch_comm_event_ctr related method
     *	Description: Fetch communications event counter.
     *
     *	@param status return status
     *	@param eventcnt return event counter
     *	@returns number of elements read/write
     */
    int ModbusProtocol::fetch_comm_event_ctr(uint16_t*status,uint16_t*eventcnt){
        return 0;
    }

    /**
     *	Command force_multiple_coils related method
     *	Description: Write multiple coils (digital I/O) state.
     *	@param add coil address
     *  @param buf returning buffer
     *  @param len16 no. of coils
     *	@returns number of elements read/write
     */
    int ModbusProtocol::force_multiple_coils(uint16_t add, uint8_t*buf,uint16_t len16){
        return 0;
    }

    
    /**
     *	Command read_multiple_coils related method
     *	Description: Read multiple coil (digital I/O) status.
     *	@param add register address
     *  @param buf returning buffer
     *  @param len16 no. of coils
     *	@returns number of elements read/write
     */
    int ModbusProtocol::read_multiple_coils(uint16_t add, uint8_t*buf,uint16_t len16){
        return 0;
    }

    
    /**
     *	Command PresetMultipleRegisters related method
     *	Description: Write multiple 16bits registers.
     *	@param add register address
     *  @param buf returning buffer
     *  @param len16 no. of coils
     *	@returns number of elements read/write
     */
    int ModbusProtocol::preset_multiple_registers(uint16_t add, uint16_t*buf,uint16_t len16){
        return write_registers(PRESET_MULTIPLE_REGISTERS,add,buf,len16);
    }
    
    /**
     *	Command mask_write_register related method
     *	Description: Mask write a 16bits register.
     *
     *	@param add register address
     *  @param andmask AND mask
     *  @param ormask OR mask
     *	@returns number of elements read/write
     */
    
    int ModbusProtocol::mask_write_register(uint16_t add,uint16_t andmask,uint16_t ormask){
        return 0;
    }

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
     *	@returns number of elements read/write
     */
    int ModbusProtocol::read_write_register(uint16_t radd, uint16_t*rbuf,uint16_t rlen16,uint16_t wadd, uint16_t*wbuf,uint16_t wlen16){
        return 0;
    }
    
    int ModbusProtocol::init(){
        if(m_dev){
            std::string add;
            if(m_dev->getAttribute("address",add)){
                address = (int)strtoul(add.c_str(),0,0);
            } else {
                DERR("device must specify address attribute\n");
                return DevError::PROTOCOL_ERROR;
            }
        }
        if(m_bus){
            int ret;
            if(m_bus->getUID()==CommChannelUid::RTU_CHANNEL){
                m_uid = ProtocolDriverUid::MODBUS_RTU_PROTOCOL;
            } else if(m_bus->getUID()==CommChannelUid::TCP_CHANNEL){
                m_uid = ProtocolDriverUid::MODBUS_TCP_PROTOCOL;
            } else {
                DERR("invalid protocol : %d\n",m_bus->getUID());
                return DevError::PROTOCOL_INVALID;
            }
            if((ret=m_bus->init())==0)
                m_state = ModBus::MODBUS_CONNECTED;
            
            return ret;
        }
        DERR("invalid communication channel");
        return DevError::BUS_INVALID;
    }
    
    
    int ModbusProtocol::deinit(){
        m_state = ModBus::MODBUS_NONE;
        return m_bus->deinit();
    }

    int ModbusProtocol::reset(){
        m_state = ModBus::MODBUS_NONE;
        return init();
        
    }

    
    size_t ModbusProtocol::read(basichw*,uint32_t add,char*buf, uint32_t sizeb,int timeo,uint32_t flags){
        size_t ret;
        setDeadLine(timeo);
        ret = read_input_registers(add, (uint16_t*)buf,sizeb/sizeof(uint16_t));
        setDeadLine(0);
        return 0;
    }
    
    size_t ModbusProtocol::write(basichw*,uint32_t add,char*buf, uint32_t sizeb,int timeo,uint32_t flags){
        size_t ret;
        setDeadLine(timeo);
        ret= preset_multiple_registers(add, (uint16_t*)buf,sizeb/sizeof(uint16_t));
        setDeadLine(0);

        return ret;
    }
    

  

};