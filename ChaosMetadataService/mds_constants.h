//
//  mds_constants.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 21/01/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

#ifndef CHAOSFramework_mds_constants_h
#define CHAOSFramework_mds_constants_h

namespace chaos {
    namespace metadata_service {
#define OPT_BATCH_SANDBOX_SIZE          "batch-sandbox-size"
#define OPT_PERSITENCE_IMPL				"persistence-impl"
#define OPT_PERSITENCE_SERVER_ADDR_LIST	"persistence-servers"
#define OPT_PERSITENCE_KV_PARAMTER		"persistence-kv-param"
        
#define OPT_CRON_JOB_CHECK              "cron-job-check-repeat-time"
#define OPT_CRON_JOB_AGEING_MANAGEMENT	"cron-job-ageing-management-repeat-time"
        
        
#define OPT_ENABLE_DATA_SERVICE                 "enable-dataservice"
#define OPT_ENABLE_METADATA_SERVICE				"enable-metadata-service"
        
        //cache configuration
#define OPT_CACHE_LOG_METRIC                    "cache-log-metric"
#define OPT_CACHE_LOG_METRIC_UPDATE_INTERVAL    "cache-log-metric-update-interval"
#define OPT_CACHE_SERVER_LIST                   "cache-servers"
#define OPT_CACHE_DRIVER                        "cache-driver"
#define OPT_CACHE_DRIVER_KVP                    "cache-driver-kvp"
#define OPT_CACHE_DRIVER_POOL_MIN_INSTANCE      "cache-driver-pool-min-instance"
        
#define OPT_DATA_WORKER_LOG_METRIC                  "data-worker-log-metric"
#define OPT_DATA_WORKER_LOG_METRIC_UPDATE_INTERVAL  "data-worker-log-metric-update-interval"
#define OPT_DATA_WORKER_NUM                         "data-worker-num"
#define OPT_DATA_WORKER_THREAD                      "data-worker-thread"
#define DATA_WORKER_NUMBER                          1
        
        //stage data indexer
#define OPT_INDEXER_WORKER_NUM			"indexer-worker-num"
#define OPT_INDEXER_SCAN_DELAY			"indexer-scan-delay"
#define INDEXER_DEFAULT_WORKER_NUMBER	1
#define INDEXER_DEFAULT_SCAN_DELAY		60
    }
}
#endif
