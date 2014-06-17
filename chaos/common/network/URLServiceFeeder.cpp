/*
 *	URLServiceFeeder.cpp
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
#include <chaos/common/global.h>
#include <chaos/common/network/URLServiceFeeder.h>

using namespace chaos::common::network;

//size of every chunk of the list for sequential allocation
#define URL_CHUNK_LEN (sizeof(URLServiceFeeder::URLService)*10)
#define GET_LIST_SIZE (number_of_url * sizeof(URLServiceFeeder::URLService))

#define URLServiceFeeder_LAPP LAPP_ << "[URLServiceFeeder]-"<<getName()<<"-"
#define URLServiceFeeder_LDBG LDBG_ << "[URLServiceFeeder]-"<<getName()<<"-"
#define URLServiceFeeder_LERR LERR_ << "[URLServiceFeeder]-"<<getName()<<"-"

URLServiceFeeder::URLServiceFeeder(std::string alias, URLServiceFeederHandler *_handler):
NamedService(alias),
list_size(0),
current_service(NULL),
service_list(NULL),
handler(_handler),
feed_mode(URLServiceFeeder::URLServiceFeedModeRoundRobin) {
    CHAOS_ASSERT(handler)
}

URLServiceFeeder::~URLServiceFeeder() {
	clear();
    for(int idx = 0; idx < (list_size/sizeof(URLServiceFeeder::URLService)); idx++) {
		//allocate space for new url service
        delete (service_list[idx]);
    }
	delete(service_list);
}

/*!
 Remove all url and service
 */
void URLServiceFeeder::clear(bool dispose_service) {
	URLServiceFeeder_LAPP << "Remove all URL and service";
	online_urls.clear();
	
	for(int idx = 0; idx < (list_size/sizeof(URLServiceFeeder::URLService)); idx++) {
		//allocate space for new url service
        if(service_list[idx]->service && dispose_service) {
			handler->disposeService(service_list[idx]->service);
		}
		service_list[idx]->priority = 0;
		service_list[idx]->service = NULL;
		//add new available index to the available queue
		available_url.insert(idx);
		offline_urls.erase(idx);
    }
	
}

void URLServiceFeeder::removeFromOnlineQueue(uint32_t url_index) {
	URLService *tmp_element = NULL;
	OnlineQueueType work_queue;
	while (!online_urls.empty()) {
		tmp_element = online_urls.top();
		online_urls.pop();
		if(tmp_element->index == url_index) {
			continue;
		}
		work_queue.push(tmp_element);
	}
	online_urls.clear();
	
	//merge the new queue into empyt old one
	boost::heap::heap_merge(online_urls, work_queue);
}

void URLServiceFeeder::grow() {
	if(available_url.size() == 0) {
		uint32_t old_element_num = list_size/sizeof(URLServiceFeeder::URLService);
		//enlarge memory list for new element
		service_list = (URLServiceFeeder::URLService**)realloc(service_list, (list_size += URL_CHUNK_LEN));
		for(int idx = old_element_num; idx < (list_size/sizeof(URLServiceFeeder::URLService)); idx++) {
			//allocate space for new url service
			service_list[idx] = new URLServiceFeeder::URLService();
			service_list[idx]->index = idx;
			service_list[idx]->priority = idx;
			service_list[idx]->service = NULL;
			//add new available index to the available queue
			available_url.insert(idx);
		}
	}
}

uint32_t URLServiceFeeder::addURL(const URL& new_url, uint32_t priority) {
	//lock the queue
	uint32_t service_index = 0;
	grow();
	std::set<uint32_t>::iterator new_index = available_url.begin();
	service_index = *new_index;
	online_urls.push(service_list[*new_index]);
	available_url.erase(new_index);
	
	service_list[service_index]->url = new_url;
	service_list[service_index]->service = handler->serviceForURL(new_url, service_index);
	service_list[service_index]->online = true;
	service_list[service_index]->priority = priority;
	
	return service_index;
}

URLServiceFeeder::URLService* URLServiceFeeder::getNext() {
	URLService *result = NULL;
	while (!online_urls.empty()) {
		if(online_urls.top()->online) {
			result = online_urls.top();
			online_urls.pop();
			break;
		} else {
			//push the index of this offline element into
			offline_urls.insert(online_urls.top()->index);
		}
		online_urls.pop();
	}
	return result;
}

void* URLServiceFeeder::getService() {
	if(!current_service) {
		if (!(current_service = getNext())) return NULL;
	} else if (feed_mode == URLServiceFeeder::URLServiceFeedModeRoundRobin) {
		URLService *last_service = current_service;
		if((current_service = getNext())){
			//we have a new service so we can push the oldone
			online_urls.push(last_service);
		} else {
			current_service = last_service;
		}
	}
	return current_service->service;
}

void URLServiceFeeder::setURLOffline(uint32_t idx) {
	if(idx > (list_size/sizeof(URLServiceFeeder::URLService))) {
		URLServiceFeeder_LERR << "Index out of range";
		return;
	}
	if(current_service && current_service->index == idx) {
		current_service = NULL;
	}
	service_list[idx]->online = false;
	//set service offline
	offline_urls.insert(idx);
}

void URLServiceFeeder::setURLOnline(uint32_t idx) {
	if(idx > (list_size/sizeof(URLServiceFeeder::URLService))) {
		URLServiceFeeder_LERR << "Index out of range";
		return;
	}
	//set service offline
	std::set<uint32_t>::iterator iter = offline_urls.find(idx);
	if(iter != offline_urls.end()) {
		offline_urls.erase(iter);
	}
	service_list[idx]->online = true;
	online_urls.push(service_list[idx]);
}

void URLServiceFeeder::removeURL(uint32_t idx, bool deallocate_service) {
	if(idx > (list_size/sizeof(URLServiceFeeder::URLService))) {
		URLServiceFeeder_LERR << "Index out of range";
		return;
	}
	//delete service instance
	if(deallocate_service) handler->disposeService(service_list[idx]->service);
	removeFromOnlineQueue(idx);
	available_url.insert(idx);
}


void URLServiceFeeder::setFeedMode(URLServiceFeedMode new_feed_mode) {
	feed_mode = new_feed_mode;
}