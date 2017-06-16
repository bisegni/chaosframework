/*
 *	chaos_errors.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 02/05/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__chaos_errors_h
#define __CHAOSFramework__chaos_errors_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/Singleton.h>

#include <string>
#include <map>
#include <cassert>
namespace chaos{
    namespace error {
        
        CHAOS_DEFINE_MAP_FOR_TYPE(int, std::string, ChaosErrorMap);
        
        //!error mapping helper class
        class ChaosErrorCodeMapping:
        public chaos::common::utility::Singleton<ChaosErrorCodeMapping>{
            friend class chaos::common::utility::Singleton<ChaosErrorCodeMapping>;
        private:
            ChaosErrorCodeMapping();
            ChaosErrorMap map_error_description;
        public:
            
            //!register the message for the an erorr code that need to be unique
            /*!
             if the error code s present an exception is thrown
             */
            void registerErrorDescription(int error_code, const std::string& error_message);
            const std::string& getErrorMessage(int error_code);
        };
    }
    
    
#define CHAOS_DECLARE_ERROR_CODE_MAPPING(e)\
const int e
    //class e ## Register {public:e ## Register(int c, const std::string& m);};\
    //extern e ## Register e ## RegisterInstancer;
    
    
#define CHAOS_DEFINE_ERROR_CODE_MAPPING(e, c, m)
    //const int e = c;\
    //e ## Register  e ## RegisterInstance(c, m);\
    //e ## Register::e ## Register(int _c, const std::string& _m){chaos::error::ChaosErrorCodeMapping::getInstance()->registerErrorDescription(_c,_m);}
    
    
#define ChaosGetErrorMessage(error_code)\
chaos::error::ChaosErrorCodeMapping::getInstance()->getErrorMessage(error_code)
    
    
    /** @defgroup ChaosErrorCode
     *  This is the collection of the definition of the chaos error code
     *  @{
     */
    //! Name space for grupping the definition of the chaos error code
    namespace ErrorCode {
        //!the list of principal chaos error code
        typedef enum {
            //!no error
            EC_NO_ERROR = 0,
            //! rpc timeout
            EC_TIMEOUT = 100,
            //! dataset attribute not found
            EC_ATTRIBUTE_NOT_FOUND, // 101 ...
            //! dataset attribute bad direction
            EC_ATTRIBUTE_BAD_DIR ,
            //!dataset attribute not supported
            EC_ATTRIBUTE_TYPE_NOT_SUPPORTED ,
            //! has been called a not supported method
            //! to be used for instance in driver methods that not are supported in a particular Abstraction
            EC_NODE_OPERATION_NOT_SUPPORTED=-10000,
            
            //!unit server registration is gone well
            EC_MDS_NODE_REGISTRATION_OK = 500,
            //!unit server registration has failed for invalid alias
            EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS,
            //!unit server registration for duplicated alias
            EC_MDS_NODE_REGISTRATION_FAILURE_INSTANCE_ALREADY_RUNNING,
            //! node bad state machine state in response to mds ack event
            EC_MDS_NODE_BAD_SM_STATE,
            //!work unit is not self manageable and need to be loaded within an unit server
            EC_MDS_NODE_ID_NOT_SELF_MANAGEABLE
            
        } ErrorCode;
    }
    /** @} */ // end of ChaosErrorCode
    
    /** @defgroup ChaosRPCErrorCode
     *  This is the collection of the definition of the rpc error code
     *  @{
     */
    //! Name space for grupping the definition of the rpc error code
    namespace ErrorRpcCoce {
        typedef enum {
            //! rpc system has not been able to get a socket
            EC_RPC_NO_SOCKET = -1000,
            //!error occuring udring the forwarding of the data
            EC_RPC_SENDING_DATA = -1001,
            //!error receiving data
            EC_RPC_GETTING_ACK_DATA = -1002,
            EC_RPC_NO_DOMAIN_FOUND_IN_MESSAGE = -1003,
            EC_RPC_NO_ACTION_FOUND_IN_MESSAGE = -1004,
            EC_RPC_NO_DOMAIN_REGISTERED_ON_SERVER = -1005,
            EC_RPC_NO_MORE_SPACE_ON_DOMAIN_QUEUE = -1006,
            EC_RPC_UNMANAGED_ERROR_DURING_FORWARDING = -1007,
            EC_RPC_IMPL_ERR = -1100,
            EC_RPC_CHANNEL_OFFLINE = -1001,
            EC_RPC_REQUEST_FUTURE_NOT_AVAILABLE = -1002
        } ErrorRpcCoce;
        static const std::string EC_RPC_ERROR_DOMAIN               = "RPC Error";
        static const std::string EC_REQUEST_FUTURE_NOT_AVAILABLE   = "No future available";
#define CHAOS_IS_RPC_ERROR_CODE(x)\
((x<= ErrorRpcCoce::EC_RPC_NO_SOCKET) && \
(x>= ErrorRpcCoce::EC_RPC_IMPL_ERR))
        
#define CHAOS_IS_RPC_SERVER_OFFLINE(x)\
((x == ErrorRpcCoce::EC_RPC_GETTING_ACK_DATA) || \
(x == ErrorRpcCoce::EC_RPC_NO_DOMAIN_REGISTERED_ON_SERVER) ||\
(x == ErrorRpcCoce::EC_RPC_UNMANAGED_ERROR_DURING_FORWARDING) ||\
(x == ErrorRpcCoce::EC_RPC_SENDING_DATA) ||\
(x == ErrorRpcCoce::EC_RPC_NO_SOCKET))
    }
    
    /** @defgroup ErrorDirectIOCoce
     *  This is the collection of the definition of the direct_io error code
     *  @{
     */
    //! This is the collection of the definition of the direct_io error code
    namespace ErrorDirectIOCoce {
        typedef enum {
            //! direct io socket not found
            EC_NO_SOCKET = -2000,
        } ErrorDirectIOCoce;
        /** @} */ // end of ErrorDirectIOCoce
    }
}
#endif /* __CHAOSFramework__chaos_errors_h */
