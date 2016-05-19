/*	
 *	ActionDescriptor.h
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

namespace chaos {
    using namespace std;
    using namespace boost;
    
	namespace chaos_data = chaos::common::data;
	
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
        string actionDomain;
        
        //action key
        string actionName;
        
        //action desription
        string actionDescription;
        
            //map for action
        vector< boost::shared_ptr<ActionParamDescription> > paramDescriptionVec;
        
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
        virtual ~AbstractActionDescriptor();
        
        /*!
         virtual method for call the action
         \param actionParam the data sent to the action
         \param detachParam the action can set this param to true, in this case the deallocation is demanded to the action
         \return the result of the action
         */
        virtual chaos_data::CDataWrapper* call(chaos_data::CDataWrapper *actionParam, bool& detachParam)  throw (CException) = 0;
        
        /*!
            set the string value for the determinated type
         */
        void setTypeValue(ActionStringType, const string &);
        
        bool isFired();
        bool setFiredWriteLocked(bool _fired);
        bool setFired(bool _fired);
        bool isEnabled();

        /*!
            get the string value for the determinated type, a reference
            has been return so keep in mind that string live within object life
         */        
        const string & getTypeValue(ActionStringType);
        
#pragma mark Param Method
        /*!
         Return the array list of the param defined by this action
         */
        vector< boost::shared_ptr<ActionParamDescription> >& getParamDescriptions();
        
        /*!
         Add a new param
         */
        void addParam(const std::string& param_name,
                      DataType::DataType type,
                      const std::string& description);
    };
    
    //define the ptr style defined
    typedef boost::shared_ptr<AbstractActionDescriptor> AbstActionDescShrPtr;

#pragma mark Template for Action Definition
    
#define IN_ACTION_PARAM_CHECK(x, e, m) if(x) throw CException(e, m, __PRETTY_FUNCTION__);
	
    /*!
     Template class for register a class method as an action. The T*
     msut not be deleted from this class when it will be deallocate
     */
    template <typename T>
    class ActionDescriptor : public AbstractActionDescriptor {
    public:
        typedef chaos_data::CDataWrapper* (T::*ActionPointerDef)(chaos_data::CDataWrapper*, bool&);

        /*!
         construct the action class with objectClass pointer,object method pointer action domain name and action name
         that implement the action. be aware that the object reference is never deallocated by
         this class
         */
        ActionDescriptor(T* _objectReference,
                         ActionPointerDef _actionPointer,
                         const std::string& _domainName,
                         const std::string& _actionName) {
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
        chaos_data::CDataWrapper* call(chaos_data::CDataWrapper *actionParam, bool& detachParam)  throw (CException) {
                //call the action with param
            CHAOS_ASSERT(objectReference)
            return ((*objectReference).*actionPointer)(actionParam, detachParam);
        }
        
    private:
        //!pointer to the action to be executed
        ActionPointerDef actionPointer;
        
        //!object pointer
        T* objectReference;
        
    };
    
}
#endif
