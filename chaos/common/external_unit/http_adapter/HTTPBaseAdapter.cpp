/*
 * Copyright 2012, 10/10/2017 INFN
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
#include <chaos/common/external_unit/http_adapter/HTTPBaseAdapter.h>

#define INFO    INFO_LOG(HTTPBaseAdapter)
#define DBG     DBG_LOG(HTTPBaseAdapter)
#define ERR     ERR_LOG(HTTPBaseAdapter)

using namespace chaos::common::data;
using namespace chaos::common::external_unit::http_adapter;

HTTPBaseAdapter::HTTPBaseAdapter() {}

HTTPBaseAdapter::~HTTPBaseAdapter() {}

void HTTPBaseAdapter::sendHTTPJSONError(mg_connection *nc,
                                        int status_code,
                                        const int error_code,
                                        const std::string& error_message) {
    CDataWrapper err_data_pack;
    err_data_pack.addInt32Value("error_code", error_code);
    err_data_pack.addStringValue("error_message", error_message);
    const std::string json_error = err_data_pack.getJSONString();
    ERR << CHAOS_FORMAT("Error response:%1%", %json_error);
    mg_send_head(nc, 400, 0, "Content-Type: application/json");
    mg_printf(nc, "%s", json_error.c_str());
}

void HTTPBaseAdapter::sendWSJSONError(mg_connection *nc,
                                      const int error_code,
                                      const std::string& error_message,
                                      bool close_connection) {
    CDataWrapper err_data_pack;
    err_data_pack.addInt32Value("error_code", error_code);
    err_data_pack.addStringValue("error_message", error_message);
    const std::string json_error = err_data_pack.getJSONString();
    ERR << CHAOS_FORMAT("Error response:%1%", %json_error);
    mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, json_error.c_str(), json_error.size());
    if(close_connection){mg_send_websocket_frame(nc, WEBSOCKET_OP_CLOSE, NULL, 0);}
}

void HTTPBaseAdapter::sendWSJSONAcceptedConnection(mg_connection *nc,
                                                   bool accepted,
                                                   bool close_connection) {
    CDataWrapper err_data_pack;
    err_data_pack.addInt32Value("accepted_connection", accepted);
    const std::string accepted_json = err_data_pack.getJSONString();
    mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, accepted_json.c_str(), accepted_json.size());
    if(close_connection){mg_send_websocket_frame(nc, WEBSOCKET_OP_CLOSE, NULL, 0);}
}
