#include "stdafx.h"

//#include "Wordsearch.h"
#include "Dialog Setup.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CSetupDialog, CDialog)

BEGIN_MESSAGE_MAP(CSetupDialog, CDialog)

	ON_BN_CLICKED(IDC_CHECK_UPPERCASE,	&CSetupDialog::on_check_uppercase)
	ON_EN_CHANGE(IDC_EDIT_WORD_LIST,	&CSetupDialog::on_edit_change)
	//ON_EN_UPDATE(IDC_EDIT_GRID_WIDTH,	&CSetupDialog::on_grid_size_change)
	//ON_EN_UPDATE(IDC_EDIT_GRID_HEIGHT,&CSetupDialog::on_grid_size_change)
	ON_BN_CLICKED(IDC_CHECK_DIAGONALS,	&CSetupDialog::on_check_diagonals)
	ON_BN_CLICKED(IDC_CHECK_REVERSE,	&CSetupDialog::on_check_reverse)
	ON_BN_CLICKED(IDC_CHECK_OVERLAP,	&CSetupDialog::on_check_overlap)

END_MESSAGE_MAP()

void CSetupDialog::on_check_diagonals()	{ m_reset = true; }
void CSetupDialog::on_check_reverse()	{ m_reset = true; }
void CSetupDialog::on_check_overlap()	{ m_reset = true; }

CSetupDialog::CSetupDialog(CWnd* pParent) : CDialog(CSetupDialog::IDD, pParent)
{	
	//m_title				= _T( "- title -" );
	m_combined_words	= _T( "" );
	m_max_word_length	= 0;
	m_character_total	= 0;
	m_reset				= false;
	m_word_list.clear();

	//m_grid_size.cx	= 10;
	//m_grid_size.cy	= 10;
	
	//m_grid_pen_width	= 2; // ** too implement edit

	//m_uppercase		= true;
	//m_diagonals		= true;
	//m_reversed		= true;
	//m_overlaped		= true;
	//m_show_grid_lines	= true;
	//m_show_found_list	= true;
	//m_show_found_grid	= true;

	//m_colour_background	= RGB( 220, 220, 220 );
	//m_colour_grid			= RGB( 0,   0,   0 );
	//m_colour_selected		= RGB( 0,   255, 255 );
	//m_colour_found		= RGB( 255, 0,   0 );
}

CSetupDialog::~CSetupDialog()
{
	m_word_list.clear();
}

void CSetupDialog::DoDataExchange( CDataExchange * p_dx )
{
	CDialog::DoDataExchange(p_dx);

	DDX_Text(p_dx, IDC_EDIT_TITLE,				m_title);
	DDX_Text(p_dx, IDC_EDIT_WORD_LIST,			m_combined_words);

	DDX_Text(p_dx, IDC_EDIT_GRID_WIDTH,			m_grid_size.cx);
	DDX_Text(p_dx, IDC_EDIT_GRID_HEIGHT,		m_grid_size.cy);	

	DDX_Control(p_dx, IDC_EDIT_GRID_WIDTH,		m_control_grid_width);
	DDX_Control(p_dx, IDC_EDIT_GRID_HEIGHT,		m_control_grid_height);
	
	DDV_MinMaxInt(p_dx, m_grid_size.cx, 1, 1000);
	DDV_MinMaxInt(p_dx, m_grid_size.cy, 1, 1000);

	DDX_Control(p_dx, IDC_CHECK_UPPERCASE,		m_control_uppercase);
	DDX_Control(p_dx, IDC_EDIT_WORD_LIST,		m_control_word_list);
	
	DDX_Check(p_dx, IDC_CHECK_DIAGONALS,		m_diagonals);
	DDX_Check(p_dx, IDC_CHECK_REVERSE,			m_reversed);
	DDX_Check(p_dx, IDC_CHECK_UPPERCASE,		m_uppercase);
	DDX_Check(p_dx, IDC_CHECK_OVERLAP,			m_overlaped);
	DDX_Check(p_dx, IDC_CHECK_GRIDLINES,		m_show_grid_lines);
	DDX_Check(p_dx, IDC_CHECK_SHOW_FOUND_LIST,	m_show_found_list);
	DDX_Check(p_dx, IDC_CHECK_SHOW_FOUND_GRID,	m_show_found_grid);
	
	DDX_Control(p_dx, IDC_COLOUR_BACKGROUND,	m_control_colour_background);
	DDX_Control(p_dx, IDC_COLOUR_GRID,			m_control_colour_grid);
	DDX_Control(p_dx, IDC_COLOUR_SELECTED,		m_control_colour_selected);
	DDX_Control(p_dx, IDC_COLOUR_FOUND,			m_control_colour_found);
}

BOOL CSetupDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	setup_edit_word_list();

	m_control_colour_background.SetColor( m_colour_background );
	m_control_colour_grid.SetColor		( m_colour_grid );
	m_control_colour_selected.SetColor	( m_colour_selected );
	m_control_colour_found.SetColor		( m_colour_found );
	//EnableAutomaticButton(LPCTSTR lpszLabel, COLORREF colorAutomatic, BOOL bEnable=TRUE );

	CString edit_text;
	m_control_word_list.GetWindowText( edit_text );

	if( m_uppercase )
	{
		edit_text.MakeUpper();
		m_control_word_list.ModifyStyle( ES_LOWERCASE, ES_UPPERCASE );
		m_control_word_list.SetWindowText( edit_text );
	}
	else
	{
		edit_text.MakeLower();
		m_control_word_list.ModifyStyle( ES_UPPERCASE, ES_LOWERCASE );
		m_control_word_list.SetWindowText( edit_text );
	}

	on_edit_change();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CSetupDialog::setup_edit_word_list() 
{ 
	m_combined_words = _T("");

	vector<CString>::const_iterator word_it = m_word_list.begin();

	while( word_it != m_word_list.end() )
	{
		m_combined_words += (*word_it);
		m_combined_words += "\r\n";

		++word_it;
	}

	m_combined_words = m_combined_words.Left( m_combined_words.GetLength() - 2 );
	m_control_word_list.SetWindowText( m_combined_words );
}

