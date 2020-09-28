
// instdrvDlg.h : header file
//

#pragma once


// CinstdrvDlg dialog
class CinstdrvDlg : public CDialogEx
{
// Construction
public:
	CinstdrvDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INSTDRV_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
//	CButton m_butAdd;
//	CButton m_butClose;
//	CButton m_butOpen;
//	CButton m_butPath;
//	CButton m_butRemove;
//	CButton m_butStart;
//	CButton m_butStop;
	CEdit m_edtLog;
	CEdit m_edtPath;
	CEdit m_edtSer;
	CEdit m_edtSym;
	afx_msg void OnClickedIdbAdd();
	afx_msg void OnClickedIdbClose();
	afx_msg void OnClickedIdbOpen();
	afx_msg void OnClickedIdbPath();
	afx_msg void OnClickedIdbRemove();
	afx_msg void OnClickedIdbStart();
	afx_msg void OnClickedIdbStop();
};
