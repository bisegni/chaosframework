//
//  HealtMetric.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 06/05/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__HealtMetric__
#define __CHAOSFramework__HealtMetric__

#include <chaos/common/global.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataWrapper.h>

namespace chaos{
    namespace common {
        namespace healt_system {
            
            //! metric element
            struct HealtMetric {
                HealtMetric(const std::string& _name,
                            const chaos::DataType::DataType _type);
                
                virtual void addMetricToCD(chaos::common::data::CDataWrapper& data) = 0;
                void addMetricToCD(chaos::common::data::CDataWrapper *data);
            protected:
                const std::string name;
                const chaos::DataType::DataType type;
            };
            
            struct BoolHealtMetric:
            public HealtMetric {
                bool value;
                BoolHealtMetric(const std::string& _name);
                void addMetricToCD(chaos::common::data::CDataWrapper& data);
            };
            
            struct Int32HealtMetric:
            public HealtMetric {
                int32_t value;
                Int32HealtMetric(const std::string& _name);
                void addMetricToCD(chaos::common::data::CDataWrapper& data);
            };
            
            struct Int64HealtMetric:
            public HealtMetric {
                int64_t value;
                Int64HealtMetric(const std::string& _name);
                void addMetricToCD(chaos::common::data::CDataWrapper& data);
            };
            
            struct DoubleHealtMetric:
            public HealtMetric {
                double value;
                DoubleHealtMetric(const std::string& _name);
                void addMetricToCD(chaos::common::data::CDataWrapper& data);
            };
            
            struct StringHealtMetric:
            public HealtMetric {
                std::string value;
                StringHealtMetric(const std::string& _name);
                void addMetricToCD(chaos::common::data::CDataWrapper& data);
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__HealtMetric__) */
