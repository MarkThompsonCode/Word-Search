#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
	#include "Word Search.h"
#endif

#include "Word Search Doc.h"
#include <propkey.h>
#include "Dialog Setup.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CWordsearchDoc, CDocument)

BEGIN_MESSAGE_MAP(CWordsearchDoc, CDocument)

	ON_COMMAND( ID_BUTTON_SETUP, & CWordsearchDoc::on_dialog_setup )
	ON_COMMAND( ID_BUTTON_FONT, & CWordsearchDoc::on_dialog_font )

	ON_COMMAND( ID_FILE_NEW, & CWordsearchDoc::on_file_new )
	ON_COMMAND( ID_FILE_OPEN, & CWordsearchDoc::on_file_open )
	ON_COMMAND( ID_FILE_SAVE, & CWordsearchDoc::on_file_save )

	ON_COMMAND( ID_CONFIGURE, & CWordsearchDoc::on_dialog_setup )

END_MESSAGE_MAP()

CWordsearchDoc::CWordsearchDoc()
{
	m_grid_size.cx = 8; 
	m_grid_size.cy = 8;
	
	m_margins.left		= 40;
	m_margins.top		= 40;
	m_margins.right		= 200;
	m_margins.bottom	= 40;

	m_longest_word_width = 0;

	m_colour_background	= RGB( 240, 240, 240 );
	m_colour_font		= RGB( 0, 0, 0 );

	m_log_brush_background.lbStyle	= BS_SOLID;
	m_log_brush_background.lbColor	= m_colour_background;
	m_log_brush_background.lbHatch	= HS_BDIAGONAL;

	set_brush_background( m_log_brush_background );

	m_colour_selected			= RGB( 0, 255, 255 );
	m_colour_found				= RGB( 255,0,0) ;

	m_pen_grid_width.x			= 2;
	m_pen_grid_syle				= PS_SOLID;	
	m_pen_colour_grid			= RGB( 0, 0, 0 );

	// use extended pen style
	m_pen_width_line_select.x	= 10;
	m_pen_syle_line_select		= PS_SOLID;
	m_pen_colour_line_select	= RGB( 200, 200, 200 );

	m_title						= "- title -";
	m_show_grid_lines			= false;
	m_show_found_list			= false;
	m_show_found_grid			= true;

	m_uppercase_words			= false;
	m_reversed_words			= true;
	m_diagonal_words			= true;
	m_overlapping_words			= true;	

	m_font_template.lfHeight			= 20;	
	m_font_template.lfWidth				= 0;
	m_font_template.lfEscapement		= 0;
	m_font_template.lfOrientation		= 0;
	m_font_template.lfWeight			= FW_REGULAR; // FW_THIN 100 .. FW_BOLD 700 .. FW_BLACK 900  
	m_font_template.lfItalic			= false;
	m_font_template.lfUnderline			= false;
	m_font_template.lfStrikeOut			= false;
	m_font_template.lfCharSet			= DEFAULT_CHARSET;
	m_font_template.lfOutPrecision		= OUT_DEFAULT_PRECIS;
	m_font_template.lfClipPrecision		= CLIP_DEFAULT_PRECIS;
	m_font_template.lfQuality			= CLEARTYPE_QUALITY;
	m_font_template.lfPitchAndFamily	= FIXED_PITCH | FF_MODERN;//DEFAULT_PITCH | FF_SWISS;
	//_tcsncpy_s(m_font_template.lfFaceName, LF_FACESIZE, _T("Arial"), 7); 

	set_font_grid		( m_font_template );
	set_font_strike_out	( m_font_template );
	set_font_shadow		( m_font_template );
	
	m_null_character = _T(".");

	reset();
}

void CWordsearchDoc::on_file_new()
{
	m_word_list.clear();

	reset_grid_selected();
	reset_grid_found();

	reset();
}

void CWordsearchDoc::set_pen_grid()
{
	m_pen_grid.DeleteObject();

	m_pen_log_grid.lopnStyle		= m_pen_grid_syle;
	m_pen_log_grid.lopnWidth.x	= m_pen_grid_width.x;
	m_pen_log_grid.lopnColor		= m_pen_colour_grid;

	m_pen_grid.CreatePenIndirect( &m_pen_log_grid );
}

void CWordsearchDoc::set_pen_line_select()
{
	m_pen_line_select.DeleteObject();

	m_pen_log_line_select.lopnStyle		= m_pen_syle_line_select;
	m_pen_log_line_select.lopnWidth.x	= m_pen_width_line_select.x;
	m_pen_log_line_select.lopnColor		= m_pen_colour_line_select;

	m_pen_line_select.CreatePenIndirect( &m_pen_log_line_select );
}

