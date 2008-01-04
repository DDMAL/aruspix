/////////////////////////////////////////////////////////////////////////////
// Name:        recwg.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_RECOGNITION

#ifdef __GNUG__
    #pragma implementation "recwg.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "recwg.h"
#include "rec.h"
#include "recim.h"
#include "recfile.h"
#include "models.h"

#include "app/axapp.h"

#include "wg/mlfbmp.h"
#include "wg/wgtoolpanel.h"

#include "im/impage.h"
#include "im/imstaff.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// RecWgController
//----------------------------------------------------------------------------

// WDR: event table for RecWgController

BEGIN_EVENT_TABLE(RecWgController,wxPanel)
    EVT_SIZE( RecWgController::OnSize )
END_EVENT_TABLE()

RecWgController::RecWgController( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
    //WgOutputFunc4( this, TRUE );	
	m_envPtr = NULL;
	m_viewPtr = NULL;
	
	m_imControlPtr = NULL;
	m_imViewPtr = NULL;
	
	m_recFilePtr = NULL;
	
    m_mlfBmp = NULL;
    m_staffbmp_show = false;	
}

RecWgController::RecWgController()
{
    if ( m_mlfBmp )
        delete m_mlfBmp;
}

void RecWgController::Init( RecEnv *env, RecWgWindow *window )
{
	// environement
    wxASSERT_MSG(env,"Environment cannot be NULL");
    m_envPtr = env;

	// view
    wxASSERT_MSG(window,"View cannot be NULL");
    m_viewPtr = window;
}

void RecWgController::SetImViewAndController( RecImWindow *recImWindow, RecImController *recImController )
{
	m_imControlPtr = recImController;
	m_imViewPtr = recImWindow;
	m_viewPtr->SetImViewAndController( recImWindow, recImController );
}



void RecWgController::SetRecFile( RecFile *recFile )
{
	m_recFilePtr = recFile;
	m_viewPtr->SetRecFile( recFile );
}


void RecWgController::LoadBitmapsForFont( )
{

    if ( m_mlfBmp )
    {
		delete m_mlfBmp;
        m_mlfBmp = NULL;
	}

	wxString builtin_font = wxGetApp().m_resourcesPath + "/fnt/types.xml";
	wxString tmp = wxGetApp().m_workingDir + "/.bitmaps"; // ouvre un descripteur de fichier inutile,
														  // pas terrible .....
	
	WgFile wgtmp;																									
	m_mlfBmp = new MLFBitmaps( &wgtmp, tmp );
	m_mlfBmp->m_bitmap_types.Load( builtin_font );
}


void RecWgController::SyncZoom()
{
	wxASSERT_MSG( m_viewPtr, "WG Window cannot be NULL ");
	wxASSERT_MSG( m_imViewPtr, "RecImWindow cannot be NULL ");

	m_viewPtr->SetZoom( m_imViewPtr->GetZoomPercent() );
	SyncStaffBitmap();
}

