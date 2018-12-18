#ifdef _WIN32

#include <Windows.h>
#include <LM.h>
#include <signal.h>


#include "ChaosCommonWin.h"

std::string strsignal(int sig)
{
	switch (sig)
	{
	case SIGABRT:
	case SIGABRT_COMPAT: return "SIGABRT";
	case SIGFPE: return "SIGFPE";
	case SIGILL: return "SIGILL";
	case SIGBREAK: return "SIGBREAK";
	case SIGSEGV: return "SIGSEGV";
	case SIGINT: return "SIGINT";
	default: return "UNKNOWN";
	}


}

int uname(struct utsname* out)
{
	SYSTEM_INFO info;
	GetNativeSystemInfo(&info);
	switch (info.wProcessorArchitecture)
	{
	case PROCESSOR_ARCHITECTURE_AMD64: strncpy(out->machine, "x64", 65); break;
	case PROCESSOR_ARCHITECTURE_ARM: strncpy(out->machine, "ARM", 65); break;
	case PROCESSOR_ARCHITECTURE_IA64: strncpy(out->machine, "Intel Itanium-based", 65); break;
	case PROCESSOR_ARCHITECTURE_INTEL: strncpy(out->machine, "x86", 65); break;
	default: strncpy(out->machine, "Unknown", 65);
	}
	DWORD dwLevel = 100;
	LPWKSTA_INFO_100 pBuf = NULL;
	NET_API_STATUS nStatus;
	nStatus = NetWkstaGetInfo(NULL, dwLevel,(LPBYTE*) pBuf);
	if (nStatus == NERR_Success)
	{
		char tmpVer[16];
		std::string platform;
		switch (pBuf->wki100_platform_id)
		{
		case PLATFORM_ID_DOS: platform = "dos"; break;
		case PLATFORM_ID_NT:  platform = "WinNT"; break;
		case PLATFORM_ID_OS2: platform = "os/2"; break;
		case PLATFORM_ID_OSF: platform = "OSF"; break;
		case PLATFORM_ID_VMS: platform = "OpenVMS"; break;
		default: platform = "Unknown"; break;
		}

		strncpy(out->sysname, platform.c_str(), 65);
		char tmpName[65];
		wcstombs(tmpName, pBuf->wki100_computername, 65);
		strncpy(out->nodename, tmpName, 65);
		itoa(pBuf->wki100_ver_major, tmpVer, 10);
		strncpy(out->release, tmpVer, 65);
		itoa(pBuf->wki100_ver_minor, tmpVer, 10);
		strncpy(out->version, tmpVer, 65);
		return 0;
	}
	return -1;
}


#endif