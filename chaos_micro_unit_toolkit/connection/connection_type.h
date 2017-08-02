//
//  connection_type.h
//  CHAOSFramework
//
//  Created by bisegni on 02/08/2017.
//  Copyright © 2017 INFN. All rights reserved.
//

#ifndef connection_type_h
#define connection_type_h

namespace chaos {
    namespace micro_unit_toolkit {
        namespace connection {
            
            //! Proxy type definition
            typedef enum {
                ProxyTypeRaw /*!< Specify a proxy that permit to sned and receive raw message from remote unit */
            } ProxyType;
            
            //! Proxy type definition
            typedef enum {
                SerializationTypeBSONJson /*!< Specify the extended json used for BSON serialization to use in comunication */
            } SerializationType;
            
            //! Proxy type definition
            typedef enum {
                ProtocolTypeHTTP /*!< Specify protocol used to connect to remote endpoint */
            } ProtocolType;
        }
    }
}

#endif /* connection_type_h */
