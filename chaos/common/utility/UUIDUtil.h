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
using namespace std;
using namespace boost;
using namespace boost::uuids;
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
				static string generateUUIDLite(){
					uuid uuid = random_generator()();
					string strUUID = lexical_cast<std::string>(uuid);
					size_t foundFirstSegment = strUUID.find("-");
					return (foundFirstSegment!=string::npos)?strUUID.substr(0, foundFirstSegment):strUUID;
				}
				
				/*
				 Return the first part(of the four) of a random UUID
				 */
				static string generateUUID(){
					uuid uuid = random_generator()();
					return lexical_cast<std::string>(uuid);
				}
			};
		}
	}
}
#endif
