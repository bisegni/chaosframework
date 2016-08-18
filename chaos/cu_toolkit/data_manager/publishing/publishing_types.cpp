/*
 *	publishing_types.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 18/08/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/data_manager/publishing/publishing_types.h>

using namespace chaos::cu::data_manager::publishing;

PublishableElement::PublishableElement():
dataset_ptr(NULL),
publishing_mode(common::direct_io::channel::DirectIODeviceClientChannelPutModeStoricizeAnLive),
publish_rate(0){}

PublishableElement::PublishableElement(const PublishableElement& _dataset_element):
dataset_ptr(_dataset_element.dataset_ptr),
publishing_mode(_dataset_element.publishing_mode),
publish_rate(_dataset_element.publish_rate){}

PublishableElement::PublishableElement(const chaos::cu::data_manager::DatasetElement& _dataset_reference,
                                       const common::direct_io::channel::DirectIODeviceClientChannelPutMode _publishing_mode,
                                       const uint64_t _publish_rate):
dataset_ptr(&_dataset_reference),
publishing_mode(_publishing_mode),
publish_rate(_publish_rate){}
