#include "pch.h"

#pragma once
class CScMgr
{

private:
	SC_HANDLE SCManager;
	void PrintLastError();
	void PrintError(std::wstring);

public:
	CScMgr();
	~CScMgr();
	SC_HANDLE InstallDriver(LPCWSTR path, LPCWSTR name);
	BOOL DeleteDriver(LPCWSTR name);
	BOOL RunDriver(SC_HANDLE service, LPCWSTR name);
	BOOL StopDriver(SC_HANDLE service, LPCWSTR name);
	HANDLE OpenDevice(LPCWSTR symlink);
	BOOL CloseDevice(HANDLE driveSymLink);
	
};

