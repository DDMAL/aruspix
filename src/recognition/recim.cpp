/////////////////////////////////////////////////////////////////////////////
// Name:        recim.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_RECOGNITION

#ifdef __GNUG__
    #pragma implementation "recim.cpp"
#endif

#include <algorithm>
using std::min;
using std::max;

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "recim.h"
#include "rec.h"
#include "recmus.h"
#include "recfile.h"

#include "app/axapp.h"

#include "im/impage.h"


//----------------------------------------------------------------------------
// RecImController
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(RecImController,AxImageController)


BEGIN_EVENT_TABLE(RecImController,AxImageController)
    EVT_MENU_RANGE( ID4_POPUP_MENU_MUSIC, ID4_POPUP_MENU_VOID, RecImController::OnChangeClassification )
END_EVENT_TABLE()

RecImController::RecImController( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style , int flags) :
    AxImageController( parent, id, position, size, style, flags )
{
    m_envPtr = NULL;
    
    m_musControlPtr = NULL;
    m_musViewPtr = NULL;
    
    m_recFilePtr = NULL;
	
	m_magic_selection = false;
}

RecImController::RecImController()
{
}

RecImController::~RecImController()
{
}

void RecImController::SetEnv( RecEnv *env )
{
    m_envPtr = env;
}

void RecImController::SetMusViewAndController( RecMusWindow *recMusWindow, RecMusController *recMusController )
{   
    wxASSERT_MSG( m_viewPtr->IsKindOf( CLASSINFO( RecImWindow ) ) , wxT("View must be a RecImWindow") );

    m_musControlPtr = recMusController;
    m_musViewPtr = recMusWindow;
    
    ((RecImWindow*)m_viewPtr)->SetMusViewAndController( recMusWindow, recMusController );
}


void RecImController::SetRecFile( RecFile *recFile )
{
    wxASSERT_MSG( m_viewPtr->IsKindOf( CLASSINFO( RecImWindow ) ) , wxT("View must be a RecImWindow") );

    m_recFilePtr = recFile;
	((RecImWindow*)m_viewPtr)->SetRecFile( recFile );
}

void RecImController::CloseDraggingSelection(wxPoint start, wxPoint end)
{
    if ( start == end )
    {
        this->m_viewPtr->EndSelection();
        return;
    }       
    
    m_end = end;
    m_start = start;

    /*if (m_selectCounter == 0)
    {
        m_selectCounter++;
    }
    else if (m_selectCounter == 1)
    {
        m_selectCounter++;
    }
    else if (m_selectCounter == 2)
    {
        m_selectCounter = 0;
        this->m_viewPtr->EndSelection();
    }*/
}

void RecImController::LeftDClick( wxPoint point )
{
	wxASSERT_MSG( m_recFilePtr , wxT("RecFile cannot be NULL") );
	wxASSERT_MSG( m_envPtr , wxT("RecEnv cannot be NULL") );
	
	if ( m_recFilePtr->IsRecognized() )
		return;
	
	m_selection_img.Destroy();

	//wxLogMessage("%d %d", point.x, point.y );
	if ( m_recFilePtr->m_imPagePtr->MagicSelection( point.x, point.y, 
		&m_selection_img, &m_selection_img_x, &m_selection_img_y ) )
	{
		//m_selection_point.x = point.x - m_selection_img_x; // point to start flood fill
		//m_selection_point.y = point.y - m_selection_img_y;
		m_magic_selection = true;
		SyncSelectionBitmap();
	}
	else
		m_magic_selection = false;
}


void RecImController::SyncSelectionBitmap( )
{
	wxASSERT_MSG( m_viewPtr , wxT("View cannot be NULL") );

    if ( !m_selection_img.Ok() || !m_magic_selection )
        return;
    
    m_selection_bmp = wxBitmap( m_selection_img ); 
    if ( !m_selection_bmp.Ok() )
        return;
		
	m_selection_bmp_x = m_selection_img_x;
	m_selection_bmp_y = m_selection_img_y;
	//int flood_fill_x = m_selection_point.x;
	//int flood_fill_y = m_selection_point.y;

	//wxMask *mask = new wxMask( m_selection_bmp, *wxBLACK );
    wxMask *mask = new wxMask( m_selection_bmp, *wxWHITE );
    m_selection_bmp.SetMask( mask );

	int zoom = m_viewPtr->GetZoomPermil();
    if ( zoom != 1000 )
    {
        wxImage img = m_selection_bmp.ConvertToImage();
        float w = (float)m_selection_bmp.GetWidth() * zoom;
        w /= 1000;
        float h = (float)m_selection_bmp.GetHeight() * zoom;
        h /= 1000;
        img.Rescale( ceil(w) , ceil(h) );
        m_selection_bmp = wxBitmap( img );
		float y = m_selection_bmp_y * zoom; 
        y /= 1000;
		m_selection_bmp_y = floor(y);
        float x = m_selection_bmp_x * zoom;
        x /= 1000;
		m_selection_bmp_x = floor(x);
		//flood_fill_y *= zoom; 
        //flood_fill_y /= 1000;
        //flood_fill_x *= zoom;
        //flood_fill_x /= 1000;
	}
	
	//wxMemoryDC temp_dc;
	//temp_dc.SelectObject( m_selection_bmp );
	//temp_dc.SetBrush( wxBrush( *wxRED, wxCROSSDIAG_HATCH ) );
	//temp_dc.FloodFill( flood_fill_x, flood_fill_y, *wxWHITE, wxFLOOD_SURFACE );
	//temp_dc.SelectObject( wxNullBitmap );
	
	m_viewPtr->Refresh();
}

