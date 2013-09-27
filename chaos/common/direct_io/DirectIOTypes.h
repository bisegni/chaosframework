//
//  DirectIOTypes.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 25/09/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef CHAOSFramework_DirectIOTypes_h
#define CHAOSFramework_DirectIOTypes_h

#include <string>

namespace chaos {
	namespace common {
		//! Namespace that enclose all direct io infrastructure.
		/*!
			DirectIO infrastructure is an unidirectional channel that send data from client to server. Each
			channel connet two node client -> server. A cient can connect to more server to send the same pachet
			to every one of these. On the other side each server can accept connection form more client and receive 
			data form everyone. At this layer data noto managed so waht is done with data is not discussed. Here
			is specified how connection two node together.
		 */
		namespace direct_io {
			
			//! Structure to identify the target server of the client connection
			typedef struct DirectIOConnection {
				std::string connection_name;
				std::string server_address;
				
			} DirectIOCOnnection;
		}
	}
}

#endif
