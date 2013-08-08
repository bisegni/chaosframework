/*
 Part of Libnodave, a free communication libray for Siemens S7 200/300/400 via
 the MPI adapter 6ES7 972-0CA22-0XAC
 or  MPI adapter 6ES7 972-0CA23-0XAC
 or  TS adapter 6ES7 972-0CA33-0XAC
 or  MPI adapter 6ES7 972-0CA11-0XAC,
 IBH/MHJ-NetLink or CPs 243, 343 and 443
 or VIPA Speed7 with builtin ethernet support.
 
 (C) Thomas Hergenhahn (thomas.hergenhahn@web.de) 2002..2005
 
 Libnodave is free software; you can redistribute it and/or modify
 it under the terms of the GNU Library General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
 any later version.
 
 Libnodave is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU Library General Public License
 along with Libnodave; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef __cplusplus

extern "C" {
#endif
    
#ifndef __nodave
#define __nodave
    
    /*
     * We define that we use Linux. On Windows parts of our EPICS code will not
     * compile anyway (perhaps with Cygwin, which is somehow like Linux). All
     * other systems that work should be close enough to Linux as far as
     * libnodave is concerned.
     */
#define DAVE_LINUX
    
#define daveSerialConnection 0
#define daveTcpConnection 1
#define daveS7OnlineConnection 2
    
#ifdef DAVE_LINUX
#define DAVE_DECL2
#define DAVE_EXPORTSPEC
    typedef struct dost {
        int rfd;
        int wfd;
        //    int connectionType;
    } daveFileDescriptors;
#include <stdlib.h>
#define dave_tmotype int
#define DAVE_OS_KNOWN	// get rid of nested ifdefs.
#endif
    
#ifdef DAVE_BCCWIN
#define DAVE_WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#define DAVE_DECL2 __stdcall
#define dave_tmotype int
    
#ifdef DAVE_DOEXPORT
#define DAVE_EXPORTSPEC __declspec (dllexport)
#else
#define DAVE_EXPORTSPEC __declspec (dllimport)
#endif
    
    typedef struct dost {
        HANDLE rfd;
        HANDLE wfd;
        //    int connectionType;
    } daveFileDescriptors;
    
#define DAVE_OS_KNOWN
#endif
    
#ifdef DAVE_DOS
#include <stdio.h>
#include <stdlib.h>
    //#define DAVE_DECL2 WINAPI
    // #define DAVE_DECL2
#define DAVE_DECL2 __cedcl
#define DAVE_EXPORTSPEC
    
    typedef struct dost {
        int rfd;
        int wfd;
        int connectionType;
    } daveFileDescriptors;
#define DAVE_OS_KNOWN
#endif
    
#ifdef DAVE_AVR
#include <stdio.h>
#include <stdlib.h>
#define DAVE_DECL2
#define DAVE_EXPORTSPEC
    typedef struct dost {
        int rfd;
        int wfd;
        int connectionType;
    } daveFileDescriptors;
#define dave_tmotype long
#define DAVE_OS_KNOWN
#endif
    
#ifndef DAVE_OS_KNOWN
#error Fill in what you need for your OS or API.
#endif
    
    /*
     some frequently used ASCII control codes:
     */
#define DAVE_DLE 0x10
#define DAVE_ETX 0x03
#define DAVE_STX 0x02
#define DAVE_SYN 0x16
#define DAVE_NAK 0x15
#define DAVE_EOT 0x04	//  for S5
#define DAVE_ACK 0x06	//  for S5
    /*
     Protocol types to be used with newInterface:
     */
#define daveProtoMPI	0	/* MPI for S7 300/400 */
#define daveProtoMPI2	1	/* MPI for S7 300/400, "Andrew's version" without STX */
#define daveProtoMPI3	2	/* MPI for S7 300/400, Step 7 Version, not yet implemented */
#define daveProtoMPI4	3	/* MPI for S7 300/400, "Andrew's version" with STX */
    
#define daveProtoPPI	10	/* PPI for S7 200 */
    
#define daveProtoAS511	20	/* S5 programming port protocol */
    
#define daveProtoS7online 50	/* use s7onlinx.dll for transport */
    
#define daveProtoISOTCP	122	/* ISO over TCP */
#define daveProtoISOTCP243 123	/* ISO over TCP with CP243 */
#define daveProtoISOTCPR 124	/* ISO over TCP with Routing */
    
#define daveProtoMPI_IBH 223	/* MPI with IBH NetLink MPI to ethernet gateway */
#define daveProtoPPI_IBH 224	/* PPI with IBH NetLink PPI to ethernet gateway */
    
#define daveProtoNLpro 230	/* MPI with NetLink Pro MPI to ethernet gateway */
    
#define daveProtoUserTransport 255	/* Libnodave will pass the PDUs of S7 Communication to user */
    /* defined call back functions. */
    
    /*
     *    ProfiBus speed constants:
     */
#define daveSpeed9k     0
#define daveSpeed19k    1
#define daveSpeed187k   2
#define daveSpeed500k   3
#define daveSpeed1500k  4
#define daveSpeed45k    5
#define daveSpeed93k    6
    
    /*
     Some MPI function codes (yet unused ones may be incorrect).
     */
#define daveFuncOpenS7Connection	0xF0
#define daveFuncRead			0x04
#define daveFuncWrite			0x05
#define daveFuncRequestDownload		0x1A
#define daveFuncDownloadBlock		0x1B
#define daveFuncDownloadEnded		0x1C
#define daveFuncStartUpload		0x1D
#define daveFuncUpload			0x1E
#define daveFuncEndUpload		0x1F
#define daveFuncInsertBlock		0x28
    /*
     S7 specific constants:
     */
#define daveBlockType_OB  '8'
#define daveBlockType_DB  'A'
#define daveBlockType_SDB 'B'
#define daveBlockType_FC  'C'
#define daveBlockType_SFC 'D'
#define daveBlockType_FB  'E'
#define daveBlockType_SFB 'F'
    
#define daveS5BlockType_DB  0x01
#define daveS5BlockType_SB  0x02
#define daveS5BlockType_PB  0x04
#define daveS5BlockType_FX  0x05
#define daveS5BlockType_FB  0x08
#define daveS5BlockType_DX  0x0C
#define daveS5BlockType_OB  0x10
    
    /*
     Use these constants for parameter "area" in daveReadBytes and daveWriteBytes
     */
