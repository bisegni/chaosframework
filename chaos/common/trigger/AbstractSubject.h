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

#ifndef __CHAOSFramework__F95AD0C_755D_4EE8_8709_A0118CE00CAB_AbstractSubjectReceiver_h
#define __CHAOSFramework__F95AD0C_755D_4EE8_8709_A0118CE00CAB_AbstractSubjectReceiver_h

#include <chaos/common/trigger/trigger_types.h>

#include <string>

namespace chaos {
    namespace common {
        namespace trigger {
            
            //! present the abstraction of an event receiver
            /*!
             Event receiver is the subject of event->consumer lifecicle. An event
             receiver receive event of a determinated type and permit to
             attach to it some consumer for a determinated event
             */
            class AbstractSubject {
                const std::string subject_name;
                const std::string subject_uuid;
                
            public:
                AbstractSubject(const std::string& _recevier_name);
                virtual ~AbstractSubject();
                
                const std::string& getSubjectUUID() const;
                
                const std::string& getSubjectName() const;
            };
        }
    }
}

#endif /* __CHAOSFramework__F95AD0C_755D_4EE8_8709_A0118CE00CAB_AbstractSubject_h */
