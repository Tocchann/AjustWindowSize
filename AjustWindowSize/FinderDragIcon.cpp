// CFinderDragIcon.cpp : 実装ファイル
//

#include "stdafx.h"
#include "AjustWindowSize.h"
#include "FinderDragIcon.h"


// CFinderDragIcon

IMPLEMENT_DYNAMIC(CFinderDragIcon, CStatic)

CFinderDragIcon::CFinderDragIcon()
	: m_hwndFind( nullptr )
	, m_stcDragCaption( nullptr )
	, m_isInsideTrack( false )
	, m_trackCursor( theApp.LoadCursor( IDC_FINDER ) )
{

}

CFinderDragIcon::~CFinderDragIcon()
{
}


BEGIN_MESSAGE_MAP(CFinderDragIcon, CStatic)
END_MESSAGE_MAP()

class SearchItem
{
public:
	SearchItem()
		: hwndFind( nullptr )
	{
	}
	SearchItem( const SearchItem& ) = delete;
	SearchItem& operator=( const SearchItem& ) = delete;
public:
	CString prevSelCaption;
	HWND	hwndFind;
};

static BOOL CALLBACK enumProc( HWND hwnd, LPARAM lParam )
{
	SearchItem& searchItem = *reinterpret_cast<SearchItem*>(lParam);

	if( IsWindowVisible( hwnd ) )
	{
		int length = GetWindowTextLength( hwnd );
		if( length > 0 )
		{
			length++;
			CTempBuffer<TCHAR>	caption( length );
			GetWindowText( hwnd, caption, length );
			if( searchItem.prevSelCaption.CompareNoCase( caption ) == 0 )
			{
				searchItem.hwndFind = hwnd;
				return FALSE;
			}
		}
	}
	return TRUE;

}
bool CFinderDragIcon::SearchPrevSelectWindow()
{
	SearchItem searchItem;
	searchItem.prevSelCaption = theApp.GetProfileString( _T( "Settings" ), _T( "SelectWindowCaption" ) );
	if( searchItem.prevSelCaption.IsEmpty() )
	{
		return false;
	}
	EnumWindows( enumProc, reinterpret_cast<LPARAM>(&searchItem) );
	if( searchItem.hwndFind != nullptr )
	{
		m_hwndFind = searchItem.hwndFind;
		::SetWindowText( m_stcDragCaption, searchItem.prevSelCaption );	//	一致するものを選択するので同じ文字列
		return true;
	}
	return false;
}

// CFinderDragIcon メッセージ ハンドラー
static HWND APIENTRY GetTopLevelWindow( HWND hwnd )
{
	HWND hwndParent = hwnd;
	while( hwnd != nullptr )
	{
		hwndParent = hwnd;
		hwnd = GetParent( hwnd );
	}
	if( hwndParent == nullptr )
	{
		hwndParent = hwnd;
	}
	return hwndParent;
}
#if __USE_OLE_DRAGDROP__
class CFindDropSource : public COleDropSource
{
public:
	static UINT GetClipbordFormat()
	{
		static UINT s_cf = 0;
		if( s_cf == 0 )
		{
			s_cf = RegisterClipboardFormat( _T( "Morrin::AjustWndowSize" ) );
		}
		return s_cf;
	}
	CFindDropSource() = delete;
	CFindDropSource( const CFindDropSource& ) = delete;
	CFindDropSource& operator=( const CFindDropSource& ) = delete;

	CFindDropSource( HWND hwndCaption )
		: m_cursor( theApp.LoadCursor( IDC_FINDER ))
		, m_caption( hwndCaption )
		, m_hwndFind( nullptr )
	{
	}
	virtual SCODE QueryContinueDrag( BOOL bEscapePressed, DWORD dwKeyState );
	virtual SCODE GiveFeedback( DROPEFFECT dropEffect );
	virtual BOOL OnBeginDrag( CWnd* pWnd );
private:
	HCURSOR m_cursor;
	HWND m_caption;
	HWND m_hwndFind;
};

