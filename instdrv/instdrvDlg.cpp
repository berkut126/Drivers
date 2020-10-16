/*
* This is a personal academic project. Dear PVS-Studio, please check it.
* PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
*/

// instdrvDlg.cpp : implementation file
//

// ReSharper disable CppMemberFunctionMayBeConst
// ReSharper disable CppMemberFunctionMayBeStatic
#include "pch.h"
#include "framework.h"
#include "instdrv.h"
#include "instdrvDlg.h"
#include "afxdialogex.h"
#include "CScMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CinstdrvDlg dialog



CinstdrvDlg::CinstdrvDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INSTDRV_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CinstdrvDlg::DoDataExchange(CDataExchange* p_dx)
{
	CDialogEx::DoDataExchange(p_dx);
	//  DDX_Control(pDX, IDB_ADD, m_butAdd);
	//  DDX_Control(pDX, IDB_CLOSE, m_butClose);
	//  DDX_Control(pDX, IDB_OPEN, m_butOpen);
	//  DDX_Control(pDX, IDB_PATH, m_butPath);
	//  DDX_Control(pDX, IDB_REMOVE, m_butRemove);
	//  DDX_Control(pDX, IDB_START, m_butStart);
	//  DDX_Control(pDX, IDB_STOP, m_butStop);
	DDX_Control(p_dx, IDC_LOG, m_edt_log);
	DDX_Control(p_dx, IDC_PATH, m_edt_path);
	DDX_Control(p_dx, IDC_SER, m_edt_ser);
	DDX_Control(p_dx, IDC_SYM, m_edt_sym);
}

BEGIN_MESSAGE_MAP(CinstdrvDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDB_ADD, &CinstdrvDlg::on_clicked_idb_add)
	ON_BN_CLICKED(IDB_CLOSE, &CinstdrvDlg::on_clicked_idb_close)
	ON_BN_CLICKED(IDB_OPEN, &CinstdrvDlg::on_clicked_idb_open)
	ON_BN_CLICKED(IDB_PATH, &CinstdrvDlg::on_clicked_idb_path)
	ON_BN_CLICKED(IDB_REMOVE, &CinstdrvDlg::on_clicked_idb_remove)
	ON_BN_CLICKED(IDB_START, &CinstdrvDlg::on_clicked_idb_start)
	ON_BN_CLICKED(IDB_STOP, &CinstdrvDlg::on_clicked_idb_stop)
END_MESSAGE_MAP()


// CinstdrvDlg message handlers

BOOL CinstdrvDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_edt_path.SetWindowText(L"C:\\Users\\WDKRemoteUser\\Desktop\\simple3\\simple3.sys");
	m_edt_ser.SetWindowText(L"Service3");
	m_edt_sym.SetWindowText(L"\\\\.\\\\Simple3Link");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CinstdrvDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		const auto cx_icon = GetSystemMetrics(SM_CXICON);
		const auto cy_icon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		const auto x = (rect.Width() - cx_icon + 1) / 2;
		const auto y = (rect.Height() - cy_icon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CinstdrvDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CinstdrvDlg::on_clicked_idb_add()
{
	c_sc_mgr manager;
	CString path, name;
	m_edt_path.GetWindowText(path);
	m_edt_ser.GetWindowText(name);
	if(manager.install_driver(path, name))
	{
		print(L"Driver added");
	}
	else
	{
		print(L"Driver not added");
	}
}


void CinstdrvDlg::on_clicked_idb_close()
{
	c_sc_mgr manager;
	CString symlink;
	m_edt_sym.GetWindowText(symlink);
	if(manager.close_device(symlink))
	{
		print(L"Device closed");
	}
	else
	{
	print(L"Device not closed");
	}
}


void CinstdrvDlg::on_clicked_idb_open()
{
	c_sc_mgr manager;
	CString symlinkPath;
	m_edt_sym.GetWindowText(symlinkPath);
	if(manager.open_device(symlinkPath))
	{
		print(L"Device opened");
	}
	else
	{
	print(L"Device not opened");
	}
}


void CinstdrvDlg::on_clicked_idb_path()
{
	CFileDialog open_dialog(TRUE);
	if (open_dialog.DoModal() == IDOK) {
		m_edt_path.SetWindowText(open_dialog.GetPathName());
	}
}


void CinstdrvDlg::on_clicked_idb_remove()
{
	c_sc_mgr manager;
	CString name;
	m_edt_ser.GetWindowText(name);
	if(manager.delete_driver(name))
	{
		print(L"Driver removed");
	}
	else
	{
	print(L"Driver not removed");
	}
}


void CinstdrvDlg::on_clicked_idb_start()
{
	c_sc_mgr manager;
	CString name;
	m_edt_ser.GetWindowText(name);
	if(manager.run_driver(name))
	{
		print(L"Driver started");
	}
	else
	{
	print(L"Driver not started");
	}
}


void CinstdrvDlg::on_clicked_idb_stop()
{
	c_sc_mgr manager;
	CString name;
	m_edt_ser.GetWindowText(name);
	if(manager.stop_driver(name))
	{
		print(L"Driver stopped");
	}
	else
	{
		print(L"Driver not stopped");
	}
}

void CinstdrvDlg::print(const std::wstring& message) {


	CString cur_text;
	m_edt_log.GetWindowText(cur_text);
	cur_text = cur_text + (message + L"\r\n").c_str();
	m_edt_log.SetWindowText(cur_text);

}

void CinstdrvDlg::print_success(const std::wstring& error)
{

	m_edt_log.ReplaceSel((error + L"\r\n").c_str());

}

