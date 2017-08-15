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

#ifndef BOOST_EXTENSION_COMMON_HPP
#define BOOST_EXTENSION_COMMON_HPP

#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/if.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition.hpp>
#include <boost/preprocessor/iteration/iterate.hpp>
#include <boost/concept_check.hpp>
/** This determines the maximum number of parameters that a constructor
  * or exported shared library function can have. 10 is the same default
  * as Boost.Function.
  */
#ifndef BOOST_EXTENSION_MAX_FUNCTOR_PARAMS
#define BOOST_EXTENSION_MAX_FUNCTOR_PARAMS 10
#endif

#endif  // BOOST_EXTENSION_COMMON_HPP
