/*
 *	URL.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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
