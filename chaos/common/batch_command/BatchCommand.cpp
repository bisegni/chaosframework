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
sticky(false),
already_setupped(false),
runningProperty(RunningPropertyType::RP_NORMAL),
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
BatchCommand::~BatchCommand() {}

AbstractSharedDomainCache * const BatchCommand::getSharedCacheInterface() {
    return sharedAttributeCachePtr;
}

//return the unique id
uint64_t BatchCommand::getUID() {
	return unique_id;
}

std::string BatchCommand::getAlias() {
    return command_alias;
}

void BatchCommand::setCommandAlias(const std::string& _command_alias) {
	fault_description.source = command_alias = _command_alias;
}

bool BatchCommand::setRunningProperty(uint8_t property)  {
    if(lockFeaturePropertyFlag.test(1)) return false;
    runningProperty = property;
    return true;
}

//! return the current running property
uint8_t BatchCommand::getRunningProperty() {
    return runningProperty;
}

/*
 Start the slow command sequence
 */
void BatchCommand::setHandler(CDataWrapper *data) {}

/*
 implement the data acquisition handler
 */
void BatchCommand::acquireHandler() {}

/*
 implement the data correlation handler
 */
void BatchCommand::ccHandler() {}

//timeout handler
bool BatchCommand::timeoutHandler() {return false;}

//! called befor the command start the execution
void BatchCommand::commandPre() {
	timing_stats.command_set_time_usec = TimingUtil::getTimeStampInMicroseconds();
}
void BatchCommand::endHandler() {;
}
#define SET_FAULT(c, m, d) \
BC_FAULT_RUNNING_PROPERTY \
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
		if(time_offset >= commandFeatures.featureCommandTimeout) {
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
const uint64_t BatchCommand::getSetTime() const {
    return (timing_stats.command_set_time_usec/1000);
}

const uint64_t BatchCommand::getStepCounter() const {
    return timing_stats.command_step_counter;
}

//! return the start step time of the sandbox
const uint64_t BatchCommand::getStartStepTime() const {
    return (shared_stat->last_cmd_step_start_usec / 1000);
}

//! return the last step time of the sandbox
const uint64_t BatchCommand::getLastStepDuration() const {
    return (shared_stat->last_cmd_step_duration_usec);
}
