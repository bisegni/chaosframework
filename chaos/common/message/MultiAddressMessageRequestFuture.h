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
#ifndef __CHAOSFramework__MultiAddressMessageRequestFuture__
#define __CHAOSFramework__MultiAddressMessageRequestFuture__

#include <chaos/common/message/MessageRequestFuture.h>

namespace chaos{
    namespace common{
        namespace message{
            class MultiAddressMessageChannel;
            
            //! Future class that manage the HA waiting the result of a request for an MultiAddressMessageChannel
            /*!
             this future class embed the real future class givin calling the send request of a channel. If there is any send error
             or timeout while we are waiting a requets, another remote node is used and a new future for this is received.
             The HA is interelly achieved by this future class.
             NOTE: This class is no more valid after that the MultiAddressMessageChannel from which has been created. If used after parent deallcoation
             memory crash will occur.
             */
            class MultiAddressMessageRequestFuture {
                //friend class for private constructor
                friend class chaos::common::message::MultiAddressMessageChannel;
                
                //global timeout
                bool auto_retry;
                int32_t timeout_in_milliseconds;
                
                //! point to the parent channel
                MultiAddressMessageChannel *parent_mn_message_channel;
                //!information for the request for the retransmission
                std::string last_used_address;
                std::string action_domain;
                std::string action_name;
                chaos::common::data::CDWUniquePtr message_pack;
                
                //! point to the current future
                ChaosUniquePtr<MessageRequestFuture> current_future;
                
                //!private constructor
                /*!
                 the ownership of the message pack to forward is kept by this class. To get the ownership detachResult)
                 methdo need to be called
                 */
                MultiAddressMessageRequestFuture(chaos::common::message::MultiAddressMessageChannel *_parent_mn_message_channel,
                                                 const std::string &_action_domain,
                                                 const std::string &_action_name,
                                                 chaos::common::data::CDWUniquePtr _message_pack,
                                                 int32_t _timeout_in_milliseconds = 500 /*RpcConfigurationKey::GlobalRPCTimeoutinMSec*/);
                inline void retryOfflineServer();
                inline bool switchOnOtherServer();
            public:
                //!private destructor
                ~MultiAddressMessageRequestFuture();
                //set the global timeout
                /*!
                 if the timeout is reached and no data is received,
                 the current server is put offline.
                 */
                void setTimeout(int32_t _timeout_in_milliseconds);
                
                //! wait until data is received
                /*!
                 * The wait for an aswer of for the reached timeout
                 * \return true if something has been received
                 */
                bool wait();
                
                void setAutorRetry(bool _auto_retry);
                
                //! try to get the result waiting for a determinate period of time
                chaos::common::data::CDataWrapper *getResult();
                
                
                chaos::common::data::CDWUniquePtr detachResult();
                
                int getError() const;
                
                const std::string &getErrorDomain() const;
                
                const std::string &getErrorMessage() const;
                
                //!return the ownership of the message to the caller
                //chaos::common::data::CDWUniquePtr detachMessageData();
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__MultiAddressMessageRequestFuture__) */
