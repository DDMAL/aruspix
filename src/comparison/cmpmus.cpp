/////////////////////////////////////////////////////////////////////////////
// Name:        cmpmus.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_RECOGNITION
	#ifdef AX_COMPARISON

#include <algorithm>
using std::max;

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "cmpmus.h"
#include "cmp.h"
#include "cmpfile.h"
#include "recognition/recfile.h"

#include "app/axapp.h"

#include "im/impage.h"
#include "im/imstaff.h"

#include "mus/muswxdc.h"
#include "mus/musdoc.h"
#include "mus/musstaff.h"
#include "mus/muslayerelement.h"


//----------------------------------------------------------------------------
// CmpMusController
//----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(CmpMusController,wxPanel)
    EVT_SIZE( CmpMusController::OnSize )
END_EVENT_TABLE()

CmpMusController::CmpMusController( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
    //MusOutputFunc4( this, TRUE );	
	m_envPtr = NULL;
	m_viewPtr = NULL;
    m_collationCtrl = false;
	
	m_imControlPtr1 = NULL;
	m_imViewPtr1 = NULL;
	m_imControlPtr2 = NULL;
	m_imViewPtr2 = NULL;
	
	m_cmpFilePtr = NULL;
}

CmpMusController::CmpMusController()
{
}

void CmpMusController::Init( CmpEnv *env, CmpMusWindow *window, bool collationCtrl )
{
	// environement
    wxASSERT_MSG(env,"Environment cannot be NULL");
    m_envPtr = env;

	// view
    wxASSERT_MSG(window,"View cannot be NULL");
    m_viewPtr = window;
    m_collationCtrl = collationCtrl;
}


void CmpMusController::SetImViewAndController( CmpMusWindow *cmpImWindow1, CmpMusController *cmpImController1,
		CmpMusWindow *cmpImWindow2, CmpMusController *cmpImController2 )
{
	m_imControlPtr1 = cmpImController1;
	m_imViewPtr1 = cmpImWindow1;
	m_imControlPtr2 = cmpImController2;
	m_imViewPtr2 = cmpImWindow2;
	m_viewPtr->SetImViewAndController( cmpImWindow1, cmpImController1, cmpImWindow2, cmpImController2 );
}


void CmpMusController::SetCmpFile( CmpFile *cmpFile )
{
	m_cmpFilePtr = cmpFile;
	m_viewPtr->SetCmpFile( cmpFile );
}


void CmpMusController::LoadSources()
{
    if ( !m_collationCtrl || !m_viewPtr->m_currentElement ) {
        // we are not in a collationCtrl or no element is selected
        return;
    }
    
    m_imControlPtr1->LoadSource( m_viewPtr->m_currentElement );
    m_imControlPtr2->LoadSource( m_viewPtr->m_currentElement );
}


void CmpMusController::LoadSource( MusLayerElement *element )
{
    if ( m_collationCtrl ) {
        // this should never happen because we do not load the source from the collation controller
        return;
    }
    
    if ( !m_viewPtr->m_page ) {
        // this happen when we load a source when clicking during delimiter selection
        return;
    }
    
    wxString id, filename;
    int i = 0;
    while ( element->GetSameAs( &id, &filename, i) ) {
        if ( filename == m_viewPtr->m_doc->m_fname ) {
            break;
        }
        i++;
    }
    
    if ( filename.IsEmpty() ) {
        // we did not find a sameAs with our filename
        return;
    }
    
    MusPage *currentPage = m_viewPtr->m_page;    
    
    MusObject *viewElement = NULL;
    wxArrayPtrVoid params;
    uuid_t uuid;
    uuid_parse( id.c_str(), uuid );
	params.Add( uuid );
    params.Add( &viewElement );
    MusFunctor findLayerElement( &MusObject::FindByUuid );
    m_viewPtr->m_doc->Process( &findLayerElement, params );
    
    MusLayerElement *layerElement;
    if ( !viewElement ) { // || !(layerElement == dynamic_cast<MusLayerElement*>(viewElement)) ) {
        return; // we did not find it or it is not a MusLayerElement
    }
    
    MusPage *page = dynamic_cast<MusPage*>(viewElement->GetFirstParent( &typeid(MusPage) ));
    if ( page != currentPage ) {
        //wxLogMessage( "load page" );
        m_viewPtr->SetPage( page );
    }
    m_viewPtr->m_currentSystem = dynamic_cast<MusSystem*>(viewElement->GetFirstParent( &typeid(MusSystem) ));
    m_viewPtr->m_currentStaff = dynamic_cast<MusStaff*>(viewElement->GetFirstParent( &typeid(MusStaff) ));
    m_viewPtr->m_currentLayer = dynamic_cast<MusLayer*>(viewElement->GetFirstParent( &typeid(MusLayer) ));
    m_viewPtr->m_currentElement = dynamic_cast<MusLayerElement*>(viewElement);
    m_viewPtr->UpdateCmpScroll();
    m_viewPtr->Refresh();
}



void CmpMusController::OnSize( wxSizeEvent &event )
{
    this->Layout();
    /*wxWindow *w = FindWindowById( ID4_MUSWINDOW );
    if ( w && w->IsKindOf( CLASSINFO( MusWindow ) ))
    {
        ((MusWindow*)w)->Resize( );
    }*/
	if ( m_viewPtr )
		m_viewPtr->Resize();
    event.Skip();
}



//----------------------------------------------------------------------------
// CmpMusWindow
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CmpMusWindow, MusWindow)