CWordsearchDoc::~CWordsearchDoc() 
{ 
	m_word_list.clear();
	m_grid.clear();
	m_directions.clear();
	m_grid_selected.clear();
	m_grid_found.clear();
}

void CWordsearchDoc::reset()
{
	new_grid();

	set_directions();
	get_font_height();
	
	int attempt = 0;

	if( ! m_word_list.empty() ) 
	{
		m_current_word = m_word_list.begin();

		// ** require true back-tacking ** 
		// for(postition = start(); position != end(); direction++) 
		//  for(direction = start(); direction != end(); direction++) 
	
		while( ! place_words_recursive( m_word_list, m_grid ) && attempt < 10 );
		{
			place_words_recursive( m_word_list, m_grid );
			++attempt;
		}
	} 

	reset_grid_selected();
	reset_grid_found();

	// ** on_size() // on mouse_up { set window size }  ( if less than grid+margins )
				
	add_random_chars();

	set_pen_grid();
	set_pen_line_select();	

	UpdateAllViews( NULL );
}

void CWordsearchDoc::new_grid()
{
	m_grid.clear();

	for( int row = 0; row < m_grid_size.cy; row++ )
	{
		vector<CString> column;

		for( int col = 0; col < m_grid_size.cx; col++ )
			column.push_back( m_null_character );

		m_grid.push_back( column );
	}	
}

void CWordsearchDoc::add_random_chars()
{
	for(int y = 0; y < m_grid_size.cy; y++)
		for(int x = 0; x < m_grid_size.cx; x++)

			if( m_grid.at(y).at(x) == m_null_character )

				if( m_uppercase_words )
				{
					TCHAR uchar = 65 + (rand() % 26); // + local_region
					m_grid.at(y).at(x) = uchar;
				}
				else 
				{
					TCHAR uchar = 97 + (rand() % 26);
					m_grid.at(y).at(x) = uchar;  
				}
}

void CWordsearchDoc::reset_grid_selected()
{
	m_grid_selected.clear();

	for( int i = 0; i < m_grid_size.cy; i++ )
	{
		vector< bool > row;

		for( int j = 0; j < m_grid_size.cx; j++ )
			row.push_back( false );

		m_grid_selected.push_back( row );
	}
}

void CWordsearchDoc::reset_found_words()
{
	vector< CWord >::iterator word_it = m_word_list.begin();

	while( word_it != m_word_list.end() )
	{
		(*word_it).set_found( false );
		word_it++;
	}
}

void CWordsearchDoc::reset_grid_found()
{
	m_grid_found.clear();

	for( int i = 0; i < m_grid_size.cy; i++)
	{
		vector< bool > row;

		for( int j = 0; j < m_grid_size.cx; j++ )
			row.push_back( false );

		m_grid_found.push_back( row );
	}
}

void CWordsearchDoc::selected_changed()
{
	CString test;

	for( int y = 0; y < m_grid_size.cy; y++ ) 
		for( int x = 0; x < m_grid_size.cx; x++ ) 

			if( m_grid_selected.at( y ).at( x ) )
				test += m_grid.at( y ).at( x );

	vector< CWord >::iterator word_it = m_word_list.begin();

	while( word_it != m_word_list.end() )
	{
		if( (*word_it).cstring() == test || (*word_it).cstring() == test.MakeReverse() )
		{
			copy_selected_to_found();
			reset_grid_selected();
			(*word_it).set_found( true );

			TRACE("Found word: %S\n", (*word_it).cstring() );
		}
		word_it++;
	}
}

void CWordsearchDoc::copy_selected_to_found()
{
	for(int y = 0; y < m_grid_size.cy; y++)
		for(int x = 0; x < m_grid_size.cx; x++)

			if( m_grid_selected.at(y).at(x) == true)
				m_grid_found.at(y).at(x) = true;
}

const int CWordsearchDoc::get_font_height()
{
	LOGFONT logfontmain;
	m_font_main.GetLogFont( & logfontmain );
	m_font_height = logfontmain.lfHeight;

	return m_font_height;
}

void CWordsearchDoc::set_font_shadow( LOGFONT new_font )
{ 
	m_font_shadow.DeleteObject();	
	new_font.lfWeight	= FW_BLACK; // FW_THIN 100 .. FW_BOLD 700 .. FW_BLACK 900  	
	m_font_shadow.CreateFontIndirect(&new_font); 
}

