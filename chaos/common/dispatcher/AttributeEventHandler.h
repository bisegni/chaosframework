//
//  AttributeEventHandler.h
//  ChaosFramework
//
//  Created by Bisegni Claudio on 05/11/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

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