void RecImController::GetSelectionBitmap( wxBitmap *bmp, int *x, int *y )
{
    *bmp = m_selection_bmp;
    *x = m_selection_bmp_x;
    *y = m_selection_bmp_y;
}

void RecImController::CancelMagicSelection(  )
{
	wxASSERT_MSG( m_viewPtr, "View Window cannot be NULL ");

    m_magic_selection = false;

    // erase
	m_selection_bmp = wxBitmap();
	m_selection_bmp_x = -1;
	m_selection_bmp_y = -1;
	m_selection_img.Destroy();
	
	m_viewPtr->Refresh();
}


void RecImController::OnChangeClassification( wxCommandEvent &event )
{
	wxASSERT_MSG( m_recFilePtr , wxT("RecFile cannot be NULL") );
	wxASSERT_MSG( m_envPtr , wxT("RecEnv cannot be NULL") );
	
	wxGetApp().AxBeginBusyCursor();
	int plane_number = 0;
	switch ( event.GetId() )
	{
	case ( ID4_POPUP_MENU_MUSIC ): plane_number = IMAGE_MUSIC; break;
	case ( ID4_POPUP_MENU_TITLE ): plane_number = IMAGE_TITLE; break;
	case ( ID4_POPUP_MENU_LYRICS ): plane_number = IMAGE_LYRICS; break;
	case ( ID4_POPUP_MENU_ORNATE_LETTER ): plane_number = IMAGE_ORNATE_LETTER; break;
	case ( ID4_POPUP_MENU_TEXT_IN_STAFF ): plane_number = IMAGE_TEXT_IN_STAFF; break;
	case ( ID4_POPUP_MENU_VOID ): plane_number = IMAGE_BLANK; break;
	}


	if ( m_selection_img.Ok() && m_magic_selection ) // double check
		m_recFilePtr->m_imPagePtr->ChangeClassification( plane_number );
	else
		m_recFilePtr->m_imPagePtr->ChangeClassification( m_start.x, m_start.y, m_end.x,  m_end.y, plane_number);	

	this->CancelMagicSelection();
	this->m_viewPtr->EndSelection();
	((RecEnv*)this->m_envPtr)->UpdateViews( REC_UPDATE_FAST );
	wxGetApp().AxEndBusyCursor();
}

//----------------------------------------------------------------------------
// RecImWindow
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(RecImWindow, AxScrolledWindow)


BEGIN_EVENT_TABLE(RecImWindow,AxScrolledWindow)
    EVT_MOUSE_EVENTS( RecImWindow::OnMouse )
    EVT_SCROLLWIN(RecImWindow::OnScroll )
    EVT_PAINT( RecImWindow::OnPaint )
END_EVENT_TABLE()

