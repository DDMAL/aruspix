/////////////////////////////////////////////////////////////////////////////
// Name:        recmus.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_RECOGNITION

#include <algorithm>
using std::min;
using std::max;

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "recmus.h"
#include "rec.h"
#include "recim.h"
#include "recfile.h"
#include "recmodels.h"
#include "recmlfbmp.h"

#include "app/axapp.h"

#include "mus/musdoc.h"
#include "mus/mustoolpanel.h"
//#include "mus/musstaff.h"
#include "mus/muspage.h"
#include "mus/muslayerelement.h"

#include "im/impage.h"
#include "im/imstaff.h"


//----------------------------------------------------------------------------
// RecMusController
//----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(RecMusController,wxPanel)
    EVT_SIZE( RecMusController::OnSize )
END_EVENT_TABLE()

RecMusController::RecMusController( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
    //MusOutputFunc4( this, TRUE );	
	m_envPtr = NULL;
	m_viewPtr = NULL;
	
	m_imControlPtr = NULL;
	m_imViewPtr = NULL;
	
	m_recFilePtr = NULL;
	
    m_mlfBmp = NULL;
    m_staffbmp_show = false;	
}

RecMusController::RecMusController()
{
    if ( m_mlfBmp )
        delete m_mlfBmp;
}

void RecMusController::Init( RecEnv *env, RecMusWindow *window )
{
	// environement
    wxASSERT_MSG(env,"Environment cannot be NULL");
    m_envPtr = env;

	// view
    wxASSERT_MSG(window,"View cannot be NULL");
    m_viewPtr = window;
}

void RecMusController::SetForMEI()
{
	m_staffbmp_show = true;
}

void RecMusController::SetImViewAndController( RecImWindow *recImWindow, RecImController *recImController )
{
	m_imControlPtr = recImController;
	m_imViewPtr = recImWindow;
	m_viewPtr->SetImViewAndController( recImWindow, recImController );
}



void RecMusController::SetRecFile( RecFile *recFile )
{
	m_recFilePtr = recFile;
	m_viewPtr->SetRecFile( recFile );
}


void RecMusController::LoadBitmapsForFont( )
{

    if ( m_mlfBmp )
    {
		delete m_mlfBmp;
        m_mlfBmp = NULL;
	}

	wxString builtin_font = wxGetApp().m_resourcesPath + "/fnt/types.xml";
	wxString tmp = wxGetApp().m_workingDir + "/.bitmaps"; // ouvre un descripteur de fichier inutile,
														  // pas terrible .....
	
	MusDoc mustmp;																									
	m_mlfBmp = new RecMLFBmp( &mustmp, tmp );
	m_mlfBmp->m_bitmap_types.Load( builtin_font );
}


void RecMusController::SyncZoom()
{
	wxASSERT_MSG( m_viewPtr, "WG Window cannot be NULL ");
	wxASSERT_MSG( m_imViewPtr, "RecImWindow cannot be NULL ");

	m_viewPtr->SetZoom( m_imViewPtr->GetZoomPercent() );
	SyncStaffBitmap();
}

void RecMusController::SyncStaffBitmap( )
{

	wxASSERT_MSG( m_viewPtr, "WG Window cannot be NULL ");
	wxASSERT_MSG( m_imViewPtr, "RecImWindow cannot be NULL ");
	wxASSERT_MSG( m_recFilePtr, "RecFile cannot be NULL ");
    

    if ( !m_staffbmp_show )
        return;

    //if ( !m_recFilePtr || !m_recFilePtr->GetImPage() || !m_recFilePtr->GetMusFile() || !m_recFilePtr->GetTypModel() || !m_recFilePtr->GetTypModel()->m_isLoaded )
	if ( !m_recFilePtr->GetImPage() || !m_recFilePtr->GetMusFile() )
        return;

    if ( !m_viewPtr->m_currentStaff || !m_viewPtr->m_currentLayer )
        return;

    int staff_pos = m_viewPtr->m_page->GetStaffPosOnPage( m_viewPtr->m_currentStaff );
    if ( (staff_pos < 0) || (staff_pos > (int)m_recFilePtr->GetImPage()->m_staves.GetCount() - 1) )
        return;

    ImStaff *imstaff = &m_recFilePtr->GetImPage()->m_staves[staff_pos];

    int currentElementNo = -1;
    if ( m_viewPtr->m_currentElement )
        currentElementNo = m_viewPtr->m_currentElement->GetElementNo();
    
    m_staffbmp = wxBitmap(); // clear bitmap;
    m_staffbmp = m_mlfBmp->GenerateBitmap( imstaff, m_viewPtr->m_currentLayer, currentElementNo );
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

    //wxLogDebug( "SyncStaffBitmap method missing in ax2") ;
}