void RecWgController::SyncStaffBitmap( )
{
	wxASSERT_MSG( m_viewPtr, "WG Window cannot be NULL ");
	wxASSERT_MSG( m_imViewPtr, "RecImWindow cannot be NULL ");
	wxASSERT_MSG( m_recFilePtr, "RecFile cannot be NULL ");

    if ( !m_staffbmp_show )
        return;

    //if ( !m_recFilePtr || !m_recFilePtr->GetImPage() || !m_recFilePtr->GetWgFile() || !m_recFilePtr->GetTypModel() || !m_recFilePtr->GetTypModel()->m_isLoaded )
	if ( !m_recFilePtr->GetImPage() || !m_recFilePtr->GetWgFile() )
        return;

    if ( !m_viewPtr->m_currentStaff )
        return;

    if ( m_viewPtr->m_currentStaff->no > m_recFilePtr->GetImPage()->m_staves.GetCount() - 1 )
        return;

    ImStaff *imstaff = &m_recFilePtr->GetImPage()->m_staves[m_viewPtr->m_currentStaff->no];

    int currentElementNo = -1;
    if ( m_viewPtr->m_currentElement )
        currentElementNo = m_viewPtr->m_currentElement->no;
    
    m_staffbmp = wxBitmap(); // clear bitmap;
    m_staffbmp = m_mlfBmp->GenerateBitmap( imstaff, m_viewPtr->m_currentStaff, currentElementNo );
    if ( !m_staffbmp.Ok() )
        return;

    wxMask *mask = new wxMask( m_staffbmp, *wxWHITE );
    m_staffbmp.SetMask( mask );
    int old_y = m_staffbmp_y;
    m_staffbmp_y = m_recFilePtr->GetImPage()->ToViewY(imstaff->m_y + STAFF_HEIGHT / 2);
    int x1;
    imstaff->GetMinMax( &m_staffbmp_x, &x1 );

    if ( m_imViewPtr )
    {
        int zoom = m_imViewPtr->GetZoomPermil();
        if ( zoom != 100 )
        {
            wxImage img = m_staffbmp.ConvertToImage();
            int w = m_staffbmp.GetWidth() * zoom;
            w /= 1000;
            int h = m_staffbmp.GetHeight() * zoom;
            h /= 1000;
            img.Rescale( w, h );
            m_staffbmp = wxBitmap( img );
            m_staffbmp_y *= zoom; 
            m_staffbmp_y /= 1000;
            m_staffbmp_x *= zoom;
            m_staffbmp_x /= 1000;
        }
        if ( old_y != m_staffbmp_y ) // staff changed - redraw all page;
            m_imViewPtr->Refresh();
        else if ( m_mlfBmp->GetCurrentX() != -1 ) // same staff with current element - draw last and current only
        {
            int xx, yy;
            m_imViewPtr->CalcScrolledPosition( m_staffbmp_x, m_staffbmp_y, &xx, &yy );

            // first redraw last current element
            if ( (m_lastX != m_mlfBmp->GetCurrentX()) || (m_lastWidth != m_mlfBmp->GetCurrentWidth() ) )
            {
                int cx = m_lastX * zoom;
                cx /= 1000;
                int cw = m_lastWidth * zoom;
                cw /= 1000;
                m_imViewPtr->RefreshRect( wxRect( xx + cx, yy,
                    cw, m_staffbmp.GetHeight() ) );
            }

            // redraw current element
            int cx = m_mlfBmp->GetCurrentX() * zoom;
            cx /= 1000;
            int cw = m_mlfBmp->GetCurrentWidth() * zoom;
            cw /= 1000;
            m_imViewPtr->RefreshRect( wxRect( xx + cx, yy,
                cw, m_staffbmp.GetHeight() ) );

            //m_imViewPtr->RefreshRect( wxRect( xx, yy,
            //  m_staffbmp.GetWidth(), m_staffbmp.GetHeight() ) );
        }
        else // same staff but no current element - draw all staff
        {
            int xx, yy;
            m_imViewPtr->CalcScrolledPosition( m_staffbmp_x, m_staffbmp_y, &xx, &yy );
            m_imViewPtr->RefreshRect( wxRect( xx, yy,
                m_staffbmp.GetWidth(), m_staffbmp.GetHeight() ) );
        }
        
        // keep current x and width for next time
        m_lastX = m_mlfBmp->GetCurrentX();
        m_lastWidth = m_mlfBmp->GetCurrentWidth();
    }
}

void RecWgController::GetStaffBitmap( wxBitmap *bmp, int *x, int *y )
{
    *bmp = m_staffbmp;
    *x = m_staffbmp_x;
    *y = m_staffbmp_y;
}

void RecWgController::CancelShowStaffBitmap( )
{
    m_staffbmp_show = true; // force, but will be inversed
    InverseShowStaffBitmap( );
}

void RecWgController::InverseShowStaffBitmap( )
{
	wxASSERT_MSG( m_viewPtr, "WG Window cannot be NULL ");

    m_staffbmp_show = !m_staffbmp_show;

    if ( m_staffbmp_show )
	{
		m_viewPtr->SetInsertMode( false );
        SyncStaffBitmap();
	}
    else // erase
    {
        m_staffbmp = wxBitmap();
        m_lastX = -1;
        m_lastWidth = -1;
        m_staffbmp_x = -1;
        m_staffbmp_y = -1;
        if ( m_imViewPtr )
            m_imViewPtr->Refresh();
    }
}


