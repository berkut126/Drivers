
// instdrvDlg.h : header file
//

#pragma once


// CinstdrvDlg dialog
class CinstdrvDlg final : public CDialogEx
{
// Construction
public:
	CinstdrvDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INSTDRV_DIALOG };
#endif

	protected:
	void DoDataExchange(CDataExchange* p_dx) override;	// DDX/DDV support
	HICON m_hIcon;
	// Generated message map functions
	BOOL OnInitDialog() override;
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
	CEdit m_edt_log;
	CEdit m_edt_path;
	CEdit m_edt_ser;
	CEdit m_edt_sym;
	afx_msg void on_clicked_idb_add();
	afx_msg void on_clicked_idb_close();
	afx_msg void on_clicked_idb_open();
	afx_msg void on_clicked_idb_path();
	afx_msg void on_clicked_idb_remove();
	afx_msg void on_clicked_idb_start();
	afx_msg void on_clicked_idb_stop();
	void print_success(const std::wstring&);
	void print(const std::wstring&);
};
