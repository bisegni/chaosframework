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
#ifndef CachingSystem_IdFactory_h
#define CachingSystem_IdFactory_h

namespace chaos {

namespace caching_system {
    namespace helper {
        static int lastNumber=0;
       
        static int getNewId(){
            
            return  __sync_fetch_and_add(&lastNumber,1);
        }
        
    }
}
    
}
#endif
