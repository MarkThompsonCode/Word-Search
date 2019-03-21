#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
	#include "Word Search.h"
#endif

#include "Word Search Doc.h"
#include "Word Search View.h"
#define _USE_MATH_DEFINES
#include <math.h>

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE( CWordsearchView, CView )

BEGIN_MESSAGE_MAP( CWordsearchView, CView )

	// Standard printing commands
	ON_COMMAND( ID_FILE_PRINT, & CView::OnFilePrint )
	ON_COMMAND( ID_FILE_PRINT_DIRECT, & CView::OnFilePrint )
	ON_COMMAND( ID_FILE_PRINT_PREVIEW, & CWordsearchView::OnFilePrintPreview )

	//ON_COMMAND( ID_HINT, & CWordsearchView::show_hint ) { flash fist letter of a word }
	ON_WM_MOUSEMOVE()	
	ON_WM_RBUTTONUP()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()	
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()

	ON_WM_ERASEBKGND()

END_MESSAGE_MAP()

CWordsearchView::CWordsearchView()
{
	m_previous_line = false;

	DIRECTION North		= {-1,  0};
	DIRECTION NorthEast	= {-1,  1};
	DIRECTION East		= { 0,  1};
	DIRECTION SouthEast	= { 1,  1};
	DIRECTION South		= { 1,  0};
	DIRECTION SoutWest	= { 1, -1};
	DIRECTION West		= { 0, -1};
	DIRECTION NorthWest	= {-1, -1};
	
	m_directions.push_back( North );
	m_directions.push_back( NorthEast );
	m_directions.push_back( East );
	m_directions.push_back( SouthEast );
	m_directions.push_back( South );
	m_directions.push_back( SoutWest );	
	m_directions.push_back( West );	
	m_directions.push_back( NorthWest );

	int sector = 1;

	for( int i = 0; i < 8; ++i )
	{		
		m_sectors.push_back( 22.5 * sector );
		sector += 2;
	}

	m_brush_background.CreateSolidBrush( RGB( 100, 100, 100 ) );
}

CWordsearchView::~CWordsearchView() 
{ 
	m_directions.clear();
	m_p_doc = nullptr;
}

BOOL CWordsearchView::PreCreateWindow( CREATESTRUCT & cs )
{
	//CFont m_font_strikeout 
	//CFont m_font_main
	//m_font_main.CreateFontIndirect( m_p_doc->get_font_log_main() );	
	//m_font_strikeout.CreateFontIndirect( m_p_doc->get_font_strike_out() );

	// Modify the Window class or styles here by modifying the CREATESTRUCT cs
	return CView::PreCreateWindow(cs);
}

void CWordsearchView::OnDraw( CDC * p_dc )
{
	m_p_doc = GetDocument();
	// save dc

	//draw background colour
	//p_dc->SelectObject( m_p_doc->get_brush_background() );
	CBrush background( m_p_doc->get_colour_background() );//( RGB( 200,200,200) );

	//LOGBRUSH brush_background = m_p_doc->get_log_brush_background();
	//background.CreateBrushIndirect( &brush_background );

	p_dc->SelectObject( & background );

	CRect rect;
	p_dc->GetClipBox( & rect );     // Erase the area needed

	p_dc->PatBlt( rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY );

	draw_grid_box_colours	( p_dc );
	draw_grid				( p_dc );
	draw_list_words			( p_dc );
	draw_grid_characters	( p_dc );	
	draw_title				( p_dc );

	//p_dc->SelectObject( p_doc->get_pen_line_select() )
}

BOOL CWordsearchView::OnEraseBkgnd( CDC * p_dc )
{
	//HBRUSH brush = CreateSolidBrush(RGB(0, 0, 255));
   //SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG)brush);

	//CWordsearchDoc* p_doc = GetDocument();

	//LOGBRUSH log_brush = ;
	
	//m_brush_background.DeleteObject();
	//m_brush_background.CreateBrushIndirect( &log_brush );	

	// Save old brush
	//HGDIOBJ p_brush_previous = m_p_doc->get_brush_background().GetSafeHanfl
	
	//p_dc->SelectObject( p_brush_previous );

	//return CView::OnEraseBkgnd( p_dc );
	return TRUE;
}

