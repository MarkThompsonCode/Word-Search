#pragma once

class CWordsearchView : public CView
{
protected: // create from serialization only
	CWordsearchView();
	DECLARE_DYNCREATE( CWordsearchView )	

public:
	virtual				~CWordsearchView();

	CWordsearchDoc *	GetDocument() const;

	virtual void		OnDraw( CDC * p_dc );  // overridden to draw this view
	virtual BOOL		PreCreateWindow( CREATESTRUCT & cs );

	#ifdef _DEBUG
		virtual void	AssertValid() const;
		virtual void	Dump( CDumpContext & dc ) const;
	#endif

private:
	DECLARE_MESSAGE_MAP()

	virtual BOOL	OnPreparePrinting	( CPrintInfo* p_info );
	virtual void	OnBeginPrinting		( CDC*, CPrintInfo* p_info );
	virtual void	OnEndPrinting		( CDC*, CPrintInfo* p_info );

	afx_msg BOOL	OnEraseBkgnd		( CDC* );

	afx_msg void	OnFilePrintPreview	();

	afx_msg void	OnRButtonDown		( UINT nFlags, CPoint );
	afx_msg void	OnRButtonUp			( UINT n_flags, CPoint );
	
	afx_msg void	OnLButtonDown		( UINT n_flags, CPoint );
	afx_msg void	OnLButtonUp			( UINT nFlags, CPoint );

	afx_msg void	OnMouseMove			( UINT nFlags, CPoint );
	
	afx_msg void	OnContextMenu		( CWnd* p_wnd, CPoint );	

	void toggle_selected_boxes			( UINT nFlags, CPoint );
		
	void	draw_grid				( CDC * );
	void	draw_title				( CDC * );
	void	draw_list_words			( CDC * );
	void	draw_grid_characters	( CDC * );
	void	draw_grid_box_colours	( CDC * );

	CWordsearchDoc * m_p_doc;

	bool	m_previous_line;
	CPoint	m_point_begin;			// beginning point 
	CPoint	m_point_end;			// new endpoint 
	CPoint	m_point_previous_end;	// previous endpoint 

	void	select_boxes_line();

	vector< DIRECTION >		m_directions;
	vector< double >		m_sectors;	// circle sectors in degrees

	CBrush					m_brush_background;
};

#ifndef _DEBUG
	inline CWordsearchDoc* CWordsearchView::GetDocument() const{ return reinterpret_cast<CWordsearchDoc*>(m_pDocument); }
#endif