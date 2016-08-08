/*
 *	DataManager.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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
				boost::shared_ptr<chaos_io::IODataDriver> liveDriver;
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
				void pushDeviceDataByIdKey(const std::string &device_id, chaos_data::CDataWrapper*) throw(CException);
				
				/*
				 Get the last CDataWrapper from the live data for the device id key
				 */
				common::utility::ArrayPointer<chaos_data::CDataWrapper> *getLastCDataWrapperForDeviceIdKey(const std::string &device_id)  throw(CException);
				/*
				 return a new instance of CDataWrapper filled with a mandatory data
				 according to key
				 */
				chaos_data::CDataWrapper *getNewDataWrapperForDeviceIdKey(const std::string &device_id);
				
				/*
				 Configure the datamanager
				 */
				void updateConfigurationForDeviceIdKey(const std::string &device_id, chaos_data::CDataWrapper*);
			};
		}
	}
}
#endif
