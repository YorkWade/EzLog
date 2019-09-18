#include "process_info.h"


#include <Windows.h>
#include "winsock.h"


pid_t ProcessInfo::pid()
{
	return GetCurrentProcessId();/*::getpid();*/
}

std::string ProcessInfo::pidString()
{
	char buf[32];
	_snprintf/*snprintf*/(buf, sizeof buf, "%d", pid());
	return buf;
}





std::string ProcessInfo::hostname()
{
	// HOST_NAME_MAX 64
	// _POSIX_HOST_NAME_MAX 255
	char buf[256];
	if (::gethostname(buf, sizeof buf) == 0)
	{
		buf[sizeof(buf)-1] = '\0';
		return buf;
	}
	else
	{
		return "unknownhost";
	}
}