void CWordsearchDoc::set_font_strike_out( LOGFONT new_font )	
{ 
	m_font_strikeout.DeleteObject();	
	new_font.lfStrikeOut	= true;	
	m_font_strikeout.CreateFontIndirect( &new_font ); 
}

const LOGFONT CWordsearchDoc::get_font_log_main()
{ 
	LOGFONT lf;
	//LOGFONT * plf = new LOGFONT;
	m_font_main.GetLogFont( & lf );
	return lf;
}

const RECT CWordsearchDoc::get_client_size() 
{ 
	RECT window_position; 
	AfxGetApp()->m_pMainWnd->GetWindowRect( &window_position);
	return window_position;
}

void CWordsearchDoc::set_client_size( const RECT new_size )
{		
	RECT window_position;

	AfxGetApp()->m_pMainWnd->GetWindowRect( &window_position);
	AfxGetApp()->m_pMainWnd->MoveWindow( window_position.left, window_position.top, new_size.right, new_size.bottom, TRUE );

	//AfxGetMainWnd()->m_hWnd
}

void CWordsearchDoc::set_directions()
{
	m_directions.clear();

		DIRECTION East  = { 0,  1};
		DIRECTION South = { 1,  0};

		m_directions.push_back( East );
		m_directions.push_back( South );
	
	if( m_diagonal_words )
	{
		DIRECTION NorthEast  = {-1,  1};
		DIRECTION SouthEast  = { 1,  1};

		m_directions.push_back( NorthEast );
		m_directions.push_back( SouthEast );
	}

	if( m_reversed_words )
	{
		DIRECTION North  = {-1,  0};
		DIRECTION West   = { 0, -1};

		m_directions.push_back( North );
		m_directions.push_back( West );
	}

	if( m_diagonal_words && m_reversed_words )
	{
		DIRECTION SoutWest    = { 1, -1 };
		DIRECTION NorthWest   = {-1, -1 };

		m_directions.push_back( SoutWest );
		m_directions.push_back( NorthWest );
	}

	TRACE("direction count = %i \n", m_directions.size() );
}

void CWordsearchDoc::set_grid_box_selected( const POINT box )			
{ 
	if( box.y < m_grid_selected.size() && box.x < m_grid_selected.at(box.y).size() )
		m_grid_selected.at(box.y).at(box.x) = true; 
} 

void CWordsearchDoc::toggle_grid_box_selected( const POINT box ) 
{ 
	POINT selected_box;
	selected_box.x = (box.x - m_margins.left) / m_grid_box_size.cx;
	selected_box.y = (box.y - m_margins.top) / m_grid_box_size.cy;

	if( selected_box.y < m_grid_selected.size() && selected_box.x < m_grid_selected.at(selected_box.y).size() )
		m_grid_selected.at(selected_box.y).at(selected_box.x) = ! m_grid_selected.at(selected_box.y).at(selected_box.x); 
}

void CWordsearchDoc::set_brush_background( const LOGBRUSH brush_new )	
{ 
	if( m_brush_background.GetSafeHandle() )		
		m_brush_background.DeleteObject(); 
	
	m_brush_background.CreateBrushIndirect( & brush_new ); 
}

void CWordsearchDoc::add_word( const CString word )
{	
	m_word_list.push_back( CWord( word, m_directions, m_grid_size, & m_grid, m_null_character ) );
}

void CWordsearchDoc::on_dialog_font()
{
	LOGFONT font_current = get_font_log_main();

	CFontDialog dialog_font( & font_current );

	dialog_font.m_cf.Flags |= CF_EFFECTS;
	dialog_font.m_cf.rgbColors = m_colour_font;
	
	if( dialog_font.DoModal() == IDOK )
	{
		LOGFONT font_new;

		dialog_font.GetCurrentFont( & font_new );

		// ? word list colour different to grid font colour
		set_font_grid( font_new );

		m_colour_font = dialog_font.GetColor();
		set_font_strike_out( font_new );

		//LOGFONT output = get_font_log_main();
		//TRACE("lfFaceName = %S, lfHeight = %d, lfWidth = %d, lfStrikeOut = %d\n", output.lfFaceName, output.lfHeight, output.lfWidth, output.lfStrikeOut);
	}	

	UpdateAllViews(NULL);
}

void CWordsearchDoc::on_dialog_setup()
{ 
	CSetupDialog setup_dialog;	//(NULL);  CSetupDialog setup_dialog() = "most vexing parse". = function setup_dialog() returns a CSetupDialog
								// eh? already defined / no local funtion definition

	setup_edit_data( & setup_dialog );

	if( setup_dialog.DoModal() == IDOK ) 
	{
		retrieve_edit_data( & setup_dialog );

		( AfxGetMainWnd() )->SetWindowText( m_title );
		
		//if( setup_dialog.reset_required() )
		//	reset();

		UpdateAllViews( NULL );
	}	
}