SCODE CFindDropSource::QueryContinueDrag( BOOL bEscapePressed, DWORD dwKeyState )
{
	SetCursor( m_cursor );
	return COleDropSource::QueryContinueDrag( bEscapePressed, dwKeyState );
}
SCODE CFindDropSource::GiveFeedback( DROPEFFECT dropEffect )
{
	//	カーソルを無条件にこれに切り替える
	if( m_caption != nullptr )
	{
		CPoint pt;
		GetCursorPos( &pt );
		HWND hwnd = WindowFromPoint( pt );
		hwnd = GetTopLevelWindow( hwnd );
		int textLen = GetWindowTextLength( hwnd );
		if( textLen > 0 )
		{
			textLen++;
			CTempBuffer<TCHAR> caption( textLen );
			GetWindowText( hwnd, caption, textLen );
			SetWindowText( m_caption, caption );
		}
	}
	return COleDropSource::GiveFeedback( dropEffect );
}
BOOL CFindDropSource::OnBeginDrag( CWnd* pWnd )
{
	return TRUE;
}
bool CFinderDragIcon::TrackFindWindow( CPoint point )
{
	COleDataSource	dataSrc;
	HGLOBAL hGlobal = GlobalAlloc( GHND, sizeof( HWND ) );
	HWND* phwnd = static_cast<HWND*>(GlobalLock( hGlobal ));
	*phwnd = *this;
	GlobalUnlock( hGlobal );
	dataSrc.CacheGlobalData( CFindDropSource::GetClipbordFormat(), hGlobal );
	CFindDropSource	dropSrc( m_stcDragCaption );
	dataSrc.DoDragDrop( DROPEFFECT_COPY, nullptr, &dropSrc );
	return true;
}
#else
bool CFinderDragIcon::OnTrackHandle()
{
	const MSG* pMsg = AfxGetCurrentMessage();
	CPoint point;
	if( WM_MOUSEFIRST <= pMsg->message && pMsg->message <= WM_MOUSELAST )
	{
		point.x = GET_X_LPARAM( pMsg->lParam );
		point.y = GET_Y_LPARAM( pMsg->lParam );
		//	ウィンドウハンドルが異なるので、設定を修正する
		::ClientToScreen( pMsg->hwnd, &point );	//	都合がいいのでスクリーン座標で処理する
	}
	switch( pMsg->message )
	{
	case WM_LBUTTONDOWN:	return TrackLButtonDown( pMsg->hwnd, pMsg->wParam, point );
	case WM_MOUSEMOVE:		return TrackMouseMove( pMsg->hwnd, pMsg->wParam, point );
	case WM_LBUTTONUP:		return TrackLButtonUp( pMsg->hwnd, pMsg->wParam, point );
	case WM_RBUTTONDOWN:	return TrackRButtonDown( pMsg->hwnd, pMsg->wParam, point );
	case WM_CAPTURECHANGED:
		m_isInsideTrack = false;
		break;
	case WM_KEYDOWN:
		if( m_isInsideTrack )
		{
			if( pMsg->wParam == VK_ESCAPE )
			{
				ReleaseCapture();
			}
		}
		break;
	}
	return false;
}
#include "SelectWindowDlg.h"

