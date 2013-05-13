//
//  TcpChannel.h
//  chaosDeviceDriver
//
//  Created by andrea michelotti on 4/23/13.
//  Copyright (c) 2013 andrea michelotti. All rights reserved.
//

#ifndef __chaosDeviceDriver__TcpChannel__
#define __chaosDeviceDriver__TcpChannel__

#include <iostream>
#include "CommunicationChannel.h"

#include <boost/asio.hpp>
using boost::asio::ip::tcp;


namespace chaos {
    class TcpChannel:public CommunicationChannel {
        
    private:
       tcp::socket* sock;
        
    protected:
        /**
         @return connected fd 
         */
        
    public:
        TcpChannel(const char*chan_name);
        TcpChannel();
        ~TcpChannel();
        
        /**
         */
        int connectTCP(const char*host,const char* port);
        
        /**
         close and reopen the connection
         */
        int reset();
        /**
         open and connect to target
         @return 0 on success
         */
        int init();
        
        /**
         close the connection
         @return 0 on success
         */
        int deinit();
        
        /**
         read buffer from the target hw
         @param buf destination buffer
         @param sizeb size in byte of the read
         @param flags optional read flags
         */

        size_t read(char*buf, uint32_t sizeb,uint32_t flags=0);

        /**
         write buffer to the target hw
         
         @param buf source buffer
         @param sizeb size in byte of the read
         @param flags optional write flags
         */

        size_t write(char*buf, uint32_t sizeb,uint32_t flags=0);

        /**
         read buffer from the target hw
         
         @param basic_hw target device
         @param add address inside the device
         @param buf destination buffer
         @param sizeb size in byte of the read
         @param timeomilli timeout in milliseconds
         @param flags optional read flags
         */
        size_t read(basichw*,uint32_t add, char*buf, uint32_t sizeb,int timeomilli=0,uint32_t flags=0);
        
        /**
         write buffer to the target hw
         
         @param basic_hw target device
         @param add address inside the device
         @param buf source buffer
         @param sizeb size in byte of the read
         @param timeomilli timeout in milliseconds
         @param flags optional write flags
         */
        size_t write(basichw*,uint32_t add, char*buf, uint32_t sizeb,int timeomilli=0,uint32_t flags=0);

    };
}; 
#endif /* defined(__chaosDeviceDriver__TcpChannel__) */
