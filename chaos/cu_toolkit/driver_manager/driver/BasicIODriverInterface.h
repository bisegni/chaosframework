//
//  BasicIODriverInterface.h
//  simple BasicChannel driver interface 
//
//  Created by Andrea Michelotti 26/4/2015.
//  Copyright (c) 2014 andrea michelotti. All rights reserved.
//

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
                cu_driver::DrvMsg message;
                
            public:
                
                BasicIODriverInterface(cu_driver::DriverAccessor*_accessor):accessor(_accessor){assert (_accessor);};
                
                cu_driver::DriverAccessor* accessor;
                
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
