/*
 *	TDSObjectHandler.h
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

#ifndef CHAOSFramework_TDSObjectHandler_h
#define CHAOSFramework_TDSObjectHandler_h


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
            template <typename T, typename U>
            class TDSObjectHandler : public TDSAttributeHandler<U> {
            protected:
                
                    //! call the method that as been choosen to manage the value
                /*!
                 *  \param attributeValue the int32_t value that need to be managed
                 */
                inline void attributeHandler(U& attributeValue)  throw (CException) {
                    CHAOS_ASSERT(objectPointer)
                    ((*objectPointer).*handler)(TDSAttributeHandler<U>::attributeName, attributeValue);
                }
                
            public:
                
                    //! The function point to the handle
                typedef void (T::*TDSHandler)(const std::string & , const U&);
                
                /*!
                 Default constructor
                 */
                TDSObjectHandler(T *_objectPointer,
                                TDSHandler _handler):objectPointer(_objectPointer),handler(_handler) {};
                
                /*!
                 Default constructor
                 */
                TDSObjectHandler(std::string attrName,
                                T *_objectPointer,
                                TDSHandler _handler):TDSAttributeHandler<U>(attrName),objectPointer(_objectPointer),handler(_handler) {};
                
                
            private:
                    //! Object pointer tha own the method
                T *objectPointer;
                
                    //! the handler pointer
                TDSHandler handler;
            };
            
        }
    }
}
#endif
