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

#ifndef DataManager_H
#define DataManager_H
#include <map>
#include <string>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <chaos/common/io/IODataDriver.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/utility/StartableService.h>

#include <chaos/cu_toolkit/data_manager/KeyDataStorage.h>

namespace chaos_io = chaos::common::io;
namespace chaos_data = chaos::common::data;

namespace chaos{
    namespace cu {
        using namespace boost;
		
		namespace data_manager {
			/*
			 * This class manage and organize the creation of output pipeline,
			 * there is only one instance of this class
			 */
			class DataManager:
			public common::utility::StartableService,
			public common::utility::Singleton<DataManager> {
				friend class common::utility::Singleton<DataManager>;
				//mutex for lock operation on service request
				boost::mutex managing_data_mutex;
				ChaosSharedPtr<chaos_io::IODataDriver> liveDriver;
				std::map<std::string, KeyDataStorage*>  deviceIDKeyDataStorageMap;
				
				/*
				 * Constructor
				 */
				DataManager();
				
				/*
				 * Distructor
				 */
				virtual ~DataManager();
				
			public:
				/*
				 * Initzialize the datamanager
				 */
				void init(void *initParamter) throw(CException);
				/*
				 * Deinitzialize the datamanager
				 */
				void deinit() throw(CException);
				
				/*
				 * Start all sub process
				 */
				void start() throw(CException);
				
				/*
				 Configure the sandbox and all subtree of the CU
				 */
				chaos_data::CDataWrapper* updateConfiguration(chaos_data::CDataWrapper*);
				/*
				 *
				 */
				KeyDataStorage *getKeyDataStorageNewInstanceForKey(const std::string& device_id) throw(CException);
				//-------per test------
				/*
				 * Return an instance for the configured data live driver
				 */
				chaos_io::IODataDriver *getDataLiveDriverNewInstance() throw(CException);
				
				/*
				 Initialize a device id KeyDataStorageBuffer
				 */
				void initDeviceIDKeyDataStorage(const std::string &device_id, chaos_data::CDataWrapper*) throw(CException);
				
				/*
				 Initialize a device id KeyDataStorageBuffer
				 */
				void deinitDeviceIDKeyDataStorage(const std::string &device_id) throw(CException);
				
				/*
				 Submit a CDataWrapper on device id KeyDataStorage
				 */
				void pushDeviceDataByIdKey(const std::string& device_id, chaos_data::CDWShrdPtr dataset) throw(CException);
				
				/*
				 return a new instance of CDataWrapper filled with a mandatory data
				 according to key
				 */
				chaos_data::CDWShrdPtr getNewDataWrapperForDeviceIdKey(const std::string &device_id);
				
				/*
				 Configure the datamanager
				 */
				void updateConfigurationForDeviceIdKey(const std::string &device_id, chaos_data::CDataWrapper*);
			};
		}
	}
}
#endif
