/*
 *	UUIDUtil.h
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
#ifndef ChaosFramework_UUIDUtil_h
#define ChaosFramework_UUIDUtil_h

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>


namespace chaos {
	namespace common {
		namespace utility {
			
			/*
			 Class for UUID managment
			 */
			class UUIDUtil {
    
    
			public:
				/*
				 Return the first part(of the four) of a random UUID
				 */
				static std::string generateUUIDLite(){
					boost::uuids::uuid _uuid = boost::uuids::random_generator()();
					std::string str_uuid = boost::lexical_cast<std::string>(_uuid);
					size_t foundFirstSegment = str_uuid.find("-");
					return (foundFirstSegment!=std::string::npos)?str_uuid.substr(0, foundFirstSegment):str_uuid;
				}
				
				/*
				 Return the first part(of the four) of a random UUID
				 */
				static std::string generateUUID(){
					boost::uuids::uuid _uuid = boost::uuids::random_generator()();
					return boost::lexical_cast<std::string>(_uuid);
				}
			};
		}
	}
}
#endif
