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

#include "FileCommandOutputStream.h"
#include <fcntl.h>
using namespace chaos::agent::external_command_pipe;

FileCommandOutputStream::FileCommandOutputStream(const std::string& _file_path):
file_path(_file_path),
output_file(-1){
    //output_file = open(file_path.c_str(), O_RDWR|O_NONBLOCK);
}

FileCommandOutputStream::~FileCommandOutputStream() {
    //close(output_file);
}

void FileCommandOutputStream::init(void *init_data) throw (chaos::CException) {
    
}

void FileCommandOutputStream::deinit() throw (chaos::CException) {
    
}

AbstractCommandOutputStream& FileCommandOutputStream::operator<<(const std::string& string) {
    ssize_t err  = 0;
    output_file = open(file_path.c_str(), O_RDWR|O_NONBLOCK);
    err = write(output_file, string.c_str(), string.size());
    err = fsync(output_file);
    close(output_file);
    return *this;
}

AbstractCommandOutputStream& FileCommandOutputStream::operator<<(const char * string) {
    ssize_t err = 0;
    output_file = open(file_path.c_str(), O_RDWR|O_NONBLOCK);
    err = write(output_file, string, strlen(string));
    err = fsync(output_file);
    close(output_file);
    return *this;
}
