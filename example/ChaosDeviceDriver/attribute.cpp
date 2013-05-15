//
//  attribute.cpp
//  chaosDeviceDriver
//
//  Created by andrea michelotti on 4/17/13.
//  Copyright (c) 2013 andrea michelotti. All rights reserved.
//

#include "attribute.h"

namespace chaos {
    namespace Attribute{
        AttributeMode basic_attribute::getMode(){
            return  m_mode;
        }
        const char* basic_attribute::getName(){
            return m_name.c_str();
        }
    };
}