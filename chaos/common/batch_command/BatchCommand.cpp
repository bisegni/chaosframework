/*
 *	SlowCommand.cpp
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
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/batch_command/BatchCommand.h>
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::data::cache;
using namespace chaos::common::batch_command;

#define LOG_HEAD_SL "[SlowCommand-" << unique_id << "] "
#define BCLAPP_ LAPP_ << LOG_HEAD_SL
#define BCLDBG_ LDBG_ << LOG_HEAD_SL
#define BCLERR_ LERR_ << LOG_HEAD_SL

// default constructor
BatchCommand::BatchCommand():
already_setupped(false),
runningProperty(RunningPropertyType::RP_Normal),
submissionRule(SubmissionRuleType::SUBMIT_NORMAL){
	//reset all lock flag
    lockFeaturePropertyFlag.reset();
	//setup feautere fields
	std::memset(&commandFeatures, 0,sizeof(features::Features));
	commandFeatures.featuresFlag = 0;
	
	//reset the timing flags
	std::memset(&timing_stats,0,sizeof(CommandTimingStats));
}

// default destructor
BatchCommand::~BatchCommand() {
    
}

//return the unique id
uint64_t BatchCommand::getUID() {
	return unique_id;
}

void BatchCommand::setCommandAlias(const std::string& _command_alias) {
	fault_description.source = command_alias = _command_alias;
}

/*
 Start the slow command sequence
 */
void BatchCommand::setHandler(CDataWrapper *data) {}

/*
 implemente thee data acquisition for the command
 */
void BatchCommand::acquireHandler() {}

/*
 Performe correlation and send command to the driver
 */
void BatchCommand::ccHandler() {}

//timeout handler
bool BatchCommand::timeoutHandler() {return true;}

//! called befor the command start the execution
void BatchCommand::commandPre() {
	timing_stats.command_set_time_usec = TimingUtil::getTimeStampInMicrosends();
}

#define SET_FAULT(c, m, d) \
BC_FAULT_RUNNIG_PROPERTY \
BCLERR_ << "Exception -> err:" << c << " msg: "<<m<<" domain:"<<d; \
fault_description.code = c; \
fault_description.description = m; \
fault_description.domain = d;
//! called after the command step excecution
void BatchCommand::commandPost() {
	if(commandFeatures.featuresFlag & features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT) {
            //timing_stats.command_running_time_usec += shared_stat->last_cmd_step_duration;
		//check timeout
        int64_t time_offset = shared_stat->last_cmd_step_start_usec - timing_stats.command_set_time_usec;
		if(time_offset  >= commandFeatures.featureCommandTimeout) {
			//call the timeout handler
			try {
				if(timeoutHandler()) {
					SET_FAULT(-1, "Command timeout", __FUNCTION__)
				}
			} catch(chaos::CException& ex) {
				SET_FAULT(ex.errorCode, ex.errorMessage, ex.errorDomain)
			} catch(std::exception& ex) {
				SET_FAULT(-2, ex.what(), "Acquisition Handler");
			} catch(...) {
				SET_FAULT(-3, "Unmanaged exception", "Acquisition Handler");
			}
		}
	}
}

// return the set handler time
uint64_t BatchCommand::getSetTime() {
    return (timing_stats.command_set_time_usec/1000);
}

uint64_t BatchCommand::getStepCounter() {
    return timing_stats.command_step_counter;
}

//! return the start step time of the sandbox
uint64_t BatchCommand::getStartStepTime() {
    return (shared_stat->last_cmd_step_start_usec / 1000);
}

//! return the last step time of the sandbox
uint64_t BatchCommand::getLastStepDuration() {
    return (shared_stat->last_cmd_step_duration_usec);
}
