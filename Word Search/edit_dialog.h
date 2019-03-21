#pragma once


// edit_dialog dialog

class edit_dialog : public CDialog
{
	DECLARE_DYNAMIC(edit_dialog)

public:
	edit_dialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~edit_dialog();

// Dialog Data
	enum { IDD = IDD_EDIT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
