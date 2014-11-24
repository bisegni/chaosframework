//
//  DirectIODeviceChannelGlobal.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 18/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef CHAOSFramework_DirectIODeviceChannelGlobal_h
#define CHAOSFramework_DirectIODeviceChannelGlobal_h

#include <string>
#include <stdint.h>
#include <arpa/inet.h>

namespace chaos {
	namespace common {
		namespace direct_io {
			namespace channel {
				
				namespace opcode {
                    
					/*!
                     \enum DeviceChannelOpcode
                     Opcode used by the DirectIO device channel
					 */
					typedef enum DeviceChannelOpcode {
						DeviceChannelOpcodePutOutput				= 1,	/**< send the output dataset [synchronous]*/
						DeviceChannelOpcodeGetLastOutput            = 2,	/**< request the last output dataset from live cache [synchronous]*/
						DeviceChannelOpcodeQueryDataCloud			= 4,	/**< query the chaos data cloud [async]*/
						DeviceChannelOpcodeQueryDataCloudStartResult= 8,	/**< Start the answering sequence to the endpoint [async]*/
						DeviceChannelOpcodeQueryDataCloudResult		= 16,	/**< contain the answer of the query [async]*/
						DeviceChannelOpcodeQueryDataCloudEndResult	= 24	/**< End the answering sequence [async]*/
					} DeviceChannelOpcode;
					
					/*!
                     \enum PerformanceChannelOpcode
                     Opcode used by the DirectIO device channel
					 */
					typedef enum PerformanceChannelOpcode {
						PerformanceChannelOpcodeReqRoundTrip			= 1,	/**< forwarda a start of a roundtrip test */
						PerformanceChannelOpcodeRespRoundTrip			= 2,	/**< perform an answer to the roundtrip test */
					} PerformanceChannelOpcode;
					
					/*!
					 \enum DeviceChannelOpcode
					 Opcode used by the DirectIO system api channel
					 and collect all API for system managment
					 */
					typedef enum SystemAPIChannelOpcode {
						SystemAPIChannelOpcodeNewNewSnapshotDataset	= 1,		/**< start new datasets snapshot creation process*/
						SystemAPIChannelOpcodeNewDeleteSnapshotDataset	= 2,	/**< delete the snapshot associated to the input tag */
					} SystemAPIChannelOpcode;
				}
				
				/*!
				 Parameter for the query used in CDataWrapper structure
				 */
				namespace DeviceChannelOpcodeQueryDataCloudParam {
					static const char * const QUERY_PARAM_STAR_TS_I64			= "qp_data_cloud_start_ts";
					static const char * const QUERY_PARAM_END_TS_I64			= "qp_data_cloud_end_ts";
					static const char * const QUERY_PARAM_SEARCH_KEY_STRING		= "qp_data_cloud_key";
				}
                
                //! Name space for grupping the varius headers for every DeviceChannelOpcode
                namespace opcode_headers {
					//-----------------------------------DEVICE CHANNEL--------------------------------
#pragma mark Device Channel
#define	GET_PUT_OPCODE_FIXED_PART_LEN	2
					//!macro used to get pointer to the start of the key data
#define GET_PUT_OPCODE_KEY_PTR(h) (void*)((char*)h+2)
					
