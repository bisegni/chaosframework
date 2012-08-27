/*
 *	EventAction.h
 *	CHAOSFramework
 *	Created by Claudio Bisegni on 27/08/12.
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

#ifndef __CHAOSFramework__EventAction__
#define __CHAOSFramework__EventAction__

namespace chaos {
    
    namespace event{
    
        namespace channel {
            
                //! An action attaccehd to an event
            /*!
                The class event action define the structure of an action
                that can be attacched to a specified event.
             */
            class EventAction {
             
            protected:
                
                void handleEvent() = 0;
                
            public:
                virtual ~EventAction() {};
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__EventAction__) */
