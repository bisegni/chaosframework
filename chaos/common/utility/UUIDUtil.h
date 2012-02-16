//
//  UUIDUtil.h
//  ChaosFramework
//
//  Created by Bisegni Claudio on 01/01/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef ChaosFramework_UUIDUtil_h
#define ChaosFramework_UUIDUtil_h

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

namespace chaos {
    using namespace std;
    using namespace boost;
    using namespace boost::uuids;
    
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
#endif
