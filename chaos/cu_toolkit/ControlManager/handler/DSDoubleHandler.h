/*
 *	DSDoubleHandler.h
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

#ifndef CHAOSFramework_DSDoubleHandler_h
#define CHAOSFramework_DSDoubleHandler_h


#include <chaos/common/global.h>
#include <chaos/cu_toolkit/ControlManager/handler/TDSAttributeHandler.h>



namespace chaos {
    namespace cu {
        namespace  handler {
            
                //! Implement and handler of int32_t type
            /*! \class DSInt32Handler
             \brief
             Thsi class implement and int32_t handler using an object ethod as handler pointer
             */
            template <typename T>
            class DSDoubleHandler : public TDSAttributeHandler<double_t> {
            protected:
                
                    //! call the method that as been choosen to manage the value
                /*!
                 *  \param attributeValue the int32_t value that need to be managed
                 */
                inline void attributeHandler(double_t& attributeValue)  throw (CException) {
                    CHAOS_ASSERT(objectPointer)
                    ((*objectPointer).*handler)(&attributeName, attributeValue);
                }
                
            public:
                
                    //! The function point to the handle
                typedef void (T::*DoubleHandler)(const std::string * const, double_t);
                
                /*!
                 Default constructor
                 */
                DSDoubleHandler(std::string attrName,
                                T *_objectPointer,
                                DoubleHandler _handler):TDSAttributeHandler<double_t>(attrName),objectPointer(_objectPointer),handler(_handler) {};
                
                
            private:
                    //! Object pointer tha own the method
                T *objectPointer;
                
                    //! the handler pointer
                DoubleHandler handler;
            };
            
        }
    }
}



#endif
