#ifndef CHAOSDATASETIO_H
#define CHAOSDATASETIO_H

#include <chaos/common/io/ManagedDirectIODataDriver.h>

namespace chaos{
    namespace common{
        namespace message{
            class MDSMessageChannel;
        }
        namespace io{
            class QueryCursor;
        };
        namespace network{
            class NetworkBroker;
        }
    }
};

namespace driver{
    namespace misc{
        typedef ChaosSharedPtr<chaos::common::data::CDataWrapper> ChaosDataSet;
        class ChaosDatasetIO             :protected chaos::common::async_central::TimerHandler{
            // chaos::common::io::IODataDriverShrdPtr ioLiveDataDriver;
            ChaosSharedPtr<chaos::common::io::ManagedDirectIODataDriver> ioLiveDataDriver;
            chaos::common::network::NetworkBroker        *network_broker;
            chaos::common::message::MDSMessageChannel    *mds_message_channel;
            
            typedef struct {uint64_t qt;
                uint32_t page_len;
                chaos::common::io::QueryCursor * qc;
            } qc_t;
            typedef std::map<uint64_t,qc_t> query_cursor_map_t;
            query_cursor_map_t query_cursor_map;
            uint64_t query_index;
            chaos::common::data::CDataWrapper wrapper2dataset(chaos::common::data::CDataWrapper& in,int dir=chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT);
        protected:
            
            uint64_t runid;
            std::string datasetName; // cu name
            std::string groupName; // US name
            uint64_t ageing;
            uint64_t timeo;
            uint64_t last_seq;
            int storageType;
            std::map<int,ChaosDataSet > datasets;
            uint64_t pkids[16];
            void createMDSEntry();
            bool entry_created;
            uint32_t defaultPage;
            std::string uid;
            double last_push_rate_grap_ts;
            void updateHealth();
            void timeout();
        public:
            
            ChaosDatasetIO(const std::string& dataset_name,const std::string &group_name="DATASETIO");
            ~ChaosDatasetIO();
            int setAgeing(uint64_t secs);
            int setStorage(int st);
            int setTimeo(uint64_t t);
            ChaosDataSet allocateDataset(int type=chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT);
            /**
             
             */
            int registerDataset ();
            int pushDataset( int type=chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT);
            /**
             Retrieve its own datasets from live
             */
            ChaosDataSet getDataset(int type=chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT);
            /**
             Retrieve whatever dataset
             */
            ChaosDataSet getDataset(const std::string &dsname,int type=chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT);
            /**
             Perform the query and return uid for pages
             \param dsname dataset name
             \param type type (output, input)
             \param start epoch in ms start of search
             \param end epoch in ms stop of search
             \param page number of item to return
             \return uid to be used to get page of datas, 0 if nothing found
             */
            uint64_t queryHistoryDatasets(const std::string &dsname, uint64_t ms_start,uint64_t ms_end,uint32_t page,int type=chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT);
            /**
             Same query over owned datasets
             */
            uint64_t queryHistoryDatasets(uint64_t ms_start,uint64_t ms_end,uint32_t page,int type=chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT);
            /**
             Not paged query, returns whole query
             */
            std::vector<ChaosDataSet> queryHistoryDatasets(const std::string &dsname, uint64_t ms_start,uint64_t ms_end,int type=chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT);
            std::vector<ChaosDataSet> queryHistoryDatasets(uint64_t ms_start,uint64_t ms_end,int type=chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT);
            bool queryHasNext(uint64_t uid);
            std::string getUid(){return uid;}
            uint64_t getRunID(){return runid;}
            void setRunID(uint64_t ru){runid=ru;}
            std::vector<ChaosDataSet> getNextPage(uint64_t uid);
            
        };
    }}

#endif // CHAOSCULIGHT_H
