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
#ifndef CHAOSFramework_DirectIOTypes_h
#define CHAOSFramework_DirectIOTypes_h

#include <string>
#include <chaos/common/network/CNodeNetworkAddress.h>
namespace chaos {
	namespace common {
		//! Namespace that enclose all direct io infrastructure.
		/*!
			DirectIO infrastructure is an unidirectional double channel that permit to exange data between node. Each
			channel connet two node client -> server. A client can connect to more server to send the same pachet
			to every one of these. On the other side each server can accept connection form more client and receive 
			data form everyone. At this layer data noto managed so waht is done with data is not discussed. Here
			is specified how connection two node together.
		 */
		namespace direct_io {
			
			//! Structure to identify the target server of the client connection
			typedef struct DirectIOConnection {
				std::string										connection_name;
				chaos::common::network::CNodeNetworkAddress		server_address;
			} DirectIOCOnnection;
			
            //! Namespace that enclose all type of data flow rule
			namespace DirectIOConnectionModes {
				typedef enum DirectIOConnectionModes{
					DirectIOPoll,
					DirectIOPush
				} DirectIOConnectionModes;
			}
			
            //! Namespace that enclose all type of spreadest of the data forwarding
			namespace DirectIOConnectionSpreadType {
				typedef enum DirectIOConnectionSpreadType{
					DirectIONoSetting,
					DirectIOFailOver,
					DirectIORoundRobin
				} DirectIOConnectionSpreadType;
			}
            
            //! Namespace that enclose all type of the channel
            namespace DirectIOChannelType {
				typedef enum DirectIOChannelType {
					DirectIODataChannel,
					DirectIOServiceChannel
				} DirectIOChannelType;
			}
            
            //--------------asynchronous memory managment-----------------------
            //!forward decalration
            class DirectIODeallocationHandler;
            
            //!strucutre to maintains the information about sent buffer
            //! and his associated clean handler
            struct DisposeSentMemoryInfo {
                //! specify the part of the datapack to free
                typedef enum SentPart {
                    SentPartHeader,
                    SentPartData
                }SentPart;
                
                //!memory deallcoation handler
                DirectIODeallocationHandler	*data_deallocator;
                //! sent part type
                SentPart	sent_part;
                //! channel opcode
                uint16_t	sent_opcode;
                
                //! defautl constructor
                DisposeSentMemoryInfo(DirectIODeallocationHandler *_data_deallocator,
                                      SentPart _sent_part,
                                      uint16_t _sent_opcode):
                data_deallocator(_data_deallocator),
                sent_part(_sent_part),
                sent_opcode(_sent_opcode){};
            };
            
            //! forward decalration
            class DirectIOForwarder;
            class DirectIOServer;
            
            //! handler for the theallocation of sent data
            class DirectIODeallocationHandler {
                friend class DirectIOServer;
                friend class DirectIOForwarder;
            protected:
                //overriding ofr free object fuunction for the tempalted key object container superclass
                virtual void freeSentData(void* sent_data_ptr, DisposeSentMemoryInfo *free_info_ptr) = 0;
            };
            
#define CLEAN_DIO_DATA_WITH_HANDLER(hndlr, part, opcode, data)\
{\
        DisposeSentMemoryInfo minfo(hndlr, part, opcode);\
            hndlr->freeSentData(data, &minfo);\
}
            
		}
	}
}

#endif
