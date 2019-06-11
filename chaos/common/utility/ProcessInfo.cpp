#include "ProcessInfo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#ifndef _WIN32
#include <unistd.h>
#endif
namespace chaos {
namespace common {
namespace utility {
namespace process {
ProcessInfo::ProcessInfo(){
    epoch_start=time(NULL);
#ifndef _WIN32
    mypid=getpid();
#else
	mypid = GetCurrentProcessId();
#endif
     cpu_total_time=prev_user_tick=prev_system_tick=prev_total=0;
    for(int cnt=0;cnt<sizeof(prev_cpu_time)/sizeof(unsigned long);cnt++) prev_cpu_time[cnt]=0;
}
ProcessInfo::ProcessInfo(int pid){
    mypid=pid;
    epoch_start=time(NULL);
    cpu_total_time=prev_user_tick=prev_system_tick=prev_total=0;
    for(int cnt=0;cnt<sizeof(prev_cpu_time)/sizeof(unsigned long);cnt++) prev_cpu_time[cnt]=0;
                    

}
void ProcessInfo::elab(){
  #ifdef __linux__
  FILE* fstat = fopen("/proc/stat", "r");
  if (fstat == NULL) {
    fprintf(stderr, "cannot open /proc/stat\n");
    return;
  }
  unsigned long cpu_time[10];

  // bzero(cpu_time, sizeof(cpu_time));
  if (fscanf(fstat, "%*s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu", &cpu_time[0], &cpu_time[1], &cpu_time[2], &cpu_time[3], &cpu_time[4], &cpu_time[5], &cpu_time[6], &cpu_time[7], &cpu_time[8], &cpu_time[9]) == EOF) {
    fclose(fstat);
    return ;
  }
  fclose(fstat);

  for (int i = 0; i < 10; i++) {
    cpu_total_time += cpu_time[i];
  }
  if (prev_total == 0) {
    lastServerStat.userTime   = ((float)100.0 * cpu_time[0]) / cpu_total_time;
    lastServerStat.sysTime    = ((float)100.0 * cpu_time[2]) / cpu_total_time;
    lastServerStat.idleTime   = ((float)100.0 * cpu_time[3]) / cpu_total_time;
    lastServerStat.ioWaitTime = ((float)100.0 * cpu_time[4]) / cpu_total_time;
  } else {
    lastServerStat.userTime   = ((float)100.0 * (cpu_time[0] - prev_cpu_time[0])) / (cpu_total_time - prev_total);
    lastServerStat.sysTime    = ((float)100.0 * (cpu_time[2] - prev_cpu_time[2])) / (cpu_total_time - prev_total);
    lastServerStat.idleTime   = ((float)100.0 * (cpu_time[3] - prev_cpu_time[3])) / (cpu_total_time - prev_total);
    lastServerStat.ioWaitTime = ((float)100.0 * (cpu_time[4] - prev_cpu_time[4])) / (cpu_total_time - prev_total);
  }

  for (int i = 0; i < 10; i++) {
    prev_cpu_time[i] = cpu_time[i];
  }
  // printf("++++++> %u %u %u",cpu_time[0],cpu_time[2],cpu_time[3]);
#endif
#ifdef __linux__

  FILE*                fpstat;
  char                 stat_filepath[30];
  unsigned long        utime_ticks, stime_ticks, cutime_ticks, cstime_ticks, vsize, rss;

  sprintf(stat_filepath, "/proc/%d/stat", mypid);
  fpstat = fopen(stat_filepath, "r");
  if (fpstat == NULL) {
    fprintf(stderr, "cannot open %s\n", stat_filepath);

    return ;
  }
  if (fscanf(fpstat, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu"
                     "%lu %ld %ld %*d %*d %*d %*d %*u %u %ld",
             &utime_ticks,
             &stime_ticks,
             &cutime_ticks,
             &cstime_ticks,
             &lastProcStat.vmem,
             &rss) == EOF) {
    fclose(fpstat);
    return ;
  }
  fclose(fpstat);
  lastProcStat.rmem = rss * getpagesize();
  if ( prev_total == 0) {
    lastProcStat.userTime = ((float)100.0 * (utime_ticks + cutime_ticks)) / cpu_total_time;
    lastProcStat.sysTime  = ((float)100.0 * (stime_ticks + cstime_ticks)) / cpu_total_time;

  } else {
    lastProcStat.userTime = ((float)100.0 * (utime_ticks + cutime_ticks -  prev_user_tick)) / (cpu_total_time -  prev_total);
    lastProcStat.sysTime  = ((float)100.0 * (stime_ticks + cstime_ticks -  prev_system_tick)) / (cpu_total_time -  prev_total);
  }
  prev_total = cpu_total_time;

   prev_user_tick   = (utime_ticks + cutime_ticks);
   prev_system_tick = (stime_ticks + cstime_ticks);
  // printf("----> %lu %lu %u",utime_ticks,cutime_ticks,total_time);
#endif
   
              ;

}
pserverStat_t ProcessInfo::serverStat() {
  elab();
  return lastServerStat;
}
psprocStat_t ProcessInfo::processStat() {
  elab();
  return lastProcStat;
}
chaos::common::data::CDWUniquePtr ProcessInfo::fullStat(){
    chaos::common::data::CDWUniquePtr info(new chaos::common::data::CDataWrapper());
    elab();
    uint64_t n=time(NULL) - epoch_start;
    int day = n / (24 * 3600); 
  
    n %= (24 * 3600); 
    int hour = n / 3600; 
  
    n %= 3600; 
    int minutes = n / 60 ; 
  
    n %= 60; 
    int seconds = n; 
    char uptime[256];
    sprintf(uptime,"%.3dd%.2dh%.2dm%.2ds",day,hour,minutes,seconds);
    info->addInt32Value("pid",mypid);
    info->addStringValue("uptime",uptime);
    info->addDoubleValue("user",lastServerStat.userTime);
    info->addDoubleValue("sys",lastServerStat.sysTime);
    info->addDoubleValue("idle",lastServerStat.idleTime);
    info->addDoubleValue("io",lastServerStat.ioWaitTime);
    

    info->addDoubleValue("Puser",lastProcStat.userTime);
    info->addDoubleValue("Psys",lastProcStat.sysTime);
    info->addDoubleValue("Vmem",lastProcStat.vmem);
    info->addDoubleValue("Rmem",lastProcStat.rmem);
    return info;

}

};  // namespace process
};  // namespace utility
};  // namespace common
};