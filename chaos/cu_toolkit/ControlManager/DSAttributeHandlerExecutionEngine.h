/*
 *	DSAttributeHandlerExecutionEngine.h
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

#ifndef __CHAOSFramework__DSAttributeHandlerExecutionEngine__
#define __CHAOSFramework__DSAttributeHandlerExecutionEngine__
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <string>
#include <set>
#include <map>
#include <vector>

#include <chaos/common/data/DatasetDB.h>

#include <chaos/cu_toolkit/ControlManager/handler/DSAttributeHandler.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/StartableService.h>
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

namespace chaos{
    namespace cu {
        namespace control_manager {
            //! Engine for the execution of the attribute handler
			/*! \class DSAttributeHandlerExecutionEngine
			 \brief
			 This class is the inegine for the execetuion of the handler on the device attribute
			 */
			class DSAttributeHandlerExecutionEngine : public chaos::utility::StartableService {
				
				boost::shared_mutex engineManagmentLock;
				
                //!has the infromatin schema for the control unit
				chaos::common::data::DatasetDB *referenceDeviceSchemaDB;
				
                //! point to the name of the attribute manage by this environment
				std::set<std::string> managedAttributesName;
				
                //! point
				std::map<std::string, std::vector< handler::DSAttributeHandler* > * > attrNameHandlerMap;
				
			public:
				/*!
				 Default constructor with std string
				 */
				DSAttributeHandlerExecutionEngine();
				/*!
				 Default constructor with std string
				 */
				DSAttributeHandlerExecutionEngine(chaos::common::data::DatasetDB  *);
				
				/*!
				 Default destructor with std string
				 */
				~DSAttributeHandlerExecutionEngine();
				
                //! Initialize instance
				void init(void*) throw(chaos::CException);
				
                //! Start the implementation
				void start() throw(chaos::CException);
				
				//-----------------------
				void stop() throw(CException){};
				
                //! Deinit the implementation
				void deinit() throw(chaos::CException);
				
				void setDeviceSchemaDB(chaos::common::data::DatasetDB  *_referenceDeviceSchemeDB);
				
                //!addHandlerForDSAttribute
				/*!
				 *  This function permit to attach an handler to an attribute name. When the control unit
				 *  receive by RPC an evento to set, a determinate attribute, to an specified value, this
				 *  handler will be called. The handler pointer is deallocate directly by this class.
				 *
				 *  \param attrName is the name of the attribute where the handler need to be attached
				 *  \param classHandler is the pointer to handler that need to be attached
				 *  \exception when something goes wrong a CExcetion instance is sent with the occured error
				 */
				void addHandlerForDSAttribute(handler::DSAttributeHandler* classHandler) throw (CException);
				
                //!execute one o more handler according with the message information
				/*!
				 *  \param message is the RPC message tha that need to be constructed in a determinated why to be
				 * worked by engine
				 *
				 *  \exception excetion description
				 *  \return boolean value that is true if one o more attribute has been processed, false if the device identification
				 *          is not equal on the device id in the essage or it is not present
				 */
				bool executeHandler(chaos::common::data::CDataWrapper *message) throw (CException);
			};
		}
    }
}

#endif /* definerExecutionEngine__) */
