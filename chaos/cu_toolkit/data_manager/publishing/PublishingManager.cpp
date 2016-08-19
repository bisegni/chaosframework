/*
 *	PublishingManager.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 19/08/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/data_manager/publishing/PublishingManager.h>

using namespace chaos::cu::data_manager::publishing;


#pragma mark PublishableTargetManagerElement
PublishableTargetManagerElement::PublishableTargetManagerElement():
target(),
auto_remove(false){}

PublishableTargetManagerElement::PublishableTargetManagerElement(const PublishableTargetManagerElement& src):
target(src.target),
auto_remove(src.auto_remove){}

PublishableTargetManagerElement::PublishableTargetManagerElement(boost::shared_ptr<PublishTarget> _target,
                                                                 bool _auto_remove):
target(_target),
auto_remove(_auto_remove){}

#pragma mark PublishingManager Public Method

#pragma mark PublishingManager Private Method
