#pragma once


// CFinderDragIcon

class CFinderDragIcon : public CStatic
{
	DECLARE_DYNAMIC(CFinderDragIcon)

public:
	CFinderDragIcon();
	virtual ~CFinderDragIcon();

	HWND	m_hwndFind;			//	見つけたウィンドウ(常にトップレベルウィンドウ)
	HWND	m_stcDragCaption;	//	ドラッグ中にヒットしたウィンドウのキャプション(常にトップレベルウィンドウを出す)
private:
	bool	m_isInsideTrack;	//	トラック中
	HCURSOR	m_trackCursor;		//	トラック中のカーソル
protected:
	DECLARE_MESSAGE_MAP()
public:
	bool	SearchPrevSelectWindow();
	bool	OnTrackHandle();
//	bool	TrackFindWindow( HWND hwndDlg );
private:
	bool TrackLButtonDown( HWND hwnd, WPARAM nFlags, CPoint point );
	bool TrackLButtonUp( HWND hwnd, WPARAM nFlags, CPoint point );
	bool TrackMouseMove( HWND hwnd, WPARAM nFlags, CPoint point );
	bool TrackRButtonDown( HWND hwnd, WPARAM nFlags, CPoint point );
};


