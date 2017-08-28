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

#include <string>
#include <sched.h>

#include <chaos/common/utility/UUIDUtil.h>

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriver.h>
#include <chaos/cu_toolkit/driver_manager/driver/DriverAccessor.h>

using namespace chaos::common::utility;
//namespace chaos_thread_ns = chaos::common::thread;
using namespace chaos::cu::driver_manager::driver;

#define ADLAPP_ INFO_LOG_1_P(AbstractDriver, driver_uuid)
#define ADLDBG_ DBG_LOG_1_P(AbstractDriver, driver_uuid)
#define ADLERR_ ERR_LOG_1_P(AbstractDriver, driver_uuid)

/*------------------------------------------------------

 ------------------------------------------------------*/
AbstractDriver::AbstractDriver(ChaosUniquePtr<BaseBypassDriver> custom_bypass_driver):
accessor_count(0),
bypass_driver(ChaosMoveOperator(custom_bypass_driver)),
o_exe(this),
is_json_param(false),
driver_need_to_deinitialize(false),
driver_uuid(UUIDUtil::generateUUIDLite()),
command_queue(new DriverQueueType()){}

/*------------------------------------------------------

 ------------------------------------------------------*/
AbstractDriver::~AbstractDriver() {}

// Initialize instance
void AbstractDriver::init(void *init_param) throw(chaos::CException) {
    driver_need_to_deinitialize = false;

    //!try to decode parameter string has json document
    is_json_param = json_reader.parse(static_cast<const char *>(init_param), json_parameter_document);

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

    DrvMsg init_msg;
    ResponseMessageType id_to_read;
    AccessorQueueType result_queue;
    std::memset(&init_msg, 0, sizeof(DrvMsg));
    init_msg.opcode = OpcodeType::OP_INIT_DRIVER;
    init_msg.id = 0;
    init_msg.inputData = init_param;
    init_msg.drvResponseMQ = &result_queue;
    command_queue->push(&init_msg);
    result_queue.wait_and_pop(id_to_read);
    if(init_msg.ret){
        //in case we have error throw the exception
        throw CException(init_msg.ret, init_msg.err_msg, init_msg.err_dom);
    }
}

// Deinit the implementation
void AbstractDriver::deinit() throw(chaos::CException) {
    ADLAPP_ << "Call custom driver deinitialization";
    // driverDeinit();
    DrvMsg deinit_msg;
    ResponseMessageType id_to_read;
    AccessorQueueType result_queue;

    std::memset(&deinit_msg, 0, sizeof(DrvMsg));
    deinit_msg.opcode = OpcodeType::OP_DEINIT_DRIVER;
    deinit_msg.drvResponseMQ = &result_queue;
    //send opcode to driver implemetation
    driver_need_to_deinitialize = true;
    command_queue->push(&deinit_msg);
    //wait for completition
    result_queue.wait_and_pop(id_to_read);

    //now join to  the thread if joinable
    if (thread_message_receiver->joinable()) {
        thread_message_receiver->join();
    }
}

const bool AbstractDriver::isDriverParamInJson() const {
    return is_json_param;
}

const Json::Value& AbstractDriver::getDriverParamJsonRootElement() const {
    return json_parameter_document;
}

/*------------------------------------------------------

 ------------------------------------------------------*/