void CWordsearchView::OnLButtonDown( UINT n_flags, CPoint point )
{
	CView::OnLButtonDown(n_flags, point);

	m_point_begin = point;
}

void CWordsearchView::OnRButtonDown( UINT n_flags, CPoint point )
{
	CView::OnRButtonDown(n_flags, point);

	m_p_doc->reset_grid_selected();

	InvalidateRect(NULL);	
}

void CWordsearchView::OnMouseMove( UINT n_flags, CPoint point )
{
	CDC* p_dc = GetDC();

	if (n_flags & MK_LBUTTON)
	{
		LOGBRUSH logBrush;
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = RGB(0,255,0);

		CPen line_pen( PS_SOLID | PS_GEOMETRIC | PS_ENDCAP_ROUND, 10, &logBrush );
		p_dc->SelectObject( line_pen );

		p_dc->SetROP2(R2_NOTXORPEN);

		// If a line was drawn during an earlier WM_MOUSEMOVE message, draw over it. 
		// This erases the line by setting the color of its pixels to white. 

		if (m_previous_line)  // draging_line - erase previous dragged lines
		{ 
			p_dc->MoveTo(m_point_begin.x, m_point_begin.y); 
			p_dc->LineTo(m_point_previous_end.x, m_point_previous_end.y); 
		} 
				
		m_point_end = point; // line lenght is 0 - start a new line.

		p_dc->MoveTo(m_point_begin.x, m_point_begin.y); 
		p_dc->LineTo(m_point_end.x, m_point_end.y); 		 

		// Set the previous line flag, 
		m_previous_line = true; // draging_line

		// capture releative mouse position
		m_point_previous_end = m_point_end; 	// dragged line end point - save the ending point of the new line			
	}	
	else 
	{
		m_point_end = point;
		m_point_begin = point;
		m_point_previous_end = point;
	}

	ReleaseDC(p_dc);
}

void CWordsearchView::OnLButtonUp( UINT n_flags, CPoint point )
{
	CView::OnLButtonUp(n_flags, point);
	
	CDC* p_dc = GetDC();
	
	if( m_previous_line )
	{
		// erase last line drawn
		p_dc->SetROP2(R2_NOTXORPEN); 

		m_point_end = point; // if line not dragged - single click - line size = 0 

		p_dc->MoveTo(m_point_begin.x, m_point_begin.y); // point where first mouse LB down was
		p_dc->LineTo(m_point_previous_end.x, m_point_previous_end.y);	

		m_previous_line = false; // draging_line = false; // no dragged lines to erase

		// if( next_direction != current_direction ) { direction_changed = true; }
		select_boxes_line();
	}
	else
	{
		GetDocument()->toggle_grid_box_selected(point);
		GetDocument()->selected_changed();
	}

	InvalidateRect(NULL);

	/* OnLeftMouseUp()
	if( p_doc->square_grid() )
	{
		int box_height = box_width;		
	}
	else
	{
		int box_height = ( client_size.bottom - margins.top  - margins.bottom ) / grid_size.second; 

		int window_height = margins.top + ( box_height * grid_size.second ) + margins.bottom;
		int window_width = margins.left + (box_width * grid_size.first) + margins.right;

		RECT new_size = {0, 0, window_width, window_height }; 
		p_doc->set_client_size( new_size ); 
	}
	*/

	ReleaseDC(p_dc);
}

