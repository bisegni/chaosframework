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
# define N BOOST_PP_ITERATION()
// No ifndef headers - this is meant to be included multiple times.
template <class ReturnValue
          BOOST_PP_COMMA_IF(N)
          BOOST_PP_ENUM_PARAMS(N, class Param) >
ReturnValue (*get(const std::string& name))
    (BOOST_PP_ENUM_PARAMS(N, Param)) {
  // Cast the handle or pointer to the function to the correct type.
  // This is NOT typesafe. See the documentation of shared_library::get
  return reinterpret_cast<ReturnValue (*)(BOOST_PP_ENUM_PARAMS(N, Param))>
      (impl::get_function(handle_, name.c_str()));
}
#undef N
