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
#ifndef ChaosFramework_ChaosUIToolkit_h
#define ChaosFramework_ChaosUIToolkit_h

#include <chaos/common/global.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/ChaosCommon.h>
#include <chaos/common/utility/Atomic.h>
#include <chaos/ui_toolkit/HighLevelApi/HLDataApi.h>
#include <chaos/ui_toolkit/HighLevelApi/HLInfrastructureApi.h>

namespace chaos{
    //! Default chaos namespace used to group all ui api
    namespace ui{
  
        /*! \page page_cuit The Chaos User Interface Toolkit 
         *  \section page_cuit_sec Why an User Interface Toolkit
         *  
         */
        
        //! ChaosUIToolkit class is the main singleton needed to use UI chaos API
        /*! 
         Every developer that need to access chaos resource must use this pachecke to
         allocate and instantiate other api pachage
         */
        class ChaosUIToolkit : public ChaosCommon<ChaosUIToolkit> {
            friend class common::utility::Singleton<ChaosUIToolkit>;
            /** \brief Client unique id */
            string clientInstanceUUID;
            /** \brief point to the class the need to be allert when this UIToolkit must exit*/
            ServerDelegator *serverDelegator;  

                //!Constructor
            ChaosUIToolkit();
                //!Destructor
            ~ChaosUIToolkit();
        public:
                //! C and C++ attribute parser
            /*!
             Specialized option for startup c and cpp program main options parameter
             */
            void init(int argc, char* argv[]) throw (CException);
                //!stringbuffer parser
            /*
             specialized option for string stream buffer with boost semantics
             */
            void init(istringstream &initStringStream) throw (CException);
            
                //!Initializaiton Method
            /*!
             This method provide at the all API package initialization it must be called only at applciaiton startup
             \param argc the argc of main cpp program function
             \param argv the argv of main cpp program function
             \exception CException instance is created when something goes wrong
             */
            void init(void *init_data) throw(CException);
            
                //!Deinitializaiton Method
            /*!
                this method provide all the resource deallocation
             \exception CException instance is created when something goes wrong
             */
            void deinit() throw(CException);
        };
    }
}
#endif
