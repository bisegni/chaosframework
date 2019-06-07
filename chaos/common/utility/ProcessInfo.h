#ifndef __PROCESSINFO__
#define __PROCESSINFO__
#include <chaos/common/data/CDataWrapper.h>
namespace chaos {
    namespace common{
        namespace utility {
            namespace process {
                typedef struct pserverStat {
                    float userTime;
                    float sysTime;
                    float ioWaitTime;
                    float idleTime;
                    pserverStat():idleTime(0),ioWaitTime(0),sysTime(0),userTime(0){}
                } pserverStat_t;
                typedef struct psprocStat {
                    float userTime;
                    float sysTime;
                    unsigned int vmem;
                    unsigned int rmem;
                    psprocStat():userTime(0),sysTime(0),vmem(0),rmem(0){}
                } psprocStat_t;

                class ProcessInfo {

                    private:
                    unsigned long cpu_total_time; 
                    unsigned long prev_cpu_time[10];
                    unsigned long prev_user_tick, prev_system_tick, prev_total;
                    pserverStat_t lastServerStat;
                    psprocStat_t lastProcStat;
                    uint64_t epoch_start;
                    int mypid;
                    void elab();
                    public:
                        ProcessInfo();
                        ProcessInfo(int pid);

                 
                    pserverStat_t serverStat();
                    psprocStat_t processStat();
                    chaos::common::data::CDWUniquePtr fullStat();
                };
            };
        };
    };
};


#endif