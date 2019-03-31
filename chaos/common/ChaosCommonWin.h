#pragma once
#ifdef _WIN32
#ifndef ChaosCommonWindows_h
#define ChaosCommonWindows_h
#include <string>


 
std::string strsignal(int sig);

#ifndef _SYS_UTSNAME_H
#define _SYS_UTSNAME_H
struct utsname
{
#define _UTSNAME_LENGTH 65
	char sysname[_UTSNAME_LENGTH];
	char nodename[_UTSNAME_LENGTH];
	char release[_UTSNAME_LENGTH];
	char version[_UTSNAME_LENGTH];
	char machine[_UTSNAME_LENGTH];
#ifdef __USE_GNU
	char domainname[_UTSNAME_LENGTH];
#else
	char __domainname[_UTSNAME_LENGTH];
#endif

	
};
int uname(struct utsname* out);
#endif







#endif
#endif