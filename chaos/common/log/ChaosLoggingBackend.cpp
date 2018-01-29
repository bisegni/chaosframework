/*
 * Copyright 2012, 29/01/2018 INFN
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
#include <chaos/common/log/ChaosLoggingBackend.h>
#include <boost/log/expressions.hpp>

using namespace boost::log;
using namespace chaos::common::io;
using namespace chaos::common::log;

//BOOST_LOG_ATTRIBUTE_KEYWORD(sent, "Sent", unsigned int)
//BOOST_LOG_ATTRIBUTE_KEYWORD(received, "Received", unsigned int)
ChaosLoggingBackend::ChaosLoggingBackend(){
    data_driver.reset(new ManagedDirectIODataDriver(), "ManagedDirectIODataDriver");
    if(data_driver.get() == NULL) throw chaos::CException(-1, "error creating driver", __PRETTY_FUNCTION__);
    data_driver.init(NULL, __PRETTY_FUNCTION__);
}

ChaosLoggingBackend::~ChaosLoggingBackend() {
    CHAOS_NOT_THROW(data_driver.deinit(__PRETTY_FUNCTION__););
}

void ChaosLoggingBackend::setSource(const std::string& _log_source) {
    log_source = _log_source;
}

void ChaosLoggingBackend::consume(record_view const& rec) {
    log_entries.push_back(*rec[expressions::smessage]);
    data_driver->storeLogEntries(log_source,
                                 log_entries);
    log_entries.clear();
}

void ChaosLoggingBackend::flush() {
    log_entries.clear();
}
