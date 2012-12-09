/*
 *	TDSAttributeHandler.h
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

#ifndef __CHAOSFramework__TDSAttributeHandler__
#define __CHAOSFramework__TDSAttributeHandler__

#include <chaos/cu_toolkit/ControlManager/handler/DSAttributeHandler.h>

namespace chaos {
    namespace cu {
        namespace  handler {
            
                //---------------help macro------------

            
                //! Templated Attribute Handler base class
            /*! \class TDSAttributeHandler
             \brief
             This class is the base attributed class that permit to define a typed handler
             to process parameter value
             */
            template<typename T>
            class TDSAttributeHandler : public DSAttributeHandler {
                
                    //!Handle the abstarct managment of the value
                /*!
                 *  \param value the abstract value
                 *  \exception in case of error a the exception is thrown
                 */
                void handle(void *value) throw (CException) {
                    attributeHandler(*((T*)value));
                }
                
            protected:
                
                    //!attributeHandler implement the handler for a specified attribute
                /*!
                 *  \param attributeValue the value for the attribute to handle
                 *  \exception when something goes wrong the implementation need to send an exception to notify it
                 */
                virtual void attributeHandler(T& attributeValue)  throw (CException) = 0;
                
            public:
                /*!
                 *  Default constructor with attribute name as parameter
                 */
                TDSAttributeHandler(){};
                
                /*!
                 *  Default constructor with attribute name as parameter
                 */
                TDSAttributeHandler(std::string attrName):DSAttributeHandler(attrName){};
                
                /*!
                 * Default destructor
                 */
                virtual ~TDSAttributeHandler(){};
            };
        }
    }
}
#endif /* defined(__CHAOSFramework__TDSAttributeHandler__) */
