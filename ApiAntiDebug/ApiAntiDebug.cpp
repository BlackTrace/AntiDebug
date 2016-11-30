// ApiAntiDebug.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "ApiAntiDebug.h"
#include <windows.h>

BOOL WINAPI IsDebug()
{
	DWORD dwRet = 0;
	BOOL bRet = FALSE;

	// 检查函数开头是否有CC断点以及是否被hook
	if (*(BYTE*)IsDebug == 0xCC)
	{
		bRet = TRUE;
	}
	if (*(BYTE*)IsDebug == 0x64)
	{
		bRet = TRUE;
	}
	
	// 1.直接调用API
	bRet = IsDebuggerPresent();

	__asm
	{
		// 2. IsDebuggerPresent函数原型，获取PEB地址，PEB第三个字节存放的调试标志
		mov     eax, fs:[0x30]
		movzx   eax,byte ptr ds:[eax+2]
		mov		bRet, eax
	}

	// 3. 查看制定进程是否被调试，本质还是调用NtQueryInformationProcess查看debugport
	CheckRemoteDebuggerPresent(OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId()), &bRet);

	// 4. 查看PEB的NtGlobalFlags标志位
	__asm
	{
		mov eax, fs:[30h]
		mov eax, [eax+68h]
		and eax, 0x70
		mov bRet, eax
	}

	return (bRet == 1) || (dwRet == 1);
}


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (IsDebug())
	{
		MessageBoxW(NULL, L"process is being debugged", L"AntiDebug", MB_OK);
	}
	else
	{
		MessageBoxW(NULL, L"process is not being debugged", L"AntiDebug", MB_OK);
	}

	return 1;
}