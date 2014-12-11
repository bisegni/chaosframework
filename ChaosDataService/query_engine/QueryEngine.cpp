/*
 *	QueryAnswerEngine.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
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

#define QueryEngine_FORWARDING_PAGE_DIMENSION 50
#define QueryEngine_LOG_HEAD "[QueryDataConsumer] - "

#define QEAPP_ LAPP_ << QueryEngine_LOG_HEAD
#define QEDBG_ LDBG_ << QueryEngine_LOG_HEAD << __FUNCTION__ << " - "
#define QEERR_ LERR_ << QueryEngine_LOG_HEAD << __FUNCTION__ << "(" << __LINE__ << ") - "

#define QUERY_INFO(x) "query on key:" << x.query.did << "(since: " << x.query.start_ts << " to: " << x.query.end_ts << ")"


/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
DataCloudQuery::DataCloudQuery(const std::string& _query_id,
							   const db_system::DataPackIndexQuery& _query,
							   const std::string& _answer_endpoint):
query_id(_query_id),
answer_endpoint(_answer_endpoint),
query_computed_unique_id(boost::str(boost::format("%1%_%2%") % query_id % answer_endpoint)),
query(_query),
query_phase(DataCloudQueryPhaseNeedSearch),
total_data_pack_sent(0),
vfs_query(NULL){
	//set teh dafault value on element to forward for server-> client paging
	fetchedAndForwadInfo.number_of_element_to_forward = 0;
};

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
DataCloudQuery::~DataCloudQuery(){
	fetchedAndForwadInfo.fetched_data_vector.clear();
};

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
QueryEngine::QueryEngine(chaos_direct_io::DirectIOClient *_directio_client_instance,
						 unsigned int _thread_pool_size,
						 vfs::VFSManager *_vfs_manager_ptr):
directio_client_instance(_directio_client_instance),
work_on_query(false),
thread_pool_size(_thread_pool_size),
thread_semaphore(NULL),
vfs_manager_ptr(_vfs_manager_ptr),
query_queue(1) {
	
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
QueryEngine::~QueryEngine() {
	
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void QueryEngine::init(void *init_data)  throw(chaos::CException) {
	if(!directio_client_instance) throw chaos::CException(-1, "No direct io setupped", __FUNCTION__);
	utility::InizializableService::initImplementation(directio_client_instance, init_data, "directio_client_instance", __PRETTY_FUNCTION__);
	
	if(!vfs_manager_ptr) throw chaos::CException(-2, "No vfs manager setupped", __FUNCTION__);
	
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void QueryEngine::start() throw(chaos::CException) {
	QEAPP_ << "Starting thread pool of size " << thread_pool_size;
	
	//allocate the semahore for thread size
	thread_semaphore = new chaos::common::thread::ThreadSemaphore(thread_pool_size);
	
	//add al thread to the pool
	work_on_query = true;
	for(int idx = 0; idx < thread_pool_size; idx++) {
		answer_thread_pool.add_thread(new boost::thread(boost::bind(&QueryEngine::process_query, this)));
	}
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void QueryEngine::stop() throw(chaos::CException) {
	work_on_query = false;
	//unlock all thread
	thread_semaphore->signal(thread_pool_size);
	
	//waith all thread
	answer_thread_pool.join_all();

	//delete all connection
	for(MapConnectionIterator it = map_query_id_connection.begin();
		it != map_query_id_connection.end();
		it++) {
		disposeClientConnectionInfo(it->second);
	}
	
	if(thread_semaphore) delete(thread_semaphore);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void QueryEngine::deinit() throw(CException) {
	utility::InizializableService::deinitImplementation(directio_client_instance, "directio_client_instance", __PRETTY_FUNCTION__);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void QueryEngine::executeQuery(DataCloudQuery *query) {
	if(!query) return;
	CHAOS_ASSERT(vfs_manager_ptr)
    
	//get a new vfs query
	if(vfs_manager_ptr->getVFSQuery(query->query, &query->vfs_query)) {
		QEERR_ << "error quetting VFSQuery for " << QUERY_INFO((*query));
	}
	//set the next phase of the query
	query->query_phase = DataCloudQuery::DataCloudQueryPhaseNeedSearch;
	
	if(!query_queue.push(query)) {
		QEERR_ << "error submitting for "<< QUERY_INFO((*query));
		delete(query);
	} else {
		thread_semaphore->signal();
		QEDBG_ << "Successfull submission for " << QUERY_INFO((*query));
	}
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void QueryEngine::disposeQuery(DataCloudQuery *query) {
	if(!query) return;
	QEDBG_ << "Deallocating " << QUERY_INFO((*query));
	//get a new vfs query
	if(query->vfs_query) {
		QEDBG_ << "Deleteing VFSQuery for " << QUERY_INFO((*query));
		
		delete(query->vfs_query);
		
		query->vfs_query = NULL;
	}
	
	//delete query
	QEDBG_ << "Deleting " << QUERY_INFO((*query));
	delete(query);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
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
	
	free(client_info);
	DEBUG_CODE(QEDBG_ << "ending purge operation for " << server_desc;)
}


/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void QueryEngine::handleEvent(chaos_direct_io::DirectIOClientConnection *client_connection,
							  chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event) {
	
	//get the unique key for query
	boost::shared_lock<boost::shared_mutex> rlock(mutex_map_query_id_connection);
	
	std::string unique_query_key = client_connection->getCustomStringIdentification();
	MapConnectionIterator it =  map_query_id_connection.find(unique_query_key);
	if(it == map_query_id_connection.end()) {
		return;
	}
	//set the connection state in query
	if(event == chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected) {
		//if client disconnects set query in error
		it->second->query->query_phase = DataCloudQuery::DataCloudQueryPhaseClientDisconnected;
	}
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int  QueryEngine::getChannelForQuery(DataCloudQuery *query,
									 ClientConnectionInfo **connection_info_handle) {
	
	boost::upgrade_lock<boost::shared_mutex> readLock(mutex_map_query_id_connection);
	//allocate server key
	MapConnectionIterator it = map_query_id_connection.find(query->query_computed_unique_id);
	if(it != map_query_id_connection.end()) {
		*connection_info_handle = it->second;
		return 0;
	}
	
	//lock for write
	boost::upgrade_to_unique_lock<boost::shared_mutex> writeLock(readLock);
	//allocate the client info struct
	*connection_info_handle = (ClientConnectionInfo*)calloc(sizeof(ClientConnectionInfo), 1);
	
	if(!*connection_info_handle) return -1;
	
	//get connection
	(*connection_info_handle)->connection = directio_client_instance->getNewConnection(query->answer_endpoint);
	
	if(!(*connection_info_handle)->connection) {
		disposeClientConnectionInfo(*connection_info_handle);
		*connection_info_handle = NULL;
		return -1;
	}
	
	//! set connection custo identification key
	(*connection_info_handle)->connection->setCustomStringIdentification(query->query_computed_unique_id);
	
	//get the channel
	(*connection_info_handle)->channel = dynamic_cast<chaos_direct_io_ch::DirectIODeviceClientChannel*>((*connection_info_handle)->connection->getNewChannelInstance("DirectIODeviceClientChannel"));
	if(!(*connection_info_handle)->channel) {
		disposeClientConnectionInfo(*connection_info_handle);
		*connection_info_handle = NULL;
		return -1;
	}
	
	//set the query pointer to the connection info
	(*connection_info_handle)->query = query;
	
	//all is gone well
	//now we can add client and channel to the maps
	map_query_id_connection.insert(make_pair(query->query_computed_unique_id, *connection_info_handle));
	return 0;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int  QueryEngine::sendStartResultFaseToClient(DataCloudQuery *query) {
	ClientConnectionInfo	*connection_info_ptr = NULL;
	int err = getChannelForQuery(query, &connection_info_ptr);
	if(err || !connection_info_ptr) return err;
	
	//send to cient the start result phase message
	err = (int)connection_info_ptr->channel->startQueryDataCloudResult(query->query_id, query->vfs_query->getNumberOfElementFound());
	return err;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int  QueryEngine::sendDataToClient(DataCloudQuery *query) {
	//fecth connection channel
	vfs::FoundDataPack		*datapack_to_forward = NULL;
	ClientConnectionInfo	*connection_info_ptr = NULL;
	int err = getChannelForQuery(query, &connection_info_ptr);
	if(!err && connection_info_ptr) {
		//QueryEngine_FORWARDING_PAGE_DIMENSION
		//all found datapack are processed, if there is an error on a cicle,
		//in the next will not be send data over channel, but only discarded
		//in this way at the end all the found datapack will be erased
		for (int sent_count = 0;											//keep track of the page dimension of element to send
			 (sent_count<QueryEngine_FORWARDING_PAGE_DIMENSION &&			//step until the page is not sent or
			  query->fetchedAndForwadInfo.number_of_element_to_forward &&	//until we have cicle all vector
			  !err);														//and we have no err
			 sent_count++) {												//increment the page element
			
			//get next element to send
			datapack_to_forward = query->fetchedAndForwadInfo.fetched_data_vector[--query->fetchedAndForwadInfo.number_of_element_to_forward];
			//send data packet
			if(!err) {
				//previoous iteration has not failed
				datapack_to_forward->delete_on_dispose = false;
				
				//send data to client
				err = (int)connection_info_ptr->channel->sendResultToQueryDataCloud(query->query_id,
																					++query->total_data_pack_sent,
																					datapack_to_forward->data_pack_buffer,
																					datapack_to_forward->data_pack_size,
																					this);
				//check for error
				if(query->query_phase != DataCloudQuery::DataCloudQueryPhaseForwardData || err) {
					if(!err) err = -1;
				}
			}
			//delete the datapack
			delete(datapack_to_forward);
		}
	}
	return err;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int  QueryEngine::sendEndResultFaseToClient(DataCloudQuery *query, int32_t error) {
	ClientConnectionInfo	*connection_info_ptr = NULL;
	int err = getChannelForQuery(query, &connection_info_ptr);
	if(err || !connection_info_ptr) return err;
	
	//send to cient the start result phase message
	err = (int)connection_info_ptr->channel->endQueryDataCloudResult(query->query_id, error);
	return err;

}


/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void QueryEngine::freeSentData(void* sent_data_ptr, common::direct_io::DisposeSentMemoryInfo *free_info_ptr) {
	if(free_info_ptr->sent_part != common::direct_io::DisposeSentMemoryInfo::SentPartData) return;
	//deelte sent result;
	free(sent_data_ptr);
	
	QEDBG_ << "Deleted data for part "<<  free_info_ptr->sent_part<< " and opcode " << free_info_ptr->sent_opcode;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void QueryEngine::process_query() {
	int err = 0;
	int32_t query_error = 0;
	DataCloudQuery *query = NULL;
	QEDBG_ << "Enter process query thread";
	while(work_on_query) {
		if(query_queue.pop(query) && query) {
			
			//step on current query
			switch(query->query_phase) {
				case DataCloudQuery::DataCloudQueryPhaseNeedSearch:
					//start the query
                    CHAOS_ASSERT(query->vfs_query)
					QEDBG_ << "Start "<< QUERY_INFO((*query));
					if((err = query->vfs_query->executeQuery()) ) {
						QEERR_ << "Error executing query";
						query->query_phase = DataCloudQuery::DataCloudQueryPhaseError;
						
						//set error
						query_error = -1;
					} else {
						query->query_phase = DataCloudQuery::DataCloudQueryPhaseFetchData;
						sendStartResultFaseToClient(query);
					}
					break;
					
				case DataCloudQuery::DataCloudQueryPhaseFetchData:
                    CHAOS_ASSERT(query->vfs_query)
					QEDBG_ << "Answer "<< QUERY_INFO((*query));
					if((err = query->vfs_query->readDataPackPage(query->fetchedAndForwadInfo.fetched_data_vector, 100))) {
						query->query_phase = DataCloudQuery::DataCloudQueryPhaseError;
						query_error = -2;
					} else {
						if(query->fetchedAndForwadInfo.fetched_data_vector.size() == 0 &&
						   query->vfs_query->getNumberOfFetchedElement() == query->vfs_query->getNumberOfElementFound()) {
							query->query_phase = DataCloudQuery::DataCloudQueryPhaseEnd;
						}else if(query->fetchedAndForwadInfo.fetched_data_vector.size() &&
								 query->query_phase == DataCloudQuery::DataCloudQueryPhaseFetchData) {
							//got to forwarding state
							query->query_phase = DataCloudQuery::DataCloudQueryPhaseForwardData;
						}
					}
					
					//keep track of how many element we need to return to the client or manage in case of error
					query->fetchedAndForwadInfo.number_of_element_to_forward = query->fetchedAndForwadInfo.fetched_data_vector.size();
					break;
					
				case DataCloudQuery::DataCloudQueryPhaseForwardData:
					if((err = sendDataToClient(query))) {
						//if there is an error sending data back to client remove the query
						query->query_phase = DataCloudQuery::DataCloudQueryPhaseError;
						query_error = -3;
					} else if(!query->fetchedAndForwadInfo.number_of_element_to_forward) {
						//we have forward all element so whe ned to fetch another page of data
						query->query_phase = DataCloudQuery::DataCloudQueryPhaseFetchData;
						
						//reset fetched info of the query
						query->fetchedAndForwadInfo.number_of_element_to_forward = 0;
						query->fetchedAndForwadInfo.fetched_data_vector.clear();
					} else {
						//continue to send subffer of data
					}
					break;
					
				default:
					//do noting query will be erased
					break;
			}
			
			if(query->query_phase == DataCloudQuery::DataCloudQueryPhaseError ||
			   query->query_phase == DataCloudQuery::DataCloudQueryPhaseEnd ||
			   query->query_phase == DataCloudQuery::DataCloudQueryPhaseClientDisconnected) {
				//print thre reason for the query deallocation
				switch(query->query_phase) {
					case DataCloudQuery::DataCloudQueryPhaseError:
						QEDBG_ << "Error on "<< QUERY_INFO((*query));
						sendEndResultFaseToClient(query, query_error);
						break;
						
					case DataCloudQuery::DataCloudQueryPhaseEnd:
						QEDBG_ << "End on "<< QUERY_INFO((*query));
						sendEndResultFaseToClient(query, 0);
						break;
						
					case DataCloudQuery::DataCloudQueryPhaseClientDisconnected:
						QEDBG_ << "Client disconnection for "<< QUERY_INFO((*query));
						sendEndResultFaseToClient(query, -3);
						break;
						
					default:
						break;
				}
				
				//remove unsent data
				if(query->fetchedAndForwadInfo.number_of_element_to_forward) {
					QEDBG_ << "Deleting " << query->fetchedAndForwadInfo.number_of_element_to_forward << " unsend data for " << QUERY_INFO((*query));
					while(query->fetchedAndForwadInfo.number_of_element_to_forward) {
						delete(query->fetchedAndForwadInfo.fetched_data_vector[--query->fetchedAndForwadInfo.number_of_element_to_forward]);
					}
				}
				
				//read from map
				boost::upgrade_lock<boost::shared_mutex> readLock(mutex_map_query_id_connection);
				
				MapConnectionIterator it =  map_query_id_connection.find(query->query_computed_unique_id);
				
				if(it != map_query_id_connection.end()) {
					//lock for write
					boost::upgrade_to_unique_lock<boost::shared_mutex> writeLock(readLock);
					
					//dispose conenction info
					disposeClientConnectionInfo(it->second);
					
					//delete entry on map
					map_query_id_connection.erase(it);
				}
				disposeQuery(query);
			} else {
				//reschedule query for next step
				if(!query_queue.push(query)) {
					QEERR_ << "Error rescheduling "<< QUERY_INFO((*query));
					delete(query);
				} else {
					QEDBG_ << "Succcesfull rescheduled "<< QUERY_INFO((*query));
				}
			}
			query = NULL;
		} else {
			thread_semaphore->wait();
			//boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
		}
	}
	QEDBG_ << "Leave process query thread";
	
}