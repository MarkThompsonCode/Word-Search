#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

class CWordsearchApp : public CWinAppEx
{
public:
	CWordsearchApp();
	virtual BOOL InitInstance();
	
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();	

	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	
private:
	DECLARE_MESSAGE_MAP()
};

extern CWordsearchApp theApp;
