//
//  ActionDescriptor.h
//  ChaosFramework
//
//  Created by bisegni on 06/07/11.
//  Copyright 2011 INFN. All rights reserved.
//

#ifndef ChaosFramework_ActionDescriptor_h
#define ChaosFramework_ActionDescriptor_h

#include "../global.h"

#include <string>
#include <vector>
#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>
#include <chaos/common/cconstants.h>
#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
    using namespace std;
    using namespace boost;
    
    typedef boost::shared_mutex ActionSharedLock;
    typedef boost::unique_lock< boost::shared_mutex >   ActionWriteLock;
    typedef boost::shared_lock< boost::shared_mutex >   ActionReadLock;
    
        /*
         struct for describe a param for an action
         */
    struct ActionParamDescription {
        string paramName;
        string paramDescription;
        DataType::DataType paramType;
        
        ActionParamDescription(){}
        ActionParamDescription(const char*const _paramName):paramName(_paramName){}
    };
    
    /*
     Abstract class for thedescribe an action
     with a name and a description
     */
    class AbstractActionDescriptor {
        friend class DomainActions;
        //condition_variable disableConditionalVariable;
       
        bool fired;
        bool enabled;
    protected:
        //domain for the action, the full name will be 'actionDomain::actioName
        string actionDomain;
        
        //action key
        string actionName;
        
        //action desription
        string actionDescription;
        
            //map for action
        vector< shared_ptr<ActionParamDescription> > paramDescriptionVec;
        
        //only domain action can be set this value
        bool setEnabled(bool);

    public:
        ActionSharedLock actionAccessMutext;
        
        typedef enum {
            ActionDomain,
            ActionName,
            ActionDescription
        } ActionStringType;
        
            //default constructor
        AbstractActionDescriptor();
        ~AbstractActionDescriptor();
        
            //virtual method for call the action
        virtual CDataWrapper* call(CDataWrapper *actionParam) = 0;
        
        /*
            set the string value for the determinated type
         */
        void setTypeValue(ActionStringType, string&);
        
        /*
         set the string value for the determinated type
         */
        void setTypeValue(ActionStringType, const char*const);
        
        bool isFired();
        bool setFiredWriteLocked(bool _fired);
        bool setFired(bool _fired);
        bool isEnabled();

        /*
            get the string value for the determinated type, a reference
            has been return so keep in mind that string live within object life
         */        
        string& getTypeValue(ActionStringType);
        
#pragma mark Param Method
        /*
         Return the array list of the param defined by this action
         */
        vector< shared_ptr<ActionParamDescription> >& getParamDescriptions();
        
        /*
         Add a new param
         */
        void addParam(const char*const, DataType::DataType, const char*const);
    };
    
    //define the ptr style defined
    typedef shared_ptr<AbstractActionDescriptor> AbstActionDescShrPtr;

#pragma mark Template for Action Definition
    
    /*
     Template class for register a class method as an action. The T*
     msut not be deleted from this class when it will be deallocate
     */
    template <typename T>
    class ActionDescriptor : public AbstractActionDescriptor {
    public:
        typedef CDataWrapper* (T::*ActionPointerDef)(CDataWrapper*);

        /*
         construct the action class with objectClass pointer,object method pointer action domain name and action name
         that implement the action. be aware that the object reference is never deallocated by
         this class
         */
        ActionDescriptor(T* _objectReference, ActionPointerDef _actionPointer, const char*const _domainName, const char*const _actionName) {
                //set the object reference
            objectReference = _objectReference;
                //set the action offset 
            actionPointer = _actionPointer;
                //set the action domain name
            setTypeValue(ActionDomain, _domainName);
                //set the action name
            setTypeValue(ActionName, _actionName);
        }

        
        /*
            execute the action call
         */
        CDataWrapper* call(CDataWrapper *actionParam) {
                //call the action with param
            CHAOS_ASSERT(objectReference)
            return ((*objectReference).*actionPointer)(actionParam);
        }
        
    private:
        //pointer to the action to be executed
        //poitner to the action
        ActionPointerDef actionPointer;
        
        //object pointer
        T* objectReference;
        
    };
    
}
#endif
