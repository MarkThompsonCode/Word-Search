#pragma once

#include <vector>
#include "CWord.h"

class CSetupDialog;

using std::vector;

struct CASE { int upper_case; int lower_case; };

class CWordsearchDoc : public CDocument
{
public:
	CWordsearchDoc();
	virtual			~CWordsearchDoc();

	//virtual BOOL	OnNewDocument();	// ? private
	virtual void	Serialize( CArchive& ar );

	// #### FONTS ####
	void			set_font_grid( LOGFONT new_font )		{ m_font_main.DeleteObject(); m_font_main.CreateFontIndirect( & new_font ); }
	void			set_font_strike_out( LOGFONT new_font ); 
	const CFont &	get_font_strike_out() const				{ return m_font_strikeout; }

	void			set_font_shadow( LOGFONT new_font );
	const CFont &	get_font_shadow() const					{ return m_font_shadow; }

	const CFont &	get_font_main() const					{ return m_font_main; }
	const LOGFONT	get_font_log_main();
	const COLORREF	get_font_colour() const					{ return m_colour_font; }
	const int		get_font_height();

	// #### GRID ####
	void			set_box_size( SIZE box_size )			{ m_grid_box_size = box_size; }
	vector< vector< CString > > * const get_grid()			{ return & m_grid; }

	void			set_pen_line_select();

	void			set_pen_grid();
	const CPen &	get_pen_grid() const					{ return m_pen_grid; }
	//const POINT	get_pen_grid_width() const				{ return get_pen_grid_width.x; }
	const SIZE		get_grid_size() const					{ return m_grid_size; }
	const SIZE		get_box_size() const					{ return m_grid_box_size; }
	BOOL			show_grid_lines() const					{ return m_show_grid_lines; }

	const CString	get_null_character() const				{ return m_null_character; }	
	
	// #### MARGINS ####
	const RECT		get_margins() const	{ return m_margins; }
	void			set_client_size( const RECT new_size );

	const RECT		get_client_size();
	
	//void setClientSize(const RECT clientSize)				{ m_clientSize = clientSize; }
	//const RECT get_client_size() const					{ return m_clientSize; }
	
	// #### TITLE ####
	void  set_title( const CString title )					{ m_title = title; }  // CWordsearchDoc.set_title() = "subject";
	const CString get_title() const							{ return m_title; }

	// #### WORD LIST ####
	const vector<CWord>::const_iterator first_word() const	{ return m_word_list.begin(); }
	const vector<CWord>::const_iterator last_word() const	{ return m_word_list.end(); }

	const BOOL show_found_list() { return m_show_found_list; }
		
	//void set_word_found(const int wordIndex)				{ m_word_list. true; }
	//bool is_word_found(const int wordIndex) const			{ return m_found_words[wordIndex]; }

	// #### DIRECTIONS ####
	void						set_directions();	
	vector< DIRECTION > const	get_directions()			{ return m_directions; }
	//DIRECTION get_direction(const int index)				{ return m_directions.at(index); }//if( index < m_directions.size() )  else return 0; }

	// #### SELECTED ####
	void reset_grid_selected();
	void set_grid_box_selected( const POINT box );
	void toggle_grid_box_selected( const POINT box );
	
	bool is_grid_box_found( const POINT box ) const			{ return m_grid_found.at( box.y ).at( box.x ); }
	bool is_grid_box_selected( const POINT box ) const		{ return m_grid_selected.at( box.y ).at( box.x ); }
	void selected_changed();	

	// #### COLOURS ####
	void set_colour_grid_select( const COLORREF colour )	{ m_colour_selected = colour; }
	const COLORREF get_colour_grid_select() const			{ return m_colour_selected; }

	void set_brush_background( const LOGBRUSH brush_new );
	LOGBRUSH get_log_brush_background()						{ return m_log_brush_background; }
	const CBrush& get_brush_background() const				{ return m_brush_background; }