void CSetupDialog::on_check_uppercase()
{
	CString edit_text;
	m_control_word_list.GetWindowText(edit_text);

	if( m_control_uppercase.GetCheck() == BST_CHECKED )
	{
		edit_text.MakeUpper();
		m_control_word_list.ModifyStyle( ES_LOWERCASE, ES_UPPERCASE );
		m_control_word_list.SetWindowText( edit_text );
	}
	else
	{
		edit_text.MakeLower();
		m_control_word_list.ModifyStyle( ES_UPPERCASE, ES_LOWERCASE );
		m_control_word_list.SetWindowText( edit_text );
	}
}

void CSetupDialog::on_edit_change()
{
	m_reset = true;

	int total_lines		= m_control_word_list.GetLineCount();
	int word_length		= 0;
	int current_line	= 0;
	m_character_total	= 0;

	// if(character == " ") {}
	// disable spaces

	for( int line_index = 0; line_index < total_lines ; line_index++ )
	{
		current_line = m_control_word_list.LineIndex( line_index );	// ? current line	
		word_length  = m_control_word_list.LineLength( current_line );  
		
		if( word_length > m_max_word_length ) m_max_word_length = word_length;

		m_character_total += word_length;
	}	

	SIZE current_grid_size = get_current_grid_size();

	if( current_grid_size.cx < m_max_word_length || current_grid_size.cy < m_max_word_length )
	{
		m_grid_size.cx = m_max_word_length;
		m_grid_size.cy = m_max_word_length;

		TCHAR text_grid_width[4];
		TCHAR text_grid_height[4];

		_itot_s(m_grid_size.cx, text_grid_width,  4, 10);
		_itot_s(m_grid_size.cx, text_grid_height, 4, 10);

		m_control_grid_width.SetWindowText( text_grid_width );
		m_control_grid_height.SetWindowText( text_grid_height );
	}			
	
	if( ( m_grid_size.cx * m_grid_size.cy ) < m_character_total )
	{
		while( ( m_grid_size.cx * m_grid_size.cy ) < m_character_total ) m_grid_size.cx++;

		TCHAR text_grid_width[4];
		_itot_s(m_grid_size.cx, text_grid_width,  4, 10);

		m_control_grid_width.SetWindowText( text_grid_width );
	}

	/*SIZE current_grid_size = get_current_grid_size();

	TRACE("text_grid_width = %S \n", current_grid_size.cy);
	TRACE("text_grid_height = %S\ n", current_grid_size.cx);*/

	//TRACE("m_character_total = %i \n", m_character_total);
}

SIZE CSetupDialog::get_current_grid_size()
{
	SIZE grid_size;

	CString	text_grid_width;
	CString	text_grid_height;

	m_control_grid_width.GetWindowText ( text_grid_width );
	m_control_grid_height.GetWindowText( text_grid_height );

	grid_size.cx = _tcstol( text_grid_width, NULL, 10);
	grid_size.cy = _tcstol( text_grid_height, NULL, 10);

	return grid_size;
}

void CSetupDialog::on_grid_size_change()
{	
	m_reset = true;

	SIZE current_grid_size = get_current_grid_size();

	if(current_grid_size.cx < m_max_word_length || current_grid_size.cy < m_max_word_length)
	{
		current_grid_size.cx = m_max_word_length;
		current_grid_size.cy = m_max_word_length;

		TCHAR text_grid_width[4];

		_itot_s(m_max_word_length, text_grid_width,  4, 10);

		m_control_grid_width.SetWindowText( text_grid_width );
		//m_control_grid_height.SetWindowText( text_grid_height );		
	}
	
	//TRACE("grid width = %i \n", grid_width);
}

void CSetupDialog::OnOK()
{
	CDialog::OnOK();

	get_edit_word_list();

	m_colour_background	= m_control_colour_background.GetColor();	
	m_colour_grid		= m_control_colour_grid.GetColor();	
	m_colour_selected	= m_control_colour_selected.GetColor();	
	m_colour_found		= m_control_colour_found.GetColor();
}

void CSetupDialog::get_edit_word_list()
{
	m_word_list.clear();

	CString	line_word;
	int		total_lines = m_control_word_list.GetLineCount();    
		
	// if( word  == ""\r\n" deleteline)

	//if ((nBegin = m_myEdit.LineIndex(1)) != -1)

	for( int line_index = 0; line_index < total_lines ; line_index++ )
	{
		int current_line = m_control_word_list.LineIndex( line_index );	// ? current line	
		int word_length  = m_control_word_list.LineLength( current_line );  
		
		//TRACE("word_length = %i", 
		//int GetLine( int nIndex, LPTSTR lpszBuffer, int nMaxLength  ) const;

		m_control_word_list.GetLine( line_index, line_word.GetBuffer( word_length ), word_length );		//,line_word.GetAllocLength()

		m_word_list.push_back( line_word ); 

		line_word.ReleaseBuffer( word_length );
	}
}