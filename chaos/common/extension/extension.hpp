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

#ifndef BOOST_EXTENSION_EXTENSION_HPP
#define BOOST_EXTENSION_EXTENSION_HPP
#ifdef BOOST_EXTENSION_DOXYGEN_INVOKED
/** Macro to place in a function definition to cause it
  * to be exported, if necessary on the given platform and
  * with the current compiler settings. This is always required
  * for MSVC and other compilers, but only required depending on
  * compiler settings for GCC and other compilers.
  */
#define BOOST_EXTENSION_EXPORT_DECL
#else
#include <chaos/common/extension/impl/decl.hpp>
#define BOOST_EXTENSION_TYPE_MAP_FUNCTION \
extern "C" \
void BOOST_EXTENSION_EXPORT_DECL \
boost_extension_exported_type_map_function \
  (boost::extensions::type_map& types)
#endif  // BOOST_EXTENSION_EXPORT_DECL
#endif  // BOOST_EXTENSION_EXTENSION_HPP
