// edit_dialog.cpp : implementation file
//

#include "stdafx.h"
#include "Wordsearch.h"
#include "edit_dialog.h"
#include "afxdialogex.h"


// edit_dialog dialog

IMPLEMENT_DYNAMIC(edit_dialog, CDialog)

edit_dialog::edit_dialog(CWnd* pParent /*=NULL*/)
	: CDialog(edit_dialog::IDD, pParent)
{

}

edit_dialog::~edit_dialog()
{
}

void edit_dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(edit_dialog, CDialog)
END_MESSAGE_MAP()


// edit_dialog message handlers
