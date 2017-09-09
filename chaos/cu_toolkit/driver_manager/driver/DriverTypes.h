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

#ifndef CHAOSFramework_DriverGlobal_h
#define CHAOSFramework_DriverGlobal_h

#include <stdint.h>
#include <boost/lockfree/queue.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>

#include <chaos/common/utility/Atomic.h>
#include <chaos/common/thread/TemplatedConcurrentQueue.h>
#ifdef __GNUC__
#ifndef UINT16_MAX
#define UINT16_MAX 65535
#endif
#endif

namespace chaos_thread_ns = chaos::common::thread;

namespace chaos{
    namespace cu {
        namespace driver_manager {
            
            //! The name space that group all foundamental class need by driver implementation on chaos
            namespace driver {

                typedef uintptr_t drvqueuedata_t;
				
				namespace OpcodeType {
					typedef enum {
					  OP_USER=0x0,
					  OP_USER_END=0x0FFF,
					  /// reserved
						OP_INIT_DRIVER=0x1000,
						OP_DEINIT_DRIVER,
						OP_INIT,
						OP_DEINIT,
						OP_SET_BYPASS,
						OP_CLEAR_BYPASS,
						OP_UKNOWN= 0xDEADDEAD
					} Opcode;
				}
                
				//! opcode error return type
				namespace MsgManagmentResultType {
					
					/*!
					 These enum collect all the possible error that can occurs during the submission of the opcodes to the driver
					 */
					typedef enum {
						MMR_EXECUTED = 0,		/**< The opcode has been successfully executed. */
						MMR_THREAD_DISPATCHED,	/**< has been dispatched to the thread (async operation). */
						MMR_ERROR				/**< Opcode operation ha given an error. */
					} MsgManagmentResult;
				}
				
				//! Structure for resq	uest a driver accessor
				/*
					This structure is used by driver manager to create and accessor for the
					requested driver with the info in the struct.
				 */
				typedef struct DrvRequestInfo {
					std::string alias;
					std::string version;
					std::string init_parameter;
				} DrvRequestInfo;
                
                //forward declaration
                struct DrvMsg;
                
                //!Type of the message sent as reposnse form the driver to the accessor
                typedef uint64_t ResponseMessageType;

                
                //typedef boost::interprocess::message_queue drvqueue_t;
                typedef chaos_thread_ns::TemplatedConcurrentQueue<DrvMsg*> DriverQueueType;
                typedef chaos_thread_ns::TemplatedConcurrentQueue<ResponseMessageType> AccessorQueueType;

#define DRVMSG_ERR_MSG_SIZE 255
#define DRVMSG_ERR_DOM_SIZE 255

                    //! Driver message information
                /*!
                 * This structure represent the message that is sent to the driver to perform 
                 * it's work. 
                 */
                typedef struct DrvMsg {
                    ResponseMessageType		id;					/**< The identification of the command (it is used to check the response). */
                    uint16_t				opcode;				/**< The ocode represent the code associated to a determinated command of the driver. */
                    uint16_t				property;			/**< The proprety are additional (optional) feature associated to an opcode */
                    AccessorQueueType		*drvResponseMQ;		/**< this represent the queue whre the command need to sent backward the "id" when the
                                                                     command assocaited to the opcode has ben terminated. */
                    uint32_t				inputDataLength;    /**< the length of the data (input/output) contained into the "data" field. */
                    void					*inputData;         /**< the pointer to the memory containing the data for and from the command */
					uint32_t				resultDataLength;   /**< the length of the data (input/output) contained into the "data" field. */
                    void					*resultData;        /**< the pointer to the memory containing the data for and from the command */
                    int32_t parm[2];                            /**< eventual call parameters es. addr and size, to avoid to encode in inputdata buffer*/
                    int32_t ret;                                /**< eventual return code of the code */
                    char    err_msg[DRVMSG_ERR_MSG_SIZE];       /**< error message */
                    char    err_dom[DRVMSG_ERR_DOM_SIZE];       /**< error domain */
                } *DrvMsgPtr;
                
                //!define a class that can execute driver opcode
                class OpcodeExecutor {
                public:
                    //! Execute a command
                    /*!
                     The driver implementation must use the opcode to recognize the
                     command to execute and then write it on th ememory allocate
                     by the issuer of the command.
                     \param cmd the message that needs to be executed by the driver implementation
                     \return the managment state of the message
                     */
                    virtual MsgManagmentResultType::MsgManagmentResult execOpcode(DrvMsgPtr cmd) = 0;
                };
            }
        }
    }
}

#endif
