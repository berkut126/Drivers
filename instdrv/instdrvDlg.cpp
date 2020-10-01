
// instdrvDlg.cpp : implementation file
//

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

void CinstdrvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDB_ADD, m_butAdd);
	//  DDX_Control(pDX, IDB_CLOSE, m_butClose);
	//  DDX_Control(pDX, IDB_OPEN, m_butOpen);
	//  DDX_Control(pDX, IDB_PATH, m_butPath);
	//  DDX_Control(pDX, IDB_REMOVE, m_butRemove);
	//  DDX_Control(pDX, IDB_START, m_butStart);
	//  DDX_Control(pDX, IDB_STOP, m_butStop);
	DDX_Control(pDX, IDC_LOG, m_edtLog);
	DDX_Control(pDX, IDC_PATH, m_edtPath);
	DDX_Control(pDX, IDC_SER, m_edtSer);
	DDX_Control(pDX, IDC_SYM, m_edtSym);
}

BEGIN_MESSAGE_MAP(CinstdrvDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDB_ADD, &CinstdrvDlg::OnClickedIdbAdd)
	ON_BN_CLICKED(IDB_CLOSE, &CinstdrvDlg::OnClickedIdbClose)
	ON_BN_CLICKED(IDB_OPEN, &CinstdrvDlg::OnClickedIdbOpen)
	ON_BN_CLICKED(IDB_PATH, &CinstdrvDlg::OnClickedIdbPath)
	ON_BN_CLICKED(IDB_REMOVE, &CinstdrvDlg::OnClickedIdbRemove)
	ON_BN_CLICKED(IDB_START, &CinstdrvDlg::OnClickedIdbStart)
	ON_BN_CLICKED(IDB_STOP, &CinstdrvDlg::OnClickedIdbStop)
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
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

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



void CinstdrvDlg::OnClickedIdbAdd()
{
	auto manager = CScMgr();
	CString path, name;
	m_edtPath.GetWindowText(path);
	m_edtSer.GetWindowText(name);
	service = manager.InstallDriver(path, name);
}


void CinstdrvDlg::OnClickedIdbClose()
{
	if (symlink == NULL)
		return;
	auto manager = CScMgr();
	manager.CloseDevice(symlink);
}


void CinstdrvDlg::OnClickedIdbOpen()
{
	if (symlink == NULL)
		return;
	auto manager = CScMgr();
	CString symlinkPath;
	symlink = manager.OpenDevice(symlinkPath);
}


void CinstdrvDlg::OnClickedIdbPath()
{
	CFileDialog opendialog(true);
	if (opendialog.DoModal() == IDOK) {
		m_edtPath.SetWindowText(opendialog.GetPathName());
	}
}


void CinstdrvDlg::OnClickedIdbRemove()
{
	if (service == NULL)
		// Do nothing
		return;
	auto manager = CScMgr();
	CString name;
	m_edtSer.GetWindowText(name);
	manager.DeleteDriver(name);
}


void CinstdrvDlg::OnClickedIdbStart()
{
	if (service == NULL)
		return;
	auto manager = CScMgr();
	CString name;
	m_edtSer.GetWindowText(name);
	manager.RunDriver(service, name);
}


void CinstdrvDlg::OnClickedIdbStop()
{
	if (service == NULL)
		return;
	auto manager = CScMgr();
	CString name;
	m_edtSer.GetWindowText(name);
	manager.StopDriver(service, name);
}

void CinstdrvDlg::PrintError(std::wstring error) {


	m_edtLog.ReplaceSel((error + L"\r\n").c_str());

}

void CinstdrvDlg::PrintLastError()
{

	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);
	std::wstring tmpbuf((LPWSTR)lpMsgBuf);
	tmpbuf += L"\r\n";
	m_edtLog.ReplaceSel(tmpbuf.c_str());
	// Free the buffer.
	LocalFree(lpMsgBuf);

}

