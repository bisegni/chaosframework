/*
 *	DirectIOServer.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#ifndef __CHAOSFramework__DirectIOServer__
#define __CHAOSFramework__DirectIOServer__

namespace chaos {
	namespace common {
		namespace direct_io {
			
			//! Direct IO server base class
			/*!
				This class reppresents the base interface for the operation on direct io input channel,
				The server layer will accept the client connection for the data reception. Server can accept connection
				by different client
				dio_client---> data message -->dio_server
			 */
			class DirectIOServer {
				
			};
			
		}
	}
}


#endif /* defined(__CHAOSFramework__DirectIOServer__) */