BEGIN_EVENT_TABLE(CmpMusWindow,MusWindow)
    EVT_MOUSE_EVENTS( CmpMusWindow::OnMouse )
    EVT_SCROLLWIN(CmpMusWindow::OnScroll )
    EVT_KEY_DOWN( CmpMusWindow::OnKeyDown )
    EVT_KEY_UP( CmpMusWindow::OnKeyUp )
    EVT_SIZE( CmpMusWindow::OnSize )
    EVT_PAINT( CmpMusWindow::OnPaint )
END_EVENT_TABLE()

CmpMusWindow::CmpMusWindow( CmpMusController *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style, bool center ) :
    MusWindow( parent, id, position, size, style, center )
{
	m_musControlPtr = parent;
    m_shiftDown = false;
    m_envPtr = NULL;
    //m_edition = false;
	
	m_imControlPtr1 = NULL;
	m_imViewPtr1 = NULL;
	m_imControlPtr2 = NULL;
	m_imViewPtr2 = NULL;
	
	m_lastStaff = -1, 
    m_collationWin = false;
    m_viewImage = true;
}

CmpMusWindow::CmpMusWindow()
{
}

CmpMusWindow::~CmpMusWindow()
{
}

void CmpMusWindow::SetEnv( CmpEnv *env, bool collationWin )
{
    m_envPtr = env;
    m_collationWin = collationWin;
}

void CmpMusWindow::SetImViewAndController( CmpMusWindow *cmpImWindow1, CmpMusController *cmpImController1,
		CmpMusWindow *cmpImWindow2, CmpMusController *cmpImController2 )
{
	m_imControlPtr1 = cmpImController1;
	m_imViewPtr1 = cmpImWindow1;
	m_imControlPtr2 = cmpImController2;
	m_imViewPtr2 = cmpImWindow2;
}

void CmpMusWindow::SetCmpFile( CmpFile *cmpFile )
{
	m_cmpFilePtr = cmpFile;
}


void CmpMusWindow::OnPageChange( )
{
    if ( !m_page || m_page->m_surface.IsEmpty() ){
        m_backgroundImage.Destroy();
        return;
    }
    
    if ( !m_backgroundImage.LoadFile( m_cmpFilePtr->m_basename + m_page->m_surface )  ) {
        wxLogDebug("Cannot load image '%s'", m_page->m_surface.c_str() );
    }
}


void CmpMusWindow::UpdateCmpScroll()
{
	if (!m_currentStaff)
		return;
    
	int x = 0;
	if ( m_currentElement )
		x = ToRendererX( m_currentElement->m_x_abs );
	int y = ToRendererY(  m_currentStaff->m_y_abs );
    
    x *= (double)m_zoomNum / m_zoomDen;
    y *= (double)m_zoomNum / m_zoomDen;
    
	// units
	int xu, yu;
	this->GetScrollPixelsPerUnit( &xu, &yu );
	// size
	int w, h;
	this->GetClientSize( &w, &h );

    x -= (w / 2);
    y -= (h / 2);
    x /= xu;
    y /= yu;
    
	Scroll( max( x, 0), max( y, 0 ) );
	//OnSyncScroll( x, y );
}

void CmpMusWindow::OnSize( wxSizeEvent &event )
{
	//m_musControlPtr->SyncZoom();
    event.Skip();
}

void CmpMusWindow::OnKeyUp( wxKeyEvent &event )
{
    event.Skip();
}

void CmpMusWindow::OnKeyDown( wxKeyEvent &event )
{
	// navigation with arrows only, but not with crtl down
	if (event.m_controlDown)
		return;
		
	switch ( event.GetKeyCode() )
	{
	case WXK_RIGHT:
	case WXK_LEFT:
	case WXK_UP:
	case WXK_DOWN:
	    event.Skip();
		break;
	}
}

void CmpMusWindow::OnScroll( wxScrollWinEvent &event )
{
    event.Skip();
}

void CmpMusWindow::OnMouse(wxMouseEvent &event)
{
    if ( !m_collationWin ) {
        // for now we do nothing if this is a source window
        return;
    }
    
    // this is the only mouse even we want to process
    if ((event.GetEventType() ==  wxEVT_LEFT_DOWN) || (event.GetEventType() ==  wxEVT_LEFT_UP))
	{
        event.Skip();
        m_musControlPtr->LoadSources();
        return;
    }

}

void CmpMusWindow::OnPaint(wxPaintEvent &event)
{
	if ( !m_page )
		return;
    
	// calculate scroll position
    int scrollX, scrollY;
    this->GetViewStart(&scrollX,&scrollY);
    int unitX, unitY;
    this->GetScrollPixelsPerUnit(&unitX,&unitY);
	wxSize csize = GetClientSize();
    scrollX *= unitX;
    scrollY *= unitY;
    
	wxPaintDC dc( this );
    
	this->PrepareDC( dc );
	dc.SetTextForeground( *wxBLACK );
	dc.SetMapMode( wxMM_TEXT );
	dc.SetAxisOrientation( true, false );
    dc.SetUserScale( double(GetZoom())/100.0,  double(GetZoom())/100.0 );
	
	//m_page->Init( this );
    MusWxDC ax_dc( &dc );
    if ( !m_collationWin && m_viewImage ) {
        ax_dc.SetBackgroundImage( (void*)&m_backgroundImage );
        ax_dc.DrawBackgroundImage();
    }
    else {
        DrawPage( &ax_dc, m_page );
    }
}


void CmpMusWindow::OnSyncScroll( int x, int y )
{
	if ( m_collationWin ) {
        m_musControlPtr->LoadSources();
    }
}

	#endif // AX_COMPARISON
#endif // AX_RECOGNITION

