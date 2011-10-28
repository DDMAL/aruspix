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

#ifdef __GNUG__
    #pragma implementation "cmpmus.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "cmpmus.h"
#include "cmp.h"
#include "cmpim.h"
#include "recognition/recfile.h"

#include "app/axapp.h"

#include "im/impage.h"
#include "im/imstaff.h"

#include "mus/musstaff.h"


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
	
	m_imControlPtr1 = NULL;
	m_imViewPtr1 = NULL;
	m_imControlPtr2 = NULL;
	m_imViewPtr2 = NULL;
	
	m_cmpFilePtr = NULL;
}

CmpMusController::CmpMusController()
{
}

void CmpMusController::Init( CmpEnv *env, CmpMusWindow *window )
{
	// environement
    wxASSERT_MSG(env,"Environment cannot be NULL");
    m_envPtr = env;

	// view
    wxASSERT_MSG(window,"View cannot be NULL");
    m_viewPtr = window;
}


void CmpMusController::SetImViewAndController( CmpImWindow *cmpImWindow1, CmpImController *cmpImController1,
		CmpImWindow *cmpImWindow2, CmpImController *cmpImController2 )
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
	m_lastController = 1;
}

CmpMusWindow::CmpMusWindow()
{
}

CmpMusWindow::~CmpMusWindow()
{
}

void CmpMusWindow::SetEnv( CmpEnv *env )
{
    m_envPtr = env;
}

void CmpMusWindow::SetImViewAndController( CmpImWindow *cmpImWindow1, CmpImController *cmpImController1,
		CmpImWindow *cmpImWindow2, CmpImController *cmpImController2 )
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

/*
void CmpMusWindow::OnBeginEditionClef()
{
    if ( !m_envPtr )
        return;

    m_edition = true;
    if ( m_currentStaff )
        MusMLFOutput::GetUt1( m_currentStaff, true );
}

void CmpMusWindow::OnEndEditionClef()
{
    if ( !m_envPtr )
        return;

    if ( m_edition && m_currentStaff )
        MusMLFInput::GetNotUt1( m_currentStaff, true );

    m_edition = false;
}


void CmpMusWindow::OnEndEdition()
{
	m_recFilePtr->Modify();
    m_musControlPtr->SyncStaffBitmap();
}
*/


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
    else if (event.GetEventType() ==  wxEVT_LEFT_DCLICK)
	{
		if ( m_currentStaff && m_currentElement )
		{
			if ( m_currentStaff->no != m_lastStaff )
				m_lastController = (m_lastController == 1) ? 0 : 1; // swap controller
			m_lastStaff = m_currentStaff->no;
			CmpImController *controller = (m_lastController == 0) ? m_imControlPtr1 : m_imControlPtr2;
			if ( controller )
				controller->LoadRecImage( m_lastStaff, m_currentElement->m_im_filename, m_currentElement->m_im_staff, 
					m_currentElement->m_im_pos, m_currentElement->m_cmp_flag );
		
			//wxLogMessage(" %s, %d", m_currentElement->m_im_filename.c_str(), m_currentElement->m_im_staff );
		}
		event.Skip();
	}
	else
        event.Skip();
}

void CmpMusWindow::OnSyncScroll( int x, int y )
{
	//if ( m_imViewPtr )
	//	m_imViewPtr->Scroll( x, y );
}

	#endif // AX_COMPARISON
#endif // AX_RECOGNITION

