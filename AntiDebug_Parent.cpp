// AntiDebug_Parent.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <Tlhelp32.h>
//#include <Winternl.h>

typedef struct
{
	DWORD ExitStatus;
	DWORD PebBaseAddress;
	DWORD AffinityMask; 
	DWORD BasePriority;
	ULONG UniqueProcessId;
	ULONG ParentProcessId;
} PROCESS_BASIC_INFORMATION;

typedef enum _PROCESSINFOCLASS {
	ProcessBasicInformation = 0,
	ProcessDebugPort = 7,
	ProcessWow64Information = 26,
	ProcessImageFileName = 27
} PROCESSINFOCLASS;

typedef NTSTATUS (CALLBACK *NTQUERYINFORMATIONPROCESS)(  
	HANDLE ProcessHandle,   
	PROCESSINFOCLASS processInfo,
	PVOID ProcessInformation,  
	ULONG ProcessInformationLength,  
	PULONG ReturnLength OPTIONAL  
	);  


// ��鸸�����Ƿ���explorer
BOOL WINAPI IsCreateByExplorer()
{
	HMODULE hNtdll = NULL;
	HANDLE hProcess = NULL;
	DWORD dwCurrentID = 0;
	DWORD dwReturnLength = 0;
	NTQUERYINFORMATIONPROCESS pNTQueryInformationProcess = NULL;
	PROCESS_BASIC_INFORMATION stProcessInfo;
	BOOL bRet = FALSE;
	DWORD Debugport = 0;
	HANDLE hSnapshot = NULL;
	PROCESSENTRY32 stPE;

	__try
	{
		ZeroMemory(&stProcessInfo, sizeof(PROCESS_BASIC_INFORMATION));
		ZeroMemory(&stPE, sizeof(PROCESSENTRY32));
		stPE.dwSize = sizeof(PROCESSENTRY32);

		// ��ȡ���̻�����Ϣ
		hNtdll = (HMODULE)LoadLibraryW(L"NTDLL.dll");
		if (NULL == hNtdll)
		{
			printf("��ȡntdll���ʧ�ܣ�\n");
			__leave;
		}

		pNTQueryInformationProcess = (NTQUERYINFORMATIONPROCESS)GetProcAddress(hNtdll, "NtQueryInformationProcess");
		if (NULL == pNTQueryInformationProcess)
		{
			printf("��ȡNtQueryInformationProcess������ַʧ�ܣ�\n");
			__leave;
		}

		dwCurrentID = GetCurrentProcessId();
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, dwCurrentID);
		if (NULL == hProcess)
		{
			printf("OpenProcessʧ�ܣ�\n");
			__leave;
		}

		pNTQueryInformationProcess(hProcess, ProcessBasicInformation, &stProcessInfo, sizeof(stProcessInfo), &dwReturnLength);

		// ��������
		hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (INVALID_HANDLE_VALUE == hSnapshot)
		{
			printf("��ȡ���̿���ʧ�ܣ�\n");
			__leave;
		}
		
		if (Process32FirstW(hSnapshot, &stPE))
		{
			do 
			{
				if (stProcessInfo.ParentProcessId == stPE.th32ProcessID)
				{
					if (lstrcmpiW(stPE.szExeFile, L"explorer.exe") == 0)
					{
						bRet = TRUE;
					}
					else
					{
						wprintf(L"process is created by %s\n", stPE.szExeFile);
					}
				}
			} while (Process32NextW(hSnapshot, &stPE));
		}
		else
		{
			printf("Process32FirstW failed, error code = %d\n", GetLastError());
		}
	}

	__finally
	{
		if (hNtdll)
		{
			FreeLibrary(hNtdll);
			hNtdll = NULL;
		}
		if (hProcess)
		{
			CloseHandle(hProcess);
			hProcess = NULL;
		}
		if (hSnapshot)
		{
			CloseHandle(hSnapshot);
			hSnapshot = NULL;
		}
	}

	return bRet;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (IsCreateByExplorer())
	{
		printf("process is created by explorer\n");
	}
	system("pause");
	return 0;
}

