#pragma once

#include <vector>
//#include "CWord.h"
//#include "WordsearchDoc.h"
//#include "stdafx.h"
#include "resource.h"

using std::vector;

class CSetupDialog : public CDialog
{
public:
	DECLARE_DYNAMIC(CSetupDialog)

	CSetupDialog(CWnd* pParent = NULL);
	virtual ~CSetupDialog();

	enum { IDD = IDD_DIALOG_SETUP };

	void					set_title(const CString title)	{ m_title = title; }
	const CString			get_title() const				{ return m_title; }

	//const void			set_word_list(const vector< CString > word_list)	{ m_word_list.clear(); m_word_list = word_list;	}
	void					add_word(const CString word)			{ m_word_list.push_back( word ); }
	const vector< CString >	get_word_list() const					{ return m_word_list; }

	void					set_grid_size(const SIZE grid_size )	{ m_grid_size = grid_size; }
	const SIZE				get_grid_size() const					{ return m_grid_size; }

	void set_colour_background(const COLORREF colour)	{ m_colour_background = colour; }
	void set_colour_grid(const COLORREF colour)			{ m_colour_grid = colour; }
	void set_colour_selected(const COLORREF colour)		{ m_colour_selected = colour; }
	void set_colour_found(const COLORREF colour)		{ m_colour_found = colour; }

	const COLORREF get_colour_background() const		{ return m_colour_background; }
	const COLORREF get_colour_grid() const				{ return m_colour_grid; }
	const COLORREF get_colour_selected() const			{ return m_colour_selected; }
	const COLORREF get_colour_found() const				{ return m_colour_found; }

	void		set_uppercase(const BOOL uppercase)		{ m_uppercase = uppercase; }
	const BOOL	get_uppercase() const					{ return m_uppercase; }

	void		set_reversed(const BOOL reversed)		{ m_reversed = reversed; }
	const BOOL	get_reversed() const					{ return m_reversed; }

	void		set_diaganols(const BOOL diagonals)		{ m_diagonals = diagonals; }
	const BOOL	get_diaganols() const					{ return m_diagonals; }

	void		set_overlapping(const BOOL overlapping)	{ m_overlaped = overlapping; }
	const BOOL	get_overlapping() const					{ return m_overlaped; }	

	void		set_show_gridlines(const BOOL show)		{ m_show_grid_lines = show; }
	const BOOL	get_show_gridlines() const				{ return m_show_grid_lines; }

	void		set_show_found_list(const BOOL show)	{ m_show_found_list = show; }
	const BOOL	get_show_found_list() const				{ return m_show_found_list; }	

	const BOOL	reset_required() const					{ return m_reset; }

protected:
	DECLARE_MESSAGE_MAP()
	virtual void	DoDataExchange(CDataExchange* pDX);

private:	
	virtual BOOL	OnInitDialog();
	void			setup_edit_word_list();
	void			get_edit_word_list();
	virtual void	OnOK();

	afx_msg void	on_edit_change();
	afx_msg void	on_check_uppercase();
	afx_msg void	on_grid_size_change();

	afx_msg void	on_check_diagonals();
	afx_msg void	on_check_reverse();
	afx_msg void	on_check_overlap();

	SIZE			get_current_grid_size();

	CString				m_title;
	vector< CString >	m_word_list;

	CString				m_combined_words;
	int					m_max_word_length;
	int					m_character_total;

	SIZE				m_grid_size;

	int					m_grid_pen_width; // ** to implement
	
	BOOL				m_uppercase;
	BOOL				m_diagonals;
	BOOL				m_reversed;
	BOOL				m_overlaped;
	BOOL				m_show_grid_lines;
	BOOL				m_show_found_list;
	BOOL				m_show_found_grid;

	COLORREF			m_colour_background;
	COLORREF			m_colour_grid;
	COLORREF			m_colour_selected;
	COLORREF			m_colour_found;

	CButton				m_control_uppercase;
	CEdit				m_control_word_list;
	CEdit				m_control_grid_width;
	CEdit				m_control_grid_height;

	CMFCColorButton		m_control_colour_background;
	CMFCColorButton		m_control_colour_grid;
	CMFCColorButton		m_control_colour_selected;
	CMFCColorButton		m_control_colour_found;

	bool				m_reset;
};