bool AbstractDriver::getNewAccessor(DriverAccessor **newAccessor) {

    //allocate new accessor;
    DriverAccessor *result = new DriverAccessor(accessor_count++);
    if (result) {
        //set the parent uuid
        result->driver_uuid = driver_uuid;
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
    ADLAPP_ << "Scanner thread started for driver["<<driver_uuid<<"]";
    MsgManagmentResultType::MsgManagmentResult opcode_submission_result=MsgManagmentResultType::MMR_ERROR;

    DrvMsgPtr current_message_ptr;
    do {
        //wait for the new command
        command_queue->wait_and_pop(current_message_ptr);
        //check i opcode pointer is valid
        if (current_message_ptr == NULL) {
            continue;
        }

        try{
            //clean error message and domain
            memset(current_message_ptr->err_msg, 0, DRVMSG_ERR_MSG_SIZE);
            memset(current_message_ptr->err_dom, 0, DRVMSG_ERR_DOM_SIZE);
            //! check if we need to execute the private driver's opcode
            switch (current_message_ptr->opcode) {
                case OpcodeType::OP_SET_BYPASS:
                    ADLDBG_ << "Switch to bypass driver";
                    //executor =
                    break;
                case OpcodeType::OP_CLEAR_BYPASS:
                    ADLDBG_ << "Switch to normal driver";
                    //is_bypass=false;
                    break;
                case OpcodeType::OP_INIT_DRIVER:
                    if(is_json_param){
                    	try {
                    		chaos::common::data::CDataWrapper p;
                    		p.setSerializedJsonData(static_cast<const char *>(current_message_ptr->inputData));
                    		ADLDBG_ << "JSON PARMS:"<<current_message_ptr->inputData;
                    		driverInit(p);
                    	} catch(...){
                    		driverInit(static_cast<const char *>(current_message_ptr->inputData));
                    	}
                    } else {
                        driverInit(static_cast<const char *>(current_message_ptr->inputData));
                    }
                    opcode_submission_result = MsgManagmentResultType::MMR_EXECUTED;
                    break;

                case OpcodeType::OP_DEINIT_DRIVER:
                    driverDeinit();
                    opcode_submission_result = MsgManagmentResultType::MMR_EXECUTED;
                    break;

                default: {

                    //for custom opcode we call directly the driver implementation of execOpcode
                    opcode_submission_result = o_exe->execOpcode(current_message_ptr);
                    switch (opcode_submission_result) {
                        case MsgManagmentResultType::MMR_ERROR:
                            ADLERR_ << "an error has been returned by execOcode"<<opcode_submission_result;

                        case MsgManagmentResultType::MMR_EXECUTED:
                            break;

                        default:
                            break;
                    }
                    break;
                }
            }
        } catch(CException& ex) {
            //chaos exception
            ADLERR_ << "an error has been catched code: "<<ex.errorCode<<" msg:"<< ex.errorMessage.c_str()<<" dom:"<<ex.errorDomain.c_str()<< " executing opcode:"<<current_message_ptr->opcode;;
            current_message_ptr->ret = ex.errorCode;
            strncpy(current_message_ptr->err_msg, ex.errorMessage.c_str(), DRVMSG_ERR_MSG_SIZE);
            strncpy(current_message_ptr->err_dom, ex.errorDomain.c_str(), DRVMSG_ERR_DOM_SIZE);
        } catch(std::exception e){

            opcode_submission_result = MsgManagmentResultType::MMR_ERROR;
            std::stringstream ss;
            ss<<"Unexpected exception:"<<e.what() << " executing opcode:"<<current_message_ptr->opcode;
            ADLERR_ << ss.str();
            strncpy(current_message_ptr->err_msg, ss.str().c_str(), DRVMSG_ERR_MSG_SIZE);
            strncpy(current_message_ptr->err_dom, __PRETTY_FUNCTION__, DRVMSG_ERR_DOM_SIZE);
        } catch(...) {
            //unkonwn exception
            ADLERR_ << "an unknown exception executing opcode:"<<current_message_ptr->opcode;;
            opcode_submission_result = MsgManagmentResultType::MMR_ERROR;
            strncpy(current_message_ptr->err_msg, "Unexpected exception:", DRVMSG_ERR_MSG_SIZE);
            strncpy(current_message_ptr->err_dom, __PRETTY_FUNCTION__, DRVMSG_ERR_DOM_SIZE);
        }

        //notify the caller
        if (current_message_ptr->drvResponseMQ) {
            current_message_ptr->drvResponseMQ->push(current_message_ptr->id);
        }

    } while (current_message_ptr == NULL ||
             ((current_message_ptr->opcode != OpcodeType::OP_DEINIT_DRIVER) && (!driver_need_to_deinitialize)));
    ADLAPP_ << "Scanner thread terminated for driver["<<driver_uuid<<"]";
}

void AbstractDriver::driverInit(const chaos::common::data::CDataWrapper& data) throw(chaos::CException){
	ADLERR_<<"driver "<<identification_string<<" has json parameters you should implement driverInit(const chaos::common::data::CDataWrapper& data) initialization";
	driverInit(data.getJSONString().c_str());
}

const bool AbstractDriver::isBypass()const {
    return o_exe != this;
}
void AbstractDriver::setBypass(bool bypass){
    if(bypass) {
        o_exe = bypass_driver.get();
    } else {
        o_exe = this;
    }
}
