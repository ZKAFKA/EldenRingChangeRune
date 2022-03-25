#include<Windows.h>
#include<iostream>
#include<TlHelp32.h>
using namespace std;

DWORDLONG offset0 = 0x03A45AE8;
DWORDLONG offset1 = 0x180;
DWORDLONG offset2 = 0x6C;

PVOID GetProcessImageBase1(DWORD dwProcessId)
{
	PVOID pProcessImageBase = NULL;
	MODULEENTRY32 me32 = { 0 };
	me32.dwSize = sizeof(MODULEENTRY32);

	// 获取指定进程全部模块的快照
	HANDLE hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPALL, dwProcessId);
	if (INVALID_HANDLE_VALUE == hModuleSnap)
	{
		cout << "failed" << endl;
		return 0;
	}

	// 获取快照中第一条信息
	BOOL bRet = ::Module32First(hModuleSnap, &me32);
	if (bRet)
	{
		// 获取加载基址
		pProcessImageBase = (PVOID)me32.modBaseAddr;
		cout << pProcessImageBase << endl;
	}

	// 关闭句柄
	CloseHandle(hModuleSnap);
	
	return pProcessImageBase;
}

int main()
{
	HWND hwnd = FindWindowW(NULL, L"ELDEN RING™");
	DWORD pid = 0;
	DWORD realWrite = 0;
	if (hwnd == NULL)
	{
		MessageBoxA(0, "游戏未运行", "提示", MB_OK);
		return 0;
	}
	GetWindowThreadProcessId(hwnd, &pid);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	//获取进程基址	
	PVOID runeBaseAddress = GetProcessImageBase1(pid);
	DWORDLONG runeAddressStart = (DWORDLONG)runeBaseAddress + offset0;
	cout << (LPVOID)runeAddressStart << endl;
	DWORD getLastError;
	SIZE_T dwSize = 0;

	//一级偏移
	DWORDLONG OffsetFirstValue = 0;
	if (0 == ReadProcessMemory(hProcess, (LPVOID)(runeAddressStart), &OffsetFirstValue, sizeof(DWORD), &dwSize))
	{
		printf_s("一级偏移获取失败\n");
		getLastError = GetLastError();
	}
	cout << "一级偏移后值" << (LPVOID)OffsetFirstValue << endl;
	//修饰一级偏移
	OffsetFirstValue = (DWORDLONG)OffsetFirstValue + 0x7FF300000000;
	cout << "一级偏移后值(修饰)" << (LPVOID)OffsetFirstValue << endl;

	//二级偏移
	DWORDLONG OffsetSecondValue = 0;
	if (0 == ReadProcessMemory(hProcess, (LPVOID)(OffsetFirstValue + offset1), &OffsetSecondValue, sizeof(DWORD), &dwSize))
	{
		printf_s("二级偏移获取失败\n");
		getLastError = GetLastError();
	}
	cout << "二级偏移后值" << (LPVOID)OffsetSecondValue << endl;
	//修饰二级偏移
	OffsetSecondValue = (DWORDLONG)OffsetSecondValue + 0x7FF300000000;
	cout << "二级偏移后值(修饰)" << (LPVOID)OffsetSecondValue << endl;

	//三级偏移
	DWORDLONG RuneNum = 0;
	if (0 == ReadProcessMemory(hProcess, (LPVOID)(OffsetSecondValue + offset2), &RuneNum, sizeof(DWORD), &dwSize))
	{
		printf_s("三级偏移获取失败\n");
		getLastError = GetLastError();
	}
	cout << "三级偏移后值" << RuneNum << endl;

	DWORDLONG runeRealAddress = OffsetSecondValue + offset2;
	int modifyRune;
	cout << "当前卢恩值： " << RuneNum << endl;
	cout <<"输入你要修改后的值： ";
	scanf_s("%d", &modifyRune);
	WriteProcessMemory(hProcess, (LPVOID)runeRealAddress, &modifyRune, sizeof(DWORD), &dwSize);

	CloseHandle(hProcess);
	system("pause");
	return 0;
}