// ApiAntiDebug.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "ApiAntiDebug.h"
#include <windows.h>

BOOL WINAPI IsDebug()
{
	DWORD dwRet = 0;
	BOOL bRet = FALSE;

	// ��麯����ͷ�Ƿ���CC�ϵ��Լ��Ƿ�hook
	if (*(BYTE*)IsDebug == 0xCC)
	{
		bRet = TRUE;
	}
	if (*(BYTE*)IsDebug == 0x64)
	{
		bRet = TRUE;
	}
	
	// 1.ֱ�ӵ���API
	bRet = IsDebuggerPresent();

	__asm
	{
		// 2. IsDebuggerPresent����ԭ�ͣ���ȡPEB��ַ��PEB�������ֽڴ�ŵĵ��Ա�־
		mov     eax, fs:[0x30]
		movzx   eax,byte ptr ds:[eax+2]
		mov		bRet, eax
	}

	// 3. �鿴�ƶ������Ƿ񱻵��ԣ����ʻ��ǵ���NtQueryInformationProcess�鿴debugport
	CheckRemoteDebuggerPresent(OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId()), &bRet);

	// 4. �鿴PEB��NtGlobalFlags��־λ
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