#define daveSysInfo 0x3		/* System info of 200 family */
#define daveSysFlags  0x5	/* System flags of 200 family */
#define daveAnaIn  0x6		/* analog inputs of 200 family */
#define daveAnaOut  0x7		/* analog outputs of 200 family */
    
#define daveP 0x80    		/* direct peripheral access */
#define daveInputs 0x81
#define daveOutputs 0x82
#define daveFlags 0x83
#define daveDB 0x84	/* data blocks */
#define daveDI 0x85	/* instance data blocks */
#define daveLocal 0x86 	/* not tested */
#define daveV 0x87	/* don't know what it is */
#define daveCounter 28	/* S7 counters */
#define daveTimer 29	/* S7 timers */
#define daveCounter200 30	/* IEC counters (200 family) */
#define daveTimer200 31		/* IEC timers (200 family) */
#define daveSysDataS5 0x86	/* system data area ? */
#define daveRawMemoryS5 0		/* just the raw memory */
    
    /**
     Library specific:
     **/
    /*
     Result codes. Genarally, 0 means ok,
     >0 are results (also errors) reported by the PLC
     <0 means error reported by library code.
     */
#define daveResOK 0				/* means all ok */
#define daveResNoPeripheralAtAddress 1		/* CPU tells there is no peripheral at address */
#define daveResMultipleBitsNotSupported 6 	/* CPU tells it does not support to read a bit block with a */
    /* length other than 1 bit. */
#define daveResItemNotAvailable200 3		/* means a a piece of data is not available in the CPU, e.g. */
    /* when trying to read a non existing DB or bit bloc of length<>1 */
    /* This code seems to be specific to 200 family. */
    
#define daveResItemNotAvailable 10		/* means a a piece of data is not available in the CPU, e.g. */
    /* when trying to read a non existing DB */
    
#define daveAddressOutOfRange 5			/* means the data address is beyond the CPUs address range */
#define daveWriteDataSizeMismatch 7		/* means the write data size doesn't fit item size */
#define daveResCannotEvaluatePDU -123    	/* PDU is not understood by libnodave */
#define daveResCPUNoData -124
#define daveUnknownError -125
#define daveEmptyResultError -126
#define daveEmptyResultSetError -127
#define daveResUnexpectedFunc -128
#define daveResUnknownDataUnitSize -129
#define daveResNoBuffer -130
#define daveNotAvailableInS5 -131
#define daveResInvalidLength -132
#define daveResInvalidParam -133
#define daveResNotYetImplemented -134
    
#define daveResShortPacket -1024
#define daveResTimeout -1025
    
    /*
     Error code to message string conversion:
     Call this function to get an explanation for error codes returned by other functions.
     */
    DAVE_EXPORTSPEC char * DAVE_DECL2 daveStrerror(int code); // result is char because this is usual for strings
    
    /*
     Copy an internal String into an external string buffer. This is needed to interface
     with Visual Basic. Maybe it is helpful elsewhere, too.
     */
    DAVE_EXPORTSPEC void DAVE_DECL2 daveStringCopy(char * intString, char * extString); // args are char because this is usual for strings
    
    
    /*
     Max number of bytes in a single message.
     An upper limit for MPI over serial is:
     8		transport header
     +2*240	max PDU len *2 if every character were a DLE
     +3		DLE,ETX and BCC
     = 491
     
     Later I saw some programs offering up to 960 bytes in PDU size negotiation
     
     Max number of bytes in a single message.
     An upper limit for MPI over serial is:
     8		transport header
     +2*960	max PDU len *2 if every character were a DLE
     +3		DLE,ETX and BCC
     = 1931
     
     For now, we take the rounded max of all this to determine our buffer size. This is ok
     for PC systems, where one k less or more doesn't matter.
     */
#define daveMaxRawLen 2048
    /*
     Some definitions for debugging:
     */
#define daveDebugRawRead  	0x01	/* Show the single bytes received */
#define daveDebugSpecialChars  	0x02	/* Show when special chars are read */
#define daveDebugRawWrite	0x04	/* Show the single bytes written */
#define daveDebugListReachables 0x08	/* Show the steps when determine devices in MPI net */
#define daveDebugInitAdapter 	0x10	/* Show the steps when Initilizing the MPI adapter */
#define daveDebugConnect 	0x20	/* Show the steps when connecting a PLC */
#define daveDebugPacket 	0x40
#define daveDebugByte 		0x80
#define daveDebugCompare 	0x100
#define daveDebugExchange 	0x200
#define daveDebugPDU 		0x400	/* debug PDU handling */
#define daveDebugUpload		0x800	/* debug PDU loading program blocks from PLC */
#define daveDebugMPI 		0x1000
#define daveDebugPrintErrors	0x2000	/* Print error messages */
#define daveDebugPassive 	0x4000
    
#define daveDebugErrorReporting	0x8000
#define daveDebugOpen		0x10000  /* print messages in openSocket and setPort */
    
#define daveDebugAll 0x1ffff
    /*
     IBH-NetLink packet types:
     */
