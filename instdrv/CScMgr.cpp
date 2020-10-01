#include "pch.h"
#include "CScMgr.h"
#include "instdrvDlg.h"

void CScMgr::PrintLastError()
{

	auto dlg = static_cast<CinstdrvDlg*>(AfxGetApp()->m_pMainWnd);
	dlg->PrintLastError();

}

void CScMgr::PrintError(std::wstring error)
{

	auto dlg = static_cast<CinstdrvDlg*>(AfxGetApp()->m_pMainWnd);
	dlg->PrintError(error);

}

CScMgr::CScMgr()
{

	SCManager = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CREATE_SERVICE);

	if (SCManager == NULL) {
		PrintError(L"Failed to open ServiceManager");
		PrintLastError();
	}

}

CScMgr::~CScMgr()
{
	CloseServiceHandle(SCManager);
}

SC_HANDLE CScMgr::InstallDriver(LPCWSTR path, LPCWSTR name)
{

	// Create a service to run thge driver
	auto service = CreateService(
		SCManager,
		name,
		name,
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		path,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
	);

	if (service == NULL) {
		PrintError(std::wstring(L"Failed to create service ") + name);
		PrintLastError();
		return NULL;
	}

	return service;

}

BOOL CScMgr::DeleteDriver(LPCWSTR name)
{

	// Open service manager
	auto service = OpenService(SCManager, name, SERVICE_ALL_ACCESS);
	if (service == NULL)
	{
		PrintError(std::wstring(L"Failed to open service ") + name);
		PrintLastError();
		return FALSE;
	}
	
	// Delete service
	if (DeleteService(service) == 0) {
		PrintError(std::wstring(L"Failed to delete service ") + name);
		PrintLastError();
		return FALSE;
	}

	return TRUE;

}

BOOL CScMgr::RunDriver(SC_HANDLE service, LPCWSTR name)
{

	if (StartService(service, 1, &name) == 0) {

		PrintError(std::wstring(L"Failed to create service ") + name);
		PrintLastError();


		// Delete the service
		if (DeleteService(service) == 0) {

			PrintError(std::wstring(L"Failed to delete service ") + name);
			PrintLastError();
			return FALSE;

		}

		return FALSE;

	}

	return TRUE;

}

BOOL CScMgr::StopDriver(SC_HANDLE service, LPCWSTR name)
{

	SERVICE_STATUS status;

	// Stop service
	if (ControlService(service, SERVICE_CONTROL_STOP, &status) == 0)
	{
		PrintError(std::wstring(L"Failed to stop service: ") + name);
		PrintLastError();
		return FALSE;
	}

	return TRUE;

}

HANDLE CScMgr::OpenDevice(LPCWSTR symlink)
{

	// Open driver device
	auto driverSymLink = CreateFile(
		symlink,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	if (driverSymLink == INVALID_HANDLE_VALUE) {

		PrintError(L"Failed opening driver symlink");
		PrintLastError();
		return NULL;

	}
	
	return driverSymLink;

}

BOOL CScMgr::CloseDevice(HANDLE driveSymLink)
{

	CloseHandle(driveSymLink);
	return TRUE;

}