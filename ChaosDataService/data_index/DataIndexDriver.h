//
//  DataIndexDriver.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 19/05/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__DataIndexDriver__
#define __CHAOSFramework__DataIndexDriver__



#include <string>
#include <vector>

#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/InizializableService.h>

namespace chaos {
	namespace data_service {
		
		namespace data_index {

			namespace data_index_system {
				
				typedef std::vector<std::string>			DataIndexDriverServerList;
				typedef std::vector<std::string>::iterator	DataIndexDriverServerListIterator;
				
				//! index driver setting
				typedef struct DataIndexDriverSetting {
					//! vector for all index endpoint url (usualy database url, more is the driver permit the scalability of the service)
					IndexDriverServerList servers;
					
					//! kv param for the implementations of the driver
					std::map<string,string> key_value_custom_param;
				} IndexDriverSetting;
				
				/*!
				 Base class for all driver that will manage the work on data indexing
				 */
				class DataIndexDriver : public NamedService , public chaos::utility::InizializableService {
				protected:
					//protected constructor
					DataIndexDriver(std::string alias);
					
				protected:
					DataIndexDriverSetting *setting;
				public:
					
					//! public destructor
					virtual ~DataIndexDriver();
					
					//! init
					/*!
					 Need a point to a structure IndexDriverSetting for the setting
					 */
					void init(void *init_data) throw (chaos::CException);
					
					//!deinit
					void deinit() throw (chaos::CException);
					
				};


#endif /* defined(__CHAOSFramework__DataIndexDriver__) */