bool CFinderDragIcon::TrackLButtonDown( HWND hwnd, WPARAM nFlags, CPoint point )
{
	//TRACE( _T( "TrackLButtonDown( %d, %d, (%d,%d))\n" ), m_isInsideTrack, nFlags, point.x, point.y );
	_ASSERTE( hwnd != nullptr && IsWindow( hwnd ) );
	_ASSERTE( m_isInsideTrack == false );	//	キャプチャー中に再度来ることはない。。。はず！
	//	ドラック開始位置範囲内にいない場合は無視
	CRect rc;
	GetClientRect( &rc );
	ClientToScreen( &rc );
	if( !rc.PtInRect( point ) )
	{
		return false;
	}
#if 1
	CSelectWindowDlg	dlg( CWnd::FromHandle( hwnd ) );
	if( dlg.DoModal() == IDOK )
	{
		m_hwndFind = dlg.m_hwndFind;
		::SetWindowText( m_stcDragCaption, dlg.m_caption );
	}
	return true;
#else
	//	現在誰かがキャプチャーしている場合は無視
	auto pwnd = GetCapture();
	if( pwnd != nullptr )
	{
		TRACE( _T( "他でキャプチャーされています。\n" ) );
		return false;
	}
	auto hwndCapture = ::SetCapture( hwnd );
	if( hwndCapture == nullptr )
	{
#ifdef _DEBUG
		auto lastError = GetLastError();
		CString msg;
		msg.Format( _T( "キャプチャー失敗:%d" ), lastError );
		AfxMessageBox( msg );
#endif
		return false;	//	キャプチャーできない場合もスキップ(なぜ？)
	}

	m_isInsideTrack = true;
	::SetCursor( m_trackCursor );
	SetIcon( theApp.LoadIcon( IDI_FRAME ) );
	UpdateWindow();

	return m_isInsideTrack;
#endif
}
bool CFinderDragIcon::TrackLButtonUp( HWND hwnd, WPARAM nFlags, CPoint point )
{
	//TRACE( _T( "TrackLButtonUp( %d, %d, (%d,%d))\n" ), m_isInsideTrack, nFlags, point.x, point.y );
	if( m_isInsideTrack )
	{
		m_hwndFind= ::WindowFromPoint( point );
		m_hwndFind = GetTopLevelWindow( m_hwndFind );
		if( m_hwndFind == hwnd )
		{
			m_hwndFind = nullptr;
			::SetWindowText( m_stcDragCaption, _T( "" ) );
		}
		m_isInsideTrack = false;
		ReleaseCapture();
		return true;
	}
	return false;
}
bool CFinderDragIcon::TrackMouseMove( HWND hwnd, WPARAM nFlags, CPoint point )
{
	//TRACE( _T( "TrackMouseMove( %d, %d, (%d,%d))\n" ), m_isInsideTrack, nFlags, point.x, point.y );
	if( m_isInsideTrack && m_stcDragCaption != nullptr )
	{
		//	トラック中は自分も表示対象とする
		auto hwndFind = ::WindowFromPoint( point );
		hwndFind = GetTopLevelWindow( hwndFind );
		if( hwndFind != nullptr && hwnd != hwndFind )
		{
			auto length = ::GetWindowTextLength( hwndFind );
			length++;
			CTempBuffer<TCHAR>	captionText( length );
			::GetWindowText( hwndFind, captionText, length );
			::SetWindowText( m_stcDragCaption, captionText );
		}
		else
		{
			::SetWindowText( m_stcDragCaption, _T( "" ) );
		}
	}
	return m_isInsideTrack;
}
bool CFinderDragIcon::TrackRButtonDown( HWND hwnd, WPARAM nFlags, CPoint point )
{
	//	キャンセル処理
	if( m_isInsideTrack )
	{
		m_hwndFind = nullptr;
		::SetWindowText( m_stcDragCaption, _T( "" ) );
		m_isInsideTrack = false;
		ReleaseCapture();
		return true;
	}
	return false;
}
#if 0
bool CFinderDragIcon::TrackFindWindow( HWND hwndDlg )
{
	m_hwndFind = nullptr;
	//	誰かがキャプチャーしてる状態で呼び出されたら、何もしない(ループをこっちで回すのでメッセージは来ない)
	auto hwndCapture = ::GetCapture();
	if( hwndCapture != nullptr )
	{
		return false;
	}
	hwndCapture = ::SetCapture( hwndDlg );
	//	カーソルを変更する
	auto cursor = theApp.LoadCursor( IDC_FINDER );
	auto prevCursor = ::SetCursor( cursor );
	SetIcon( theApp.LoadIcon( IDI_FRAME ) );
	UpdateWindow();
	bool loop = true;
	while( loop )
	{
		CPoint point;
		HWND hwnd;
		MSG msg;
		VERIFY( ::GetMessage( &msg, nullptr, 0, 0 ) );
		//	何らかの理由でキャプチャーが外れた場合はそこで終わり
		hwnd = ::GetCapture();
		if( hwnd != hwndCapture )
		{
			break;
		}
		switch( msg.message )
		{
		case WM_MOUSEMOVE:
			if( m_stcDragCaption != nullptr )
			{
				point.x = GET_X_LPARAM( msg.lParam );
				point.y = GET_Y_LPARAM( msg.lParam );
				ClientToScreen( &point );
				hwnd = ::WindowFromPoint( point );
				hwnd = GetTopLevelWindow( hwnd );
				if( hwnd != nullptr )
				{
					int textLen = ::GetWindowTextLength( hwnd );
					if( textLen > 0 )
					{
						textLen++;
						CTempBuffer<TCHAR> caption( textLen );
						::GetWindowText( hwnd, caption, textLen );
						::SetWindowText( m_stcDragCaption, caption );
					}
				}
			}
			break;
		case WM_LBUTTONUP:
			point.x = GET_X_LPARAM( msg.lParam );
			point.y = GET_Y_LPARAM( msg.lParam );
			ClientToScreen( &point );
			hwnd = ::WindowFromPoint( point );
			hwnd = GetTopLevelWindow( hwnd );
			m_hwndFind = hwnd;
			loop = false;
			break;
		case WM_KEYDOWN:
			if( msg.wParam == VK_ESCAPE )
			{
				loop = false;
			}
			break;
		case WM_RBUTTONDOWN:
			loop = false;
			break;
		default:
			DispatchMessage( &msg );
			break;
		}
	}
	ReleaseCapture();
	::SetCursor( prevCursor );
	SetIcon( theApp.LoadIcon( IDI_FINDER_FRAME ) );
	UpdateWindow();
	return true;
}
#endif
#endif