void RecMusController::GetStaffBitmap( wxBitmap *bmp, int *x, int *y )
{
    *bmp = m_staffbmp;
    *x = m_staffbmp_x;
    *y = m_staffbmp_y;
}

void RecMusController::CancelShowStaffBitmap( )
{
    m_staffbmp_show = true; // force, but will be inversed
    InverseShowStaffBitmap( );
}

void RecMusController::InverseShowStaffBitmap( )
{
	wxASSERT_MSG( m_viewPtr, "WG Window cannot be NULL ");

    m_staffbmp_show = !m_staffbmp_show;

    if ( m_staffbmp_show )
	{
        
		m_viewPtr->SetEditorMode( MUS_EDITOR_EDIT );
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



void RecMusController::OnSize( wxSizeEvent &event )
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
// RecMusWindow
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(RecMusWindow, MusWindow)


BEGIN_EVENT_TABLE(RecMusWindow,MusWindow)
    EVT_MOUSE_EVENTS( RecMusWindow::OnMouse )
    EVT_SCROLLWIN(RecMusWindow::OnScroll )
    EVT_KEY_DOWN( RecMusWindow::OnKeyDown )
    EVT_KEY_UP( RecMusWindow::OnKeyUp )
    EVT_SIZE( RecMusWindow::OnSize )
END_EVENT_TABLE()

RecMusWindow::RecMusWindow( RecMusController *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style, bool center ) :
    MusWindow( parent, id, position, size, style, center )
{
	m_musControlPtr = parent;
    m_shiftDown = false;
    m_envPtr = NULL;
    m_edition = false;
	
	m_imControlPtr = NULL;
	m_imViewPtr = NULL;
}

RecMusWindow::RecMusWindow()
{
}

RecMusWindow::~RecMusWindow()
{
}

void RecMusWindow::SetEnv( RecEnv *env )
{
    m_envPtr = env;
}

void RecMusWindow::SetImViewAndController( RecImWindow *recImWindow, RecImController *recImController )
{
	m_imControlPtr = recImController;
	m_imViewPtr = recImWindow;
}

void RecMusWindow::SetRecFile( RecFile *recFile )
{
	m_recFilePtr = recFile;
}

void RecMusWindow::OnBeginEditionClef()
{
    if ( !m_envPtr )
        return;

    m_edition = true;
    if ( m_currentLayer )
        MusMLFOutput::GetUt1( m_currentLayer );
}

void RecMusWindow::OnEndEditionClef()
{
    if ( !m_envPtr )
        return;

    if ( m_edition && m_currentLayer )
        MusMLFInput::GetNotUt1( m_currentLayer );

    m_edition = false;
}


void RecMusWindow::OnEndEdition()
{
	m_recFilePtr->Modify();
    m_musControlPtr->SyncStaffBitmap();
}



void RecMusWindow::OnSize( wxSizeEvent &event )
{
	m_musControlPtr->SyncZoom();
    event.Skip();
}

void RecMusWindow::OnKeyUp( wxKeyEvent &event )
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

void RecMusWindow::OnKeyDown( wxKeyEvent &event )
{
    if (event.GetKeyCode() == WXK_SHIFT) 
        m_shiftDown = true;
    if ( event.m_controlDown && (event.GetKeyCode() == 'B') )
    {
        m_musControlPtr->InverseShowStaffBitmap( );
        return;  // do not skip event;  
    }
    event.Skip();
}

void RecMusWindow::OnScroll( wxScrollWinEvent &event )
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

void RecMusWindow::OnMouse(wxMouseEvent &event)
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

void RecMusWindow::OnSyncScroll( int x, int y )
{
	if ( m_imViewPtr )
		m_imViewPtr->Scroll( x, y );
}

#endif // AX_RECOGNITION

