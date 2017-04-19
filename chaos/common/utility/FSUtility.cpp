/*
 *	FSutility.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 28/02/2017 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/utility/FSUtility.h>

#include <boost/predef/os.h>
#include <boost/filesystem.hpp>

#if (BOOST_OS_WINDOWS)
#  include <stdlib.h>
#elif (BOOST_OS_SOLARIS)
#  include <stdlib.h>
#  include <limits.h>
#elif (BOOST_OS_LINUX)
#  include <unistd.h>
#  include <limits.h>
#elif (BOOST_OS_MACOS)
#  include <mach-o/dyld.h>
#elif (BOOST_OS_BSD_FREE)
#  include <sys/types.h>
#  include <sys/sysctl.h>
#endif

using namespace chaos::common::utility;

/*
 implementation got from http://stackoverflow.com/questions/1023306/finding-current-executables-path-without-proc-self-exe
 */
std::string FSUtility::getExecutablePath() {
    std::string result;
#if (BOOST_OS_WINDOWS)
    char *exePath;
    if (_get_pgmptr(&exePath) != 0)
        exePath = "";
#elif (BOOST_OS_SOLARIS)
    char exePath[PATH_MAX];
    if (realpath(getexecname(), exePath) == NULL)
        exePath[0] = '\0';
#elif (BOOST_OS_LINUX)
    char exePath[PATH_MAX];
    ssize_t len = ::readlink("/proc/self/exe", exePath, sizeof(exePath));
    if (len == -1 || len == sizeof(exePath))
        len = 0;
    exePath[len] = '\0';
#elif (BOOST_OS_MACOS)
    char exePath[PATH_MAX];
    uint32_t len = sizeof(exePath);
    if (_NSGetExecutablePath(exePath, &len) != 0) {
        exePath[0] = '\0'; // buffer too small (!)
    } else {
        // resolve symlinks, ., .. if possible
        char *canonicalPath = realpath(exePath, NULL);
        if (canonicalPath != NULL) {
            strncpy(exePath,canonicalPath,len);
            free(canonicalPath);
        }
    }
#elif (BOOST_OS_BSD_FREE)
    char exePath[2048];
    int mib[4];  mib[0] = CTL_KERN;  mib[1] = KERN_PROC;  mib[2] = KERN_PROC_PATHNAME;  mib[3] = -1;
    size_t len = sizeof(exePath);
    if (sysctl(mib, 4, exePath, &len, NULL, 0) != 0)
        exePath[0] = '\0';
#endif
    if(strlen(exePath)>0) {
        result = boost::filesystem::path(exePath).remove_filename().parent_path().make_preferred().string();
    }
    return result;
}
