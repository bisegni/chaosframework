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
#include <chaos_micro_unit_toolkit/ChaosMicroUnitToolkit.h>

using namespace chaos::micro_unit_toolkit;
using namespace chaos::micro_unit_toolkit::connection;
using namespace chaos::micro_unit_toolkit::connection::unit_proxy;

ChaosMicroUnitToolkit::ChaosMicroUnitToolkit() {}

ChaosMicroUnitToolkit::~ChaosMicroUnitToolkit() {}

ChaosUniquePtr<connection::connection_adapter::AbstractConnectionAdapter> ChaosMicroUnitToolkit::getConnectionAdapter(connection::ConnectionType protocol_type,
                                                                                                                  const std::string& protocol_endpoint,
                                                                                                                  const std::string& protocol_option) {
    return cman.getProtocolAdapter(protocol_type,
                                   protocol_endpoint,
                                   protocol_option);
}

ChaosUniquePtr<raw_driver::RawDriverHandlerWrapper> ChaosMicroUnitToolkit::createNewRawDriverHandlerWrapper(connection::ConnectionType protocol_type,
                                                                                                            const std::string& protocol_endpoint,
                                                                                                            const std::string& protocol_option,
                                                                                                            connection::unit_proxy::UnitProxyHandler handler,
                                                                                                            void *user_data,
                                                                                                            const std::string& authorization_key) {
    ChaosUniquePtr<connection_adapter::AbstractConnectionAdapter> protocol_adater = cman.getProtocolAdapter(protocol_type,
                                                                                                        protocol_endpoint,
                                                                                                        protocol_option);
    ChaosUniquePtr<raw_driver::RawDriverUnitProxy> unit_proxy = ChaosUniquePtr<raw_driver::RawDriverUnitProxy>(static_cast< raw_driver::RawDriverUnitProxy* >(cman.getUnitProxy(raw_driver::RawDriverUnitProxy::proxy_type,
                                                                                                                                                                                protocol_adater).release()));
    
    return ChaosUniquePtr<raw_driver::RawDriverHandlerWrapper>(new raw_driver::RawDriverHandlerWrapper(handler,
                                                                                                       user_data,
                                                                                                       authorization_key,
                                                                                                       unit_proxy));
}
