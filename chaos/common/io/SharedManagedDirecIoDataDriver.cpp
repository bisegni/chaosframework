/*
 * Copyright 2012, 31/01/2018 INFN
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
#include <chaos/common/io/SharedManagedDirecIoDataDriver.h>

using namespace chaos::common::io;
using namespace chaos::common::utility;

void SharedManagedDirecIoDataDriver::init(void *data)  {
    ChaosWriteLock wl(init_mtx);
    if(getServiceState() == CUStateKey::INIT) {return;}
    shared_data_driver.reset(new ManagedDirectIODataDriver());
    InizializableService::initImplementation(shared_data_driver.get(), NULL, "SharedManagedDirecIoDataDriver", __PRETTY_FUNCTION__);
}

void SharedManagedDirecIoDataDriver::deinit()  {
    ChaosWriteLock wl(init_mtx);
    if(getServiceState() == CUStateKey::DEINIT) {return;}
    InizializableService::deinitImplementation(shared_data_driver.get(), "SharedManagedDirecIoDataDriver", __PRETTY_FUNCTION__);
}

const ChaosSharedPtr<chaos::common::io::ManagedDirectIODataDriver>& SharedManagedDirecIoDataDriver::getSharedDriver() {
    if(getServiceState() != CUStateKey::INIT) { throw chaos::CException(-1, "Driver not initilized", __PRETTY_FUNCTION__);}
    return shared_data_driver;
}
