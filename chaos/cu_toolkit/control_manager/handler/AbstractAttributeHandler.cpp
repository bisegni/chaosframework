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

#include <chaos/cu_toolkit/control_manager/handler/AbstractAttributeHandler.h>

using namespace chaos::cu::control_manager::handler;

AbstractAttributeHandler::AbstractAttributeHandler():
dataset_database(NULL) {}
AbstractAttributeHandler::~AbstractAttributeHandler() {}

void AbstractAttributeHandler::setHandlerResult(const std::string& attribute_name,
                                                const bool result) {
    map_handler_result[attribute_name] = result;
}

bool AbstractAttributeHandler::getHandlerResult(const std::string& attirbute_name) {
    //if we have no registered handle for that attribute return true(the handler can acept or no the change on the attribute)
    if(map_handler_result.count(attirbute_name) == 0) return true;
    return map_handler_result[attirbute_name];
}

void AbstractAttributeHandler::setDatasetDB(chaos::common::data::DatasetDB *ds_db_ptr) {
    dataset_database = ds_db_ptr;
}
