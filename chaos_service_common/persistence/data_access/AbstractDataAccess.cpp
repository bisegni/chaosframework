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
#include <chaos_service_common/persistence/data_access/AbstractDataAccess.h>

using namespace chaos::service_common::persistence::data_access;

AbstractDataAccess::AbstractDataAccess(const std::string& name):
NamedService(name){
	
}
AbstractDataAccess::~AbstractDataAccess() {
	
}



std::string AbstractDataAccess::log_message(const std::string& section,
                                            const std::string& subsection,
                                            const std::string& log_message) {
    return boost::str(boost::format("\n%1% %2%----------------------------------------------\n%3%\n%1% %2%----------------------------------------------\n") %
                      section %
                      subsection %
                      log_message);
}