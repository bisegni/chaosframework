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
#ifndef __BasicIODriverInterface__
#define __BasicIODriverInterface__

#include <iostream>
#include <chaos/cu_toolkit/driver_manager/driver/BasicIODriver.h>
#include <chaos/cu_toolkit/driver_manager/driver/DriverTypes.h>
#include <chaos/cu_toolkit/driver_manager/driver/DriverAccessor.h>
namespace chaos {
    
    namespace cu {
        
        namespace driver_manager {
            namespace driver {

            class BasicIODriverInterface:public BasicIODriver{
                
            protected:
                chaos::cu::driver_manager::driver::DrvMsg message;
                
            public:
                
                BasicIODriverInterface(chaos::cu::driver_manager::driver::DriverAccessor*_accessor):accessor(_accessor){assert (_accessor);};
                
                chaos::cu::driver_manager::driver::DriverAccessor* accessor;
                
                /**
                 \brief Read a channel
                 \param buffer[out] destination buffer
                 \param addr[in] channel address or identification
                 \param bcout[in] buffer count
                 \return the number of succesful read items, negative error
                 
                 */

                
                int read(void *buffer,int addr,int bcount);
                /**
                 \brief Write a channel 
                 \param buffer[out] destination buffer
                 \param addr[in] channel address or identification
                 \param bcout[in] buffer count
                 \return the number of succesful written items, negative error
                 */
                int write(void *buffer,int addr,int bcount);
                
                /**
                 \brief init the sensor
                 \param buffer[in] initialisation opaque parameter
                 \return 0 if success, error otherwise
                 
                 */
                int initIO(void *buffer,int sizeb);
                
                /**
                 \brief deinit the sensor
                 \param buffer[in] initialisation opaque parameter
                 \return 0 if success, error otherwise
                 */

                int deinitIO();
                
                
                
                int iop(int operation,void*data,int sizeb);

                /**
                 \brief return the dataset of the sensor in *data,
                 deallocate after use
                 \param sizen[in] max number of objects
                 \param data[out] dataset array
                 \return the number of sets, negative if error
                 */
                int getDataset(ddDataSet_t*data,int sizen);
                
                
               

		/**
		   \brief return the size in byte of the dataset
		   \return the size of the dataset if success, zero otherwise
		*/
		int getDatasetSize();


                
            };
            
            }
        }
    }
}


#endif /* defined(__modbus__BasicIODriverInterface__) */