                    //! Heder for the DeviceChannelOpcodePutOutput[WithCache] opcodes
                    typedef struct DirectIODeviceChannelHeaderPutOpcode {
						//! The 8 bit tag field
						uint8_t tag;
						//! The 8 bit key length field
                        uint8_t key_len;
						//the pointer to key data
						void*   key_data;
                    } DirectIODeviceChannelHeaderData,
					*DirectIODeviceChannelHeaderDataPtr;
					
#define GET_OPCODE_HEADER_LEN 14
                    //! Header for the DeviceChannelOpcodeGetOutputFromCache opcode
					/*!
					 this is the header for request the last output channel dataset
					 found on shared dataproxy cache. The key of the item to search
					 must be sent in the data part of the message
					 */
                    typedef	union DirectIODeviceChannelHeaderGetOpcode {
                        //raw data representation of the header
                        char raw_data[GET_OPCODE_HEADER_LEN];
                        struct header {
							//! The endpoint where the channel is published
                            uint16_t	endpoint;
							//! The priority port value for the device that we need to get
                            uint16_t	p_port;
							//! The priority port value for the device that we need to get
                            uint16_t	s_port;
							//! The 32bit representation for the ip where send the answer
                            uint64_t	address;
                        } field;
                    } DirectIODeviceChannelHeaderGetOpcode,
					*DirectIODeviceChannelHeaderGetOpcodePtr;
					
#define QUERY_DATA_CLOUD_OPCODE_HEADER_LEN 24
					//! Header for the DirectIODeviceChannelHeaderOpcodeQueryDataCloud opcode
					/*!
					 this is the header for query on data cloud. The header contains information
					 for answer to a determinated ip:sport:pport:end:endpoint respecting the
					 directIO formalism.
					 */
					typedef	union DirectIODeviceChannelHeaderOpcodeQueryDataCloud {
						//raw data representation of the header
						char raw_data[QUERY_DATA_CLOUD_OPCODE_HEADER_LEN];
						struct header {
							//! The endpoint where the channel is published
							uint16_t	endpoint;
							//! The priority port value for the device that we need to get
							uint16_t	p_port;
							//! The priority port value for the device that we need to get
							uint16_t	s_port;
							//! padding
							uint16_t	unused;
							//! The 32bit representation for the ip where send the answer
							uint64_t	address;
							//! is the query id associated to the request
							char		query_id[8];
						} field;
					} DirectIODeviceChannelHeaderOpcodeQueryDataCloud,
					*DirectIODeviceChannelHeaderOpcodeQueryDataCloudPtr;

					//! bring the metadata infromation of the query result
					typedef struct QueryResultMetadata {
						//! is the number of total element found by query
						uint64_t number_of_element_found;
					} QueryResultMetadata;
					
#define QUERY_DATA_CLOUD_START_RESULT_OPCODE_HEADER_LEN 16
					//! Header for the DeviceChannelOpcodeQueryDataCloudStartAnswer opcode
					/*!
					 This header bring information about the initialization of the answer to
					 a query.
					 */
					typedef	union DirectIODeviceChannelHeaderOpcodeQueryDataCloudStartResult {
						//raw data representation of the header
						char raw_data[QUERY_DATA_CLOUD_START_RESULT_OPCODE_HEADER_LEN];
						struct header {
							//! is the query id relative to the request
							char		query_id[8];
							
							//!the number of total element found for query id
							uint64_t	total_element_found;
						} field;
					} DirectIODeviceChannelHeaderOpcodeQueryDataCloudStartResult,
					*DirectIODeviceChannelHeaderOpcodeQueryDataCloudStartResultPtr;

#define QUERY_DATA_CLOUD_RESULT_OPCODE_HEADER_LEN 24
					//! Header for the DirectIODeviceChannelHeaderOpcodeQueryDataCloud opcode
					/*!
					 this is the header for query on data cloud. The header contains information
					 for answer to a determinated ip:sport:pport:end:endpoint respecting the
					 directIO formalism.
					 */
					typedef	union DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult {
						//raw data representation of the header
						char raw_data[QUERY_DATA_CLOUD_RESULT_OPCODE_HEADER_LEN];
						struct header {
							//! is the query id relative to the request
							char		query_id[8];

							//! the number, relative to the total, of the current element
							uint64_t	element_index;
						} field;
					} DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult,
					*DirectIODeviceChannelHeaderOpcodeQueryDataCloudResultPtr;
					
#define QUERY_DATA_CLOUD_END_RESULT_OPCODE_HEADER_LEN 24
					//! Header for the DeviceChannelOpcodeQueryDataCloudEndAnswer opcode
					/*!
					 This header bring information about the initialization of the answer to
					 a query.
					 */
					typedef	union DirectIODeviceChannelHeaderOpcodeQueryDataCloudEndResult {
						//raw data representation of the header
						char raw_data[QUERY_DATA_CLOUD_END_RESULT_OPCODE_HEADER_LEN];
						struct header {
							//! is the query id relative to the request
							char	query_id[8];
							
							//! error occured ifthere was one
							int32_t	error;
							
							//!string error message passed as data(if there is one)
							uint32_t error_message_length;
						} field;
					} DirectIODeviceChannelHeaderOpcodeQueryDataCloudEndResult,
					*DirectIODeviceChannelHeaderOpcodeQueryDataCloudEndResultPtr;
					
