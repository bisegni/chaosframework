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

#ifndef __CHAOSFramework__EventAction__
#define __CHAOSFramework__EventAction__

namespace chaos {
    namespace common {
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
}

#endif /* defined(__CHAOSFramework__EventAction__) */
