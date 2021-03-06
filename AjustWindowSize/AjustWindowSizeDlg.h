
// AjustWindowSizeDlg.h : ヘッダー ファイル
//

#pragma once
#include "FinderDragIcon.h"

// CAjustWindowSizeDlg ダイアログ
class CAjustWindowSizeDlg : public CDialog
{
// コンストラクション
public:
	CAjustWindowSizeDlg(CWnd* pParent = nullptr);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AJUSTWINDOWSIZE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;
	int m_prevSel;
	bool m_bDropdown;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_clientWindowSizes;
	CFinderDragIcon m_finderDragIcon;
	CStatic m_findWindowCaption;
	afx_msg void OnCbnSelendokComboClientWindowSize();
	afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnMouseMove( UINT nFlags, CPoint point );
	afx_msg void OnRButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	afx_msg void OnCaptureChanged( CWnd *pWnd );
	afx_msg void OnCbnDropdownComboClientWindowSize();
	afx_msg void OnCbnCloseupComboClientWindowSize();
	afx_msg void OnCbnSelendcancelComboClientWindowSize();
};