					//-----------------------------------PERFORMANCE CHANNEL--------------------------------
#pragma mark Performance Channel
#define PERFORMANCE_CHANNEL_ROUND_TRIP_HEADER_LEN 16

					//! Header for the DirectIOPerformanceChannelHeaderOpcodeRoundTrip opcode
					/*!
					 this is the header for request the last output channel dataset
					 found on shared dataproxy cache. The key of the item to search
					 must be sent in the data part of the message
					 */
                    typedef	union DirectIOPerformanceChannelHeaderOpcodeRoundTrip {
						//raw data representation of the header
                        char raw_data[PERFORMANCE_CHANNEL_ROUND_TRIP_HEADER_LEN];
                        struct header {
							//! The 64bit value for the timestamp get from the client part at the start of the roundtrip test
                            uint64_t	start_rt_ts;
							//! The 64bit value for the timestamp get from the server layer part at the start of the roundtrip test
                            uint64_t	receiver_rt_ts;
                        } field;
					} DirectIOPerformanceChannelHeaderOpcodeRoundTrip,
					*DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr;
					
					//-----------------------------------SYSTEM CHANNEL--------------------------------
#pragma mark System Channel
#define SYSTEM_API_CHANNEL_NEW_Snapshot 256+4

					//! Header for the snapshot system api managment
					/*!
					 this header is usedfor the managment of the creation 
					 of a new snapshot 
					 the opcode associated to this header is:
					 SystemAPIChannelOpcodeNewSnapshotDatasetNew
					 */
					typedef	union DirectIOSystemAPIChannelOpcodeNewSnapshotHeader {
						//raw data representation of the header
						char raw_data[SYSTEM_API_CHANNEL_NEW_Snapshot];
						struct header {
							//!is the snapshot name
							char		snap_name[256];
							//! is the lenght of comma separated list of the
							//! producer to include on the snapshot, if it is
							//! empty all the producer are snapped. The list is
							//! passed into the data part of the direct io message
							uint32_t	producer_key_set_len;
						} field;
					} DirectIOSystemAPIChannelOpcodeNewSnapshotHeader,
					*DirectIOSystemAPIChannelOpcodeNewSnapshotHeaderPtr;
					
					//!result of the new and delete api
					typedef  struct DirectIOSystemAPISnapshotResult {
							int32_t		error;
							char		error_message[256];
					}DirectIOSystemAPISnapshotResult,
					*DirectIOSystemAPISnapshotResultPtr;
					
					//!result of the new and delete api
					typedef  struct DirectIOSystemAPIGetDatasetSnapshotResult {
						//! api result
						DirectIOSystemAPISnapshotResult api_result;
						
						//channels
						data::CDataWrapper *output_channel;
						data::CDataWrapper *input_channel;
						data::CDataWrapper *custom_channel;
						data::CDataWrapper *system_channel;
					}DirectIOSystemAPIGetDatasetSnapshotResult,
					*DirectIOSystemAPIGetDatasetSnapshotResultPtr;
                }
			}
		}
	}
}

#endif
