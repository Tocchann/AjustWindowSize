// SelectWindowDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "AjustWindowSize.h"
#include "SelectWindowDlg.h"
#include "afxdialogex.h"


// CSelectWindowDlg ダイアログ

IMPLEMENT_DYNAMIC(CSelectWindowDlg, CDialog)

CSelectWindowDlg::CSelectWindowDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_SELECT_WINDOW_DIALOG, pParent)
{

}

CSelectWindowDlg::~CSelectWindowDlg()
{
}

void CSelectWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_LIST_TOPLEVEL_WINDOWS, m_lstTopLevelWindows );
}


BEGIN_MESSAGE_MAP(CSelectWindowDlg, CDialog)
END_MESSAGE_MAP()


// CSelectWindowDlg メッセージ ハンドラー
static BOOL CALLBACK enumProc( HWND hwnd, LPARAM lParam )
{
	CListCtrl& lstCtrl = *reinterpret_cast<CListCtrl*>(lParam);
	if( IsWindowVisible( hwnd ) )
	{
		int length = GetWindowTextLength( hwnd );
		if( length > 0 )
		{
			length++;
			CTempBuffer<TCHAR>	caption( length );
			GetWindowText( hwnd, caption, length );
			int itemCount = lstCtrl.GetItemCount();
			if( itemCount > 0 )
			{
				itemCount++;
			}
			int index = lstCtrl.InsertItem( itemCount, caption );
			if( index != -1 )
			{
				lstCtrl.SetItemData( index, reinterpret_cast<DWORD_PTR>(hwnd) );
			}
		}
	}
	return TRUE;
}

BOOL CSelectWindowDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_lstTopLevelWindows.DeleteAllItems();
	CRect rc;
	m_lstTopLevelWindows.GetClientRect( rc );
	int cyScroll = GetSystemMetrics( SM_CYHSCROLL )/* + GetSystemMetrics( SM_CXBORDER )*/;
	m_lstTopLevelWindows.InsertColumn( 0, _T( "ウィンドウキャプション" ), LVCFMT_LEFT, rc.Width() - cyScroll );
	m_lstTopLevelWindows.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	EnumWindows( enumProc, reinterpret_cast<LPARAM>(&m_lstTopLevelWindows) );
	CString prevSelCaption = theApp.GetProfileString( _T( "Settings" ), _T( "SelectWindowCaption" ) );
	if( !prevSelCaption.IsEmpty() )
	{
		LVFINDINFO	findInfo;
		findInfo.flags = LVFI_PARTIAL|LVFI_STRING;
		findInfo.psz = prevSelCaption;
		int lastSel = m_lstTopLevelWindows.FindItem( &findInfo );
		if( lastSel != -1 )
		{
			m_lstTopLevelWindows.SetItemState( lastSel, LVIS_SELECTED, LVIS_SELECTED );
		}
	}
	return TRUE;
}
void CSelectWindowDlg::OnOK()
{
	auto pos = m_lstTopLevelWindows.GetFirstSelectedItemPosition();
	int selItem = m_lstTopLevelWindows.GetNextSelectedItem( pos );
	if( selItem != -1 )
	{
		m_caption = m_lstTopLevelWindows.GetItemText( selItem, 0 );
		m_hwndFind = reinterpret_cast<HWND>( m_lstTopLevelWindows.GetItemData( selItem ) );
		theApp.WriteProfileString( _T( "SEttings" ), _T( "SelectWindowCaption" ), m_caption );
		CDialog::OnOK();
	}
}
