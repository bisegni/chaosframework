/*
* Copyright 2012, 02/11/2017 INFN
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

#include "ExamplePlugin.h"


//define the general plugin
#include <cstring>

OPEN_PLUGIN_CLASS_DEFINITION(PluginAlias, 1.0, DRV, PluginLibrary)
CLOSE_PLUGIN_CLASS_DEFINITION


OPEN_EUAPI_LUGIN_CLASS_DEFINITION(EUPluginAlgotest, 1.0, EUTestApiPLugin)
CLOSE_EUAPI_CLASS_DEFINITION

//register the two plugin
OPEN_REGISTER_PLUGIN
REGISTER_PLUGIN(PluginAlias)
REGISTER_PLUGIN(EUPluginAlgotest)
CLOSE_REGISTER_PLUGIN

PluginLibrary::PluginLibrary() {}

int PluginLibrary::init(const char * init_data) {return 0;}

void PluginLibrary::deinit() {}

int EUTestApiPLugin::init(const char * init_data) {return 0;}

int EUTestApiPLugin::execute(const char *in_data,
                             uint32_t in_data_size,
                             char **out_data,
                             uint32_t *out_data_size){
    std::string in_str(in_data, in_data_size);
    std::string ou_str = "[out]" + in_str;
    *out_data = (char*)malloc(ou_str.size()+1);
    std::strcpy(*out_data,
                ou_str.c_str());
    *out_data_size = (uint32_t) ou_str.size();
    return 0;
}

void EUTestApiPLugin::deinit() {}

const char *EUTestApiPLugin::getApiName() {
    return "plugin_test";
}
