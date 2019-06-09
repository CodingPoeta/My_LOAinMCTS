#pragma once
#include "windows.h"

//windows 获取内存使用率
inline DWORD getWin_MemUsage() {
	MEMORYSTATUS ms;
	::GlobalMemoryStatus(&ms);
	return ms.dwMemoryLoad;
}
