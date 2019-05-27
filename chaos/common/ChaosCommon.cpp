/*
 * Copyright 2012, 04/10/2018 INFN
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
#include <chaos/common/ChaosCommon.h>

#include <errno.h>
#include <csignal>
#include <fstream>
#include <sstream>
//#include <signal.h>
//#include <csignal>
#ifndef _WIN32

#include <execinfo.h>
#include <sys/utsname.h>
#else

#include <DbgHelp.h>
#include <signal.h>
#include <windows.h>
#include "ChaosCommonWin.h"

#endif
using namespace chaos;

#ifndef _WIN32
//http://stackoverflow.com/questions/11465148/using-sigaction-c-cpp
void crit_err_hdlr(int sig_num, siginfo_t *info, void *ucontext) {
  std::cerr << "signal " << sig_num
            << " (" << strsignal(sig_num) << "), address is "
            << info->si_addr << std::endl
            << std::endl;

  void *array[50];

  int size = backtrace(array, 50);

  char **messages = backtrace_symbols(array, size);

  // skip first stack frame (points here)
  for (int i = 1; i < size && messages != NULL; ++i) {
    char *mangled_name = 0, *offset_begin = 0, *offset_end = 0;

    // find parantheses and +address offset surrounding mangled name
    for (char *p = messages[i]; *p; ++p) {
      if (*p == '(') {
        mangled_name = p;
      } else if (*p == '+') {
        offset_begin = p;
      } else if (*p == ')') {
        offset_end = p;
        break;
      }
    }

    // if the line could be processed, attempt to demangle the symbol
    if (mangled_name && offset_begin && offset_end &&
        mangled_name < offset_begin) {
      *mangled_name++ = '\0';
      *offset_begin++ = '\0';
      *offset_end++   = '\0';

      int   status;
      char *real_name = abi::__cxa_demangle(mangled_name, 0, 0, &status);

      // if demangling is successful, output the demangled function name
      if (status == 0) {
        std::cerr << "[bt]: (" << i << ") " << messages[i] << " : "
                  << real_name << "+" << offset_begin << offset_end
                  << std::endl;

      }
      // otherwise, output the mangled function name
      else {
        std::cerr << "[bt]: (" << i << ") " << messages[i] << " : "
                  << mangled_name << "+" << offset_begin << offset_end
                  << std::endl;
      }
      free(real_name);
    }
    // otherwise, print the whole line
    else {
      std::cerr << "[bt]: (" << i << ") " << messages[i] << std::endl;
    }
  }
  std::cerr << std::endl;

  free(messages);

  exit(EXIT_FAILURE);
}
#else

void crit_err_hdlr(int sig_num) {
  std::cerr << "signal " << sig_num << " (" << strsignal(sig_num) << ")";

  void *       array[50];
  SYMBOL_INFO *symbol;
  HANDLE       process;

  process = GetCurrentProcess();

  SymInitialize(process, NULL, TRUE);
  int size             = CaptureStackBackTrace(0, 50, array, NULL);
  symbol               = (SYMBOL_INFO *)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
  symbol->MaxNameLen   = 255;
  symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
  // skip first stack frame (points here)
  for (int i = 1; i < size; i++) {
    SymFromAddr(process, (DWORD64)(array[i]), 0, symbol);

    //printf("%i: %s - 0x%0X\n", size - i - 1, symbol->Name, symbol->Address);
    std::cerr << "[bt]: (" << i << ") " << symbol->Name << " : "
              << symbol->Address << std::endl;
  }

  free(symbol);
  std::cerr << std::endl;

  exit(EXIT_FAILURE);
}
#endif
ChaosAbstractCommon::ChaosAbstractCommon()
    : ingore_unreg_po(false), initialized(false), deinitialized(false) {
  GlobalConfiguration::getInstance()->preParseStartupParameters();
  initialized = deinitialized = false;
  stringstream ss,vv,dd;
  ss<< (BOOST_VERSION / 100000) << "." << ((BOOST_VERSION / 100) % 1000) << "." << (BOOST_VERSION / 100000);
  vv<<CSLIB_VERSION_MAJOR<<"."<<CSLIB_VERSION_MINOR<<"."<<CSLIB_VERSION_NUMBER;
  dd<<__DATE__ <<" " <<__TIME__;
  buildInfo.addStringValue("BoostVer",ss.str());
  buildInfo.addStringValue("CompilerVer",BOOST_COMPILER);
  buildInfo.addStringValue("ChaosVersion",vv.str());
  buildInfo.addStringValue("BuildDate",dd.str());
  
  buildInfo.addInt32Value("BuildID",CSLIB_BUILD_ID);
}
const std::string ChaosAbstractCommon::getBuildInfo(){
  return buildInfo.getCompliantJSONString();
}

ChaosAbstractCommon::~ChaosAbstractCommon() {}

void ChaosAbstractCommon::preparseConfigFile(std::istream &config_file_stream) {
  GlobalConfiguration::getInstance()->loadStreamParameter(config_file_stream);
  GlobalConfiguration::getInstance()->scanOption();
}

void ChaosAbstractCommon::preparseCommandOption(int argc, const char *argv[]) {
  GlobalConfiguration::getInstance()->loadStartupParameter(argc, argv);
  GlobalConfiguration::getInstance()->scanOption();

  //check if we have a config file
  if (GlobalConfiguration::getInstance()->hasOption(InitOption::OPT_CONF_FILE)) {
    //reload configuraiton from file
    std::string   file_option = GlobalConfiguration::getInstance()->getOption<std::string>(InitOption::OPT_CONF_FILE);
    std::ifstream option_file_stream;
    option_file_stream.open(file_option.c_str(), std::ifstream::in);
    if (!option_file_stream) {
      throw chaos::CException(-1, "Error opening configuration file", "Startup sequence");
    }
    //reparse the config file
    preparseConfigFile(option_file_stream);
  }
  //parse the dafult framework option
  GlobalConfiguration::getInstance()->checkDefaultOption();
}

void ChaosAbstractCommon::init(int argc, const char *argv[]) {
  preparseCommandOption(argc, argv);
  if (argv != NULL) {
    if (ingore_unreg_po) {
      GlobalConfiguration::getInstance()->parseStartupParametersAllowingUnregistered(argc, argv);
    } else {
      GlobalConfiguration::getInstance()->parseStartupParameters(argc, argv);
    }
  }
  init(NULL);
}

void ChaosAbstractCommon::init(std::istream &initStream) {
  GlobalConfiguration::getInstance()->parseStringStream(initStream);
  init(NULL);
}

void ChaosAbstractCommon::init(void *init_data) {
  int            err = 0;
  struct utsname u_name;
#ifndef _WIN32
  struct sigaction sigact;
  std::memset(&sigact, 0, sizeof(struct sigaction));
  sigact.sa_sigaction = crit_err_hdlr;
  sigact.sa_flags     = SA_RESTART | SA_SIGINFO;
#endif
  if (initialized)
    return;
  try {
#ifdef _WIN32
    std::string lockFileName = "./chaos_init.lock";
#else
    std::string lockFileName = "/tmp/chaos_init.lock";
#endif
    //lock file for permit to choose different tcp port for services
    std::fstream                                                     domain_file_lock_stream(lockFileName.c_str(),
                                         std::ios_base::out |
                                             std::ios_base::binary);  //check if we have got the lock
    boost::interprocess::file_lock                                   flock(lockFileName.c_str());
    boost::interprocess::scoped_lock<boost::interprocess::file_lock> e_lock(flock);
    __file_remover__                                                 fr(lockFileName.c_str());

    initialized   = true;
    deinitialized = false;

    //startup logger
    chaos::common::log::LogManager::getInstance()->init();

    //print chaos library header
    PRINT_LIB_HEADER
#ifndef _WIN32
    if (sigaction(SIGSEGV, &sigact, (struct sigaction *)NULL) != 0) {
      LERR_ << "error setting signal handler for SIGSEGV";
    }
#else
    if (signal(SIGSEGV, crit_err_hdlr) == SIG_ERR) {
      LERR_ << "error setting signal handler for SIGSEGV";
    }
#endif

    err = uname(&u_name);
    if (err == -1) {
      LAPP_ << "Platform: " << strerror(errno);
    } else {
      LAPP_ << "Platform: " << u_name.sysname << " " << u_name.nodename << " " << u_name.release << " " << u_name.version << " " << u_name.machine;
    }

    LAPP_ << "Boost version: " << (BOOST_VERSION / 100000) << "." << ((BOOST_VERSION / 100) % 1000) << "." << (BOOST_VERSION / 100000);
    LAPP_ << "Compiler Version: " << BOOST_COMPILER;
    LAPP_ << "-----------------------------------------";
    //find our ip
    string local_ip;
    if (GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_PUBLISHING_IP)) {
      local_ip = GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(InitOption::OPT_PUBLISHING_IP);
    } else {
      if (GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_PUBLISHING_INTERFACE))
        local_ip = common::utility::InetUtility::scanForLocalNetworkAddress(GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(InitOption::OPT_PUBLISHING_INTERFACE));
      else
        local_ip = common::utility::InetUtility::scanForLocalNetworkAddress();
    }
    GlobalConfiguration::getInstance()->addLocalServerAddress(local_ip);

    LAPP_ << "The local address choosen is:  " << GlobalConfiguration::getInstance()->getLocalServerAddress();

    //Starting Async central
    common::utility::InizializableService::initImplementation(chaos::common::async_central::AsyncCentralManager::getInstance(), init_data, "AsyncCentralManager", __PRETTY_FUNCTION__);
    common::utility::StartableService::initImplementation(chaos::common::network::NetworkBroker::getInstance(), init_data, "NetworkBroker", __PRETTY_FUNCTION__);
    common::utility::StartableService::startImplementation(chaos::common::network::NetworkBroker::getInstance(), "NetworkBroker", __PRETTY_FUNCTION__);

    if (GlobalConfiguration::getInstance()->hasOption(InitOption::OPT_TIME_CALIBRATION)) {
      //enable timestamp calibration
      chaos::common::utility::TimingUtil::getInstance()->enableTimestampCalibration();
    }

    if (GlobalConfiguration::getInstance()->hasOption(InitOption::OPT_PLUGIN_ENABLE)) {
      if (GlobalConfiguration::getInstance()->hasOption(InitOption::OPT_PLUGIN_DIRECTORY_PATH) == false) {
        throw chaos::CException(-1, "Plugin path need to be specified", __PRETTY_FUNCTION__);
      }
      //initialize the plugin manager
      chaos::common::utility::InizializableService::initImplementation(chaos::common::plugin::PluginManager::getInstance(), NULL, "PluginManager", __PRETTY_FUNCTION__);
    }
  } catch (...) {
    throw CException(-1, "NO chaos exception received", __PRETTY_FUNCTION__);
  }
}

void ChaosAbstractCommon::deinit() {
  if (deinitialized) return;
  deinitialized = true;
  initialized   = false;
  if (GlobalConfiguration::getInstance()->hasOption(InitOption::OPT_PLUGIN_ENABLE)) {
    //initialize the plugin manager
    chaos::common::utility::InizializableService::deinitImplementation(chaos::common::plugin::PluginManager::getInstance(), "PluginManager", __PRETTY_FUNCTION__);
  }
  if (GlobalConfiguration::getInstance()->hasOption(InitOption::OPT_TIME_CALIBRATION)) {
    //enable timestamp calibration
    chaos::common::utility::TimingUtil::getInstance()->disableTimestampCalibration();
  }
  //dellocate all
  CHAOS_NOT_THROW(common::utility::StartableService::stopImplementation(chaos::common::network::NetworkBroker::getInstance(), "NetworkBroker", __PRETTY_FUNCTION__););
  CHAOS_NOT_THROW(common::utility::StartableService::deinitImplementation(chaos::common::network::NetworkBroker::getInstance(), "AsyncCentralManager", __PRETTY_FUNCTION__););
  CHAOS_NOT_THROW(common::utility::InizializableService::deinitImplementation(chaos::common::async_central::AsyncCentralManager::getInstance(), "AsyncCentralManager", __PRETTY_FUNCTION__););

  //shutdown logger
  chaos::common::log::LogManager::getInstance()->deinit();
}

void ChaosAbstractCommon::start() {}
void ChaosAbstractCommon::stop() {}

GlobalConfiguration *ChaosAbstractCommon::getGlobalConfigurationInstance() {
  return GlobalConfiguration::getInstance();
}
