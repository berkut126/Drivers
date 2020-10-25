/*
* This is a personal academic project. Dear PVS-Studio, please check it.
* PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
*/
#include "pch.h"
#include "CScMgr.h"
#include "instdrvDlg.h"

void c_sc_mgr::print_last_error(const DWORD code)
{
	
	LPVOID lp_msg_buf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr,
		code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		reinterpret_cast<LPTSTR>(&lp_msg_buf),
		0,
		nullptr
	);
	std::wstring temp_buf(static_cast<LPWSTR>(lp_msg_buf));
	temp_buf += L"\t\r\n";
	// Free the buffer.

	auto* dlg = dynamic_cast<CinstdrvDlg*>(AfxGetApp()->m_pMainWnd);
	dlg->print(temp_buf);

	LocalFree(lp_msg_buf);
	
}

void c_sc_mgr::print(const std::wstring& message)
{
	auto* dlg = dynamic_cast<CinstdrvDlg*>(AfxGetApp()->m_pMainWnd);
	dlg->print(message);

}

c_sc_mgr::c_sc_mgr()
{

	sc_manager_ = OpenSCManager(nullptr, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CREATE_SERVICE);

	if (sc_manager_ == nullptr) {
		const auto code = GetLastError();
		print(L"\tFailed to open ServiceManager");
		print_last_error(code);
	}

}

c_sc_mgr::~c_sc_mgr()
{
	CloseServiceHandle(sc_manager_);
}

BOOL c_sc_mgr::install_driver(LPCWSTR path, LPCWSTR name)
{

	// Create a service to run the driver
	auto* const service = CreateService(
		sc_manager_,
		name,
		name,
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		path,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr
	);

	print(L"\tCreating service");

	if (service == nullptr) {
		const auto code = GetLastError();
		print(std::wstring(L"\tFailed to create service ") + name);
		print_last_error(code);
		return FALSE;
	}

	print(L"\tCreated service");
	
	CloseServiceHandle(service);

	print(L"\tClosed handle");

	return TRUE;

}

BOOL c_sc_mgr::delete_driver(LPCWSTR name)
{

	// Open service manager
	auto* const service = OpenService(sc_manager_, name, SERVICE_ALL_ACCESS);

	print(L"\tOpening service");
	
	if (service == nullptr)
	{
		const auto code = GetLastError();
		print(std::wstring(L"\tFailed to open service ") + name);
		print_last_error(code);
		return FALSE;
	}

	print(L"\tDeleting service");
	
	// Delete service
	if (DeleteService(service) == 0) {
		const auto code = GetLastError();
		print(std::wstring(L"\tFailed to delete service ") + name);
		print_last_error(code);
		return FALSE;
	}

	print(L"\tClosing handle");
	
	CloseServiceHandle(service);

	return TRUE;

}

BOOL c_sc_mgr::run_driver(LPCWSTR name)
{

	auto* const service = OpenService(sc_manager_, name, SERVICE_ALL_ACCESS);

	print(L"\tOpening service");

	if(service == nullptr)
	{
		const auto code = GetLastError();
		print(std::wstring(L"\tFailed to open service ") + name);
		print_last_error(code);
		return FALSE;
	}

	if (StartService(service, 1, &name) == 0) {
		const auto code = GetLastError();
		print(std::wstring(L"\tFailed to run service ") + name);
		print_last_error(code);


		// Delete the service
		if (DeleteService(service) == 0) {
			const auto code = GetLastError();
			print(std::wstring(L"\tFailed to delete service ") + name);
			print_last_error(code);
			return FALSE;

		}

		return FALSE;

	}

	CloseServiceHandle(service);

	return TRUE;

}

BOOL c_sc_mgr::stop_driver(LPCWSTR name)
{
	auto* const service = OpenService(sc_manager_, name, SERVICE_ALL_ACCESS);
	
	SERVICE_STATUS status;

	// Stop service
	if (ControlService(service, SERVICE_CONTROL_STOP, &status) == 0)
	{
		const auto code = GetLastError();
		print(std::wstring(L"\tFailed to stop service: ") + name);
		print_last_error(code);
		return FALSE;
	}

	CloseServiceHandle(service);

	return TRUE;

}

BOOL c_sc_mgr::open_device(const LPCWSTR symlink)
{

	// Open driver device
	auto* const driver_sym_link = CreateFile(
		symlink,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		OPEN_EXISTING,
		0,
		nullptr
	);

	if (driver_sym_link == INVALID_HANDLE_VALUE) {
		const auto code = GetLastError();
		print(L"\tFailed opening driver symlink");
		print_last_error(code);
		return FALSE;

	}

	CloseHandle(driver_sym_link);
	
	return TRUE;

}

BOOL c_sc_mgr::close_device(const LPCWSTR symlink)
{

	return DeleteFile(
		symlink
	);
	

}