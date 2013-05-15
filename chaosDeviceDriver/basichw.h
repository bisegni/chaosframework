//
//  basichw.h
//  chaosDeviceDriver
//
//  Created by andrea michelotti on 4/17/13.
//  Copyright (c) 2013 andrea michelotti. All rights reserved.
//

#ifndef __chaosDeviceDriver__basichw__
#define __chaosDeviceDriver__basichw__

#include <iostream>
#include "attribute.h"
#include <stdint.h>
#include <map>
#include <vector>
#include "DriverUID.h"
#include <boost/asio.hpp>
#include <string>

namespace chaos{
    

    class basichw {
        
   
    protected:
        boost::asio::io_service io_service;
        boost::asio::deadline_timer deadline_;

        // name of device, bus, driver
        std::string m_name;
        // unique vendor identifier for device, bus, driver
        uint32_t m_uid;
        
    // list of attribute of the HW
        std::map<const char* ,Attribute::basic_attribute*> m_attr;
        
    public:
        basichw();
        basichw(const char*n,uint32_t i);
        
        const char* getName();
        uint32_t getUID();
        void setUID(uint32_t);
        /**
         Set the deadline for a given operation
         @param milli set the timeout in milli seconds, 0 = no timeout(default)
         */
        void setDeadLine(int milli);
        virtual void deadLineHandler();
        
        template<typename T>
        bool getAttribute(const char*name,T&def){
            std::map<const char* ,Attribute::basic_attribute*>::iterator i;
            if((i=m_attr.find(name))==m_attr.end()){
                return false;
            }
            Attribute::attribute<T> *p = (Attribute::attribute<T> *)i->second;
            def = p->getValue();
            return true;
        }
        
        template<typename T>
        bool setAttribute(const char*name,T def, Attribute::AttributeMode mode=Attribute::RW){
            std::map<const char* ,Attribute::basic_attribute*>::iterator i;
            
            if((i=m_attr.find(name))==m_attr.end()){
                // if not exists create
                Attribute::attribute<T>* pnt= new Attribute::attribute<T>(name,def,mode);
                if(pnt){
                    m_attr.insert(std::pair<const char*,Attribute::basic_attribute*>(name,pnt));
                   // m_attr[name]=*pnt;
                     DPRINT("create and set attribute attribute \"%s\"\n",name);
                    return true;
                }
                DERR("## cannot create attribute \"%s\"\n",name);
                return false;
            } else {
                if(m_attr[name]->getMode() == Attribute::RW || m_attr[name]->getMode() == Attribute::WO){
                    Attribute::attribute<T> *p =(Attribute::attribute<T> *)m_attr[name];
                    *p = def;
                    
                    DPRINT("setting attribute attribute \"%s\"\n",name);

                    return true;
                } else {
                     DERR("setting attribute access violation \"%s\" cannot write such attribute\n",name);
                }
            }
            return false;
        }

        
        /** 
         reset the device,bus,driver
         @return 0 on success
         */
        virtual int reset();
        /**
            initialize hw
         @return 0 on success
         */
        virtual int init()=0;
        
        /**
         deinitialize hw
         @return 0 on success
         */
        virtual int deinit()=0;
        
        /**
         read registers
         
         @param basic_hw target device
         @param add address inside the device
         @param buf destination buffer
         @param sizeb size in byte of the read
         @param timeomilli timeout in milliseconds
         @param flags optional read flags
         @return the number of bytes read
         */
        virtual size_t read_reg(basichw*,uint32_t add, char*buf, uint32_t sizeb,int timeomilli=0,uint32_t flags=0);
        
        /**
         write registers
         
         @param basic_hw target device
         @param add address inside the device
         @param buf destination buffer
         @param sizeb size in byte of the read
         @param timeomilli timeout in milliseconds
         @param flags optional read flags
         @return the number of bytes read
         */
        virtual size_t write_reg(basichw*,uint32_t add, char*buf, uint32_t sizeb,int timeomilli=0,uint32_t flags=0);
        
        /**
         read buffer from the target hw
         
         @param basic_hw target device
         @param add address inside the device
         @param buf destination buffer
         @param sizeb size in byte of the read
         @param timeomilli timeout in milliseconds
         @param flags optional read flags
         @return the number of bytes read
         */
        virtual size_t read(basichw*,uint32_t add,char*buf, uint32_t sizeb,int timeomilli=0,uint32_t flags=0)=0;
        
        /**
         write buffer to the target hw
         
         @param basic_hw target device
         @param add address inside the device
         @param buf source buffer
         @param sizeb size in byte of the read
         @param timeomilli timeout in milliseconds
         @param flags optional write flags
         @return the number of bytes written
         */
        virtual size_t write(basichw*,uint32_t add,char*buf, uint32_t sizeb,int timeomilli=0,uint32_t flags=0)=0;
        
        
        /**
         read stream buffer from the target hw
         @param basic_hw target device
         @param buf destination buffer
         @param sizeb size in byte of the read
         @param timeomilli timeout in milliseconds
         @param flags optional read flags
         @return the number of bytes read
         */
        virtual size_t read(basichw*,char*buf, uint32_t sizeb,int timeomilli=0,uint32_t flags=0);
        
        /**
         write buffer to the target hw
         
         @param basic_hw target device
         @param buf source buffer
         @param sizeb size in byte of the read
         @param timeomilli timeout in milliseconds
         @param flags optional write flags
         @return the number of bytes written
         */
        virtual size_t write(basichw*,char*buf, uint32_t sizeb,int timeomilli=0,uint32_t flags=0);

            
    };
    /**
        add a HW to a list
        @return 0 on success
     */
    template<typename T>
    int addHWToList(std::vector<T*>& ref,T* hw){
        typename std::vector<T*>::iterator i;
        // check if already there
        for(i=ref.begin();i!=ref.end();i++){
            if(*i == hw) {
                return -1;
            }
        }
        ref.push_back(hw);
        return 0;
    }

    /**
     remove a HW to a list
     @return 0 on success
     */
    template<typename T>
    int removeHWToList(std::vector<T*>& ref,T* hw){
        typename std::vector<T*>::iterator i;
        // check if already there
        for(i=ref.begin();i!=ref.end();i++){
            if(*i == hw) {
                ref.erase(i);
                return 0;
            }
        }
        return -2;
    }

    class devexcept: public std::exception {
        
        std::string w;
    public:
        devexcept(basichw*,const char * stringa,...);
        devexcept(basichw*,std::string s);
        ~devexcept() throw(){}
        const char* what() {return w.c_str();};
        
    };
    
};
#endif /* defined(__chaosDeviceDriver__basichw__) */
