
// AjustWindowSize.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CAjustWindowSizeApp:
// このクラスの実装については、AjustWindowSize.cpp を参照してください
//

class CAjustWindowSizeApp : public CWinApp
{
public:
	CAjustWindowSizeApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CAjustWindowSizeApp theApp;
