#pragma once
#include "windows.h"

//windows ��ȡ�ڴ�ʹ����
inline DWORD getWin_MemUsage() {
	MEMORYSTATUS ms;
	::GlobalMemoryStatus(&ms);
	return ms.dwMemoryLoad;
}