void CWordsearchView::select_boxes_line()
{
	SIZE	grid_size	= m_p_doc->get_grid_size();
	SIZE	box_size		= m_p_doc->get_box_size();
	POINT box_start;
	POINT box_end;
	POINT box_count;
	RECT	margins		= m_p_doc->get_margins();

	double x1 = m_point_begin.x;
	double y1 = m_point_begin.y;
	double x2 = m_point_previous_end.x;
	double y2 = m_point_previous_end.y;

	double line_length = pow( (y2 - y1), 2) + pow( (x2 - x1), 2);
	line_length = std::sqrt(line_length);
	
	double angle = atan2( ( y2 - y1 ), ( x2 - x1 ) ) * ( 180.0 / M_PI ); // arc sin (inverse sine^-1)

	angle += 90.00;

	if(angle < 0) 
		angle += 360;
		
	vector<double>::const_iterator sector_it = m_sectors.begin();

	int direction_index = 0; 

	while( angle > (*sector_it) && angle < 337.5) // 8 sectors 
	{
		direction_index++;
		sector_it++;
	}

	box_start.x = ( x1 - margins.left ) / box_size.cx;
	box_start.y = ( y1 - margins.top )  / box_size.cy;

	box_end.x = ( x2 - margins.left ) / box_size.cx;
	box_end.y = ( y2 - margins.top )  / box_size.cy;
		
	box_count.x = abs( box_start.x - box_end.x );
	box_count.y = abs( box_start.y - box_end.y );

	if( box_count.y > box_count.x ) box_count.x = box_count.y;
	
	if (   box_start.x >= 0 && box_end.x < grid_size.cx
		 && box_start.y >= 0 && box_end.y < grid_size.cy )
	{
		for(int i = 0; i <= box_count.x; i++)
		{
			m_p_doc->set_grid_box_selected(box_start);

			box_start.x += m_directions.at( direction_index ).horizontal;
			box_start.y += m_directions.at( direction_index ).vertical;

			m_p_doc->selected_changed();

			InvalidateRect( NULL );
		}
	}	
}
	
void CWordsearchView::toggle_selected_boxes( UINT n_flags, CPoint point )
{
	RECT margins	= m_p_doc->get_margins();
	SIZE box_size	= m_p_doc->get_box_size();
	SIZE grid_size = m_p_doc->get_grid_size();

	if(    point.x >= margins.left && point.x <= margins.left + ( box_size.cx * grid_size.cx ) 
		 && point.y >= margins.top  && point.y <= margins.top  + ( box_size.cy * grid_size.cy ) )
	{
		POINT grid_select;

		grid_select.x = ( point.x - margins.left ) / box_size.cx;
		grid_select.y = ( point.y - margins.top ) / box_size.cy;

		m_p_doc->set_grid_box_selected( grid_select );
		m_p_doc->selected_changed();

		InvalidateRect( NULL );
	}
}

void CWordsearchView::draw_grid_box_colours( CDC * p_dc )
{
	//int save = p_dc->SaveDC();

	CBrush brush_selected;
	CBrush brush_found;

	p_dc->SelectStockObject( NULL_PEN );

	// outlines on selected ? make optional
	brush_selected.CreateSolidBrush( m_p_doc->get_colour_grid_select() );
	brush_found.CreateSolidBrush( m_p_doc->get_colour_grid_found() );

	RECT margins	= m_p_doc->get_margins();
	SIZE box_size	= m_p_doc->get_box_size();
	SIZE grid_size = m_p_doc->get_grid_size();

	POINT box;
	CRect rect;

	for( int y = 0; y < grid_size.cy; y++ ) 
		for( int x = 0; x < grid_size.cx; x++ ) 
		{
			box.x = x;
			box.y = y;

			if( m_p_doc->is_grid_box_selected(box) )
			{
				int x1 = margins.left + ( box.x * box_size.cx );
				int y1 = margins.top  + ( box.y * box_size.cy );
				int x2 = margins.left + ( box.x * box_size.cx ) + box_size.cx;
				int y2 = margins.top  + ( box.y * box_size.cy ) + box_size.cy;
				
				p_dc->SelectObject( &brush_selected );

				rect.SetRect( x1, y1, x2, y2 );
				p_dc->RoundRect(&rect, CPoint( 4,4 ) );

			}
			else 
				// if( m_show_grid_found )
				if ( m_p_doc->is_grid_box_found( box ) )
				{
					p_dc->SelectObject( &brush_found );

					int x1 = margins.left + ( box.x * box_size.cx );
					int y1 = margins.top  + ( box.y * box_size.cy );
					int x2 = margins.left + ( box.x * box_size.cx ) + box_size.cx;
					int y2 = margins.top  + ( box.y * box_size.cy ) + box_size.cy;

					rect.SetRect( x1, y1, x2, y2 );
					p_dc->RoundRect( &rect, CPoint( 4,4 ) );
				}
		}	
		//p_dc->RestoreDC(save);
}

