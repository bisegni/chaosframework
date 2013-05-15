//
//  TcpChannel.cpp
//  chaosDeviceDriver
//
//  Created by andrea michelotti on 4/23/13.
//  Copyright (c) 2013 andrea michelotti. All rights reserved.
//

#include "TcpChannel.h"
#include "basichw.h"



namespace chaos {
    TcpChannel::TcpChannel(const char *name):CommunicationChannel(name,CommChannelUid::TCP_CHANNEL){
        
        sock = new tcp::socket(io_service);
        
    }
    
    TcpChannel::TcpChannel():CommunicationChannel("TCP Channel",CommChannelUid::TCP_CHANNEL){
        
        sock = new tcp::socket(io_service);
        
        
    }
    TcpChannel::~TcpChannel(){
        deinit();
    }
    
    
    int TcpChannel::connectTCP(const char*host,const char* port){
        if(sock){
            try{
                tcp::resolver resolver(io_service);
                tcp::resolver::query query(tcp::v4(),host, port);
                tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
                boost::asio::connect(*sock, endpoint_iterator);
            } catch (std::exception&e){
                DERR("error connecting to \"%s\" port %s\n",host,port);
                return DevError::BUS_INIT_ERROR;
            }
            return DevError::NOERROR;
        }
        
        return DevError::BUS_INVALID;
    }
    
    int TcpChannel::init(){
        std::string address,port;
        // point to point connection just one driver and device
        device_proxy*dev = m_driver_list[0]->getDevice();
        if(m_driver_list.size()!=1){
            DERR("invalid number of drivers attached to a point to point: %lu\n",m_driver_list.size());
            return DevError::BUS_INIT_ERROR;
        }
        if(sock){
            if(dev){
                //these two attribute must be defined for target device
                if(dev->getAttribute("address",address)&& dev->getAttribute("port", port)){
                    return connectTCP(address.c_str(), port.c_str());
                }
                DERR("no \"address\" and/or \"port\" attribute found in device :\"%s\"\n",dev->getName());
                return DevError::BUS_INIT_ERROR;
                
            }
            DERR("no valid device specified\n");
            return DevError::BUS_INIT_ERROR;
        }
        
        return DevError::BUS_INIT_ERROR;
    }
    int TcpChannel::reset(){
        if(sock){
            delete sock;
        }
        sock = new tcp::socket(io_service);
        return init();
    }
    int TcpChannel::deinit(){
        if(sock){
            delete sock;
            sock = NULL;
        }
        return 0;
    }
    
    size_t TcpChannel::write(char *buf, uint32_t sizeb,uint32_t flags){
        size_t ret;
        try{
            ret = boost::asio::write(*sock, boost::asio::buffer(buf, sizeb));
        } catch (std::exception&e){
            std::cerr << "TcpChannel::write() Exception: " << e.what() << "\n";
            return ret;
        }
        return ret;
    }
    
    size_t TcpChannel::read(char *buf, uint32_t sizeb,uint32_t flags){
        size_t ret;
        try{
            ret = boost::asio::read(*sock, boost::asio::buffer(buf, sizeb));
        } catch (std::exception&e){
            std::cerr << "TcpChannel::read() Exception: " << e.what() << "\n";
            return ret;
        }
        return ret;
        
    }

    size_t TcpChannel::read(chaos::basichw *hw, uint32_t add, char *buf, uint32_t sizeb,int timeo,uint32_t flags){
        setDeadLine(timeo);
        size_t ret =read(buf, sizeb,flags);
        setDeadLine(0);
        return ret;
    }
    
    size_t TcpChannel::write(chaos::basichw *hw, uint32_t add, char *buf, uint32_t sizeb,int timeo,uint32_t flags){
        setDeadLine(timeo);
        size_t ret =write(buf, sizeb,flags);
        setDeadLine(0);
        return ret;
    }
}