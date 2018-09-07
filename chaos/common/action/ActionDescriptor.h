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
#ifndef ChaosFramework_ActionDescriptor_h
#define ChaosFramework_ActionDescriptor_h


#include <string>
#include <vector>
#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>

#include <chaos/common/global.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/exception/exception.h>

#include <boost/thread.hpp>

namespace chaos {
    typedef boost::shared_mutex ActionSharedLock;
    typedef boost::unique_lock< boost::shared_mutex >   ActionWriteLock;
    typedef boost::shared_lock< boost::shared_mutex >   ActionReadLock;
    
    /*
     struct for describe a param for an action
     */
    struct ActionParamDescription {
        std::string paramName;
        std::string paramDescription;
        DataType::DataType paramType;
        
        ActionParamDescription():
        paramType(DataType::TYPE_UNDEFINED){}
        
        ActionParamDescription(const std::string& _paramName):
        paramName(_paramName),
        paramType(DataType::TYPE_UNDEFINED){}
    };
    
    /*!
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
        std::string actionDomain;
        
        //action key
        std::string actionName;
        
        //action desription
        std::string actionDescription;
        
        //!tag action as executables by more thread
        bool shared_execution;
        
        //!whe action is not shared executable mutex need to lock all other thread
        boost::mutex mutex_execution_lock;
        
        //map for action
        std::vector< ChaosSharedPtr<ActionParamDescription> > paramDescriptionVec;
        
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
        AbstractActionDescriptor(bool shared_execution);
        virtual ~AbstractActionDescriptor();
        
        /*!
         virtual method for call the action
         \param actionParam the data sent to the action
         \param detachParam the action can set this param to true, in this case the deallocation is demanded to the action
         \return the result of the action
         */
        virtual chaos::common::data::CDWUniquePtr call(chaos::common::data::CDWUniquePtr actionParam) = 0;
        
        /*!
         set the string value for the determinated type
         */
        void setTypeValue(ActionStringType,
                          const std::string &);
        
        bool isShared();
        bool isFired();
        bool setFiredWriteLocked(bool _fired);
        bool setFired(bool _fired);
        bool isEnabled();
        
        /*!
         get the string value for the determinated type, a reference
         has been return so keep in mind that string live within object life
         */
        const std::string & getTypeValue(ActionStringType type);
        
#pragma mark Param Method
        /*!
         Return the array list of the param defined by this action
         */
        std::vector< ChaosSharedPtr<ActionParamDescription> >& getParamDescriptions();
        
        /*!
         Add a new param
         */
        void addParam(const std::string& param_name,
                      DataType::DataType type,
                      const std::string& description);
    };
    
    //define the ptr style defined
    typedef ChaosSharedPtr<AbstractActionDescriptor> AbstActionDescShrPtr;
    
#pragma mark Template for Action Definition
    
#define IN_ACTION_PARAM_CHECK(x, e, m) if(x) throw CException(e, m, __PRETTY_FUNCTION__);
    
    /*!
     Template class for register a class method as an action. The T*
     msut not be deleted from this class when it will be deallocate
     */
    template <typename T>
    class ActionDescriptor:
    public AbstractActionDescriptor {
    public:
        typedef chaos::common::data::CDWUniquePtr (T::*ActionPointerDef)(chaos::common::data::CDWUniquePtr);
        
        /*!
         construct the action class with objectClass pointer,object method pointer action domain name and action name
         that implement the action. be aware that the object reference is never deallocated by
         this class
         */
        ActionDescriptor(T* _objectReference,
                         ActionPointerDef _actionPointer,
                         const std::string& _domainName,
                         const std::string& _actionName,
                         bool shared_execution = false):
        AbstractActionDescriptor(shared_execution){
            //set the object reference
            objectReference = _objectReference;
            //set the action offset
            actionPointer = _actionPointer;
            //set the action domain name
            setTypeValue(ActionDomain, _domainName);
            //set the action name
            setTypeValue(ActionName, _actionName);
        }
        
        /*!
         execute the action call
         */
        chaos::common::data::CDWUniquePtr call(chaos::common::data::CDWUniquePtr action_data)  throw (CException) {
            //call the action with param
            CHAOS_ASSERT(objectReference)
            boost::unique_lock<boost::mutex> wl(mutex_execution_lock, boost::defer_lock);
            if(isShared() == false){wl.lock();};
            return ((*objectReference).*actionPointer)(MOVE(action_data));
        }
        
    private:
        //!pointer to the action to be executed
        ActionPointerDef actionPointer;
        
        //!object pointer
        T* objectReference;
        
    };
    
}
#endif