	const COLORREF get_colour_background() const			{ return m_colour_background; }
	
	void set_colour_font_grid( const COLORREF colour )		{ m_colour_font = colour; }
	const COLORREF get_colour_font_grid() const				{ return m_colour_font; }

	void set_colour_grid_found( const COLORREF colour )		{ m_colour_found = colour; }
	const COLORREF get_colour_grid_found() const			{ return m_colour_found; }
	
	#ifdef SHARED_HANDLERS
		virtual void InitializeSearchContent();
		virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
	#endif

	#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
	#endif

private:
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNCREATE( CWordsearchDoc )

	// #### EDIT DIALOG ####
	void setup_edit_data( CSetupDialog * const); 
	void retrieve_edit_data( const CSetupDialog * ); 
	
	// #### MESSAGE HANDLERS ####
	afx_msg void on_dialog_setup(); // edit exising search - if words changed = re-position words.
	afx_msg void on_dialog_font();

	afx_msg void on_file_new();
	afx_msg void on_file_open();
	afx_msg void on_file_save();

	void reset();

	// #### TITLE ####
	CString			m_title;
	
	// #### WORD LIST ####
	void			add_word( const CString );
	void			change_case( CASE ); //** implement - rand chars and words // change_case( UPPER / LOWER );
	void			reset_found_words();

	vector< CWord >	m_word_list;
	BOOL			m_uppercase_words;
	BOOL			m_show_found_list;
	BOOL			m_show_found_grid;
	//bool			m_sort_words;

	// #### FONTS ####
	LOGFONT			m_font_template;
	CFont			m_font_main;
	COLORREF		m_colour_font; // main
	CFont			m_font_strikeout;
	int				m_font_height;
	CFont			m_font_shadow;

	// #### GRID ####
	vector< vector< CString > > m_grid;
	SIZE			m_grid_size;
	SIZE			m_grid_box_size;
	BOOL			m_show_grid_lines;

	void			new_grid();
	void			add_random_chars();
	
	CPen			m_pen_grid;
	LOGPEN			m_pen_log_grid;	
	unsigned int	m_pen_grid_syle;	
	POINT			m_pen_grid_width; 
	COLORREF		m_pen_colour_grid;

	CPen			m_pen_line_select;
	LOGPEN			m_pen_log_line_select;
	unsigned int	m_pen_syle_line_select;
	POINT			m_pen_width_line_select;
	COLORREF		m_pen_colour_line_select;
	
	// #### RANDOM CHARS ####
	CString			m_null_character;
	const CString	get_null_char() const { return m_null_character; }

	// #### PLACEMENT ####
	vector<CWord>::iterator m_current_word;
	bool				place_words_recursive( vector< CWord > & word_list, vector< vector< CString > > & grid);	// shuffle words about until words placed
	bool				find_not_placed( vector< CWord > & word_list, vector< CWord >::iterator & word_it);

	// #### COLOURS ####
	COLORREF			m_colour_background;
	COLORREF			m_colour_selected;
	COLORREF			m_colour_found;
	//LOGBRUSH			m_grid_select_brush;

	LOGBRUSH			m_log_brush_background;
	CBrush				m_brush_background;

	// #### MARGINS ####	
	RECT				m_margins;
	RECT				m_clientSize;
	int					m_longest_word_width;

	// #### DIRECTIONS ####
	vector<DIRECTION>	m_directions;
	BOOL				m_reversed_words;
	BOOL				m_diagonal_words;	
	BOOL				m_overlapping_words;	

	// #### SELECTED ####	
	void copy_selected_to_found();
	void reset_grid_found();

	vector< vector< bool > >	m_grid_selected;
	vector< vector<bool > >		m_grid_found;	
	
	//BOOL CMyDoc::OnNewDocument ()
	//BOOL CMyDoc::OnOpenDocument (LPCTSTR lpszPathName)

	#ifdef SHARED_HANDLERS
		void SetSearchContent(const CString& value);
	#endif
public:
	
};
