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

#include "ChaosAgent.h"
#include "chaos_agent_constants.h"
#include <chaos/common/utility/FSUtility.h>
#include <chaos/common/healt_system/HealtManager.h>
#include <chaos/common/io/SharedManagedDirecIoDataDriver.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
#include "utility/ProcRestUtil.h"
#define INFO    INFO_LOG(ChaosAgent)
#define ERROR   ERR_LOG(ChaosAgent)
#define DBG     DBG_LOG(ChaosAgent)

using namespace chaos::agent;
using namespace chaos::common::io;
using namespace chaos::common::utility;
using namespace chaos::common::healt_system;

chaos::WaitSemaphore ChaosAgent::wait_close_semaphore;
ChaosSharedPtr <chaos::agent::utility::ProcRestUtil> ChaosAgent::procRestUtil;

ChaosAgent::ChaosAgent() {}

ChaosAgent::~ChaosAgent() {}

void ChaosAgent::init(int argc, const char* argv[])  {
    ChaosCommon<ChaosAgent>::init(argc, argv);
}

void ChaosAgent::init(istringstream &initStringStream)  {
    ChaosCommon<ChaosAgent>::init(initStringStream);
}

void ChaosAgent::init(void *init_data)  {
    ChaosCommon<ChaosAgent>::init(init_data);
    if (signal((int) SIGINT, ChaosAgent::signalHanlder) == SIG_ERR) {
        throw CException(-1, "Error registering SIGINT signal", __PRETTY_FUNCTION__);
    }
    
    if (signal((int) SIGQUIT, ChaosAgent::signalHanlder) == SIG_ERR) {
        throw CException(-2, "Error registering SIGERR signal", __PRETTY_FUNCTION__);
    }
    
    if (signal((int) SIGTERM, ChaosAgent::signalHanlder) == SIG_ERR) {
        throw CException(-3, "Error registering SIGTERM signal", __PRETTY_FUNCTION__);
    }
    /*if (signal((int) SIGKILL, ChaosAgent::signalHanlder) == SIG_ERR) {
        throw CException(-3, "Error registering SIGKILL signal", __PRETTY_FUNCTION__);
    }*/
    if(settings.working_directory.size() == 0) {
        settings.working_directory = FSUtility::getExecutablePath();
    }
    
    
    //settings.agent_uid = CHAOS_FORMAT("ChaosAgent_%1%",%chaos::GlobalConfiguration::getInstance()->getLocalServerAddressAnBasePort());
    
    InizializableService::initImplementation(SharedManagedDirecIoDataDriver::getInstance(), NULL, "SharedManagedDirecIoDataDriver", __PRETTY_FUNCTION__);
    StartableService::initImplementation(HealtManager::getInstance(), NULL, "HealthManager", __PRETTY_FUNCTION__);
    agent_register.reset(new AgentRegister(), "AgentRegister");
    CHECK_ASSERTION_THROW_AND_LOG((agent_register.get() != NULL), ERROR, -1, "AgentRegister instantiation failed");
    agent_register.init(NULL, __PRETTY_FUNCTION__);
#ifdef OLD_PROCESS_MANAGEMENT

    external_cmd_executor.reset(new external_command_pipe::ExternaCommandExecutor(), "ExternaCommandExecutor");
    CHECK_ASSERTION_THROW_AND_LOG((external_cmd_executor.get() != NULL), ERROR, -2, "ExternaCommandExecutor instantiation failed");
    external_cmd_executor.init(NULL, __PRETTY_FUNCTION__);
#else
    int restport=8071;
    if(GlobalConfiguration::getInstance()->hasOption(OPT_REST_PORT)) {
           restport=getGlobalConfigurationInstance()->getOption< int >(OPT_REST_PORT);
    }
    procRestUtil=ChaosMakeSharedPtr<utility::ProcRestUtil>(restport,getGlobalConfigurationInstance()->getOption< std::string >(OPT_SCRIPT_DIR));
#endif
}

void ChaosAgent::start() {
    StartableService::startImplementation(HealtManager::getInstance(), "HealthManager", __PRETTY_FUNCTION__);
    ChaosCommon<ChaosAgent>::start();
    agent_register.start(__PRETTY_FUNCTION__);
#ifndef OLD_PROCESS_MANAGEMENT

    procRestUtil->start(true); // start in background
#endif

 if (signal((int) SIGINT, ChaosAgent::signalHanlder) == SIG_ERR) {
        throw CException(-1, "Error registering SIGINT signal", __PRETTY_FUNCTION__);
    }
    
    if (signal((int) SIGQUIT, ChaosAgent::signalHanlder) == SIG_ERR) {
        throw CException(-2, "Error registering SIGERR signal", __PRETTY_FUNCTION__);
    }
    
    if (signal((int) SIGTERM, ChaosAgent::signalHanlder) == SIG_ERR) {
        throw CException(-3, "Error registering SIGTERM signal", __PRETTY_FUNCTION__);
    }
    wait_close_semaphore.waitRaw();
    
    #ifndef OLD_PROCESS_MANAGEMENT
    LAPP_<<"Stop process";

    procRestUtil->stop();
    LAPP_<<"Kill processes";
    procRestUtil.reset();
    #endif
    stop();
    deinit();
    LAPP_<<"EXITING";

}
ChaosSharedPtr <chaos::agent::utility::ProcRestUtil> ChaosAgent::getProcessManager(){
    return procRestUtil;
}

void ChaosAgent::stop() {
    LAPP_<<"STOP";

    CHAOS_NOT_THROW(StartableService::stopImplementation(HealtManager::getInstance(), "HealthManager", __PRETTY_FUNCTION__););
    agent_register.stop(__PRETTY_FUNCTION__);
#ifndef OLD_PROCESS_MANAGEMENT
    procRestUtil->stop();
#endif
    CHAOS_NOT_THROW(ChaosCommon<ChaosAgent>::stop(););
}

void ChaosAgent::deinit() {
        LAPP_<<"DEINIT";

#ifdef OLD_PROCESS_MANAGEMENT

    CHAOS_NOT_THROW(external_cmd_executor.deinit(__PRETTY_FUNCTION__););
#endif
    CHAOS_NOT_THROW(StartableService::deinitImplementation(HealtManager::getInstance(), "HealthManager", __PRETTY_FUNCTION__););
    InizializableService::deinitImplementation(SharedManagedDirecIoDataDriver::getInstance(), "SharedManagedDirecIoDataDriver", __PRETTY_FUNCTION__);
    agent_register.deinit(__PRETTY_FUNCTION__);
    CHAOS_NOT_THROW(ChaosCommon<ChaosAgent>::deinit(););
}

void ChaosAgent::signalHanlder(int signal_number) {
    LAPP_<<"Signal caught:"<<signal_number;
   // procRestUtil->stop();
    wait_close_semaphore.notifyAll();

}
