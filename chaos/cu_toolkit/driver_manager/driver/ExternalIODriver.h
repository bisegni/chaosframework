/*
 *	ExternalIODriver.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 31/07/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__061D444_6ECF_448C_8D63_98139B76FA9D_ExternalIODriver_h
#define __CHAOSFramework__061D444_6ECF_448C_8D63_98139B76FA9D_ExternalIODriver_h

#include <chaos/cu_toolkit/driver_manager/driver/AbstractRemoteIODriver.h>

namespace chaos {
    namespace cu {
        namespace driver_manager {
            namespace driver {
                
                class ExternalIODriver:
                public AbstractRemoteIODriver {
                public:
                    //!define the type of the message that can be sent to remote driver
                    typedef enum {
                        MessageTypeMetadataGetDeviceList = 0,
                        MessageTypeMetadataGetDeviceDataset,
                        MessageTypeVariableWrite,
                        MessageTypeVariableRead,
                        MessageTypeError
                    } MessageType;
                    
                    
                    //!liste of error
                    typedef enum {
                        IO_ERROR_INVALID_MESSAGE_TYPE = AR_ERROR_LAST,
                        IO_ERROR_INVALID_MESSAGE_STRUCTURE,
                        IO_ERROR_NO_CACHED_DEVICE_AT_INDEX,
                        IO_ERROR_NO_CACHED_VARIABLE_TYPE,
                        IO_ERROR_NO_CACHED_VARIABLE_INDEX
                    } IO_ERROR;
                    
                    //!define the type of the variable hosted by the driver for every managed device
                    typedef enum {
                        VariableOpRead      = 0,
                        VariableOpWrite     = 1,
                        VariableOpListen    = 2
                    } VariableOperativity;
                    
                    //!define the types of a variabl eof a device
                    typedef enum {
                        //!variable that permit to configurate an aspect of he device
                        VariableTypeConfiguration = 0,
                        //! a type of variabl ethat permit to check a determinate aspect of a stete of the device
                        VariableTypeStateCheck,
                        //! a variable that permit to change an aspect of the device
                        VariableTypeStateChange
                    } VariableType;
                    
                    
                    //! define the description of the variable of a device
                    /*!
                     the index to find the variable witin the driver is,
                     device_index, var type, var_index.
                     */
                    typedef struct DeviceVariable {
                        //!the name of the variable
                        std::string name;
                        //!the descirption of the variable
                        std::string description;
                        //!specify the type of the variable, how it can interact with asepct of the device
                        VariableType type;
                        //!specify what operation can be requested for the specified variable
                        //can be and concatenation fo the VariableOperativity values
                        int32_t admit_operation;
                        
                        
                        DeviceVariable(){}
                        DeviceVariable(DeviceVariable& src):
                        name(src.name),
                        description(src.description),
                        type(src.type),
                        admit_operation(src.admit_operation){}
                        
                    } DeviceVariable;
                    
                    CHAOS_DEFINE_VECTOR_FOR_TYPE(DeviceVariable, VectorDeviceVariable);
                    
                    typedef struct {
                        //!define the name of the device
                        std::string name;
                        //!define the version of the firmware if available
                        std::string firmware_v;
                        //!is the list of exposed variable
                        VectorDeviceVariable variables;
                        
                        void clear() {
                            name.clear();
                            firmware_v.clear();
                            variables.clear();
                        }
                    } Device;
                    
                    ExternalIODriver();
                    
                    ~ExternalIODriver();
                    
                    //!Get the device list from remote driver
                    /*!
                     Get the list of managed device asking it to the driver
                     \param hosted_device the list of device that are controlled by the remote driver
                     \param timout time waited for the answere
                     */
                    int getDeviceList(ChaosStringSet& hosted_device,
                                      uint32_t timeout = 5000);
                    
                    //!Request the dataset to the driver
                    /*!
                     Message is composed and sent to the remote driver. The answer is wait
                     for the maximum number of milliseconds the data receive returned
                     \param force_update_cache, for the cache to be update with the remote driver contents fro the s
                     needed device
                     */
                    int getDeviceDescription(const int32_t& device_index,
                                             Device& device,
                                             bool force_update_cache = false,
                                             uint32_t timeout = 5000);
                    
                    //!read a variable from the driver
                    /*!
                     read the value form the internal cache if neede an update is requested, and internal cache updated
                     \param device_index is th eindex of the device that own the variable
                     \param var_type is the type of the variable
                     \param var_index is hte index of the variable
                     \param value filled with the value read from the remote driver
                     \param timeout number of milliseconds and returned (if one).
                     */
                    int readVariable(const uint32_t device_index,
                                     const VariableType var_type,
                                     const uint32_t var_index,
                                     chaos::common::data::CDataVariant& value);
                    
                    //!request an update of a variable
                    /*!
                     Request an update for the variable value and internal cache is updated
                     \param device_index is th eindex of the device that own the variable
                     \param var_type is the type of the variable
                     \param var_index is hte index of the variable
                     \param value filled with the value read from the remote driver
                     \param timeout number of milliseconds and returned (if one).
                     */
                    int updateVariable(const uint32_t device_index,
                                       const VariableType var_type,
                                       const uint32_t var_index,
                                       uint32_t timeout = 5000);
                    
                    //!update a device variable value into the driver
                    /*!
                     New value of the driver is forward to the driver, an answer is received to
                     confirm the result of the oepration
                     \param device_index is th eindex of the device that own the variable
                     \param var_type is the type of the variable
                     \param var_index is hte index of the variable
                     \param value sent to the driver for update the variable
                     \param timeout number of milliseconds and returned (if one).
                     */
                    int writeVariable(const uint32_t device_index,
                                      VariableType var_type,
                                      const uint32_t var_index,
                                      const chaos::common::data::CDataVariant& value,
                                      bool wait_confirmation = false,
                                      uint32_t timeout = 5000);

                private:
                    //!define the local cache of variable by his index
                    CHAOS_DEFINE_MAP_FOR_TYPE(uint32_t, chaos::common::data::CDataVariant, MapVarCacheValues);
                    
                    //!define tha cache fo the variable grouped by type
                    CHAOS_DEFINE_MAP_FOR_TYPE(VariableType, MapVarCacheValues, MapVarTypeCache);
                    
                    //!internal structure for device cache
                    typedef struct {
                        Device device;
                        //cache map that group variable for type
                        MapVarTypeCache map_type_cache;
                    } DeviceCache;
                    
                    //!map the index and the device info for cache purphose
                    CHAOS_DEFINE_MAP_FOR_TYPE(uint32_t, ChaosSharedPtr<DeviceCache>, MapDeviceCache);
                    CHAOS_DEFINE_LOCKABLE_OBJECT(MapDeviceCache, LMapDeviceCache);
                    
                    LMapDeviceCache map_device_cache;
                    
                    //!fill device info with remote driver content
                    inline int buildDeviceInfo(chaos::common::data::CDataWrapper& received_data,
                                               Device& device_info);
                    //! update the variable in cache
                    inline int updateVariableCachedValue(chaos::common::data::CDataWrapper& received_data);
                    //! return the message type found within message
                    inline int getMessageType(chaos::common::data::CDataWrapper& received_data,
                                              MessageType& type);
                    //!return the error
                    inline int getErrorFromMessage(chaos::common::data::CDataWrapper& received_data,
                                                   int& error_code);
                    
                    //!Send raw request to the remote driver
                    /*!
                     \param message_type is the type of the raw message to be sent to remote driver
                     \param message_data is the raw data to be transmitted to the remote driver
                     \param received_data si the raw data received from the driver
                     */
                    int sendRawRequest(MessageType message_type,
                                       chaos::common::data::CDWUniquePtr message_data,
                                       chaos::common::data::CDWShrdPtr& message_response,
                                       uint32_t timeout = 5000);
                    
                    //!Send raw message to the remote driver
                    /*!
                     \param message_type is the type of the raw message to be sent to remote driver
                     \param message_data is the raw data to be transmitted to the remote driver
                     */
                    int sendRawMessage(MessageType message_type,
                                       chaos::common::data::CDWUniquePtr message_data);
                    
                    
                    int asyncMessageReceived(chaos::common::data::CDWUniquePtr message);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__061D444_6ECF_448C_8D63_98139B76FA9D_ExternalIODriver_h */
