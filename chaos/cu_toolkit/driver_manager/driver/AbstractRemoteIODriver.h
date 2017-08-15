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

#ifndef __CHAOSFramework__CE5788A_1D68_477E_BB9E_55BE8F7D3373_AbstractRemoteIODriver_h
#define __CHAOSFramework__CE5788A_1D68_477E_BB9E_55BE8F7D3373_AbstractRemoteIODriver_h

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#include <chaos/cu_toolkit/external_gateway/ExternalUnitEndpoint.h>

#include <chaos/common/data/CDataVariant.h>
#include <chaos/common/utility/LockableObject.h>

#include <chaos/common/async_central/async_central.h>

#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>

namespace chaos {
    namespace cu {
        namespace driver_manager {
            namespace driver {
                
                //! define the abstraction fo comunicate with a remote driver
                /*!
                 Remote driver is a !CHAOS feature taht permit to a remote process to connect interact with
                 a control unit using an http protocols and work as a driver for that control unit
                 
                 the messages sent to the driver has the follow json scheme:
                 autorization:
                 {
                 authorization_key:string vlaue
                 .....
                 }
                 
                 request:
                 {
                 request_id:int32
                 .....
                 }
                 response:
                 {
                 request_id:int32 <--- the same of the answer
                 .....
                 }
                 
                 message: //not have the request field
                 {
                 .....
                 }
                 */
                class AbstractRemoteIODriver:
                ADD_CU_DRIVER_PLUGIN_SUPERCLASS,
                chaos::cu::external_gateway::ExternalUnitEndpoint,
                chaos::common::async_central::TimerHandler {
                    
                    typedef ChaosPromise< chaos::common::data::CDWShrdPtr > DriverResultPromise;
                    typedef ChaosFuture< chaos::common::data::CDWShrdPtr > DriverResultFuture;
                    
                    
                    typedef enum {
                        RDConnectionPhaseDisconnected,
                        RDConnectionPhaseConnected,
                        RDConnectionPhaseAutorized,
                    } RDConnectionPhase;
                    
                    struct DriverResultInfo {
                        uint32_t request_id;
                        int64_t request_ts;
                        DriverResultPromise promise;
                        typedef ChaosSharedPtr< DriverResultInfo > DriverResultInfoShrdPtr;
                        //!key accessors for multindix infrastructure
                        struct extract_index {
                            typedef uint32_t result_type;
                            const result_type &operator()(const DriverResultInfoShrdPtr &p) const;
                        };
                        
                        struct extract_req_ts {
                            typedef int64_t result_type;
                            const result_type &operator()(const DriverResultInfoShrdPtr &p) const;
                        };
                    };
                    
                    //tag
                    struct tag_req_id{};
                    struct tag_req_ts{};
                    
                    //multi-index set
                    typedef boost::multi_index_container<
                    DriverResultInfo::DriverResultInfoShrdPtr,
                    boost::multi_index::indexed_by<
                    boost::multi_index::ordered_unique<boost::multi_index::tag<tag_req_id>,  DriverResultInfo::extract_index>,
                    boost::multi_index::ordered_unique<boost::multi_index::tag<tag_req_ts>,  DriverResultInfo::extract_req_ts>
                    >
                    > SetPromise;
                    
                    typedef boost::multi_index::index<SetPromise, tag_req_id>::type               SetPromisesReqIdxIndex;
                    typedef boost::multi_index::index<SetPromise, tag_req_id>::type::iterator     SetPromisesReqIdxIndexIter;
                    typedef boost::multi_index::index<SetPromise, tag_req_ts>::type                   SetPromisesReqTSIndex;
                    typedef boost::multi_index::index<SetPromise, tag_req_ts>::type::iterator         SetPromisesReqTSIndexIter;
                    
                    CHAOS_DEFINE_LOCKABLE_OBJECT(SetPromise, LSetPromise);
                    CHAOS_DEFINE_LOCKABLE_OBJECT(std::string, LString);
                    
                    //contains the autorization key that need to be passed by the remote driver
                    std::string         authorization_key;
                    RDConnectionPhase   conn_phase;
                    
                    ChaosAtomic<uint32_t>   message_counter;
                    LString                 current_connection_identifier;
                    //set that contains all promise
                    LSetPromise             set_p;
                    SetPromisesReqIdxIndex& set_p_req_id_index;
                    SetPromisesReqTSIndex&  set_p_req_ts_index;
                    
                    //!initialization and deinitialization driver methods
                    void driverInit(const char *initParameter) throw (chaos::CException);
                    void driverInit(const chaos::common::data::CDataWrapper& init_parameter) throw(chaos::CException);
                    void driverDeinit() throw (chaos::CException);
                public:
                    
                    //!liste of error
                    typedef enum {
                        AR_ERROR_OK = 0,
                        AR_ERROR_NO_CONNECTION,
                        AR_ERROR_NOT_AUTORIZED,
                        AR_ERROR_TIMEOUT,
                        AR_ERROR_LAST = 100
                    } AR_ERROR;
                    
                    AbstractRemoteIODriver();
                    
                    ~AbstractRemoteIODriver();
                    
                    //!Send raw request to the remote driver
                    /*!
                     \param message_data is the raw data to be transmitted to the remote driver
                     \param received_data si the raw data received from the driver
                     */
                    int sendRawRequest(chaos::common::data::CDWUniquePtr message_data,
                                       chaos::common::data::CDWShrdPtr& message_response,
                                       uint32_t timeout = 5000);
                    
                    //!Send raw message to the remote driver
                    /*!
                     \param message_data is the raw data to be transmitted to the remote driver
                     */
                    int sendRawMessage(chaos::common::data::CDWUniquePtr message_data);
                    
                    using ExternalUnitEndpoint::setNumberOfAcceptedConnection;
                    using ExternalUnitEndpoint::getNumberOfAcceptedConnection;
                protected:
                    //! handle called when a new message has been received
                    /*!
                     A new message has been received from the rmeote server in an asyc way. It
                     will be forward to sublcass only if remote server has been successfully registered
                     */
                    virtual int asyncMessageReceived(chaos::common::data::CDWUniquePtr message) = 0;
                    
                    //!send an errore to remote driver
                    int sendError(int error_code,
                                   std::string& error_message,
                                   std::string& error_domain);
                private:
                    //!inherited method by @ExternalUnitEndpoint
                    void handleNewConnection(const std::string& connection_identifier);
                    //!inherited method by @ExternalUnitEndpoint
                    void handleDisconnection(const std::string& connection_identifier);
                    
                    int handleReceivedeMessage(const std::string& connection_identifier,
                                               ChaosUniquePtr<chaos::common::data::CDataWrapper> message);
                    //!inherited from chaos::common::async_central::TimerHandler
                    void timeout();
                    
                    //!
                    void sendAuthenticationACK();
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__CE5788A_1D68_477E_BB9E_55BE8F7D3373_AbstractRemoteIODriver_h */