// WDR: handler implementations for RecWgController

void RecWgController::OnSize( wxSizeEvent &event )
{
    this->Layout();
    /*wxWindow *w = FindWindowById( ID4_WGWINDOW );
    if ( w && w->IsKindOf( CLASSINFO( WgWindow ) ))
    {
        ((WgWindow*)w)->Resize( );
    }*/
	if ( m_viewPtr )
		m_viewPtr->Resize();
    event.Skip();
}



//----------------------------------------------------------------------------
// RecWgWindow
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(RecWgWindow, WgWindow)

// WDR: event table for RecWgWindow

BEGIN_EVENT_TABLE(RecWgWindow,WgWindow)
    EVT_MOUSE_EVENTS( RecWgWindow::OnMouse )
    EVT_SCROLLWIN(RecWgWindow::OnScroll )
    EVT_KEY_DOWN( RecWgWindow::OnKeyDown )
    EVT_KEY_UP( RecWgWindow::OnKeyUp )
    EVT_SIZE( RecWgWindow::OnSize )
END_EVENT_TABLE()

RecWgWindow::RecWgWindow( RecWgController *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style, bool center ) :
    WgWindow( parent, id, position, size, style, center )
{
	m_wgControlPtr = parent;
    m_shiftDown = false;
    m_envPtr = NULL;
    m_edition = false;
	
	m_imControlPtr = NULL;
	m_imViewPtr = NULL;
}

RecWgWindow::RecWgWindow()
{
}

RecWgWindow::~RecWgWindow()
{
}

void RecWgWindow::SetEnv( RecEnv *env )
{
    m_envPtr = env;
}

void RecWgWindow::SetImViewAndController( RecImWindow *recImWindow, RecImController *recImController )
{
	m_imControlPtr = recImController;
	m_imViewPtr = recImWindow;
}

void RecWgWindow::SetRecFile( RecFile *recFile )
{
	m_recFilePtr = recFile;
}

void RecWgWindow::OnBeginEditionClef()
{
    if ( !m_envPtr )
        return;

    m_edition = true;
    if ( m_currentStaff )
        MLFOutput::GetUt1( m_currentStaff, true );
}

void RecWgWindow::OnEndEditionClef()
{
    if ( !m_envPtr )
        return;

    if ( m_edition && m_currentStaff )
        MLFInput::GetNotUt1( m_currentStaff, true );

    m_edition = false;
}


void RecWgWindow::OnEndEdition()
{
	m_recFilePtr->Modify();
    m_wgControlPtr->SyncStaffBitmap();
}

// WDR: handler implementations for RecWgWindow


void RecWgWindow::OnSize( wxSizeEvent &event )
{
	m_wgControlPtr->SyncZoom();
    event.Skip();
}

void RecWgWindow::OnKeyUp( wxKeyEvent &event )
{
    if (event.GetKeyCode() == WXK_SHIFT) 
        m_shiftDown = false;
    //if ( (event.GetKeyCode() == 'S') && m_envPtr)
    //{
    //  m_envPtr->SetShowStaffBitmap( );
    //  return;  // do not skip event;  
    //}
    event.Skip();
}

void RecWgWindow::OnKeyDown( wxKeyEvent &event )
{
    if (event.GetKeyCode() == WXK_SHIFT) 
        m_shiftDown = true;
    if ( event.m_controlDown && (event.GetKeyCode() == 'B') )
    {
        m_wgControlPtr->InverseShowStaffBitmap( );
        return;  // do not skip event;  
    }
    event.Skip();
}

void RecWgWindow::OnScroll( wxScrollWinEvent &event )
{
    //this->SynchronizeScroll();

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
        
    OnSyncScroll( scrollX, scrollY );
}

void RecWgWindow::OnMouse(wxMouseEvent &event)
{
    if (event.GetEventType() == wxEVT_MOUSEWHEEL)
    {
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
        
        this->Scroll( x, y );
		OnSyncScroll( x, y );        
    }
    else
        event.Skip();
}

void RecWgWindow::OnSyncScroll( int x, int y )
{
	if ( m_imViewPtr )
		m_imViewPtr->Scroll( x, y );
}

#endif // AX_RECOGNITION

