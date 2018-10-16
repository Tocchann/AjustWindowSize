
// AjustWindowSizeDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "AjustWindowSize.h"
#include "AjustWindowSizeDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAjustWindowSizeDlg ダイアログ



CAjustWindowSizeDlg::CAjustWindowSizeDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_AJUSTWINDOWSIZE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAjustWindowSizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_COMBO_CLIENT_WINDOW_SIZE, m_clientWindowSizes );
	DDX_Control( pDX, IDC_FINDER_ICON, m_finderDragIcon );
	DDX_Control( pDX, IDC_FIND_WINDOW_CAPTION, m_findWindowCaption );
}

BEGIN_MESSAGE_MAP(CAjustWindowSizeDlg, CDialog )
	ON_CBN_SELENDOK( IDC_COMBO_CLIENT_WINDOW_SIZE, &CAjustWindowSizeDlg::OnCbnSelendokComboClientWindowSize )
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()


// CAjustWindowSizeDlg メッセージ ハンドラー

BOOL CAjustWindowSizeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	m_finderDragIcon.m_stcDragCaption = m_findWindowCaption;
	int lastSel = theApp.GetProfileInt( _T( "Settings" ), _T( "ClientWIndowSizes" ), -1 );
	if( lastSel != -1 )
	{
		m_clientWindowSizes.SetCurSel( lastSel );
	}
	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}
void CAjustWindowSizeDlg::OnCbnSelendokComboClientWindowSize()
{
	int curSel = m_clientWindowSizes.GetCurSel();
	CString text;
	m_clientWindowSizes.GetLBText( curSel, text );
	if( text.Find( _T( ':' ) ) != -1 )
	{
		curSel++;
		m_clientWindowSizes.SetCurSel( curSel );
	}
	theApp.WriteProfileInt( _T( "Settings" ), _T( "ClientWIndowSizes" ), curSel );
}
void CAjustWindowSizeDlg::OnOK()
{
	//	各種設定の確認
	int curSel = m_clientWindowSizes.GetCurSel();
	CString windowSize;
	if( curSel != -1 )
	{
		m_clientWindowSizes.GetLBText( curSel, windowSize );
		if( windowSize.Find( _T( ':' ) ) != -1 )
		{
			windowSize.Empty();
		}
	}
	if( windowSize.IsEmpty() )
	{
		AfxMessageBox( _T( "調整後のクライアントサイズを選択してください。" ) );
		return;
	}
	int pos = windowSize.Find( 'x' );
	CString widthText = windowSize.Left( pos );
	CString heightText = windowSize.Mid( pos+1 );
	widthText.Trim();
	heightText.Trim();
	CSize clientSize( _tstol( widthText ), _tstol( heightText ) );
	CSize ncAreaSize( 0, 0 );
	if( m_finderDragIcon.m_hwndFind == nullptr )
	{
		AfxMessageBox( _T( "サイズ調整するウィンドウをファインダーで選択してください。" ) );
		return;
	}
	CRect rcWindow, rcClient;
	::GetWindowRect( m_finderDragIcon.m_hwndFind, &rcWindow );
	::GetClientRect( m_finderDragIcon.m_hwndFind, &rcClient );
	ncAreaSize = rcWindow.Size()-rcClient.Size();	//	ウィンドウの余白エリアのサイズを取り出す

	//	スクリーン上のどこに配置するかを確定する(ただし、常にメインスクリーン)
	CRect rcScreen;
	SystemParametersInfo( SPI_GETWORKAREA, 0, &rcScreen, 0 );
	CSize sizeWindow( ncAreaSize + clientSize );
	CSize sizeScreen( rcScreen.Size() );
	int xPos = rcScreen.left + (sizeScreen.cx-sizeWindow.cx)/2;
	int yPos = 0;
	//	最大化されてるかもしれないので、一度通常状態にする
	::ShowWindow( m_finderDragIcon.m_hwndFind, SW_SHOWNORMAL );
	::SetWindowPos( m_finderDragIcon.m_hwndFind, nullptr, xPos, 0, sizeWindow.cx, sizeWindow.cy, SWP_NOZORDER|SWP_FRAMECHANGED|SWP_DRAWFRAME );
}
void CAjustWindowSizeDlg::OnLButtonDown( UINT nFlags, CPoint point )
{
	//auto pwndCapture = SetCapture();
	//if( pwndCapture != this )
	//{
	//	AfxMessageBox( _T( "キャプチャー失敗!" ) );
	//	CDialog::OnLButtonDown( nFlags, point );
	//}
	if( m_finderDragIcon.OnTrackHandle() )
	{
		return;
	}
	CDialog::OnLButtonDown( nFlags, point );
}
void CAjustWindowSizeDlg::OnLButtonUp( UINT nFlags, CPoint point )
{
	if( m_finderDragIcon.OnTrackHandle() )
	{
		return;
	}
	CDialog::OnLButtonUp( nFlags, point );
}
void CAjustWindowSizeDlg::OnMouseMove( UINT nFlags, CPoint point )
{
	if( m_finderDragIcon.OnTrackHandle() )
	{
		return;
	}
	CDialog::OnMouseMove( nFlags, point );
}
void CAjustWindowSizeDlg::OnRButtonDown( UINT nFlags, CPoint point )
{
	if( m_finderDragIcon.OnTrackHandle() )
	{
		return;
	}
	CDialog::OnRButtonDown( nFlags, point );
}
void CAjustWindowSizeDlg::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if( m_finderDragIcon.OnTrackHandle() )
	{
		return;
	}
	CDialog::OnKeyDown( nChar, nRepCnt, nFlags );
}
void CAjustWindowSizeDlg::OnCaptureChanged( CWnd *pWnd )
{
	// TODO: ここにメッセージ ハンドラー コードを追加します。

	CDialog::OnCaptureChanged( pWnd );
}
