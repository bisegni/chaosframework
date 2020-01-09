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

#ifdef _WIN32
#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__  __FUNCSIG__
#endif
#endif


#include <chaos/common/global.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/network/URLServiceFeeder.h>

using namespace chaos;
using namespace chaos::common::network;

//size of every chunk of the list for sequential allocation
#define URL_CHUNK_LEN (sizeof(URLServiceFeeder::URLService)*10)
#define GET_LIST_SIZE (number_of_url * sizeof(URLServiceFeeder::URLService))

#define URLServiceFeeder_LAPP LAPP_ << "[URLServiceFeeder]-"<<getName()<<"-"
#define URLServiceFeeder_LDBG LDBG_ << "[URLServiceFeeder]-"<<getName()<<"-"
#define URLServiceFeeder_LERR LERR_ << "[URLServiceFeeder]-"<<getName()<<"-"

URLServiceFeeder::URLServiceFeeder(std::string alias, URLServiceFeederHandler *_handler):
NamedService(alias),
last_round_robin_index(0),
list_size(0),
current_service(NULL),
service_list(NULL),
handler(_handler),
feed_mode(URLServiceFeeder::URLServiceFeedModeRoundRobin),
set_urls_pos_index(boost::multi_index::get<position_index>(set_urls_online)),
set_urls_rb_pos_index(boost::multi_index::get<rb_pos_index>(set_urls_online)){
    CHAOS_ASSERT(handler)
}

URLServiceFeeder::~URLServiceFeeder() {
	clear();
    if(service_list==NULL)
        return;
    for(int idx = 0; idx < (list_size/sizeof(URLServiceFeeder::URLService)); idx++) {
		//element with the list ar object and are allocated with with new
            delete(service_list[idx]);

    }
    //all list was allocated with malloc
    if(service_list){
	free(service_list);
        service_list=NULL;
    }
}

URLServiceFeeder::URLService *URLServiceFeeder::getNextFromSetByRoundRobin() {
    URLServiceFeeder::URLService *result_service = NULL;
    SetUrlRBPositionIndexIterator it = set_urls_online.get<rb_pos_index>().begin();
    if(it == set_urls_online.get<rb_pos_index>().end()) return NULL;

    const URLServiceIndex serv_by_it = *it;
    result_service = serv_by_it.url_service;

        //remove and resinser to schedule the next service
        //update round robin order
    set_urls_rb_pos_index.modify(it, URLServiceIndexUpdateRBPosition(++last_round_robin_index));
        //return the service
    return result_service;
}

URLServiceFeeder::URLService *URLServiceFeeder::getNextFromSetByPriority() {
    URLServiceFeeder::URLService *result_service = NULL;
    SetUrlPositionIndexIterator it = set_urls_online.get<position_index>().begin();
    if(it == set_urls_online.get<position_index>().end()) return NULL;

    URLServiceIndex serv_by_it = *it;
    result_service = serv_by_it.url_service;

        //return the service
    return result_service;
}

void URLServiceFeeder::removeFromOnlineQueue(uint32_t url_index) {
    SetUrlPositionIndexIterator it = set_urls_online.get<position_index>().find(url_index);
    if(it == set_urls_online.get<position_index>().end()) return;
    //URLServiceIndex serv_by_it = *it;
    set_urls_online.get<position_index>().erase(url_index);
}
/*!
 Remove all url and service
 */