void CWordsearchDoc::setup_edit_data( CSetupDialog * const setup_dialog )
{	
	//setup_dialog->set_word_list( m_word_list.string_words() );
	vector< CWord >::const_iterator word_it = m_word_list.begin();

	while( word_it != m_word_list.cend() )
	{
		setup_dialog->add_word( ( *word_it ).cstring() );
		++word_it;
	}

	setup_dialog->set_title				( m_title );
		
	setup_dialog->set_grid_size			( m_grid_size );
	
	setup_dialog->set_colour_background	( m_colour_background );
	setup_dialog->set_colour_grid		( m_pen_colour_grid) ;
	setup_dialog->set_colour_selected	( m_colour_selected );
	setup_dialog->set_colour_found		( m_colour_found );

	setup_dialog->set_uppercase			( m_uppercase_words );
	setup_dialog->set_reversed			( m_reversed_words );
	setup_dialog->set_diaganols			( m_diagonal_words );
	setup_dialog->set_overlapping		( m_overlapping_words);

	setup_dialog->set_show_gridlines	( m_show_grid_lines );
	setup_dialog->set_show_found_list	( m_show_found_list );	
	// show_found_grid();
}

void CWordsearchDoc::retrieve_edit_data( const CSetupDialog * setup_dialog )
{
	m_title				= setup_dialog->get_title();
	m_grid_size			= setup_dialog->get_grid_size();
	
	m_colour_background	= setup_dialog->get_colour_background();
	m_pen_colour_grid	= setup_dialog->get_colour_grid();
	m_colour_selected	= setup_dialog->get_colour_selected();
	m_colour_found		= setup_dialog->get_colour_found();

	m_show_grid_lines	= setup_dialog->get_show_gridlines();
	m_uppercase_words	= setup_dialog->get_uppercase();
	m_reversed_words	= setup_dialog->get_reversed();
	m_diagonal_words	= setup_dialog->get_diaganols();
	m_overlapping_words	= setup_dialog->get_overlapping();
	m_show_found_list	= setup_dialog->get_show_found_list();
	//m_show_found_grid	= setup_dialog->get_show_found_grid();	

	//if( setup_dialog->reset_required() )
	//{
		set_directions();

		m_word_list.clear();
	
		vector< CString > words = setup_dialog->get_word_list();
		vector< CString >::const_iterator word_it = words.begin();

		while( word_it != words.cend() )
		{
			add_word( (*word_it) );
			++word_it;
		}

		set_pen_grid();

		reset();

		//if( m_uppercase_words ) change_case( upper_case );	
		//else change_case( lower_case );
	//}
}

void CWordsearchDoc::change_case( CASE case_change )
{
}

bool CWordsearchDoc::find_not_placed( vector< CWord > & word_list, vector< CWord >::iterator & word_it)
{
	// for each recursive call check list for words too place
	// word_list = m_word_list.begin();

	while( word_it != word_list.cend() )
	{
		// if word not placed
		if( ! (*word_it).is_placed() )
		// stop at this word and return; informing a word needs placing
			return true;

		word_it++;
	}

	// all words in list placed
	return false;
}

bool CWordsearchDoc::place_words_recursive( vector< CWord > & word_list, vector< vector< CString > > & grid)
{
	vector<CWord>::iterator word_it = word_list.begin();

	// find next un-placed word -- if at end of word list = success
	if( ! find_not_placed(word_list, word_it) )
		return true;
	
	// if can place word 
	if( (*word_it).can_place() )	// for each placement option = grid_witdh * grid_height * directions (10*5*8)
	{
		// place this word
		(*word_it).place();			

		// try and place the next word
		if( place_words_recursive(word_list, grid) )		
			return true;

		// remove all words
		(*word_it).remove();
	}
	/*
	place_words()

		// for( n = rand direction; n < directions.end(); n++ )
		//		position
	
		if( ! find_not_placed(word_list, word_it) )
			return true;

		find rand empty location(grid)

		for( each direction )
		{
			if( can_place(grid) )
			{
				place(grid)

				if( place_words() )
					return true;

				remove(grid)
			}
		}
		return false;

	*/

	// remove previous word

	// if( word == begin() ) { no solution }
	// reset/remove previous word *** overlapping characters
	return false;	
	// replace_previous_words()	// vector<CWord>::reverse_iterator = 
}
	