void CWordsearchView::draw_grid( CDC * p_dc )
{
	int save_dc = p_dc->SaveDC();
	
	RECT client_size;
	GetClientRect( &client_size );
   //p_dc->DPtoLP( &client_size );

	p_dc->SetMapMode( MM_TEXT );
	p_dc->SelectObject( m_p_doc->get_pen_grid() );
	p_dc->SetBkMode( TRANSPARENT );

	SIZE	grid_size	= m_p_doc->get_grid_size();
	RECT	margins		= m_p_doc->get_margins();
	int		box_width	= ( client_size.right  - margins.left - margins.right ) / grid_size.cx;
	int		box_height	= box_width;	

	SIZE box_size = { box_width, box_height };
	m_p_doc->set_box_size( box_size );

	p_dc->SelectStockObject( NULL_BRUSH ); 
	p_dc->RoundRect( margins.left, margins.top, margins.left + ( box_width * grid_size.cx ), margins.top + ( box_height * grid_size.cy ), 8,8 );

	if( m_p_doc->show_grid_lines() )
	{
		// ## horizontal lines ##
		for( int y = 1; y < grid_size.cy; y++ )
		{		
			p_dc->MoveTo( margins.left, margins.top + ( box_width * y ) );
			p_dc->LineTo( margins.left + ( box_width * grid_size.cx ), margins.top + ( box_width * y ) ); // - pen_width 
		}

		// ## vertical lines ##
		for(int x = 1; x < grid_size.cx; x++) 
		{
			p_dc->MoveTo( margins.left + ( box_height * x ), margins.top );
			p_dc->LineTo( margins.left + ( box_height * x ), margins.top + ( box_height * grid_size.cy ) ); // - pen_width 
		}
	}
	
	p_dc->RestoreDC(save_dc);
}

void CWordsearchView::draw_title( CDC * p_dc )
{
	//int dc_saved = p_dc->SaveDC();

	RECT	margins				= m_p_doc->get_margins();
	SIZE	grid_size			= m_p_doc->get_grid_size();
	SIZE	box_size				= m_p_doc->get_box_size();	

	//p_dc->SelectObject( m_font_main );
	p_dc->SetBkMode( TRANSPARENT );
	p_dc->SelectObject( m_p_doc->get_font_main() );

	CSize text_size = p_dc->GetTextExtent( m_p_doc->get_title() );

	int pos_x = margins.left + ( ( box_size.cx * grid_size.cx ) / 2 ) + ( text_size.cx / 2 );  // client area / 2 - ( text width / 2 )
	int pos_y = margins.top / 2 - ( text_size.cy / 2); 

	p_dc->TextOut( pos_x, pos_y, m_p_doc->get_title() );	 // y = textHeight -> top_margin = text_height * 2;

	//p_dc->RestoreDC( dc_saved );
}

void CWordsearchView::draw_list_words( CDC * p_dc )
{
	//int dc_saved = p_dc->SaveDC();

	// ** CDC::DrawText ** formated rectangle

	// add: dragable and auto re-size word list

	RECT	margins		= m_p_doc->get_margins();
	SIZE	grid_size	= m_p_doc->get_grid_size();
	SIZE	box_size	= m_p_doc->get_box_size();
	int	font_height		= abs( m_p_doc->get_font_height() );
	int	spacing			= 10;
	int	x				= margins.left + ( box_size.cx * grid_size.cx ) + 20;
	int	y				= margins.top;	
	// PEN = 3;

	p_dc->SetTextAlign( TA_LEFT );
	p_dc->SetBkMode( TRANSPARENT );

	vector<CWord>::const_iterator word_it = m_p_doc->first_word();

	while( word_it != m_p_doc->last_word() )
	{
		CString word = (*word_it).cstring();
		
		if( (*word_it).is_found() )
		{
			if( m_p_doc->show_found_list() )
			{
				p_dc->SelectObject( m_p_doc->get_font_strike_out() );
				p_dc->TextOut( x, y, word );	
			}
		}
		else
		{
			p_dc->SelectObject( m_p_doc->get_font_main() );
			p_dc->TextOut( x, y, word );
		}

		y += font_height + spacing;
		word_it++;		
	}

	//p_dc->RestoreDC( dc_saved );
}

