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

#ifndef __CHAOSFramework__URL__
#define __CHAOSFramework__URL__

#include <string>
#include <vector>
#include <boost/regex.hpp>

namespace chaos {
    namespace common {
        namespace network {
            
                //!url description
            /*!
             permit to decirbe and url and with the association of some
             regular xpression permit to check the correcteness of the lexic of the url
             */
            class URL {
                    //!url description
                std::string url;
                    //regexp for the lexical checking of the url
                std::vector<boost::regex> url_reg;
            public:
                    //! Default contructor
                URL();
                    //! Default contructor
                URL(const std::string& _url);
                    //! constructor with regular expression for check server hostname and port
                URL(const std::vector<boost::regex>& _url_reg,
					const std::string& _url);
                
                    //! Default contructor
                URL(const URL &_orig_url);
                
                    //! check the lexical correctness of the url
                bool ok();
                
                const std::string getURL() const;
                
                
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__URL__) */
