/////////////////////////////////////////////////////////////////////////////
// Name:        cmpim.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_RECOGNITION
	#ifdef AX_COMPARISON

#ifdef __GNUG__
    #pragma implementation "cmpim.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "cmpim.h"
#include "cmp.h"
#include "cmpfile.h"
#include "recognition/recfile.h"

#include "app/axapp.h"

#include "im/impage.h"
#include "im/imstaff.h"


//----------------------------------------------------------------------------
// CmpImController
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CmpImController,AxImageController)

// WDR: event table for CmpImController

BEGIN_EVENT_TABLE(CmpImController,AxImageController)
    //EVT_MENU_RANGE( ID4_POPUP_MENU_MUSIC, ID4_POPUP_MENU_VOID, CmpImController::OnChangeClassification )
END_EVENT_TABLE()

CmpImController::CmpImController( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style , int flags) :
    AxImageController( parent, id, position, size, style, flags )
{
    m_envPtr = NULL;
	
    m_recFilePtr = new RecFile( "cmp_im" + wxString::Format("%d", id) );

}

CmpImController::CmpImController()
{
}

CmpImController::~CmpImController()
{
}

void CmpImController::SetEnv( CmpEnv *env )
{
    m_envPtr = env;
}

void CmpImController::LoadRecImage( int colNo, wxString filename, int staff, int segment, int pos, int flag )
{
	wxASSERT( m_envPtr );
	wxASSERT( m_recFilePtr );
    wxClassInfo *info;
    info = m_viewPtr->GetClassInfo();
    wxASSERT_MSG( info,"Class info cannot be NULL" );
    wxASSERT_MSG( info->IsKindOf( CLASSINFO( CmpImWindow ) ),
        "View must be a CmpImWindow");
	
	if ( ((CmpEnv*)m_envPtr)->GetCollationPtr() == NULL )
		return;
	if ( filename.IsEmpty() )
		return;
		
	// pas tres plombe.......
	wxString full_filename = (((CmpEnv*)m_envPtr)->GetCollationPtr())->m_collationParts[colNo].m_bookPart->m_book->m_recBookFilePtr->m_axFileDir
		+ wxFileName::GetPathSeparator() + filename + ".axz";
		
	if ( full_filename != m_recFilePtr->m_filename )
	{
		if ( !m_recFilePtr->Close() || !m_recFilePtr->Open( full_filename ) )
			return;
		AxImage img;
		m_recFilePtr->GetImage1( &img );
		this->ResetImage( img );
	}
	else
		m_viewPtr->UpdateViewFast();

    if ( staff < (int)m_recFilePtr->GetImPage()->m_staves.GetCount())
	{
		ImStaff *imstaff = &m_recFilePtr->GetImPage()->m_staves[staff];
	    int y = m_recFilePtr->GetImPage()->ToViewY(imstaff->m_y);// + STAFF_HEIGHT / 2);
		//int y = imstaff->m_y + STAFF_HEIGHT / 2;
		int x1, x2;
		const wxPen *pen = wxMEDIUM_GREY_PEN;
		wxPen blue( *wxBLUE, 1 );
		switch (flag)
		{
			case CMP_INS : pen = wxRED_PEN; break;
			//case CMP_DEL : pen = wxGREEN_PEN; break;
			case CMP_SUBST : pen = &blue; break;
		}
		imstaff->GetMinMax( &x1, &x2 );
		((CmpImWindow*)m_viewPtr)->ScrollSource(  x1 + pos, y );
		((CmpImWindow*)m_viewPtr)->SetCirclePen( pen , 2 );
		((CmpImWindow*)m_viewPtr)->DrawCircle( );
	}
	
	//wxLogMessage(" %s, %d, ID %d", full_filename.c_str(), staff, this->GetId() );
}

// WDR: handler implementations for CmpImController

//----------------------------------------------------------------------------
// CmpImWindow
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CmpImWindow, AxScrolledWindow)

// WDR: event table for CmpImWindow

BEGIN_EVENT_TABLE(CmpImWindow,AxScrolledWindow)
    //EVT_MOUSE_EVENTS( CmpImWindow::OnMouse )
    //EVT_SCROLLWIN(CmpImWindow::OnScroll )
    //EVT_PAINT( CmpImWindow::OnPaint )
END_EVENT_TABLE()

