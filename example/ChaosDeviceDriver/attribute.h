//
//  attribute.h
//  chaosDeviceDriver
//
//  Created by andrea michelotti on 4/17/13.
//  Copyright (c) 2013 andrea michelotti. All rights reserved.
//

#ifndef __chaosDeviceDriver__attribute__
#define __chaosDeviceDriver__attribute__

#include <iostream>
#include <map>
namespace chaos{
    namespace Attribute{
        enum AttributeMode{
            RO,
            WO,
            RW
        };

    
    class basic_attribute {
      
        std::string m_name;
        Attribute::AttributeMode m_mode;
        
    public:
        basic_attribute(const char*n,AttributeMode m=Attribute::RW):m_name(n),m_mode(m){
        };
        basic_attribute(){m_mode = RW;}
        Attribute::AttributeMode getMode();
        const char* getName();
        
    };
    
    template <typename T>
    class attribute:public basic_attribute {
        T value;
    public:
        attribute(const char*name,T val,AttributeMode m=Attribute::RW):basic_attribute(name,m),value(val){};
        T& operator=(T& v){
            return value=v;
        }
        operator T(){return value;}
        T& getValue(){return value;}
        void setValue(T& val){value = val;}
    };

    };
    };

#endif /* defined(__chaosDeviceDriver__attribute__) */
