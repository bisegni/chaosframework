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
#include <chaos/common/network/URL.h>

using namespace chaos::common::network;

    //! Default contructor
URL::URL() {
    
}

URL::URL(const std::vector<boost::regex>& _url_reg,
		 const std::string& _url):
url(_url),
url_reg(_url_reg) {
}

URL::URL(const std::string& _url):
url(_url){
}

URL::URL(const URL & _orig_url) {
    url = _orig_url.url;
    url_reg = _orig_url.url_reg;
}

bool URL::ok() {
    bool result = false;
    for (std::vector<boost::regex>::iterator it = url_reg.begin();
         it != url_reg.end() && !result;
         it++) {
        result = regex_match(url, *it);
    }
    return result;
}

const std::string URL::getURL() const {
    return url;
}