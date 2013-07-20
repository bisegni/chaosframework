/*
 *	DSAttributeHandler.h
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

#ifndef __CHAOSFramework__DSAttributeHandler__
#define __CHAOSFramework__DSAttributeHandler__

#include <string>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/exception/CException.h>

namespace chaos {
    
    namespace cu {
            //Forward declaration
        class RTAbstractControlUnit;
        
            //Forward declaration
        class DSAttributeHandlerExecutionEngine;
        
        namespace  handler {
            
            
            
                //! Base class for attribute set hanlder operation
            /*! \class DSAttributeHandler
             \brief
             Define an handler that can be attaccehd to aa Dataset Attribute when a
             set request is received by rpc engine
             */
            class DSAttributeHandler {
                friend class chaos::cu::DSAttributeHandlerExecutionEngine;
                friend class chaos::cu::RTAbstractControlUnit;
                
                    //!handle the abstract call to the specialize handler
                /*!
                 *  The sub class need to implement this function and cast or convert the value to teh specialized type
                 *  \param valueReference is the reference to the value of the type T
                 *  \exception CException is to be managed when something occur during the set operation
                 *  \return CDataWrapper* if the handler need to be send something
                 */
                virtual void handle(void *valueReference) throw (CException) = 0;
             
            protected:
                    //!The name of the attribute
                std::string attributeName;
            public:
                
                    //! Default constructor
                DSAttributeHandler(std::string attrName);
                
                    //! Default constructor
                DSAttributeHandler();
                
                    //! Default Sestructor
                virtual ~DSAttributeHandler();
                
                    //! Return the name of the attribute
                std::string& getAttributeName();
            };
        }
    }
}


#endif /* defined(__CHAOSFramework__DSAttributeHandler__) */
