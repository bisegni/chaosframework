/*
 *	AbstractDriver.cpp
 *	!CHAOS
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

#include <string>
#include <sched.h>

#include <chaos/common/utility/UUIDUtil.h>

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriver.h>
#include <chaos/cu_toolkit/driver_manager/driver/DriverAccessor.h>

using namespace chaos::common::utility;
    //namespace chaos_thread_ns = chaos::common::thread;
using namespace chaos::cu::driver_manager::driver;

#define ADLAPP_ INFO_LOG_1_P(AbstractDriver, driverUUID)
#define ADLDBG_ DBG_LOG_1_P(AbstractDriver, driverUUID)
#define ADLERR_ ERR_LOG_1_P(AbstractDriver, driverUUID)

/*------------------------------------------------------

 ------------------------------------------------------*/
AbstractDriver::AbstractDriver():
accessor_count(0),
driver_need_to_deinitialize(false),
driver_uuid(UUIDUtil::generateUUIDLite()),
command_queue(new DriverQueueType()){}

/*------------------------------------------------------

 ------------------------------------------------------*/
AbstractDriver::~AbstractDriver() {}

    // Initialize instance
void AbstractDriver::init(void *initParamPtr) throw(chaos::CException) {
    driver_need_to_deinitialize = false;

    ADLAPP_ << "Start in driver thread";
        //start interna thread for the waithing of the message
    thread_message_receiver.reset(new boost::thread(boost::bind(&AbstractDriver::scanForMessage, this)));

        //set the scheduler thread priority
#if defined(__linux__) || defined(__APPLE__)
    int policy;
    struct sched_param param;
    pthread_t threadID = (pthread_t) thread_message_receiver->native_handle();
    if (!pthread_getschedparam(threadID, &policy, &param)) {
        DEBUG_CODE(ADLAPP_ << "Default thread scheduler policy";)
        DEBUG_CODE(ADLAPP_ << "policy=" << ((policy == SCHED_FIFO) ? "SCHED_FIFO" :
                                            (policy == SCHED_RR) ? "SCHED_RR" :
                                            (policy == SCHED_OTHER) ? "SCHED_OTHER" :
                                            "???");)
        DEBUG_CODE(ADLAPP_ << "priority " << param.sched_priority;)

        policy = SCHED_RR;
        param.sched_priority = sched_get_priority_max(SCHED_RR);
        if (!pthread_setschedparam(threadID, policy, &param)) {
                //successfull setting schedule priority to the thread
            DEBUG_CODE(ADLAPP_ << "new thread scheduler policy";)
            DEBUG_CODE(ADLAPP_ << "policy=" << ((policy == SCHED_FIFO) ? "SCHED_FIFO" :
                                                (policy == SCHED_RR) ? "SCHED_RR" :
                                                (policy == SCHED_OTHER) ? "SCHED_OTHER" :
                                                "???");)
            DEBUG_CODE(ADLAPP_ << "priority " << param.sched_priority;)

        }
    }
#endif
    ADLAPP_ << "Call custom driver initialization";

    DrvMsg initMsg;
    ResponseMessageType id_to_read;
    AccessorQueueType result_queue;
    std::memset(&initMsg, 0, sizeof(DrvMsg));
    initMsg.opcode = OpcodeType::OP_INIT_DRIVER;
    initMsg.id = 0;
    initMsg.inputData = initParamPtr;
    initMsg.drvResponseMQ = &result_queue;
    command_queue->push(&initMsg);
    result_queue.wait_and_pop(id_to_read);
}

    // Deinit the implementation
void AbstractDriver::deinit() throw(chaos::CException) {
    ADLAPP_ << "Call custom driver deinitialization";
        // driverDeinit();
    DrvMsg deinitMsg;
    std::memset(&deinitMsg, 0, sizeof(DrvMsg));
    deinitMsg.opcode = OpcodeType::OP_DEINIT_DRIVER;

        //send opcode to driver implemetation
    driver_need_to_deinitialize = true;
    command_queue->push(&deinitMsg);

        //now join to  the thread if joinable
    if (thread_message_receiver->joinable()) {
        thread_message_receiver->join();
    }

}

/*------------------------------------------------------

 ------------------------------------------------------*/
bool AbstractDriver::getNewAccessor(DriverAccessor **newAccessor) {

        //allocate new accessor;
    DriverAccessor *result = new DriverAccessor(accessor_count++);
    if (result) {
            //set the parent uuid
        result->driver_uuid = driver_uuid;
            //share the input queue ptr
        /* result->command_queue = new boost::interprocess::message_queue(boost::interprocess::open_only    // open or create
         ,driverUUID.c_str());             // name queue  with casual UUID
         */

        result->command_queue = command_queue.get();
        boost::unique_lock<boost::shared_mutex> lock(accesso_list_shr_mux);
        accessors.push_back(result);
        lock.unlock();

        *newAccessor = result;
    }
    return result != NULL;
}

/*------------------------------------------------------

 ------------------------------------------------------*/
bool AbstractDriver::releaseAccessor(DriverAccessor *accessor) {
    if (!accessor) {
        return false;
    }

    if (accessor->driver_uuid.compare(driver_uuid) != 0) {
        ADLERR_ << "has been requested to relase an accessor with uuid=" << accessor->driver_uuid << "that doesn't belong to this driver with uuid =" << driver_uuid;
        return false;
    }
    boost::unique_lock<boost::shared_mutex> lock(accesso_list_shr_mux);

    accessors.erase(std::find(accessors.begin(), accessors.end(), accessor));
    lock.unlock();
    delete (accessor);

    return true;
}

/*------------------------------------------------------

 ------------------------------------------------------*/
void AbstractDriver::scanForMessage() {
    ADLAPP_ << "Scanner thread started for dirver["<<driver_uuid<<"]";
    DrvMsgPtr current_message_ptr;
    MsgManagmentResultType::MsgManagmentResult opcode_submission_result;
    do {
            //wait for the new command
        command_queue->wait_and_pop(current_message_ptr);
            //check i opcode pointer is valid
        if (current_message_ptr == NULL) {
            continue;
        }

            //! check if we need to execute the private driver's opcode
        switch (current_message_ptr->opcode) {
            case OpcodeType::OP_INIT_DRIVER:
                driverInit(static_cast<const char *>(current_message_ptr->inputData));
                break;

            case OpcodeType::OP_DEINIT_DRIVER:
                driverDeinit();
                break;

            default: {
                    //for custom opcode we call directly the driver implementation of execOpcode
                opcode_submission_result = execOpcode(current_message_ptr);
                switch (opcode_submission_result) {
                    case MsgManagmentResultType::MMR_ERROR:
                    case MsgManagmentResultType::MMR_EXECUTED:
                        break;

                    default:
                        break;
                }
                break;
            }

        }
            //notify the caller
        if (current_message_ptr->drvResponseMQ) {
            current_message_ptr->drvResponseMQ->push(current_message_ptr->id);
        }

    } while ((current_message_ptr->opcode != OpcodeType::OP_DEINIT_DRIVER) && (!driver_need_to_deinitialize));
    ADLAPP_ << "Scanner thread terminated for dirver["<<driver_uuid<<"]";
}
