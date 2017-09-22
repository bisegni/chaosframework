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
