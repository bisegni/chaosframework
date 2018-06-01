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

#ifndef __CHAOSFramework__DeviceDataWorker__
#define __CHAOSFramework__DeviceDataWorker__

#include "DataWorker.h"
#include "../cache_system/cache_system.h"
#include "../object_storage/object_storage.h"

#include <string>
#include <chaos/common/data/Buffer.hpp>
#include <chaos/common/direct_io/DirectIODataPack.h>
#include <chaos/common/direct_io/channel/DirectIODeviceChannelGlobal.h>

#include <boost/thread.hpp>

namespace chaos_utility		= chaos::common::utility;
namespace chaos_direct_io	= chaos::common::direct_io;

namespace chaos{
    namespace data_service {
        namespace worker {
            //forward declaration
            class DeviceSharedDataWorkerMetricCollector;
            
            //! worker information for the device live storage
            struct DeviceSharedWorkerJob :
            public WorkerJob {
                uint8_t key_tag;
                std::string key;
                int	put_operation; //0 -storicize only, 1-live only, 2-storicize and live
                uint32_t data_pack_len;
                chaos::common::data::BufferSPtr data_pack;
            };
            
            struct ThreadCookie {
                ChaosUniquePtr<chaos::service_common::persistence::data_access::AbstractPersistenceDriver> object_storage_driver;
                chaos::data_service::object_storage::abstraction::ObjectStorageDataAccess *obj_storage_da;
            };
            
            //! worker for live device sharing
            class DeviceSharedDataWorker : public DataWorker {
                friend class DeviceSharedDataWorkerMetricCollector;
            protected:
                void executeJob(WorkerJobPtr job_info,
                                void* cookie);
            public:
                DeviceSharedDataWorker();
                ~DeviceSharedDataWorker();
                void init(void *init_data) throw (chaos::CException);
                void deinit() throw (chaos::CException);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__DeviceDataWorker__) */