CmpImWindow::CmpImWindow( CmpImController *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    AxScrolledWindow( parent, id, position, size, style )
{
    m_envPtr = NULL;
		
	//wxASSERT_MSG( parent->IsKindOf( CLASSINFO( CmpImController ) ) , wxT("View must be a CmpImController") );
	m_recFilePtr = ((CmpImController*)parent)->GetRecFile( );
	m_circleCenter = wxPoint(0,0);
    
    /*item = m_popupMenuClassification.Append( ID4_POPUP_MENU_MUSIC, _("In staff music symbols") );
    item->SetBitmap( BitmapsFuncClassification( 0 ) );
    item = m_popupMenuClassification.Append( ID4_POPUP_MENU_TITLE, _("Title elements") );
    item->SetBitmap( BitmapsFuncClassification( 1 ) );
    item = m_popupMenuClassification.Append( ID4_POPUP_MENU_LYRICS, _("Lyrics of inter staves music symbols") );
    item->SetBitmap( BitmapsFuncClassification( 2 ) );
    item = m_popupMenuClassification.Append( ID4_POPUP_MENU_ORNATE_LETTER, _("Ornate letter") );
    item->SetBitmap( BitmapsFuncClassification( 3 ) );
    item = m_popupMenuClassification.Append( ID4_POPUP_MENU_TEXT_IN_STAFF, _("In staff text elements") );
    item->SetBitmap( BitmapsFuncClassification( 4 ) );
    item = m_popupMenuClassification.Append( ID4_POPUP_MENU_VOID, _("Blank") );
    item->SetBitmap( BitmapsFuncClassification( 5 ) );*/
}

CmpImWindow::CmpImWindow()
{
}

CmpImWindow::~CmpImWindow()
{
}

void CmpImWindow::SetEnv( CmpEnv *env )
{
    m_envPtr = env;
}

void CmpImWindow::SetCirclePen( const wxPen *pen, int width )
{
    m_pen = *pen;
    m_pen.SetWidth( width );
}

void CmpImWindow::ScrollSource( double x, double y )
{
    // calculate scroll position
    int scrollX = (int)(x * m_scale);
    int scrollY = (int)(y * m_scale);

    int clientX, clientY;
    this->GetClientSize( &clientX, &clientY );

    m_circleCenter.x = scrollX;
    m_circleCenter.y = scrollY;

    scrollX -= clientX / 2;
    scrollY -= clientY / 2;
    
    int unitX, unitY;
    this->GetScrollPixelsPerUnit( &unitX,&unitY );

    if ( unitX )
        scrollX /= unitX;
    if ( unitY )
        scrollY /= unitY;

    scrollX = ( scrollX < 0 ) ? 0 : scrollX;
    scrollY = ( scrollY < 0 ) ? 0 : scrollY;

    this->Scroll( scrollX, scrollY );
}

void CmpImWindow::DrawCircle( )
{
    wxASSERT_MSG(m_bitmap,"Bitmap cannot be NULL");
    wxASSERT_MSG(m_bufferBitmap,"Buffer bitmap cannot be NULL");

    // calculate scroll position
    int scrollX, scrollY;
    this->GetViewStart(&scrollX,&scrollY);
    int unitX, unitY;
    this->GetScrollPixelsPerUnit(&unitX,&unitY);
    scrollX *= unitX;
    scrollY *= unitY;

    wxMemoryDC memDC;
    memDC.SelectObject(*m_bufferBitmap);
    
    wxMemoryDC bufDC;
    bufDC.SelectObject(*m_bitmap);
    bufDC.SetBrush( *wxTRANSPARENT_BRUSH );
    bufDC.SetPen( m_pen );

    int clientX, clientY;
    this->GetClientSize( &clientX, &clientY );

    bufDC.Blit(scrollX, scrollY, clientX, clientY,
        &memDC, scrollX, scrollY );

    memDC.SelectObject( wxNullBitmap );
    
    int radius = 80; 
    bufDC.DrawCircle( m_circleCenter.x, m_circleCenter.y, radius);

    wxClientDC dc(this);
    dc.Blit(0, 0, clientX, clientY,
        &bufDC, scrollX, scrollY);

    bufDC.SelectObject( wxNullBitmap );
}


// WDR: handler implementations for CmpImWindow

	#endif // AX_COMPARISON
#endif // AX_RECOGNITION

