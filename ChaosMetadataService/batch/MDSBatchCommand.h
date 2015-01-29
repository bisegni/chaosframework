/*
 *	BatchCommand.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__BatchCommand__
#define __CHAOSFramework__BatchCommand__

namespace chaos{
    namespace metadata_service {
        namespace batch {
            
            //forward declaration
            class BatchExecutor;

            
            //! base class for all metadata service batch command
            /*!
             Repressent the base class for all metadata server command. Every 
             command is na instance of this class ans o many egual command can be launched
             with different parameter
             */
            class MDSBatchCommand {
                friend class BatchExecutor;
                
                //! default constructor
                MDSBatchCommand();
                
                //! default destructor
                virtual ~MDSBatchCommand();
                
            protected:
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__BatchCommand__) */
