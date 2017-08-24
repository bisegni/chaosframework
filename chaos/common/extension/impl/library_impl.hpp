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

#ifndef BOOST_EXTENSION_LIBRARY_IMPL_HPP
#define BOOST_EXTENSION_LIBRARY_IMPL_HPP

#include <cstring>

#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32)) \
    && !defined(BOOST_DISABLE_WIN32) && !defined(__GNUC__)

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#ifndef BOOST_EXTENSION_NO_LEAN_WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#endif

#include <Windows.h>
namespace boost {
namespace extensions {
namespace impl {
  typedef HMODULE library_handle;
  typedef FARPROC generic_function_ptr;
  inline library_handle load_shared_library(const char* library_name) {
    return LoadLibraryA(library_name);
  }
  inline generic_function_ptr get_function(library_handle handle,
                                           const char* function_name) {
    return GetProcAddress(handle, function_name);
  }
  inline bool close_shared_library(library_handle handle) {
    return FreeLibrary(handle) != 0;
  }
}  // namespace impl
}  // namespace extensions
}  // namespace boost

#   pragma comment(lib, "kernel32.lib")
#else
#include <dlfcn.h>
namespace boost {
namespace extensions {
namespace impl {
  typedef void * library_handle;
  typedef void * generic_function_ptr;
  inline library_handle load_shared_library(const char* library_name) {
    return dlopen(library_name, RTLD_LAZY);
  }
  inline generic_function_ptr get_function(library_handle handle,
                                           const char* function_name) {
    return dlsym(handle, function_name);
  }
  inline bool close_shared_library(library_handle handle) {
    return dlclose(handle)==0;
  }
}  // namespace impl
}  // namespace extensions
}  // namespace boost

#endif

#endif
