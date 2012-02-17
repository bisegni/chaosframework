/*	
 *	AttributeEventHandler.h
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
#ifndef ChaosFramework_AttributeEventHandler_h
#define ChaosFramework_AttributeEventHandler_h

#include <map>
#include <queue>
#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_map.hpp>


#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
    
    using namespace std;
    using namespace boost;
    
#pragma mark Handler
    /*
     Thisclass represent the abstract associotion ofattribute name
     and ptr class method offset. Itwil be used into AttributeEventDispatcher class
     */
    class AbstractAttributeEventHandler {
        
    protected:
        string attributeName;
        auto_ptr<CDataWrapper> data;
    public:
        AbstractAttributeEventHandler(string& _attributeName):attributeName(_attributeName){
            
        }
        /*
         abstract calling method
         */
        virtual void call()=0;
        /*
         Return the attribute name
         */
        const char * getAttributeName(){
            return attributeName.c_str();
        }
        
        void setData(CDataWrapper *_data){
            data.reset(_data);
        }
    };
    
    
        //Thepointer definition for abstract attribute handler
    typedef AbstractAttributeEventHandler *AbstractAttributeEventHandlerPtr;
    
    /*
     Template class for specialed handler
     */
    template<typename T>
    class AttributeEventHandler : public AbstractAttributeEventHandler {
           public:
        typedef void (T::*AttributeActionHandler)(CDataWrapper*);    
        
        /*
         Constructor
         */
        AttributeEventHandler(string& _attributeName, T *_objectReference, AttributeActionHandler _handler) : AbstractAttributeEventHandler(_attributeName){
            handler = _handler;
            objectReference = _objectReference;
        }
        
        /*
         call the handler
         */
        void call() {
            ((*objectReference).*handler)(*data);
        }
        
    private:
        T *objectReference;
        AttributeActionHandler handler;

    };
}
#endif
