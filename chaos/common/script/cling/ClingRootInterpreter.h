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
#ifndef ClingRootInterpreter_hpp
#define ClingRootInterpreter_hpp
#ifdef CLING_VIRTUAL_MACHINE
#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/Singleton.h>

#include <cling/Interpreter/Interpreter.h>

namespace chaos {
    namespace common {
        namespace script {
            namespace cling {
                
                //! Main cling itnepreter
                /*!
                 This class embed the root interpeter taht load for all the child the base chaos nedded includes
                 */
                class ClingRootInterpreter:
                public chaos::common::utility::Singleton<ClingRootInterpreter> {
                    ChaosUniquePtr<::cling::Interpreter> root_interpreter;
                    friend class chaos::common::utility::Singleton<ClingRootInterpreter>;
                    ClingRootInterpreter();
                    ~ClingRootInterpreter();
                public:
                    const ::cling::Interpreter& getRootInterpreter();
                };
            }
        }
    }
}
#endif /* CLING_VIRTUAL_MACHINE */
#endif /* ClingRootInterpreter_hpp */