void URLServiceFeeder::clear(bool dispose_service) {
	URLServiceFeeder_LDBG << "Remove all URL and service form multi-index";
	
	for(int idx = 0; idx < (list_size/sizeof(URLServiceFeeder::URLService)); idx++) {
		//allocate space for new url service
        if(service_list[idx] && service_list[idx]->service &&
           dispose_service) {
			handler->disposeService(service_list[idx]->service);
		}
		service_list[idx]->priority = 0;
		service_list[idx]->service = NULL;
		//add new available index to the available queue
		available_url.insert(idx);
		offline_urls.erase(idx);
    }
    set_urls_online.clear();
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

uint32_t URLServiceFeeder::addURL(const URL& new_url,
                                  uint32_t priority) {
    boost::unique_lock<boost::mutex> wl(mutex_internal);
	//lock the queue
	uint32_t service_index = 0;
        //expand memory for contain new service description
	grow();

	std::set<uint32_t>::iterator new_index = available_url.begin();
    void *tmp_srv_ptr = handler->serviceForURL(new_url, (service_index = *new_index));
    if(tmp_srv_ptr == NULL) {throw CException(-1 , "Error allocating service!", __PRETTY_FUNCTION__);}
    
    //service has been succesfully allocated
	service_list[service_index]->url = new_url;
	service_list[service_index]->service = tmp_srv_ptr;
	service_list[service_index]->online = true;
	service_list[service_index]->priority = priority;

        //we can insert new service in online structure
    set_urls_online.insert(URLServiceIndex(service_index,
                                           priority,
                                           ++last_round_robin_index,
                                           service_list[*new_index]));

        //this index is not anymore available
    available_url.erase(new_index);
    
    //insert index url association
    mapping_url_index.insert(new_url.getURL(), service_index);
    
        //return new index
	return service_index;
}

void* URLServiceFeeder::getService(uint32_t idx) {
    boost::unique_lock<boost::mutex> wl(mutex_internal);
    return service_list[idx]->service;
}
bool URLServiceFeeder::isOnline(uint32_t idx){
  return service_list[idx]->online;
}
void* URLServiceFeeder::getService() {
    boost::unique_lock<boost::mutex> wl(mutex_internal);
    switch (feed_mode) {
        case URLServiceFeeder::URLServiceFeedModeRoundRobin:
            current_service = getNextFromSetByRoundRobin();
            break;

        case URLServiceFeeder::URLServiceFeedModeFailOver:
            current_service = getNextFromSetByPriority();
            break;
    }

    return (current_service?current_service->service:NULL);
}

void URLServiceFeeder::setURLOffline(uint32_t idx) {
    boost::unique_lock<boost::mutex> wl(mutex_internal);
	if(idx > (list_size/sizeof(URLServiceFeeder::URLService))) {
		URLServiceFeeder_LERR << "Index out of range";
		return;
	}

        //set it offline from global list
    service_list[idx]->online = false;

        //remove from online set
    removeFromOnlineQueue(idx);

        //check if in case is the current
	if(current_service && current_service->index == idx) {
		current_service = NULL;
	}

	//put into ofline set service offline
	offline_urls.insert(idx);
}

void URLServiceFeeder::setURLOnline(uint32_t idx) {
    boost::unique_lock<boost::mutex> wl(mutex_internal);
	if(idx > (list_size/sizeof(URLServiceFeeder::URLService))) {
		URLServiceFeeder_LERR << "Index out of range";
		return;
	}
	//find form offline set and remove it
	std::set<uint32_t>::iterator iter = offline_urls.find(idx);
	if(iter != offline_urls.end()) {
		offline_urls.erase(iter);
	}
        //mar as online
	service_list[idx]->online = true;

        //add it into online set
    set_urls_online.insert(URLServiceIndex(service_list[idx]->index,
                                           service_list[idx]->priority,
                                           ++last_round_robin_index,
                                           service_list[idx]));
}

void URLServiceFeeder::removeURL(uint32_t idx, bool dispose_service) {
    boost::unique_lock<boost::mutex> wl(mutex_internal);
	if(idx > (list_size/sizeof(URLServiceFeeder::URLService))) {
		URLServiceFeeder_LERR << "Index out of range";
		return;
	}
	//delete service instance
    if(dispose_service) {
        handler->disposeService(service_list[idx]->service);
    }
	removeFromOnlineQueue(idx);
	available_url.insert(idx);
    //remove index url association
    mapping_url_index.removebyRightKey(idx);
    service_list[idx]->priority = 0;
    service_list[idx]->service = NULL;
}

//!return the url string from index
std::string URLServiceFeeder::getURLForIndex(uint32_t idx) {
    boost::unique_lock<boost::mutex> wl(mutex_internal);
    if(mapping_url_index.hasRightKey(idx)) {
        return mapping_url_index.findByRightKey(idx);
    } else {
        throw CException(-1, "Key not found", __PRETTY_FUNCTION__);
    }
}

uint32_t URLServiceFeeder::getIndexFromURL(const std::string& url) {
    boost::unique_lock<boost::mutex> wl(mutex_internal);
    if(mapping_url_index.hasLeftKey(url)) {
        return mapping_url_index.findByLeftKey(url);
    } else {
        throw CException(-1, "Key not found", __PRETTY_FUNCTION__);
    }
}

bool URLServiceFeeder::hasURL(const std::string& url) {
    boost::unique_lock<boost::mutex> wl(mutex_internal);
    return mapping_url_index.hasLeftKey(url);
}

void URLServiceFeeder::setFeedMode(URLServiceFeedMode new_feed_mode) {
	feed_mode = new_feed_mode;
}


size_t URLServiceFeeder::getNumberOfURL() {
    boost::unique_lock<boost::mutex> wl(mutex_internal);
    return mapping_url_index.size();
}
