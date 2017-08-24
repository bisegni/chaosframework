/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
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
