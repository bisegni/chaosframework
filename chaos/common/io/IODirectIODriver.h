/*
 *	IODirectIODriver.h
 *	!CHOAS
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

#ifndef __CHAOSFramework__IODirectIODriver__
#define __CHAOSFramework__IODirectIODriver__

#include <string>

#include <chaos/common/io/IODataDriver.h>

#include <chaos/common/direct_io/DirectIOClient.h>
#include <chaos/common/direct_io/DirectIOServerEndpoint.h>
#include <chaos/common/direct_io/channel/DirectIODeviceClientChannel.h>
#include <chaos/common/direct_io/channel/DirectIODeviceServerChannel.h>

namespace chaos{
    using namespace std;
    using namespace boost;
	
    namespace chaos_direct_io = chaos::common::direct_io;
    namespace chaos_dio_channel = chaos::common::direct_io::channel;
    
    /*!
     */
    typedef struct IODirectIODriverInitParam {
        chaos_direct_io::DirectIOClient *client_instance;
        chaos_direct_io::DirectIOServerEndpoint *endpoint_instance;
        
    } IODirectIODriverInitParam, *IODirectIODriverInitParamPtr;
    
    /*!
     */
    class IODirectIODriver : public IODataDriver {
        string dataKey;
        IODirectIODriverInitParam init_param;
    public:
        
        IODirectIODriver();
        virtual ~IODirectIODriver();
        
        /*
         * Init method
         */
        void init() throw(CException);
        
        /*
         * Deinit method
         */
        void deinit() throw(CException);
        
        /*
         * storeRawData
         */
        virtual void storeRawData(size_t dataDim, const char * buffer)  throw(CException);
        
        /*
         * retriveRawData
         */
        virtual char * retriveRawData(size_t *dim=NULL)  throw(CException);
        /*
         * updateConfiguration
         */
        chaos_data::CDataWrapper* updateConfiguration(chaos_data::CDataWrapper*);
    };
}


#endif /* defined(__CHAOSFramework__IODirectIODriver__) */
