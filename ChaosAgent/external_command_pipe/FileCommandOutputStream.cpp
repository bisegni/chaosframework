/*
 *	FileCommandOutputStream.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 24/03/2017 INFN, National Institute of Nuclear Physics
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

#include "FileCommandOutputStream.h"

using namespace chaos::agent::external_command_pipe;

FileCommandOutputStream::FileCommandOutputStream(const std::string& file_path):
output_file(NULL){
    output_file = fopen(file_path.c_str(), "wa");
}

FileCommandOutputStream::~FileCommandOutputStream() {
    fclose(output_file);
}

void FileCommandOutputStream::init(void *init_data) throw (chaos::CException) {
    
}

void FileCommandOutputStream::deinit() throw (chaos::CException) {
    
}

AbstractCommandOutputStream& FileCommandOutputStream::operator<<(const std::string& string) {
    fwrite(string.c_str(), sizeof(char), string.size(), output_file);
    fflush(output_file);
    return *this;
}

AbstractCommandOutputStream& FileCommandOutputStream::operator<<(const char * string) {
    fwrite(string, sizeof(char), strlen(string), output_file);
    fflush(output_file);
    return *this;
}
