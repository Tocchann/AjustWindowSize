#pragma once


// CSelectWindowDlg ダイアログ

class CSelectWindowDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectWindowDlg)

public:
	CSelectWindowDlg(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~CSelectWindowDlg();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SELECT_WINDOW_DIALOG };
#endif
	CString	m_caption;
	HWND	m_hwndFind;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_lstTopLevelWindows;
	virtual void OnOK();
};
