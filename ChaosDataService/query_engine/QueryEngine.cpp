/*
 *	QueryAnswerEngine.h
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

#include "QueryEngine.h"

#include <boost/format.hpp>

using namespace chaos::data_service::query_engine;
namespace chaos_direct_io = chaos::common::direct_io;
namespace chaos_direct_io_ch = chaos::common::direct_io::channel;


#define QueryEngine_LOG_HEAD "[QueryDataConsumer] - "

#define QEAPP_ LAPP_ << QueryEngine_LOG_HEAD
#define QEDBG_ LDBG_ << QueryEngine_LOG_HEAD << __FUNCTION__ << " - "
#define QEERR_ LERR_ << QueryEngine_LOG_HEAD << __FUNCTION__ << "(" << __LINE__ << ") - "

#define QUERY_INFO(x) "query on key:" << x.query.did << "(since: " << x.query.start_ts << " to: " << x.query.end_ts << ") has been submitted"


DataCloudQuery::DataCloudQuery(const std::string& _query_id,
							   const db_system::DataPackIndexQuery& _query,
							   const std::string& _answer_endpoint):
query_id(_query_id),
answer_endpoint(_answer_endpoint),
query_computed_unique_id(boost::str(boost::format("%1%_%2%") % query_id % answer_endpoint)),
query(_query),
query_phase(DataCloudQueryPhaseNeedSearch),
total_data_pack_sent(0),
vfs_query(NULL){};

DataCloudQuery::~DataCloudQuery(){};

int DataCloudQuery::startQuery() {
	int err = 0;
	if(!vfs_query) return -1;
	if((err = vfs_query->executeQuery()) ) {
		QEERR_ << "Error executing query";
		query_phase = DataCloudQueryPhaseError;
	} else {
		query_phase = DataCloudQueryPhaseHaveData;
	}
	return err;
}

int DataCloudQuery::fecthData(std::vector< boost::shared_ptr<vfs::FoundDataPack> >& found_data_pack, unsigned int element_to_fecth) {
	if(!vfs_query) return -1;
	int err = 0;
	if((err = vfs_query->nextNDataPack(found_data_pack, element_to_fecth))) {
		query_phase = DataCloudQueryPhaseError;
		QEERR_ << "Error fetching query data";
	} else {
		if(found_data_pack.size() == 0) {
			query_phase = DataCloudQueryPhaseEnd;
		}
	}
	return err;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
ClientConnectionInfo::ClientConnectionInfo():
connection(NULL),
channel(NULL){}

ClientConnectionInfo::~ClientConnectionInfo(){
	if(connection) delete(connection);
	if(channel) delete(channel);
	
}
/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
QueryEngine::QueryEngine(chaos_direct_io::DirectIOClient *_directio_client_instance,
						 unsigned int _thread_pool_size,
						 vfs::VFSManager *_vfs_manager_ptr):
directio_client_instance(_directio_client_instance),
work_on_query(false),
thread_pool_size(_thread_pool_size),
vfs_manager_ptr(_vfs_manager_ptr),
query_queue(1) {
	
}

QueryEngine::~QueryEngine() {
	
}

void QueryEngine::init(void *init_data)  throw(chaos::CException) {
	if(!directio_client_instance) throw chaos::CException(-1, "No direct io setupped", __FUNCTION__);
	utility::InizializableService::initImplementation(directio_client_instance, init_data, "directio_client_instance", __PRETTY_FUNCTION__);
	
	if(!vfs_manager_ptr) throw chaos::CException(-2, "No vfs manager setupped", __FUNCTION__);
	
}

void QueryEngine::start() throw(chaos::CException) {
	QEAPP_ << "Starting thread pool of size " << thread_pool_size;
	//add al thread to the pool
	work_on_query = true;
	for(int idx = 0; idx < thread_pool_size; idx++) {
		answer_thread_pool.add_thread(new boost::thread(boost::bind(&QueryEngine::process_query, this)));
	}
}

void QueryEngine::stop() throw(chaos::CException) {
	work_on_query = false;
	answer_thread_pool.join_all();
}

void QueryEngine::deinit() throw(CException) {
	utility::InizializableService::deinitImplementation(directio_client_instance, "directio_client_instance", __PRETTY_FUNCTION__);
}

//execute a query and manage the result
void QueryEngine::executeQuery(DataCloudQuery *query) {
	if(!query) return;
	
	//get a new vfs query
	if(vfs_manager_ptr->getVFSQuery(query->query, &query->vfs_query)) {
		QEERR_ << "error quetting VFSQuery for " << QUERY_INFO((*query));
	}
	//set the next phase of the query
	query->query_phase = DataCloudQuery::DataCloudQueryPhaseNeedSearch;
	
	if(!query_queue.push(query)) {
		QEERR_ << "error submitting for "<< QUERY_INFO((*query));
		delete(query);
	}
	QEDBG_ << "Successfull submission for " << QUERY_INFO((*query));
}

void QueryEngine::clearHashTableElement(const void *key, uint32_t key_len, ClientConnectionInfo *element) {
	disposeClientConnectionInfo(element);
}

void QueryEngine::disposeClientConnectionInfo(ClientConnectionInfo *client_info) {
	CHAOS_ASSERT(client_info)
	std::string connection_custom_id = client_info->connection->getCustomStringIdentification();
	std::string server_desc = client_info->connection->getServerDescription();
	
	DEBUG_CODE(QEDBG_ << "starting purge operation for " << server_desc;)
	if(client_info->channel) {
		client_info->connection->releaseChannelInstance(client_info->channel);
	}
	
	if(client_info->connection) {
		directio_client_instance->releaseConnection(client_info->connection);
	}
	
	delete (client_info);
	DEBUG_CODE(QEDBG_ << "ending purge operation for " << server_desc;)
}


void QueryEngine::handleEvent(chaos_direct_io::DirectIOClientConnection *client_connection,
							  chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event) {
	ClientConnectionInfo *connection_info_ptr = NULL;
	
	//get the unique key for query
	std::string unique_query_key = client_connection->getCustomStringIdentification();
	if(DirectIOChannelHashTable::getElement(unique_query_key.c_str(),
											(uint32_t)unique_query_key.size(),
											&connection_info_ptr)) {
		//we can't be here
		return;
	}
	
	//set the connection state in query
	connection_info_ptr->query->answer_connection_state = event;
}

int  QueryEngine::getChannelForQuery(const DataCloudQuery& query,
									 ClientConnectionInfo **connection_info_handle) {
	//allocate server key
	if(!DirectIOChannelHashTable::getElement(query.query_computed_unique_id.c_str(),
											 (uint32_t)query.query_computed_unique_id.size(),
											 connection_info_handle)) {
		return 0;
	}
	
	//allocate the client info struct
	*connection_info_handle = new ClientConnectionInfo();
	
	if(!*connection_info_handle) return -1;
	
	//get connection
	(*connection_info_handle)->connection = directio_client_instance->getNewConnection(query.answer_endpoint);
	
	if(!(*connection_info_handle)->connection) {
		disposeClientConnectionInfo(*connection_info_handle);
		*connection_info_handle = NULL;
		return -1;
	}
	//set query engine as handler of event on client conenction
	(*connection_info_handle)->connection->setEventHandler(this);
	
	//! set connection custo identification key
	(*connection_info_handle)->connection->setCustomStringIdentification(query.query_computed_unique_id);
	
	//get the channel
	(*connection_info_handle)->channel = dynamic_cast<chaos_direct_io_ch::DirectIODeviceClientChannel*>((*connection_info_handle)->connection->getNewChannelInstance("DirectIODeviceClientChannel"));
	if(!(*connection_info_handle)->channel) {
		disposeClientConnectionInfo(*connection_info_handle);
		*connection_info_handle = NULL;
		return -1;
	}
	(*connection_info_handle)->channel->setDeviceID(query.query.did);
	
	//all is gone well
	//now we can add client and channel to the maps
	DirectIOChannelHashTable::addElement(query.query_computed_unique_id.c_str(),
										 (uint32_t)query.query_computed_unique_id.size(),
										 *connection_info_handle);
	
	return 0;
}

int  QueryEngine::sendDataToClient(DataCloudQuery& query,
								   const std::vector<boost::shared_ptr<vfs::FoundDataPack> >& data) {
	//fecth connection channel
	ClientConnectionInfo *connection_info_ptr = NULL;
	int err = getChannelForQuery(query, &connection_info_ptr);
	if(!err && connection_info_ptr) {
		for (std::vector<boost::shared_ptr<vfs::FoundDataPack> >::const_iterator it = data.begin();
			 err == 0 && it != data.end();
			 it++) {
			
			//send data packet
			err = (int)connection_info_ptr->channel->sendResultToQueryDataCloud(query.query_id,
																				query.vfs_query->getNumberOfElementFound(),
																				++query.total_data_pack_sent,
																				(*it)->data_pack_buffer,
																				(*it)->data_pack_size);
			//check for error
			if(query.answer_connection_state == chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected ||
			   err) {
				LDBG_ << "Client has been disconnected for " << QUERY_INFO(query);
				err = -1;
			}
		}
	}
	return err;
}

void QueryEngine::process_query() {
	int err = 0;
	DataCloudQuery *query = NULL;
	std::vector<boost::shared_ptr<vfs::FoundDataPack> > found_datapac_vec;
	
	while(work_on_query) {
		if(query_queue.pop(query) && query) {
			//clear the vector of precedente result
			found_datapac_vec.clear();
			
			//step on current query
			switch(query->query_phase) {
				case DataCloudQuery::DataCloudQueryPhaseNeedSearch:
					//start the query
					LDBG_ << "Start "<< QUERY_INFO((*query));
					err = query->startQuery();
					
				case DataCloudQuery::DataCloudQueryPhaseHaveData:
					LDBG_ << "Answer "<< QUERY_INFO((*query));
					if(!(err = query->fecthData(found_datapac_vec, 50))) {
						if((err = sendDataToClient(*query, found_datapac_vec))) {
							//if there is an error sending data back to client remove the query
							query->query_phase = DataCloudQuery::DataCloudQueryPhaseError;
						}
					}
					break;
					
				default:
					//do noting query will be erased
					break;
			}
			
			if(query->query_phase == DataCloudQuery::DataCloudQueryPhaseError ||
			   query->query_phase == DataCloudQuery::DataCloudQueryPhaseEnd) {
				ClientConnectionInfo *connection_info_ptr = NULL;
				std::string query_unique_id = query->query_computed_unique_id;
				if(!DirectIOChannelHashTable::getElement(query->query_computed_unique_id.c_str(),
														 (uint32_t)query_unique_id.size(),
														 &connection_info_ptr) && connection_info_ptr) {
					//dispose conenction info
					disposeClientConnectionInfo(connection_info_ptr);
					
					//remove connection info
					DirectIOChannelHashTable::removeElement(query->query_computed_unique_id.c_str(),
															(uint32_t)query_unique_id.size());
				}
				
				delete(query);
				LDBG_ << "Deleted "<< QUERY_INFO((*query));
				
			} else {
				//reschedule query for next step
				if(!query_queue.push(query)) {
					QEERR_ << "Error rescheduling "<< QUERY_INFO((*query));
				} else {
					QEDBG_ << "Succcesfull rescheduled "<< QUERY_INFO((*query));
				}
			}
		} else {
			boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
		}
	}
}