void CWordsearchView::draw_grid_characters( CDC * p_dc )
{
	//int save_dc = p_dc->SaveDC();

	CWordsearchDoc* p_doc = GetDocument();

	p_dc->SelectObject( m_p_doc->get_font_main() );
	p_dc->SetBkMode( TRANSPARENT );	
	
	SIZE	box_size	= m_p_doc->get_box_size();
	int	box_width_half	= box_size.cx / 2;
	int	box_height_half	= box_size.cy / 2;
	RECT	margins		= m_p_doc->get_margins();
	int	font_height		= abs( m_p_doc->get_font_height() );
	//CString string_null	= m_p_doc->get_null_character();
	
	p_dc->SetTextAlign(TA_CENTER);

	/* 
	// draw character shadow
	if( p_doc->show_text_shadow() )
	{
		p_dc->SetTextColor( RGB(200,200,200) );
		//p_dc->SelectObject( p_doc->get_font_shadow() );


		for( int y = 0; y < p_doc->get_grid_size().cy; y++ ) 
			for( int x = 0; x < p_doc->get_grid_size().cx; x++ ) 

				if( (*p_doc->get_grid()).at(y).at(x) != p_doc->get_null_character() )
				{
					p_dc->TextOut(	margins.left + ( box_size.cx * x ) + box_width_half + 2,
										margins.top  + ( box_size.cy * y ) + box_height_half - ( font_height / 2 ) + 2,
										(*p_doc->get_grid()).at(y).at(x)
									 );		
				}
	}
	*/

	p_dc->SetTextColor( m_p_doc->get_font_colour() );

	for( int y = 0; y < m_p_doc->get_grid_size().cy; y++ ) 
		for( int x = 0; x < m_p_doc->get_grid_size().cx; x++ ) 

			if( ( *m_p_doc->get_grid() ).at( y ).at( x ) != m_p_doc->get_null_character() )
			{
				p_dc->TextOut(	margins.left + ( box_size.cx * x ) + box_width_half,
									margins.top  + ( box_size.cy * y ) + box_height_half - ( font_height / 2 ),
									( *m_p_doc->get_grid() ).at ( y ).at( x )
								 );		
			}

	//p_dc->RestoreDC(save_dc);
}

// CWordsearchView printing
void CWordsearchView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CWordsearchView::OnPreparePrinting( CPrintInfo * pInfo )
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CWordsearchView::OnBeginPrinting( CDC * /*pDC*/, CPrintInfo * /*pInfo*/ )
{
	// TODO: add extra initialization before printing
}

void CWordsearchView::OnEndPrinting( CDC * /*pDC*/, CPrintInfo* /*pInfo*/ )
{
	// TODO: add cleanup after printing
}

void CWordsearchView::OnRButtonUp( UINT /* nFlags */, CPoint point )
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CWordsearchView::OnContextMenu( CWnd * /* pWnd */, CPoint point )
{
	#ifndef SHARED_HANDLERS
		//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
	#endif
}

// CWordsearchView diagnostics
#ifdef _DEBUG
	void CWordsearchView::AssertValid() const { CView::AssertValid(); }
	void CWordsearchView::Dump(CDumpContext& dc) const { CView::Dump(dc); }

	CWordsearchDoc* CWordsearchView::GetDocument() const // non-debug version is inline
	{
		ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWordsearchDoc)));
		return (CWordsearchDoc*)m_pDocument;
	}
#endif
