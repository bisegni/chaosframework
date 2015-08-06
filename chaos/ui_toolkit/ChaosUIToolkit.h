/*	
 *	ChaosUIToolkit.h
 *	!CHAOS
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
#ifndef ChaosFramework_ChaosUIToolkit_h
#define ChaosFramework_ChaosUIToolkit_h

#include <chaos/common/global.h>
#include <chaos/common/exception/CException.h>
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
