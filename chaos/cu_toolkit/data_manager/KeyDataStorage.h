/*
 *	KeyDataStorage.h
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

#ifndef KeyDataStorage_H
#define KeyDataStorage_H

#include <map>
#include <string>

#include <chaos/common/io/IODataDriver.h>
#include <chaos/common/utility/ArrayPointer.h>

#include <boost/thread.hpp>

namespace chaos_data = chaos::common::data;
namespace chaos_io = chaos::common::io;
namespace chaos_utility = chaos::common::utility;
namespace chaos{
    namespace cu {
		namespace data_manager {
			
			typedef enum KeyDataStorageDomain {
				KeyDataStorageDomainOutput = 0,
				KeyDataStorageDomainInput,
				KeyDataStorageDomainCustom,
				KeyDataStorageDomainSystem
			} KeyDataStorageDomain;
			
			class KeyDataStorage {
				std::string key;
				std::string output_key;
				std::string input_key;
				std::string system_key;
				std::string custom_key;
				
				//restore poitn map
				std::map<std::string, std::map<std::string, boost::shared_ptr<chaos_data::CDataWrapper> > > restore_point_map;
				
				chaos_io::IODataDriver *io_data_driver;
				
				//mutex to protect access to data io driver
				boost::mutex mutex_push_data;
			public:
				KeyDataStorage(const std::string& _key,
							   chaos_io::IODataDriver *_io_data_driver);
				virtual ~KeyDataStorage();
				
				void init(void *init_parameter) throw (chaos::CException);

				void deinit() throw (chaos::CException);
				/*
				 Return a new instace for the CSDatawrapped
				 */
				chaos_data::CDataWrapper* getNewOutputAttributeDataWrapper();
				
				/*
				 Retrive the data from Live Storage
				 */
				chaos_utility::ArrayPointer<chaos_data::CDataWrapper>* getLastDataSet(KeyDataStorageDomain domain);
				
				//! push a dataset associated to a domain
				void pushDataSet(KeyDataStorageDomain domain, chaos_data::CDataWrapper *dataset);
				
				//! load all dataseet for a restore point
				int loadRestorePoint(const std::string& restore_point_tag);

				//! clear all loaded dataset for a restore point
				int clearRestorePoint(const std::string& restore_point_tag);
				
				//!return the dataset asccoaited to a domain in a determinated restore tag
				boost::shared_ptr<chaos_data::CDataWrapper>  getDatasetFromRestorePoint(const std::string& restore_point_tag,
																						KeyDataStorageDomain domain);

				/*
				 Permit to be live configurable
				 */
				chaos_data::CDataWrapper* updateConfiguration(chaos_data::CDataWrapper *);
				
			};
		}
    }
}
#endif