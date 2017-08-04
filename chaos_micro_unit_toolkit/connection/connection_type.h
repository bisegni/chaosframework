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
                /*!Specify a proxy that permit to act ans a driver at low level, 
                 no specified portocol is used messange and request need to be 
                 managed by developer */
                ProxyTypeRawDriver
            } ProxyType;
            
            //! Proxy type definition
            typedef enum {
                /*!Specify the extended json used 
                 for BSON serialization to use in 
                 comunication */
                SerializationTypeBSONJson
            } SerializationType;
            
            //! Proxy type definition
            typedef enum {
                /*!Specify protocol used to 
                 connect to remote endpoint */
                ProtocolTypeHTTP
            } ProtocolType;
        }
    }
}

#endif /* connection_type_h */
