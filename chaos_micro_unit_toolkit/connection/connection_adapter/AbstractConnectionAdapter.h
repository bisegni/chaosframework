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

#ifndef __CHAOSFramework_CD2B55D0_1C87_4434_BADC_420C3CB85F42_AbstractProtocolAdapter_h
#define __CHAOSFramework_CD2B55D0_1C87_4434_BADC_420C3CB85F42_AbstractProtocolAdapter_h

#include <chaos_micro_unit_toolkit/data/DataPack.h>
#include <chaos_micro_unit_toolkit/connection/connection_type.h>

#include <string>
#include <queue>
#include <map>

namespace chaos {
    namespace micro_unit_toolkit {
        namespace connection {
            namespace connection_adapter {
                
                //!Conenction state
                typedef enum {
                    //!Conenction is not estabilished
                    ConnectionStateDisconnected,
                    //!a conenction has beein initiated
                    ConnectionStateConnecting,
                    //!te connection has beel estabilished
                    ConnectionStateConnected,
                    //!the remote endpoint has accepted the connection
                    ConnectionStateAccepted,
                    //!connection has not been accepted by remote endpoint
                    ConnectionStateNotAccepted,
                    //!the is an error on connection
                    ConnectionStateConnectionError
                } ConnectionState;
                
                
                //! Abstract base class for all protocols adapter
                class AbstractConnectionAdapter {
                protected:
                    virtual int sendRawMessage(chaos::micro_unit_toolkit::data::DataPackUniquePtr& message) = 0;
                    
                    void handleReceivedMessage(chaos::micro_unit_toolkit::data::DataPackSharedPtr& received_message);
                public:
                    const std::string   protocol_endpoint;
                    const std::string   protocol_option;
                    AbstractConnectionAdapter(const std::string& _connection_endpoint,
                                            const std::string& _protocol_option);
                    virtual ~AbstractConnectionAdapter();
                    
                    //! start the connection
                    virtual int connect() = 0;
                    
                    //!execute al pending operation and waith a number milliseconds for incoming message
                    /*!
                     this need to be implemented by sublcass for executing hi neccssary action to send queued message and wait
                     for incoming one for a determinated period of time.
                     */
                    virtual void poll(int32_t milliseconds_wait = 100) = 0;
                    
                    //!close the previous estabilished connection
                    virtual int close() = 0;
                    
                    //!Return the state of connection
                    /*!
                     \return a value of @ConnectionState thaat represent the current
                     state of the connection
                     */
                    const ConnectionState& getConnectionState() const;
                    
                    //!send a message to remote enpoint
                    /*!
                     \param message that will be sent to the remote endpoint
                     */
                    int sendMessage(data::DataPackUniquePtr& message);
                    
                    //!send a request to the remote endpoint
                    /*!
                     \param message that will be sent to the remote endpoint
                     \param request_id the id associated to the new request
                     */
                    int sendRequest(data::DataPackUniquePtr& message,
                                    uint32_t& request_id);
                    
                    //!check if there are message from remote endpoint
                    /*!
                     The messages rceived are insert into a queue so all message
                     are ordered as the have been received
                     \retur true if there are messages in the queue
                     */
                    bool hasMoreMessage();
                    
                    //!return the message of top of the queue
                    data::DataPackSharedPtr getNextMessage();
                    
                    //! Chekc if there are answer by remote endpoint
                    /*!
                     \retur true if there are answer in the queue
                     */
                    bool hasResponse();
                    
                    //!Check if there is a response for a determinated id
                    /*!
                     \param request_id is the id of the request received
                     after  the call of the method sendRequest
                     */
                    bool hasResponseAvailable(uint32_t request_id);
                    
                    //!return a response for a determinated id
                    /*!
                     \param request_id the id associated to a request
                     */
                    data::DataPackSharedPtr retrieveRequestResponse(uint32_t request_id);
                protected:
                    //!define the maps for the request response
                    typedef std::map<uint32_t, data::DataPackSharedPtr> MapRequestIDResponse;
                    typedef std::pair<uint32_t, data::DataPackSharedPtr> MapRequestIDResponsePair;
                    typedef MapRequestIDResponse::iterator MapRequestIDResponseIterator;

                    //!the counter of the request id
                    uint32_t adapter_request_id;
                    //!keep trakc of the current status of the conenction @ConnectionState
                    ConnectionState connection_status;
                    //!define the map that asscoiate the request id to the reponse
                    MapRequestIDResponse                map_req_id_response;
                    //!contains the received message in the order as the have been received
                    std::queue<data::DataPackSharedPtr> queue_received_messages;
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_CD2B55D0_1C87_4434_BADC_420C3CB85F42_AbstractProtocolAdapter_h */
