/*
 *	DeviceLiveDataFetcher.h
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

#ifndef CHAOSFramework_DeviceLiveDataFetcher_h
#define CHAOSFramework_DeviceLiveDataFetcher_h

#include <map>

#include <boost/thread/mutex.hpp>

#include <chaos/common/network/CNodeNetworkAddress.h>
#include <chaos/common/io/IODataDriver.h>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/message/DeviceMessageChannel.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/thread/CThread.h>
#include <chaos/common/thread/CThreadExecutionTask.h>
#include <chaos/common/utility/SingleBufferCircularBuffer.h>
#include <chaos/common/data/DatasetDB.h>
#include <chaos/common/chaos_types.h>


namespace chaos_batch = chaos::common::batch_command;

namespace chaos {
    namespace ui{
            //! Controller for a single device instance
        /*! 
         This represent a global controller for a single device, identified by DEVICE_ID. The contorlle rpermit to
         contorl the initialization and operation phase for a device. Allow to send value for input dataset and read the last 
         freshenest value form the live data
         */
        class DeviceController {
            friend class HLDataApi;
                //!time to waith for the answer to an request;
            uint32_t millisecToWait;
                //! represent the device id controlled by this instance
            string deviceID;
                //! Metadata Server channel for get device information
            MDSMessageChannel *mdsChannel;
                //! Device MEssage channel to control via chaos rpc the device
            DeviceMessageChannel *deviceChannel;
                //! The io driver for accessing live data of the device
            IODataDriver *ioLiveDataDriver;
                //!Dataset database
            DatasetDB datasetDB;
                //!point to the freashest live value for this device dataset
            //auto_ptr<CDataWrapper> lastDeviceDefinition;
            
                //!point to the freashest live value for this device dataset
            auto_ptr<CDataWrapper> currentLiveValue;
                
                //mutext for multi threading track operation 
            boost::recursive_mutex trackMutext;
            
                //!store the type of the attribute for fast retrieve
            std::map<string, DataType::DataSetAttributeIOAttribute> attributeDirectionMap;
            std::map<string, DataType::DataType> attributeTypeMap;
            std::vector<string> trackingAttribute;
            
                //!map for live data circular buffer
            std::map<string,  chaos::SingleBufferCircularBuffer<int32_t> *> int32AttributeLiveBuffer;
            std::map<string,  chaos::SingleBufferCircularBuffer<int64_t> *> int64AttributeLiveBuffer;
            std::map<string,  chaos::SingleBufferCircularBuffer<double_t> *> doubleAttributeLiveBuffer;
            std::map<string,  PointerBuffer*> pointerAttributeLiveBuffer;
            
                //! Defautl Constructor
            /*!
             The visibility of the constructor is private becase it's is isntantiable only via HLDataApi singleton
             */
            DeviceController(string& _deviceID);
 
            //!Public destructor
            /*!
             All can destruct an isntance of the device controller
             */
            ~DeviceController();
            
                //! update inromation for talking with device
            /*!
             Perform oall the orpeation to find the rigth chaos address of the device
             */
            void updateChannel() throw(CException);
            
            /*!
             Initialize the map for the devices
             \param initiDevicedescription the reference to CDataWrapper that contain device initialization information
             */
            void initializeAttributeIndexMap();
               
                //!DeInitialize the map for the devices
            /*!
             Dispose all memory used for live data buffer
             */
            void deinitializeAttributeIndexMap();
            
                //! allocata new circular buffer for attribute and type
            /*
             
             */
            void allocateNewLiveBufferForAttributeAndType(string& attributeName, DataType::DataSetAttributeIOAttribute type, DataType::DataType attrbiuteType);
            
        protected:
                //! the fetcher thread method
            void executeOnThread(const string&) throw(CException);
        public:

            //!Return the deviceID of the device
            /*!
                Return the deviceID that identify the device managed by this controller
                \param dID the string that will be filled with the device id
             */
            void getDeviceId(string& dId);
            
            //!Set the request wait time 
            /*!
             Set the controller globally wait time, used when a request is forwarded to an device. after that time
             the method return with some information.
             \param newMillisecToWait the time that the answer is waited
             */
            void setRequestTimeWaith(uint32_t newMillisecToWait);
            
            //!Return the controller globally wait time
            /*!
             Return the controller globally wait time
             \return the wait time
             */
            uint32_t getRequestTimeWaith();
            
                //! update the scudiling of device run method
            /*!
             Set the control unit run method scheduling delay
             */
            int setScheduleDelay(uint64_t microsecondsDelay);
            /*!
             Get attribute name filtered by direction type
             */
            void getDeviceDatasetAttributesName(vector<string>& attributesName);

	    /*!
             Get time stamp of last packet
             */
	    int getTimeStamp(uint64_t& live);
            /*!
             Get description for attribute name
             */
            void getAttributeDescription(string& attributesName, string& attributeDescription);
            /*!
             Get all attribute name
             */
            void getDeviceDatasetAttributesName(vector<string>& attributesName, DataType::DataSetAttributeIOAttribute directionType);
            /*!
             Get range valu einfo for attrbiute name
             */
            void getDeviceAttributeRangeValueInfo(string& attributesName, chaos::RangeValueInfo& rangeInfo);
            /*!
             Get the direction of the attribute
             */
            int getDeviceAttributeDirection(string& attributesName, DataType::DataSetAttributeIOAttribute& directionType);
            /*!
             Get the direction of the attribute
             */
            int getDeviceAttributeType(string& attributesName, DataType::DataType& type);
            
			//! Get the type of the control unit
			/*!
			  Get the type of the contro unit
			 \control_unit_type string that will be filled with the type of the control unit
			 */
			int getType(std::string& control_unit_type);
			
                //!Device initialization
            /*!
             Perform the device initialization phase
             */
            int initDevice();
            
                //!Start the device chaos driver acquisition data scheduler
            /*!
             Perform the device start pahse. Thsi phase represent the run methdo called at certain delay in  athread
             */
            int startDevice();
            
                //!Stop(pause) the device driver acquisition data scheduler
            /*!
             Perform the device initialization phase
             */
            int stopDevice();
            
                //!Device deinitialization phase
            /*!
             Perform, if it's not be done the stop operation and afther the hardware deinitialization
             */
            int deinitDevice();
            
            int setAttributeValue(string& attributeName, int32_t attributeValue);
            int setAttributeValue(const char *attributeName, int32_t attributeValue);
            
            int setAttributeValue(string& attributeName, double attributeValue);
            int setAttributeValue(const char *attributeName, double attributeValue);
            
            int setAttributeValue(string& attributeName, string& attributeValue);
            int setAttributeValue(string& attributeName, const char* attributeValue);
            // buffer
            int setAttributeValue(string& attributeName, const char* attributeValue, uint32_t size);
            
			int setAttributeToValue(const char *attributeName, const char *attributeValue, bool noWait);
			int setAttributeToValue(const char *attributeName, void *attributeValue, bool noWait, int32_t bufferValuedDim);
            int setAttributeToValue(const char *attributeName, DataType::DataType attributeType, void *attributeValue, bool noWait = false, int32_t bufferValuedDim = 0);
            
            //! Submit a new slow command
            /*!
				\ingroup API_Slow_Control
                The submition of slow command is made collection all the information that permit to submit it
                \param commandAlias represent the alias of the command the the control unit expost from RPC subsystem
                \param submissionRule determinate the rule with which the command is submitted. This can determinate the
                    the execution of the current execution command in the control unit, according with his running state
                \param priority represent the priority beetwen the submitted command and all command in the queue that are
                    waiting to be submitted in the scheduler
				\param command_id is the assigned command id to the submitted one
                \param execution_channel (optional) choose the excution channel where execute the command [1 based]
                \param scheduler_steps_delay (optional) rapresent the intervall beetween the step of the scehduler [...acquisition -> correlation -> scheduleInterval...]
                \param submission_checker_steps_delay (optional) is the delay between two steps of the submission checker
                \param slow_command_data (optional) is the abstraction of the command data that is passed to the set handler befor the scheduler loop of the new command
                        take palce. The memory of that parameter is not free
             */
            int submitSlowControlCommand(string commandAlias,
										 chaos_batch::SubmissionRuleType::SubmissionRule submissionRule,
										 uint32_t priority,
										 uint64_t& command_id,
                                         uint32_t execution_channel = 0,
										 uint64_t scheduler_steps_delay = 0,
										 uint32_t submission_checker_steps_delay = 0,
										 CDataWrapper *slow_command_data = NULL);
            
            //! Submit a new slow command
            /*!
				\ingroup API_Slow_Control
				The submition of slow command is made collection all the information that permit to submit it
				\param commandAlias represent the alias of the command the the control unit expost from RPC subsystem
				\param submissionRule determinate the rule with which the command is submitted. This can determinate the
				\param command_id is the assigned command id to the submitted one
						the execution of the current execution command in the control unit, according with his running state
                \param execution_channel (optional) choose the excution channel where execute the command [1 based]
				\param scheduler_steps_delay (optional) rapresent the intervall beetween the step of the scehduler [...acquisition -> correlation -> scheduleInterval...]
				\param submission_checker_steps_delay (optional) is the delay between two steps of the submission checker
				\param slow_command_data (optional) is the abstraction of the command data that is passed to the set handler befor the scheduler loop of the new command
						take palce. The memory of that parameter is not free
             */
            int submitSlowControlCommand(string commandAlias,
										 chaos_batch::SubmissionRuleType::SubmissionRule submissionRule,
										 uint64_t& command_id,
                                         uint32_t execution_channel = 0,
										 uint64_t scheduler_steps_delay = 0,
										 uint32_t submission_checker_steps_delay = 0,
										 CDataWrapper *slow_command_data = NULL);
            
			//! Set the current slow command features
			/*!
				\ingroup API_Slow_Control
				Permit to set the features of the current running command. During execution the chaos::cu::control_manager::slow_command::features::Features::featuresFlag
				field is checked to see what feature need to be set. The enumeration chaos::cu::control_manager::slow_command::features::FeaturesFlagTypes::FeatureFlag need to
				bee used to set the featuresFlag property.
				\param features valorization of the features
				\param lock_features set the lock on all features
                \param execution_channel set the execution channel where set the features
			 */
			int setSlowCommandFeatures(chaos_batch::features::Features& features, bool lock_features, uint32_t execution_channel = 0);
			
			//! Set the lock on slow command features
			/*!
			 \ingroup API_Slow_Control
			 Lock the features modification by the slow command api on the current
			 \param lock_features	set the lock on all features
			 */
			int setSlowCommandLockOnFeatures(bool lock_features);
			
			//! Get the statistick for a command
			/*!
			 \ingroup API_Slow_Control
			 Permit to obtain the state of a command by it's unique id using the filed command_id of the command_state parameter
			 \param command_state will be filled with the state of the command on success
			 \return result of the execution
			 */
			int getCommandState(chaos_batch::CommandState& command_state);
			
			//! Kill the current executing command
			/*!
				\ingroup API_Slow_Control
				Kill the current executing command in without respetting the running state.
			 */
			int killCurrentCommand();
			
			//! Flush command states history
			/*!
			 \ingroup API_Slow_Control
			 Flush all the slow control command state history for non active command.
			 */
			int flushCommandStateHistory();
			
                //!Get device state
            /*!
             Return the current device state
             */
            int getState(CUStateKey::ControlUnitState& deviceState);
            
            /*!
             Setup the structure to accelerate the tracking of the live data
             */
            void setupTracking();
            
                //!Stop the live data tracking
            /*!
             Interrupt the live data tracking operation
             */
            void stopTracking();
            
                //add attrbiute to track
            /*!
             Add attribute to tracking
             */
            void addAttributeToTrack(string& attributeName);
            
            //get the CDatawrapper for the live value
            /*!
             the returned object is not own by requester but only by DeviceController isntance
             */
            CDataWrapper * getLiveCDataWrapperPtr();
            
            /*!
             Fetch the current live value form live storage
             */
            void fetchCurrentDeviceValue();
            
            chaos::DataBuffer *getBufferForAttribute(string& attributeName);
            chaos::PointerBuffer *getPtrBufferForAttribute(string& attributeName);
            chaos::DataBuffer *getPtrBufferForTimestamp(const int initialDimension = 10);
            
            CDataWrapper *getCurrentData(){
                return currentLiveValue.get();
            }
        };
    }
}
#endif
