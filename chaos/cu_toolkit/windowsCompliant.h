#pragma once
#ifdef _WIN32
#ifndef UNIFIED_SLEEP
#define UNIFIED_SLEEP
#define sleep(a) Sleep(a * 1000)
#define usleep(a)                               \
  {                                             \
    int ms = ((a / 1000) > 1) ? (a / 1000) : 1; \
    Sleep(ms);                                  \
  }

#endif
#ifndef SIGIGNORE_WINDEF
#define SIGIGNORE_WINDEF
#define sigignore(sg) signal(sg, SIG_IGN)
#endif
#endif