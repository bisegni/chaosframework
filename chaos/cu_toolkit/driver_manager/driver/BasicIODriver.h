/*
 *	BasicDriver.h
 *	!CHAOS
 *	Created by Andrea Michelotti
 *      Abstraction of a simple driver, based on read/write of channels
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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
#ifndef __BASICIODRIVER_H__
#define __BASICIODRIVER_H__
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

namespace cu_driver=chaos::cu::driver_manager::driver;

namespace chaos {
    namespace cu  {
        namespace driver_manager {
            namespace driver {

                typedef enum BasicIODriverOpcode {
                    BasicIODriverOpcode_READ,
                    BasicIODriverOpcode_WRITE,
                    BasicIODriverOpcode_IOP,
                    BasicIODriverOpcode_GET_DATASET,
                    BasicIODriverOpcode_GET_DATASETSIZE,
                    BasicIODriverOpcode_INIT,
                    BasicIODriverOpcode_DEINIT,
                    BasicIODriverOpcode_USER

                } BasicIODriverOpcode;

                typedef struct ddDataSet {
                    const char* name;
                    const char* desc;
                    chaos::DataType::DataSetAttributeIOAttribute dir;
                    int type;
                    int maxsize;
                } ddDataSet_t;

#define DEF_IO_DRIVER_DATASET \
  static ddDataSet_t dataSet[]={


#define DEF_IO_CHANNEL(_name,_desc,_dir,_type,_size)	\
  {_name,_desc,_dir, _type,_size},
#define ENDDEF_IO_DRIVER_DATASET };
#define INIT_IO_DRIVER_DATASET setDataSet(dataSet,sizeof(dataSet));

/*
                 driver definition
                 */

                class BasicIODriver : ADD_CU_DRIVER_PLUGIN_SUPERCLASS {
                    void driverInit(const char *initParameter) throw (chaos::CException);
                    void driverDeinit() throw (chaos::CException);
                protected:
                    ddDataSet_t *dataset;
                    int datasetSize;
                public:
                    BasicIODriver();

                    ~BasicIODriver();
                    //! Execute a command
                    cu_driver::MsgManagmentResultType::MsgManagmentResult execOpcode(cu_driver::DrvMsgPtr cmd);
                    /**
                     \brief Read  from the physical device
                     \param buffer[out] destination buffer
                     \param addr[in]  address or identification
                     \param bcout[in] buffer count
                     \return the number of succesful read items, negative error

                     */
                    virtual int read(void *buffer, int addr, int bcount) = 0;
                    /**
                     \brief Write a channel of the device
                     \param buffer[out] destination buffer
                     \param addr[in] channel address or identification
                     \param bcout[in] buffer count
                     \return the number of succesful written items, negative error
                     */
                    virtual int write(void *buffer, int addr, int bcount) = 0;

                    /**
                     \brief Initialize the specific driver
                     \param buffer[in] initialisation opaque parameter
                     \return 0 if success, error otherwise
     
                     */
                    virtual int initIO(void *buffer, int sizeb) = 0;

                    /**
                     \brief deinit the driver
                     \param buffer[in] initialisation opaque parameter
                     \return 0 if success, error otherwise
                     */
                    virtual int deinitIO() = 0;
                    
                    
                    /**
                     \brief perform a specific I/O operation  the device
                     \param data[inout] operation data opaque parameter
                     \param sizeb[in] operation data size/maxsize
                     \return 0 if success, error otherwise
                     */
                    virtual int iop(int operation,void*data,int sizeb) = 0;
                    /**
                       \brief return the size in byte of the dataset
                       \return the size of the dataset if success, zero otherwise
                     */
                    int getDatasetSize();

                    /**
                       \brief return the dataset copying max size bytes
                       \param data[out] array of data
                       \param sizeb[in] max byte to copy
                       \return the size of the dataset if success, zero otherwise
                     */

                    int getDataset(ddDataSet_t*data, int sizeb);

                    void setDataSet(ddDataSet_t*data, int sizeb);
                };
            }

        }
    }
}
#endif 
