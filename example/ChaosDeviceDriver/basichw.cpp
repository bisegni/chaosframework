//
//  basichw.cpp
//  chaosDeviceDriver
//
//  Created by andrea michelotti on 4/17/13.
//  Copyright (c) 2013 andrea michelotti. All rights reserved.
//

#include "basichw.h"
#include <boost/bind.hpp>
#include <stdarg.h>

namespace chaos{
    devexcept::devexcept(basichw*dev,const char * stringa,...){
        char buffer[2048];
        va_list ap;
        va_start(ap,stringa);
        vsnprintf(buffer,sizeof(buffer),stringa,ap);
        w = dev->getName();
        w= w + std::string(":")+ stringa;
    }
    devexcept::devexcept(basichw*dev,std::string s){
        w = dev->getName();
        w= w + std::string(":")+ s;
    }
    
    basichw::basichw():deadline_(io_service){
       deadline_.expires_at(boost::posix_time::pos_infin);
    }
    basichw::basichw(const char*n,uint32_t i):m_name(n),m_uid(i),deadline_(io_service){
        deadline_.expires_at(boost::posix_time::pos_infin);
    }
    
     
    size_t basichw::read_reg(chaos::basichw *hw, uint32_t add, char *buf, uint32_t sizeb, int timeo, uint32_t flags){
        return 0;
    }
    size_t basichw::write_reg(chaos::basichw *hw, uint32_t add, char *buf, uint32_t sizeb, int timeo, uint32_t flags){
        return 0;
    }
    
    uint32_t basichw::getUID(){return m_uid;}
    void basichw::setUID(uint32_t p){m_uid=p;}
    
    int basichw::reset(){
        return 0;
    }
     const char* basichw::getName(){return m_name.c_str();}
    
    
    size_t basichw::read(basichw*a,char*buf, uint32_t sizeb,int timeo, uint32_t flags){return read(a,0,buf,sizeb,timeo,flags);}
    
    size_t basichw::write(basichw*a,char*buf, uint32_t sizeb,int timeo,uint32_t flags){return write(a,0,buf,sizeb,timeo,flags);}
    

    void basichw::setDeadLine(int milli){
        boost::posix_time::time_duration t;
        if(milli==0){
            deadline_.expires_at(boost::posix_time::pos_infin);
            return;
        }
        t=boost::posix_time::milliseconds(milli);
        deadline_.expires_from_now(t);
        deadline_.async_wait(boost::bind(&basichw::deadLineHandler,this));

    }
    void basichw::deadLineHandler(){
         deadline_.expires_at(boost::posix_time::pos_infin);
        throw devexcept(this,"timeout");
    }

};

