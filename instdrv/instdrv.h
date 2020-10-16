
// instdrv.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CinstdrvApp:
// See instdrv.cpp for the implementation of this class
//

class CinstdrvApp : public CWinApp
{
public:
	CinstdrvApp() = default;

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CinstdrvApp theApp;
