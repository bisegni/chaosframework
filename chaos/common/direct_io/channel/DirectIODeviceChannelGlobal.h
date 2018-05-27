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
//#include <arpa/inet.h>
#include <chaos/common/chaos_types.h>
#include <chaos/common/data/CDataWrapper.h>

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
                        DeviceChannelOpcodeQueryDataCloud			= 4,	/**< query the chaos data associated to a key [synchronous]*/
                        DeviceChannelOpcodeDeleteDataCloud			= 8,    /**< delete the data associated with a key [synchronous]*/
                        DeviceChannelOpcodePutHeathData				= 16,	/**< send the health dataset [synchronous]*/
                        DeviceChannelOpcodeMultiGetLastOutput       = 32	/**< request the last output dataset from live cache for a set of key[synchronous]*/
                    } DeviceChannelOpcode;
                    
                    /*!
                     \enum PerformanceChannelOpcode
                     Opcode used by the DirectIO device channel
                     */
                    typedef enum PerformanceChannelOpcode {
                        PerformanceChannelOpcodeReqRoundTrip			= 1,	/**< forwarda a start of a roundtrip test */
                        PerformanceChannelOpcodeRespRoundTrip			= 2     /**< perform an answer to the roundtrip test */
                    } PerformanceChannelOpcode;
                    
                    /*!
                     \enum DeviceChannelOpcode
                     Opcode used by the DirectIO system api channel
                     and collect all API for system managment
                     */
                    typedef enum SystemAPIChannelOpcode {
                        SystemAPIChannelOpcodeEcho                      = 1,    /**< perfome an echo for testin purphose*/
                        SystemAPIChannelOpcodeNewSnapshotDataset		= 2,	/**< start new datasets snapshot creation process*/
                        SystemAPIChannelOpcodeDeleteSnapshotDataset		= 3,	/**< delete the snapshot associated to the input tag */
                        SystemAPIChannelOpcodeGetSnapshotDatasetForAKey	= 4,    /**< return the snapshoted datasets for a determinated producer key*/
                        SystemAPIChannelOpcodePushLogEntryForANode      = 5     /**< persist log entry for a node within chaos backend*/
                    } SystemAPIChannelOpcode;
                }
                
                /*!
                 Parameter for the query used in CDataWrapper structure
                 */
                namespace DeviceChannelOpcodeQueryDataCloudParam {
                    //!is the timestamp for wich we want to search our key
                    static const char * const QUERY_PARAM_STAR_TS_I64                   = "qp_data_cloud_start_ts";
                    //!is the timestamp that close the time offset of the search [packet that are <= are included into the result]
                    static const char * const QUERY_PARAM_END_TS_I64                    = "qp_data_cloud_end_ts";
                    //!is the node unique id for wich we whant the results
                    static const char * const QUERY_PARAM_SEARCH_KEY_STRING             = "qp_data_cloud_key";
                    //identify the last run id found
                    static const char * const QUERY_PARAM_SEARCH_LAST_RUN_ID            = "qp_data_cloud_last_run_id";
                    //if true the data pack will be '>=' otherwhise '>' in timestamp respect to qp_data_cloud_start_ts key
                    static const char * const QUERY_PARAM_SEARCH_LAST_DP_COUNTER		= "qp_data_cloud_last_dp_counter";
                }
                
                //! Name space for grupping the varius headers for every DeviceChannelOpcode
                namespace opcode_headers {
                    /*!
                     base result header to standardize base result for
                     those api that return something in an synchronous way
                     */
                    typedef struct DirectIOChannelBaseResultHeader {
                        int32_t err;
                    }DirectIOChannelBaseResultHeader;
                    
                    //-----------------------------------DEVICE CHANNEL--------------------------------
#define	GET_PUT_OPCODE_FIXED_PART_LEN	2
                    //!macro used to get pointer to the start of the key data
#define GET_PUT_OPCODE_KEY_PTR(h) (void*)((char*)h+2)
                    
                    //! Header for the DeviceChannelOpcodePutOutput[WithCache] opcodes
                    typedef struct DirectIODeviceChannelHeaderPutOpcode {
                        //! The 8 bit tag field
                        uint8_t tag;
                        //! The 8 bit key length field
                        uint8_t key_len;
                        //the pointer to key data
                        void*   key_data;
                    } DirectIODeviceChannelHeaderData,
                    *DirectIODeviceChannelHeaderDataPtr;
                    
                    //#define GET_OPCODE_HEADER_LEN 14
                    //! Header for the DirectIODeviceChannelHeaderGetOpcode opcode
                    /*!
                     this is the header for request the last output channel dataset
                     found on shared dataproxy cache. The key of the item to search
                     must be sent in the data part of the message
                     */
                    typedef	union DirectIODeviceChannelHeaderGetOpcode {
                        //raw data representation of the header
                        char raw_data[16];
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
                        } field;
                    } DirectIODeviceChannelHeaderGetOpcode,
                    *DirectIODeviceChannelHeaderGetOpcodePtr;
                    
                    //! Header for the DirectIODeviceChannelHeaderMultiGetOpcode opcode
                    /*!
                     this is the header for request the last output channel for more then one key
                     */
                    typedef	union DirectIODeviceChannelHeaderMultiGetOpcode {
                        //raw data representation of the header
                        char raw_data[2];
                        struct header {
                            uint16_t number_of_key;
                        } field;
                    } DirectIODeviceChannelHeaderMultiGetOpcode,
                    *DirectIODeviceChannelHeaderMultiGetOpcodePtr;
                    
                    //! Header for DirectIODeviceChannelHeaderGetOpcode asynchronous result
                    /*!
                     the found data is sent as data part of direct io protocol
                     */
                    typedef struct DirectIODeviceChannelHeaderGetOpcodeResult {
                        //! The lenght of found data
                        uint32_t value_len;
                    } DirectIODeviceChannelHeaderGetOpcodeResult,
                    *DirectIODeviceChannelHeaderGetOpcodeResultPtr;
                    
                    //! Header for DirectIODeviceChannelHeaderMultiGetOpcode asynchronous result
                    /*!
                     the header contains the number for result that are found
                     every result in encoded in a bson way sso every record as the lent at the start,
                     if no data is found for a key an empty bson is returned. For each key a bson
                     document is returned in the same order of key in the set
                     */
                    typedef struct DirectIODeviceChannelHeaderMultiGetOpcodeResult {
                        //! The lenght of found data
                        uint32_t number_of_result;
                    } DirectIODeviceChannelHeaderMultiGetOpcodeResult,
                    *DirectIODeviceChannelHeaderMultiGetOpcodeResultPtr;
                    
                    //#define QUERY_DATA_CLOUD_OPCODE_HEADER_LEN 4
                    //! Header for the DirectIODeviceChannelHeaderOpcodeQueryDataCloud opcode
                    /*!
                     this is the header for query on data cloud. The header contains information to
                     execute the query in a synchronous way. Every query need to be delimited into
                     a time offset [start-end times stamp]
                     */
                    typedef	union DirectIODeviceChannelHeaderOpcodeQueryDataCloud {
                        //raw data representation of the header
                        char raw_data[4];
                        struct header {
                            //the maximum number of record that need to have the result page
                            uint32_t    record_for_page;
                        } field;
                    } DirectIODeviceChannelHeaderOpcodeQueryDataCloud,
                    *DirectIODeviceChannelHeaderOpcodeQueryDataCloudPtr;
                    
                    //!page result
                    CHAOS_DEFINE_VECTOR_FOR_TYPE(ChaosSharedPtr<chaos::common::data::CDataWrapper>, QueryResultPage);
                    
                    //structure for identificate the sequnce fo the found record
                    typedef struct SearchSequence {
                        int64_t run_id;
                        int64_t datapack_counter;
                    }SearchSequence;
                    
                    //! Header for DirectIODeviceChannelHeaderGetOpcode asynchronous result
                    /*!
                     the found data is sent as data part of direct io protocol
                     */
                    typedef struct DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult {
                        //! The size of returned data
                        uint32_t result_data_size;
                        //! The numer of element found
                        uint32_t numer_of_record_found;
                        //!last sequence found
                        SearchSequence last_found_sequence;
                    } DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult,
                    *DirectIODeviceChannelHeaderOpcodeQueryDataCloudResultPtr;
                    
                    //! resutl structure that hase the
                    typedef struct DirectIODeviceChannelOpcodeQueryDataCloudResult {
                        DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult header;
                        char *results;
                    } DirectIODeviceChannelOpcodeQueryDataCloudResult,
                    *DirectIODeviceChannelOpcodeQueryDataCloudResultPtr;
                    
                    //-----------------------------------PERFORMANCE CHANNEL--------------------------------
                    //#define PERFORMANCE_CHANNEL_ROUND_TRIP_HEADER_LEN 16
                    
                    //! Header for the DirectIOPerformanceChannelHeaderOpcodeRoundTrip opcode
                    /*!
                     this is the header for request the last output channel dataset
                     found on shared dataproxy cache. The key of the item to search
                     must be sent in the data part of the message
                     */
                    typedef	union DirectIOPerformanceChannelHeaderOpcodeRoundTrip {
                        //raw data representation of the header
                        char raw_data[16];
                        struct header {
                            //! The 64bit value for the timestamp get from the client part at the start of the roundtrip test
                            uint64_t	start_rt_ts;
                            //! The 64bit value for the timestamp get from the server layer part at the start of the roundtrip test
                            uint64_t	receiver_rt_ts;
                        } field;
                    } DirectIOPerformanceChannelHeaderOpcodeRoundTrip,
                    *DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr;
                    
                    //-----------------------------------SYSTEM CHANNEL--------------------------------
                    //#define SYSTEM_API_CHANNEL_NEW_Snapshot 256+4+4
                    
                    //! Header for the snapshot system api managment for new, delete and get managment
                    /*!
                     this header is used for the managment of the creation, deletion and retrieve
                     of a snapshot
                     the opcode associated to this header is:
                     - SystemAPIChannelOpcodeNewNewSnapshotDataset
                     - SystemAPIChannelOpcodeNewDeleteSnapshotDataset
                     - SystemAPIChannelOpcodeGetSnapshotDatasetForAKey
                     */
                    typedef	union DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader {
                        //raw data representation of the header
                        char raw_data[256+4+4];
                        struct header {
                            //!is the snapshot name
                            char		snap_name[256];
                            
                            //! the type of channel
                            uint32_t	channel_type;
                            
                            //! is the lenght of comma separated list of the
                            //! producer to include on the snapshot, if it is
                            //! empty all the producer are snapped. The list is
                            //! passed into the data part of the direct io message
                            uint32_t	producer_key_set_len;
                        } field;
                    } DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader,
                    *DirectIOSystemAPIChannelOpcodeNDGSnapshotHeaderPtr;
                    
                    /*!
                     this header is used for the managment of the creation, deletion and retrieve
                     of a snapshot
                     the opcode associated to this header is:
                     - SystemAPIChannelOpcodeNewNewSnapshotDataset
                     - SystemAPIChannelOpcodeNewDeleteSnapshotDataset
                     - SystemAPIChannelOpcodeGetSnapshotDatasetForAKey
                     */
                    typedef  struct DirectIOSystemAPISnapshotResultHeader {
                        uint32_t    channel_data_len;
                        int32_t		error;
                        char		error_message[256];
                    }DirectIOSystemAPISnapshotResultHeader,
                    *DirectIOSystemAPISnapshotResultHeaderPtr;
                    
                    //!header user for push log entries for a node
                    /*!
                     
                     data fragment:
                        -name size
                        -name
                        for each entry:
                            -entry size
                            -entry
                     
                     the data field for this opcode contains the name and the log entries 
                     concatenated together. The name is mandatory and it is delimited by
                     null characted, the number of entries is managed by this header and
                     starts after name null characted and each entry is terminated by null 
                     characted
                     the opcode associated to this header is:
                     - SystemAPIChannelOpcodePushLogEntryForANode
                     */
                    typedef	union DirectIOSystemAPIChannelOpcodePushLogEntryForANodeHeader {
                        char raw_data[sizeof(uint32_t)];
                        struct header {
                            //! the number of entries name+log
                            uint32_t	data_entries_num;
                        } field;
                    } DirectIOSystemAPIChannelOpcodePushLogEntryForANodeHeader,
                    *DirectIOSystemAPIChannelOpcodePushLogEntryForANodeHeaderPtr;
                    
#define GET_SYSTEM_API_GET_SNAPSHOT_RESULT_BASE_PTR(h) ((char*)h+sizeof(chaos::common::direct_io::channel::opcode_headers::DirectIOSystemAPISnapshotResult)+4)
                    //!result of the new and delete api
                    typedef  struct DirectIOSystemAPIGetDatasetSnapshotResult {
                        //! api result
                        DirectIOSystemAPISnapshotResultHeader api_result;
                        
                        //!channel data
                        void* channel_data;
                    }DirectIOSystemAPIGetDatasetSnapshotResult,
                    *DirectIOSystemAPIGetDatasetSnapshotResultPtr;
                }
            }
        }
    }
}

#endif