#define _davePtEmpty -2
#define _davePtMPIAck -3
#define _davePtUnknownMPIFunc -4
#define _davePtUnknownPDUFunc -5
#define _davePtReadResponse 1
#define _davePtWriteResponse 2
    
    /*
     set and read debug level:
     */
    DAVE_EXPORTSPEC void DAVE_DECL2 daveSetDebug(int nDebug);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetDebug(void);
    
    /*
     This is a wrapper for the serial or ethernet interface. This is here to make porting easier.
     */
    
    typedef struct _daveConnection  daveConnection;
    typedef struct _daveInterface  daveInterface;
    
    /*
     Helper struct to manage PDUs. This is NOT the part of the packet I would call PDU, but
     a set of pointers that ease access to the "private parts" of a PDU.
     */
    typedef struct {
        unsigned char * header;	/* pointer to start of PDU (PDU header) */
        unsigned char * param;		/* pointer to start of parameters inside PDU */
        unsigned char * data;		/* pointer to start of data inside PDU */
        unsigned char * udata;		/* pointer to start of data inside PDU */
        int hlen;		/* header length */
        int plen;		/* parameter length */
        int dlen;		/* data length */
        int udlen;		/* user or result data length */
    } davePDU;
    
    /*
     Definitions of prototypes for the protocol specific functions. The library "switches"
     protocol by setting pointers to the protol specific implementations.
     */
    typedef int (DAVE_DECL2 *  _initAdapterFunc) (daveInterface * );
    typedef int (DAVE_DECL2 *  _connectPLCFunc) (daveConnection *);
    typedef int (DAVE_DECL2 * _disconnectPLCFunc) (daveConnection *);
    typedef int (DAVE_DECL2 * _disconnectAdapterFunc) (daveInterface * );
    typedef int (DAVE_DECL2 * _exchangeFunc) (daveConnection *, davePDU *);
    typedef int (DAVE_DECL2 * _sendMessageFunc) (daveConnection *, davePDU *);
    typedef int (DAVE_DECL2 * _getResponseFunc) (daveConnection *);
    typedef int (DAVE_DECL2 * _listReachablePartnersFunc) (daveInterface * di, char * buf); // changed to unsigned char because it is a copy of an uc buffer
    
    /*
     Definitions of prototypes for i/O functions.
     */
    typedef int (DAVE_DECL2 *  _writeFunc) (daveInterface *, char *, int); // changed to char because char is what system read/write expects
    typedef int (DAVE_DECL2 *  _readFunc) (daveInterface *, char *, int);
    
    /*
     This groups an interface together with some information about it's properties
     in the library's context.
     */
    struct _daveInterface {
        dave_tmotype timeout;	/* Timeout in microseconds used in transort. */
        daveFileDescriptors fd; /* some handle for the serial interface */
        int localMPI;	/* the adapter's MPI address */
        
        int users;		/* a counter used when multiple PLCs are accessed via */
        /* the same serial interface and adapter. */
        char * name;	/* just a name that can be used in programs dealing with multiple */
        /* daveInterfaces */
        int protocol;	/* The kind of transport protocol used on this interface. */
        int speed;		/* The MPI or Profibus speed */
        int ackPos;		/* position of some packet number that has to be repeated in ackknowledges */
        int nextConnection;
        _initAdapterFunc initAdapter;		/* pointers to the protocol */
        _connectPLCFunc connectPLC;			/* specific implementations */
        _disconnectPLCFunc disconnectPLC;		/* of these functions */
        _disconnectAdapterFunc disconnectAdapter;
        _exchangeFunc exchange;
        _sendMessageFunc sendMessage;
        _getResponseFunc getResponse;
        _listReachablePartnersFunc listReachablePartners;
        char realName[20];
        _readFunc ifread;
        _writeFunc ifwrite;
        int seqNumber;
    };
    
    DAVE_EXPORTSPEC daveInterface * DAVE_DECL2 daveNewInterface(daveFileDescriptors nfd, const char * nname, int localMPI, int protocol, int speed);
    DAVE_EXPORTSPEC daveInterface * DAVE_DECL2 davePascalNewInterface(daveFileDescriptors* nfd, char * nname, int localMPI, int protocol, int speed);
    /*
     This is the packet header used by IBH ethernet NetLink.
     */
    typedef struct {
        unsigned char ch1;	// logical connection or channel ?
        unsigned char ch2;	// logical connection or channel ?
        unsigned char len;	// number of bytes counted from the ninth one.
        unsigned char packetNumber;	// a counter, response packets refer to request packets
        unsigned char sFlags;		// my guess
        unsigned char rFlags;		// my interpretation
    } daveIBHpacket;
    
    /*
     Header for MPI packets on IBH-NetLink:
     */
    
    typedef struct {
        unsigned char src_conn;
        unsigned char dst_conn;
        unsigned char MPI;
        unsigned char localMPI;
        unsigned char len;
        unsigned char func;
        unsigned char packetNumber;
    } daveMPIheader;
    
    typedef struct {
        unsigned char src_conn;
        unsigned char dst_conn;
        unsigned char MPI;
        unsigned char xxx1;
        unsigned char xxx2;
        unsigned char xx22;
        unsigned char len;
        unsigned char func;
        unsigned char packetNumber;
    }  daveMPIheader2;
    
    typedef struct _daveS5AreaInfo  {
        int area;
        int DBnumber;
        int address;
        int len;
        struct _daveS5AreaInfo * next;
    } daveS5AreaInfo;
    
    typedef struct _daveS5cache {
        int PAE;	// start of inputs
        int PAA;	// start of outputs
        int flags;	// start of flag (marker) memory
        int timers;	// start of timer memory
        int counters;// start of counter memory
        int systemData;// start of system data
        daveS5AreaInfo * first;
    } daveS5cache;
    
    /*
     This holds data for a PLC connection;
     */
    
    struct _daveConnection {
        int AnswLen;	/* length of last message */
        unsigned char * resultPointer;	/* used to retrieve single values from the result byte array */
        int maxPDUlength;
        int MPIAdr;		/* The PLC's address */
        daveInterface * iface; /* pointer to used interface */
        int needAckNumber;	/* message number we need ackknowledge for */
        int PDUnumber; 	/* current PDU number */
        int ibhSrcConn;
        int ibhDstConn;
        unsigned char msgIn[daveMaxRawLen];
        unsigned char msgOut[daveMaxRawLen];
        unsigned char * _resultPointer;
        int PDUstartO;	/* position of PDU in outgoing messages. This is different for different transport methodes. */
        int PDUstartI;	/* position of PDU in incoming messages. This is different for different transport methodes. */
        int rack;		/* rack number for ISO over TCP */
        int slot;		/* slot number for ISO over TCP */
        int connectionNumber;
        int connectionNumber2;
        unsigned char 	messageNumber;  /* current MPI message number */
        unsigned char	packetNumber;	/* packetNumber in transport layer */
        void * hook;	/* used in CPU/CP simulation: pointer to the rest we have to send if message doesn't fit in a single packet */
        daveS5cache * cache; /* used in AS511: We cache addresses of memory areas and datablocks here */
    };
    
    /*
     Setup a new connection structure using an initialized
     daveInterface and PLC's MPI address.
     */
    DAVE_EXPORTSPEC
    daveConnection * DAVE_DECL2 daveNewConnection(daveInterface * di, int MPI,int rack, int slot);
    
    
    typedef struct {
        unsigned char type[2];
        unsigned short count;
    } daveBlockTypeEntry;
    
    typedef struct {
        unsigned short number;
        unsigned char type[2];
    } daveBlockEntry;
    
    typedef struct {
        unsigned char type[2];
        unsigned char x1[2];  /* 00 4A */
        unsigned char w1[2];  /* some word var? */
        char pp[2]; /* allways 'pp' */
        unsigned char x2[4];  /* 00 4A */
        unsigned short number; /* the block's number */
        unsigned char x3[26];  /* ? */
        unsigned short length; /* the block's length */
        unsigned char x4[16];
        unsigned char name[8];
        unsigned char x5[12];
    } daveBlockInfo;
    /**
     PDU handling:
     PDU is the central structure present in S7 communication.
     It is composed of a 10 or 12 byte header,a parameter block and a data block.
     When reading or writing values, the data field is itself composed of a data
     header followed by payload data
     **/
    typedef struct {
        unsigned char P;	/* allways 0x32 */
        unsigned char type;	/* Header type, one of 1,2,3 or 7. type 2 and 3 headers are two bytes longer. */
        unsigned char a,b;	/* currently unknown. Maybe it can be used for long numbers? */
        unsigned char number;	/* A number. This can be used to make sure a received answer */
		/* corresponds to the request with the same number. */
        unsigned short plen;	/* length of parameters which follow this header */
        unsigned short dlen;	/* length of data which follow the parameters */
        unsigned char result[2]; /* only present in type 2 and 3 headers. This contains error information. */
    } davePDUHeader;
    /*
     set up the header. Needs valid header pointer in the struct p points to.
     */
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveInitPDUheader(davePDU * p, int type);
    /*
     add parameters after header, adjust pointer to data.
     needs valid header
     */
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveAddParam(davePDU * p, unsigned char * param, unsigned short len);
    /*
     add data after parameters, set dlen
     needs valid header,and valid parameters.
     */
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveAddData(davePDU * p, void * data, int len);
    /*
     add values after value header in data, adjust dlen and data count.
     needs valid header,parameters,data,dlen
     */
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveAddValue(davePDU * p, void * data, int len);
    /*
     add data in user data. Add a user data header, if not yet present.
     */
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveAddUserData(davePDU * p, unsigned char *da, int len);
    /*
     set up pointers to the fields of a received message
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveSetupReceivedPDU(daveConnection * dc, davePDU * p);
    /*
     Get the eror code from a PDU, if one.
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetPDUerror(davePDU * p);
    /*
     send PDU to PLC and retrieve the answer
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveExchange(daveConnection * dc, davePDU *p);
    /*
     retrieve the answer
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetResponse(daveConnection * dc);
    /*
     send PDU to PLC
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveSendMessage(daveConnection * dc, davePDU * p);
    
    /******
     
     Utilities:
     
     ****/
    /*
     Hex dump PDU:
     */
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveDumpPDU(davePDU * p);
    
    /*
     This is an extended memory compare routine. It can handle don't care and stop flags
     in the sample data. A stop flag lets it return success, if there were no mismatches
     up to this point.
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveMemcmp(unsigned short * a, unsigned char *b, size_t len);
    
    /*
     Hex dump. Write the name followed by len bytes written in hex and a newline:
     */
    //DAVE_EXPORTSPEC void DAVE_DECL2 _daveDump(char *name, unsigned char *b, int len);
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveDump(char *name, void *b, int len);
    
    /*
     names for PLC objects:
     */
    DAVE_EXPORTSPEC char * DAVE_DECL2 daveBlockName(unsigned char bn);  // char or uc,to decide
    DAVE_EXPORTSPEC char * DAVE_DECL2 daveAreaName(unsigned char n); // to decide
    
    /*
     swap functions. These swap function do a swao on little endian machines only:
     */
    DAVE_EXPORTSPEC short DAVE_DECL2 daveSwapIed_16(short ff);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveSwapIed_32(int ff);
    
    /**
     Data conversion convenience functions. The older set has been removed.
     Newer conversion routines. As the terms WORD, INT, INTEGER etc have different meanings
     for users of different programming languages and compilers, I choose to provide a new
     set of conversion routines named according to the bit length of the value used. The 'U'
     or 'S' stands for unsigned or signed.
     **/
    /*
     Get a value from the position b points to. B is typically a pointer to a buffer that has
     been filled with daveReadBytes:
     */
    DAVE_EXPORTSPEC float DAVE_DECL2 daveGetFloatAt(daveConnection * dc, int pos);
    DAVE_EXPORTSPEC float DAVE_DECL2 daveGetKGAt(daveConnection * dc, int pos);
    
    
    DAVE_EXPORTSPEC float DAVE_DECL2 toPLCfloat(float ff);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveToPLCfloat(float ff);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveToKG(float ff);
    
    
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetS8from(unsigned char *b);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetU8from(unsigned char *b);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetS16from(unsigned char *b);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetU16from(unsigned char *b);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetS32from(unsigned char *b);
    DAVE_EXPORTSPEC unsigned int DAVE_DECL2 daveGetU32from(unsigned char *b);
    DAVE_EXPORTSPEC float DAVE_DECL2 daveGetFloatfrom(unsigned char *b);
    DAVE_EXPORTSPEC float DAVE_DECL2 daveGetKGfrom(unsigned char *b);
    /*
     Get a value from the current position in the last result read on the connection dc.
     This will increment an internal pointer, so the next value is read from the position
     following this value.
     */
    
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetS8(daveConnection * dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetU8(daveConnection * dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetS16(daveConnection * dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetU16(daveConnection * dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetS32(daveConnection * dc);
    DAVE_EXPORTSPEC unsigned int DAVE_DECL2 daveGetU32(daveConnection * dc);
    DAVE_EXPORTSPEC float DAVE_DECL2 daveGetFloat(daveConnection * dc);
    DAVE_EXPORTSPEC float DAVE_DECL2 daveGetKG(daveConnection * dc);
    /*
     Get a value from a given position in the last result read on the connection dc.
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetS8At(daveConnection * dc, int pos);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetU8At(daveConnection * dc, int pos);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetS16At(daveConnection * dc, int pos);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetU16At(daveConnection * dc, int pos);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetS32At(daveConnection * dc, int pos);
    DAVE_EXPORTSPEC unsigned int DAVE_DECL2 daveGetU32At(daveConnection * dc, int pos);
    /*
     put one byte into buffer b:
     */
    DAVE_EXPORTSPEC unsigned char * DAVE_DECL2 davePut8(unsigned char *b, int v);
    DAVE_EXPORTSPEC unsigned char * DAVE_DECL2 davePut16(unsigned char *b, int v);
    DAVE_EXPORTSPEC unsigned char * DAVE_DECL2 davePut32(unsigned char *b, int v);
    DAVE_EXPORTSPEC unsigned char * DAVE_DECL2 davePutFloat(unsigned char *b, float v);
    DAVE_EXPORTSPEC unsigned char * DAVE_DECL2 davePutKG(unsigned char *b, float v);
    DAVE_EXPORTSPEC void DAVE_DECL2 davePut8At(unsigned char *b, int pos, int v);
    DAVE_EXPORTSPEC void DAVE_DECL2 davePut16At(unsigned char *b, int pos, int v);
    DAVE_EXPORTSPEC void DAVE_DECL2 davePut32At(unsigned char *b, int pos, int v);
    DAVE_EXPORTSPEC void DAVE_DECL2 davePutFloatAt(unsigned char *b, int pos, float v);
    DAVE_EXPORTSPEC void DAVE_DECL2 davePutKGAt(unsigned char *b, int pos, float v);
    /**
     Timer and Counter conversion functions:
     **/
    /*
     get time in seconds from current read position:
     */
    DAVE_EXPORTSPEC float DAVE_DECL2 daveGetSeconds(daveConnection *dc);
    /*
     get time in seconds from random position:
     */
    DAVE_EXPORTSPEC float DAVE_DECL2 daveGetSecondsAt(daveConnection *dc, int pos);
    /*
     get counter value from current read position:
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetCounterValue(daveConnection *dc);
    /*
     get counter value from random read position:
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetCounterValueAt(daveConnection *dc, int pos);
    
    /*
     Functions to load blocks from PLC:
     */
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveConstructUpload(davePDU *p, char blockType, int blockNr); // char or uc,to decide
    
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveConstructDoUpload(davePDU *p, int uploadID);
    
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveConstructEndUpload(davePDU *p, int uploadID);
    /*
     Get the PLC's order code as ASCIIZ. Buf must provide space for
     21 characters at least.
     */
    
#define daveOrderCodeSize 21
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetOrderCode(daveConnection *dc, char *buf); // char, users buffer, or to decide
    
    /*
     connect to a PLC. returns 0 on success.
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveConnectPLC(daveConnection *dc);
    
    /*
     Read len bytes from the PLC. Start determines the first byte.
     Area denotes whether the data comes from FLAGS, DATA BLOCKS,
     INPUTS or OUTPUTS, etc.
     DB is the number of the data block to be used. Set it to zero
     for other area types.
     Buffer is a pointer to a memory block provided by the calling
     program. If the pointer is not NULL, the result data will be copied thereto.
     Hence it must be big enough to take up the result.
     In any case, you can also retrieve the result data using the get<type> macros
     on the connection pointer.
     
     RESTRICTION:There is no check for max. message len or automatic splitting into
     multiple messages. Use daveReadManyBytes() in case the data you want
     to read doesn't fit into a single PDU.
     
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveReadBytes(daveConnection *dc, int area, int DB, int start, int len, void *buffer);
    
    /*
     Read len bytes from the PLC. Start determines the first byte.
     In contrast to daveReadBytes(), this function can read blocks
     that are too long for a single transaction. To achieve this,
     the data is fetched with multiple subsequent read requests to
     the CPU.
     Area denotes whether the data comes from FLAGS, DATA BLOCKS,
     INPUTS or OUTPUTS, etc.
     DB is the number of the data block to be used. Set it to zero
     for other area types.
     Buffer is a pointer to a memory block provided by the calling
     program. It may not be NULL, the result data will be copied thereto.
     Hence it must be big enough to take up the result.
     You CANNOT read result bytes from the internal buffer of the
     daveConnection. This buffer is overwritten in each read request.
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveReadManyBytes(daveConnection *dc,int area, int DBnum, int start, int len, void *buffer);
    
    /*
     Write len bytes from buffer to the PLC.
     Start determines the first byte.
     Area denotes whether the data goes to FLAGS, DATA BLOCKS,
     INPUTS or OUTPUTS, etc.
     DB is the number of the data block to be used. Set it to zero
     for other area types.
     RESTRICTION: There is no check for max. message len or automatic splitting into
     multiple messages. Use daveReadManyBytes() in case the data you want
     to read doesn't fit into a single PDU.
     
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveWriteBytes(daveConnection *dc,int area, int DB, int start, int len, void *buffer);
    
    /*
     Write len bytes to the PLC. Start determines the first byte.
     In contrast to daveWriteBytes(), this function can write blocks
     that are too long for a single transaction. To achieve this, the
     the data is transported with multiple subsequent write requests to
     the CPU.
     Area denotes whether the data comes from FLAGS, DATA BLOCKS,
     INPUTS or OUTPUTS, etc.
     DB is the number of the data block to be used. Set it to zero
     for other area types.
     Buffer is a pointer to a memory block provided by the calling
     program. It may not be NULL.
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveWriteManyBytes(daveConnection *dc, int area, int DB, int start, int len, void *buffer);
    /*
     Bit manipulation:
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveReadBits(daveConnection *dc, int area, int DB, int start, int len, void *buffer);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveWriteBits(daveConnection *dc,int area, int DB, int start, int len, void *buffer);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveSetBit(daveConnection *dc,int area, int DB, int byteAdr, int bitAdr);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveClrBit(daveConnection *dc,int area, int DB, int byteAdr, int bitAdr);
    
    /*
     PLC diagnostic and inventory functions:
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveBuildAndSendPDU(daveConnection *dc, davePDU *p2, unsigned char *pa, int psize, unsigned char *ud, int usize);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveReadSZL(daveConnection *dc, int ID, int index, void *buf, int buflen);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveListBlocksOfType(daveConnection *dc, unsigned char type, daveBlockEntry *buf);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveListBlocks(daveConnection *dc, daveBlockTypeEntry * buf);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetBlockInfo(daveConnection *dc, daveBlockInfo *dbi, unsigned char type, int number);
    /*
     PLC program read functions:
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 initUpload(daveConnection *dc, char blockType, int blockNr, int *uploadID); // char or uc,to decide
    DAVE_EXPORTSPEC int DAVE_DECL2 doUpload(daveConnection *dc, int *more, unsigned char **buffer, int *len, int uploadID);
    DAVE_EXPORTSPEC int DAVE_DECL2 endUpload(daveConnection *dc, int uploadID);
    /*
     PLC run/stop control functions:
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveStop(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveStart(daveConnection *dc);
    /*
     PLC special commands
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveCopyRAMtoROM(daveConnection *dc);
    
    DAVE_EXPORTSPEC int DAVE_DECL2 daveForce200(daveConnection *dc, int area, int start, int val);
    /*
     Multiple variable support:
     */
    typedef struct {
        int error;
        int length;
        unsigned char *bytes;
    } daveResult;
    
    typedef struct {
        int numResults;
        daveResult * results;
    } daveResultSet;
    
    
    /* use this to initialize a multivariable read: */
    DAVE_EXPORTSPEC void DAVE_DECL2 davePrepareReadRequest(daveConnection *dc, davePDU *p);
    /* Adds a new variable to a prepared request: */
    DAVE_EXPORTSPEC void DAVE_DECL2 daveAddVarToReadRequest(davePDU *p, int area, int DBnum, int start, int bytes);
    /* Executes the complete request. */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveExecReadRequest(daveConnection *dc, davePDU *p, daveResultSet *rl);
    /* Lets the functions daveGet<data type> work on the n-th result: */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveUseResult(daveConnection *dc, daveResultSet *rl, int n);
    /* Frees the memory occupied by the result structure */
    DAVE_EXPORTSPEC void DAVE_DECL2 daveFreeResults(daveResultSet *rl);
    /* Adds a new bit variable to a prepared request: */
    DAVE_EXPORTSPEC void DAVE_DECL2 daveAddBitVarToReadRequest(davePDU *p, int area, int DBnum, int start, int byteCount);
    
    /* use this to initialize a multivariable write: */
    DAVE_EXPORTSPEC void DAVE_DECL2 davePrepareWriteRequest(daveConnection *dc, davePDU *p);
    /* Add a preformed variable aderess to a read request: */
    DAVE_EXPORTSPEC void DAVE_DECL2 daveAddToReadRequest(davePDU *p, int area, int DBnum, int start, int byteCount, int isBit);
    /* Adds a new variable to a prepared request: */
    DAVE_EXPORTSPEC void DAVE_DECL2 daveAddVarToWriteRequest(davePDU *p, int area, int DBnum, int start, int bytes, void *buffer);
    /* Adds a new bit variable to a prepared write request: */
    DAVE_EXPORTSPEC void DAVE_DECL2 daveAddBitVarToWriteRequest(davePDU *p, int area, int DBnum, int start, int byteCount, void *buffer);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveTestResultData(davePDU *p);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveTestReadResult(davePDU *p);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveTestPGReadResult(davePDU *p);
    
    /* Executes the complete request. */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveExecWriteRequest(daveConnection *dc, davePDU *p, daveResultSet *rl);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveTestWriteResult(davePDU *p);
    
    DAVE_EXPORTSPEC int DAVE_DECL2 daveInitAdapter(daveInterface *di);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveConnectPLC(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveDisconnectPLC(daveConnection *dc);
    
    DAVE_EXPORTSPEC int DAVE_DECL2 daveDisconnectAdapter(daveInterface *di);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveListReachablePartners(daveInterface *di, char *buf);
    
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveReturnOkDummy(daveInterface *di);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveReturnOkDummy2(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveListReachablePartnersDummy(daveInterface *di, char *buf);
    
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveNegPDUlengthRequest(daveConnection *dc, davePDU *p);
    
    /* MPI specific functions */
    
#define daveMPIReachable 0x30
#define daveMPIActive 0x30
#define daveMPIPassive 0x00
#define daveMPIunused 0x10
#define davePartnerListSize 126
    
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveListReachablePartnersMPI(daveInterface *di, char *buf);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveInitAdapterMPI1(daveInterface *di);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveInitAdapterMPI2(daveInterface *di);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveConnectPLCMPI1(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveConnectPLCMPI2(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveDisconnectPLCMPI(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveDisconnectAdapterMPI(daveInterface *di);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveExchangeMPI(daveConnection *dc, davePDU *p1);
    
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveListReachablePartnersMPI3(daveInterface *di, char *buf);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveInitAdapterMPI3(daveInterface *di);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveConnectPLCMPI3(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveDisconnectPLCMPI3(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveDisconnectAdapterMPI3(daveInterface *di);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveExchangeMPI3(daveConnection *dc, davePDU *p1);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveIncMessageNumber(daveConnection *dc);
    
    /* ISO over TCP specific functions */
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveExchangeTCP(daveConnection *dc, davePDU *p1);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveConnectPLCTCP(daveConnection *dc);
    /*
     make internal PPI functions available for experimental use:
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveExchangePPI(daveConnection *dc, davePDU * p1);
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveSendLength(daveInterface *di, int len);
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveSendRequestData(daveConnection *dc, int alt);
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveSendIt(daveInterface *di, unsigned char *b, int size);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveGetResponsePPI(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveReadChars(daveInterface *di, unsigned char *b, dave_tmotype tmo, int max);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveReadChars2(daveInterface *di, unsigned char *b, int max);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveConnectPLCPPI(daveConnection *dc);
    
    /*
     make internal MPI functions available for experimental use:
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveReadMPI(daveInterface *di, unsigned char *b);
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveSendSingle(daveInterface *di, unsigned char c);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveSendAck(daveConnection *dc, int nr);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveGetAck(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveSendDialog2(daveConnection *dc, int size);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveSendWithPrefix(daveConnection *dc, unsigned char *b, int size);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveSendWithPrefix2(daveConnection *dc, int size);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveSendWithCRC(daveInterface *di, unsigned char *b, int size);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveReadSingle(daveInterface *di);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveReadOne(daveInterface *di, unsigned char *b);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveReadMPI2(daveInterface *di, unsigned char *b);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveGetResponseMPI(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveSendMessageMPI(daveConnection *dc, davePDU *p);
    
    /*
     make internal ISO_TCP functions available for experimental use:
     */
    /*
     Read one complete packet. The bytes 3 and 4 contain length information.
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveReadISOPacket(daveInterface *di, unsigned char *b);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveGetResponseISO_TCP(daveConnection *dc);
    
    
    typedef unsigned char * (*daveUserReadFunc) (int, int, int, int, int *);
    typedef void (*daveUserWriteFunc) (int, int, int, int, int *, unsigned char *);
    extern daveUserReadFunc daveReadCallBack;
    extern daveUserWriteFunc daveWiteCallBack;
    
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveConstructReadResponse(davePDU *p);
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveConstructWriteResponse(davePDU *p);
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveConstructBadReadResponse(davePDU *p);
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveHandleRead(davePDU *p1, davePDU *p2);
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveHandleWrite(davePDU *p1, davePDU *p2);
    /*
     make internal IBH functions available for experimental use:
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveReadIBHPacket(daveInterface *di, unsigned char *b);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveWriteIBH(daveInterface *di, unsigned char *buffer, int len);
    DAVE_EXPORTSPEC int DAVE_DECL2 _davePackPDU(daveConnection *dc, davePDU *p);
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveSendMPIAck_IBH(daveConnection *dc);
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveSendIBHNetAck(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 __daveAnalyze(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveExchangeIBH(daveConnection *dc, davePDU *p);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveSendMessageMPI_IBH(daveConnection *dc, davePDU *p);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveGetResponseMPI_IBH(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveInitStepIBH(daveInterface *iface, unsigned char *chal, int cl, unsigned short *resp, int rl, unsigned char *b);
    
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveConnectPLC_IBH(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveDisconnectPLC_IBH(daveConnection *dc);
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveSendMPIAck2(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _davePackPDU_PPI(daveConnection *dc, davePDU *p);
    DAVE_EXPORTSPEC void DAVE_DECL2 _daveSendIBHNetAckPPI(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveListReachablePartnersMPI_IBH(daveInterface *di, char *buf);
    DAVE_EXPORTSPEC int DAVE_DECL2 __daveAnalyzePPI(daveConnection *dc, unsigned char sa);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveExchangePPI_IBH(daveConnection *dc, davePDU *p);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveGetResponsePPI_IBH(daveConnection *dc);
    
    /**
     C# interoperability:
     **/
    DAVE_EXPORTSPEC void DAVE_DECL2 daveSetTimeout(daveInterface *di, int tmo);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetTimeout(daveInterface *di);
    DAVE_EXPORTSPEC char * DAVE_DECL2 daveGetName(daveInterface *di);
    
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetMPIAdr(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetAnswLen(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetMaxPDULen(daveConnection *dc);
    DAVE_EXPORTSPEC daveResultSet * DAVE_DECL2 daveNewResultSet();
    DAVE_EXPORTSPEC void DAVE_DECL2 daveFree(void *dc);
    DAVE_EXPORTSPEC davePDU * DAVE_DECL2 daveNewPDU();
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetErrorOfResult(daveResultSet *, int number);
    
    /*
     Special function do disconnect arbitrary connections on IBH-Link:
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveForceDisconnectIBH(daveInterface *di, int src, int dest, int mpi);
    /*
     Special function do reset an IBH-Link:
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveResetIBH(daveInterface *di);
    /*
     Program Block from PLC:
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetProgramBlock(daveConnection *dc, int blockType, int number, char *buffer, int *length);
    /**
     PLC realtime clock handling:
     */
    /*
     read out clock:
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveReadPLCTime(daveConnection *dc);
    /*
     set clock to a value given by user:
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveSetPLCTime(daveConnection *dc, unsigned char *ts);
    /*
     set clock to PC system clock:
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 daveSetPLCTimeToSystime(daveConnection *dc);
    /*
     BCD conversions:
     */
    DAVE_EXPORTSPEC unsigned char DAVE_DECL2 daveToBCD(unsigned char i);
    DAVE_EXPORTSPEC unsigned char DAVE_DECL2 daveFromBCD(unsigned char i);
    /*
     S5:
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveFakeExchangeAS511(daveConnection *dc, davePDU *p);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveExchangeAS511(daveConnection *dc, unsigned char *b, int len, int maxLen, int trN);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveSendMessageAS511(daveConnection *dc, davePDU *p);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveConnectPLCAS511(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveDisconnectPLCAS511(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveIsS5BlockArea(unsigned char area);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveReadS5BlockAddress(daveConnection *dc, unsigned char area, unsigned char BlockN, daveS5AreaInfo *ai);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveReadS5ImageAddress(daveConnection *dc, unsigned char area, daveS5AreaInfo *ai);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveIsS5ImageArea(unsigned char area);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveIsS5DBBlockArea(unsigned char area);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveReadS5Bytes(daveConnection *dc, unsigned char area, unsigned char BlockN, int offset, int count);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveWriteS5Bytes(daveConnection *dc, unsigned char area, unsigned char BlockN, int offset, int count, void *buf);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveStopS5(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveStartS5(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 daveGetS5ProgramBlock(daveConnection *dc, int blockType, int number, char *buffer, int *length);
    
    /*
     MPI version 3:
     */
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveSendWithPrefix31(daveConnection *dc, unsigned char *b, int size);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveGetResponseMPI3(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveSendMessageMPI3(daveConnection *dc, davePDU *p);
    
    /*
     using S7 dlls for transporrt:
     */
    
#ifndef DAVE_BCCWIN  //We can use this under windows only, but avoid error messages
#define HANDLE int
#endif
    
    /*
     Prototypes for the functions in S7onlinx.dll:
     They are guessed.
     */
    typedef int (DAVE_DECL2 * _openFunc) (const unsigned char *);
    typedef int (DAVE_DECL2 * _closeFunc) (int);
    typedef int (DAVE_DECL2 * _sendFunc) (int, unsigned short, unsigned char *);
    typedef int (DAVE_DECL2 * _receiveFunc) (int, unsigned short, int *, unsigned short, unsigned char *);
    //typedef int (DAVE_DECL2 * _SetHWndMsgFunc) (int, int, ULONG);
    //typedef int (DAVE_DECL2 * _SetHWndFunc) (int, HANDLE);
    typedef int (DAVE_DECL2 * _get_errnoFunc) (void);
    
    /*
     And pointers to the functions. We load them using GetProcAddress() on their names because:
     1. We have no .lib file for s7onlinx.
     2. We don't want to link with a particular version.
     3. Libnodave shall remain useable without Siemens .dlls. So it shall not try to access them
     unless the user chooses the daveProtoS7online transport.
     */
    extern _openFunc SCP_open;
    extern _closeFunc SCP_close;
    extern _sendFunc SCP_send;
    extern _receiveFunc SCP_receive;
    //_SetHWndMsgFunc SetSinecHWndMsg;
    //_SetHWndFunc SetSinecHWnd;
    extern _get_errnoFunc SCP_get_errno;
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveCP_send(int fd, int len, unsigned char *reqBlock);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveSCP_send(int fd, unsigned char *reqBlock);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveConnectPLCS7online (daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveSendMessageS7online(daveConnection *dc, davePDU *p);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveGetResponseS7online(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveExchangeS7online(daveConnection *dc, davePDU *p);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveListReachablePartnersS7online (daveInterface *di, char *buf);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveDisconnectAdapterS7online(daveInterface *di);
    
    DAVE_EXPORTSPEC int DAVE_DECL2 stdwrite(daveInterface *di, char *buffer, int length);
    DAVE_EXPORTSPEC int DAVE_DECL2 stdread(daveInterface *di, char *buffer, int length);
    
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveInitAdapterNLpro(daveInterface *di);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveInitStepNLpro(daveInterface *iface, int nr, unsigned char *fix, int len, char *caller, unsigned char *buffer);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveConnectPLCNLpro (daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveSendMessageNLpro(daveConnection *dc, davePDU *p);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveGetResponseNLpro(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveExchangeNLpro(daveConnection *dc, davePDU *p);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveSendDialogNLpro(daveConnection *dc, int size);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveSendWithPrefixNLpro(daveConnection *dc, unsigned char *b, int size);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveSendWithPrefix2NLpro(daveConnection *dc, int size);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveDisconnectPLCNLpro(daveConnection *dc);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveDisconnectAdapterNLpro(daveInterface *di);
    DAVE_EXPORTSPEC int DAVE_DECL2 _daveListReachablePartnersNLpro(daveInterface *di, char *buf);
    
#endif /* _nodave */
    
#ifdef __cplusplus
    //#ifdef CPLUSPLUS
}
#endif


/*
 Changes:
 07/19/04  added the definition of daveExchange().
 09/09/04  applied patch for variable Profibus speed from Andrew Rostovtsew.
 09/09/04  applied patch from Bryan D. Payne to make this compile under Cygwin and/or newer gcc.
 12/09/04  added daveReadBits(), daveWriteBits()
 12/09/04  added some more comments.
 12/09/04  changed declaration of _daveMemcmp to use typed pointers.
 01/15/04  generic getResponse, more internal functions, use a single dummy to replace 
 initAdapterDummy,
 01/26/05  replaced _daveConstructReadRequest by the sequence prepareReadRequest, addVarToReadRequest
 01/26/05  added multiple write	     
 02/02/05  added readIBHpacket
 02/06/05  replaced _daveConstructBitWriteRequest by the sequence prepareWriteRequest, addBitVarToWriteRequest
 02/08/05  removed inline functions. 
 03/23/05  added _daveGetResponsePPI_IBH().
 03/24/05  added function codes for download.
 03/28/05  added some comments.
 04/05/05  reworked error reporting.
 04/06/05  renamed swap functions. When I began libnodave on little endian i386 and Linux, I used
 used Linux bswap functions. Then users (and later me) tried other systems without
 a bswap. I also cannot use inline functions in Pascal. So I made my own bswaps. Then 
 I, made the core of my own swaps dependent of LITTLEENDIAN conditional to support also
 bigendien systems. Now I want to rename them from bswap to something else to avoid 
 confusion for LINUX/UNIX users. The new names are swapIed_16 and swapIed_32. This
 shall say swap "if endianness differs". While I could have used similar functions 
 from the network API (htons etc.) on Unix and Win32, they may not be present on
 e.g. microcontrollers.
 I highly recommend to use these functions even when writing software for big endian 
 systems, where they effectively do nothing, as it will make your software portable.
 04/08/05  removed deprecated conversion functions.
 04/09/05  removed daveDebug. Use setDebug and getDebug instead. Some programming environments
 do not allow access to global variables in a shared library.
 04/09/05  removed CYGWIN defines. As there were no more differences against LINUX, it should 
 work with LINUX defines.
 04/09/05  reordered fields in daveInterface to put fields used in normal test programs at the
 beginning. This allows to make a simplified version in nodavesimple as short as 
 possible.
 05/09/05  renamed more functions to daveXXX.
 05/11/05  added some functions for the convenience of usage with .net or mono. The goal is
 that the application doesn't have to use members of data structures defined herein
 directly. This avoids "unsafe" pointer expressions in .net/MONO. It should also ease
 porting to VB or other languages for which it could be difficult to define byte by
 byte equivalents of these structures.
 07/31/05  added message string copying for Visual Basic.	      
 08/28/05  added some functions to read and set PLC realtime clock.
 09/02/05  added the pointer "hook" to daveConnection. This can be used by applications to pass
 data between function calls using the dc.
 09/11/05  added read/write functions for long blocks of data.	      
 09/17/05  incorporation of S5 functions
 09/18/05  implemented conversions from and to S5 KG format (old, propeiatary floating point format).
 */
