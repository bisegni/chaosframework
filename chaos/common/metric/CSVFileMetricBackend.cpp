//
//  CSVFileMetricBackend.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 22/07/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

#include <chaos/>

CVSFileMetricBackend(const std::string& _backend_identity,
                     const std::string& _file_path,
                     std::ios_base::openmode _open_mode);
~CVSFileMetricBackend();
void addMetric(const std::string& metric_name,
               const std::string& metric_value);
void prepare();
void flush();