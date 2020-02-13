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
#define OPT_HA_ZONE_NAME                "ha-zone-name"
#define OPT_BATCH_SANDBOX_SIZE          "batch-sandbox-size"
#define OPT_PERSITENCE_IMPL				"persistence-impl"
#define OPT_PERSITENCE_SERVER_ADDR_LIST	"persistence-servers"
#define OPT_PERSITENCE_KV_PARAMTER		"persistence-kv-param"
#define OPT_SYNCTIME_ERROR		        "timesync-max-error"
       
#define OPT_CRON_JOB_CHECK              "cron-job-check-repeat-time"
#define OPT_CRON_JOB_AGEING_MANAGEMENT	"cron-job-ageing-management-repeat-time"
        
        //cache configuration
#define OPT_CACHE_LOG_METRIC                    "cache-log-metric"
#define OPT_CACHE_LOG_METRIC_UPDATE_INTERVAL    "cache-log-metric-update-interval"
#define OPT_CACHE_SERVER_LIST                   "cache-servers"
#define OPT_CACHE_DRIVER                        "cache-driver"
#define OPT_CACHE_DRIVER_KVP                    "cache-driver-kvp"
#define OPT_CACHE_DRIVER_POOL_MIN_INSTANCE      "cache-driver-pool-min-instance"
        
#define OPT_ARCHIVER_NUM                         "archiver-instances"
#define OPT_ARCHIVER_THREAD                      "archiver-thread"
#define OPT_ARCHIVER_QUEUE_PUSH_TIMEOUT          "archiver-queue-push-timeout"
    }
}
#endif