RecImWindow::RecImWindow( AxImageController *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    AxScrolledWindow( parent, id, position, size, style )
{
    m_envPtr = NULL;
		
	m_recFilePtr = NULL;
    
    wxMenuItem *item;
    item = m_popupMenuClassification.Append( ID4_POPUP_MENU_MUSIC, _("In staff music symbols") );
    item->SetBitmap( BitmapsFuncClassification( 0 ) );
    item = m_popupMenuClassification.Append( ID4_POPUP_MENU_TITLE, _("Title elements") );
    item->SetBitmap( BitmapsFuncClassification( 1 ) );
    item = m_popupMenuClassification.Append( ID4_POPUP_MENU_LYRICS, _("Lyrics or inter staves music symbols") );
    item->SetBitmap( BitmapsFuncClassification( 2 ) );
    item = m_popupMenuClassification.Append( ID4_POPUP_MENU_ORNATE_LETTER, _("Ornate letter") );
    item->SetBitmap( BitmapsFuncClassification( 3 ) );
    item = m_popupMenuClassification.Append( ID4_POPUP_MENU_TEXT_IN_STAFF, _("In staff text elements") );
    item->SetBitmap( BitmapsFuncClassification( 4 ) );
    item = m_popupMenuClassification.Append( ID4_POPUP_MENU_VOID, _("Blank") );
    item->SetBitmap( BitmapsFuncClassification( 5 ) );
}

RecImWindow::RecImWindow()
{
}

RecImWindow::~RecImWindow()
{
}

void RecImWindow::SetEnv( RecEnv *env )
{
    m_envPtr = env;
}

void RecImWindow::SetMusViewAndController( RecMusWindow *recMusWindow, RecMusController *recMusController )
{
    m_musControlPtr = recMusController;
    m_musViewPtr = recMusWindow;
}

void RecImWindow::SetRecFile( RecFile *recFile )
{
	m_recFilePtr = recFile;
}

void RecImWindow::OnScroll( wxScrollWinEvent &event )
{
    if ( m_selection )
        this->EndSelection();
        
    //event
    event.Skip();

    int scrollX, scrollY;
    this->GetViewStart(&scrollX,&scrollY);

    /*int unitX, unitY;
    this->GetScrollPixelsPerUnit(&unitX,&unitY);
    scrollX *= unitX;
    scrollY *= unitY;*/

    if ( event.GetOrientation() == wxHORIZONTAL )
        scrollX += CalcScrollInc(event);
    else
        scrollY += CalcScrollInc(event);
    
    if ( m_musViewPtr )
        m_musViewPtr->Scroll( scrollX, scrollY);
}


void RecImWindow::OnPaint( wxPaintEvent &event )
{
    wxASSERT_MSG( m_recFilePtr, wxT("RecFile cannot be NULL") );
    wxASSERT_MSG( m_imControlPtr,"Image controller cannot be NULL" );
    wxClassInfo *info;
    info = m_imControlPtr->GetClassInfo();
    wxASSERT_MSG( info,"Class info cannot be NULL" );
    wxASSERT_MSG( info->IsKindOf( CLASSINFO( RecImController ) ), "Controller must be a RecImController");
    RecImController *controller = (RecImController*)m_imControlPtr;
    wxASSERT_MSG( m_musControlPtr, wxT("WG controller cannot be NULL") );

    wxPaintDC dc( this );
    PrepareDC( dc );

    if (m_bitmap)
	{ 
 		dc.DrawBitmap(*m_bitmap, 0, 0);

		wxBitmap bmp;
		int x, y;
    
		if ( m_recFilePtr->IsRecognized() ) // get staff bitmap (if exists)
			m_musControlPtr->GetStaffBitmap( &bmp, &x, &y );
		else // get selection bitmap (if exists)
			controller->GetSelectionBitmap( &bmp, &x, &y );
		
		if ( !bmp.Ok() )
			return;

		//wxMemoryDC symDC;
		//symDC.SetBackgroundMode( wxTRANSPARENT );
		dc.SetBackgroundMode( wxTRANSPARENT );
		dc.DrawBitmap( bmp, x, y, true );
	}
}

void RecImWindow::OnMouse(wxMouseEvent &event)
{
    wxASSERT_MSG( m_recFilePtr, wxT("RecFile cannot be NULL") );
    wxASSERT_MSG( m_imControlPtr,"Image controller cannot be NULL" );
    wxClassInfo *info;
    info = m_imControlPtr->GetClassInfo();
    wxASSERT_MSG( info,"Class info cannot be NULL" );
    wxASSERT_MSG( info->IsKindOf( CLASSINFO( RecImController ) ), "Controller must be a RecImController");
    RecImController *controller = (RecImController*)m_imControlPtr;

    if (event.GetEventType() == wxEVT_MOUSEWHEEL)
    {
		if (!m_bitmap)
            return;
	
        if ( m_selection )
            return;  
            
        int x, y;
        this->GetViewStart( &x, &y );
        int rot = 0;
        if ( event.m_wheelDelta )
            rot = event.m_wheelRotation / event.m_wheelDelta * 6;

        if (!m_shiftDown)
            y -= rot;
        else
            x -= rot;

        x = max( 0, x );
        y = max( 0, y );
        
        event.Skip();
        if ( m_musViewPtr )
            m_musViewPtr->Scroll( x, y);
    }
    //else if ( event.GetEventType() == wxEVT_LEAVE_WINDOW)
    //  return; // don't skip event
    //else if ( event.GetEventType() == wxEVT_ENTER_WINDOW)
    //  return; // don't skip event
    else if ( event.GetEventType() == wxEVT_LEFT_DOWN )
    {
		if (!m_bitmap)
            return;
			
		if ( m_recFilePtr->IsRecognized() )
			return;
			
		controller->CancelMagicSelection( );
        this->EndSelection( );
        this->BeginSelection( SHAPE_RECT );
        event.Skip();
    }
    else if ( event.GetEventType() == wxEVT_RIGHT_DOWN )
    {
        if ( m_selection || controller->m_magic_selection )
            PopupMenu(&m_popupMenuClassification, event.GetPosition()); 
        else
            event.Skip();   
    }
    else
        event.Skip();
}


#endif // AX_RECOGNITION