void CWordsearchDoc::on_file_open()
{
	CString file_name;

	CFileDialog dialog_open( TRUE );

	dialog_open.m_ofn.lStructSize		= sizeof( OPENFILENAME );
	dialog_open.m_ofn.lpstrFilter		= _T( "Word Search files (*.wdsc)\0*.wdsc\0" );
	dialog_open.m_ofn.lpstrInitialDir	= _T( ".\\wordsearches\\" );

	const int c_cbBuffSize				= ( 1 * ( MAX_PATH + 1 ) ) + 1;
	dialog_open.m_ofn.lpstrFile			= file_name.GetBuffer( c_cbBuffSize );
	dialog_open.m_ofn.nMaxFile			= c_cbBuffSize;

	//lpstrFileTitle // name and extension  (without path information) 
	//nMaxFileTitle // The size, in characters, of the buffer pointed to by lpstrFileTitle -- sizeof( CString.GetBuffer() )

	if( dialog_open.DoModal() == IDOK )
	{
		// Open document
		OnOpenDocument( file_name );

		UpdateAllViews( NULL );
	}
}

void CWordsearchDoc::on_file_save()
{
	CString file_name;

	CFileDialog dialog_save(FALSE);

	dialog_save.m_ofn.lStructSize		= sizeof( OPENFILENAME );
	dialog_save.m_ofn.lpstrFilter		= _T( "Word Search files (*.wdsc)\0*.wdsc\0");
	dialog_save.m_ofn.lpstrInitialDir	= _T( ".\\wordsearches\\" );

	const int c_cbBuffSize				= ( 1 * ( MAX_PATH + 1 ) ) + 1;
	dialog_save.m_ofn.lpstrFile			= file_name.GetBuffer( c_cbBuffSize );
	dialog_save.m_ofn.nMaxFile			= c_cbBuffSize;

	if (dialog_save.DoModal() == IDOK)
	{
		// Open document
		OnSaveDocument( file_name );
	}
}

void CWordsearchDoc::Serialize( CArchive & ar )
{
	if ( ar.IsStoring() )
	{
		ar << m_title;
		ar << m_grid_size.cx << m_grid_size.cy;
		
		ar << m_word_list.size();

		vector< CWord >::const_iterator word_it = m_word_list.begin();
		
		while( word_it != m_word_list.cend() )
		{
			ar << (*word_it).cstring();
			++word_it;
		}

		ar << m_show_grid_lines;

		ar << m_uppercase_words;
		ar << m_diagonal_words;
		ar << m_reversed_words;
		ar << m_overlapping_words;

		ar << m_show_found_list;
		ar << m_show_found_grid;

		ar << m_colour_background;
		ar << m_pen_colour_grid;
		ar << m_colour_selected;
		ar << m_colour_found;

		ar << m_colour_font;

		LOGFONT save_font_main;
		m_font_main.GetLogFont( &save_font_main );
		ar.Write( & save_font_main, sizeof( save_font_main ) );

		// on close save selected and found grids
	}
	else // if ( ar.IsLoading() )
	{
		ar >> m_title;
		ar >> m_grid_size.cx >> m_grid_size.cy;
		
		m_word_list.clear();

		int total_words;
		ar >> total_words;

		for(int i = 0; i < total_words; i++)	
		{
			CString load_word;
			ar >> load_word;
			
			add_word( load_word );
		}

		ar >> m_show_grid_lines;

		ar >> m_uppercase_words;
		ar >> m_diagonal_words;
		ar >> m_reversed_words;
		ar >> m_overlapping_words;

		ar >> m_show_found_list;
		ar >> m_show_found_grid;

		ar >> m_colour_background;
		ar >> m_pen_colour_grid;
		ar >> m_colour_selected;
		ar >> m_colour_found;

		ar >> m_colour_font;

		LOGFONT loadFont;
		ar.Read( &loadFont, sizeof(loadFont));

		set_font_grid( loadFont );
		set_font_strike_out( loadFont );

		( AfxGetMainWnd() )->SetWindowText( m_title );

		UpdateAllViews( NULL );

		reset();
	}
}

/*
BOOL CWordsearchDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// add reinitialization code here (SDI documents will reuse this document)
	return TRUE;
}
*/

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CWordsearchDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CWordsearchDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CWordsearchDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CWordsearchDoc diagnostics
#ifdef _DEBUG
	void CWordsearchDoc::AssertValid() const { CDocument::AssertValid(); }
	void CWordsearchDoc::Dump(CDumpContext& dc) const { CDocument::Dump(dc); }
#endif


	



