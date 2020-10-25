#pragma once
#include "pch.h"

#pragma once
class c_sc_mgr
{

private:
	SC_HANDLE sc_manager_;
	static void print_last_error(DWORD code);
	void print(const std::wstring& message);

public:
	c_sc_mgr();
	~c_sc_mgr();
	BOOL install_driver(LPCWSTR path, LPCWSTR name);
	BOOL delete_driver(LPCWSTR name);
	BOOL run_driver(LPCWSTR name);
	BOOL stop_driver(LPCWSTR name);
	BOOL open_device(LPCWSTR symlink);
	static BOOL close_device(LPCWSTR symlink);
	
};

