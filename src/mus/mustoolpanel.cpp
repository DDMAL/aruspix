/////////////////////////////////////////////////////////////////////////////
// Name:        wgtoolpanel.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "mustoolpanel.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "mustoolpanel.h"
#include "muswindow.h"
#include "musstaff.h"
#include "muselement.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// WgToolRow
//----------------------------------------------------------------------------

// WDR: event table for WgToolRow

WgToolRow::WgToolRow( wxWindow *parent, wxWindowID id, int type ) :
    wxPanel( parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL )
{
	m_type = type;
	switch (type)
	{
	case (WG_TOOLS_NOTES): NotesPanel( this, true, true ); break;
	case (WG_TOOLS_KEYS): KeysPanel( this, true, true ); break;
	case (WG_TOOLS_SIGNS): SignsPanel( this, true, true ); break;
	case (WG_TOOLS_OTHER): SymbolesPanel( this, true, true ); break;
	}

	m_buttons[WG_TOOLS_NOTES] = (wxBitmapButton*) FindWindow( ID_MUS_BT_NOTES );
	m_buttons[WG_TOOLS_KEYS] = (wxBitmapButton*) FindWindow( ID_MUS_BT_KEY );
	m_buttons[WG_TOOLS_SIGNS] = (wxBitmapButton*) FindWindow( ID_MUS_BT_SIGNS );
	m_buttons[WG_TOOLS_OTHER] = (wxBitmapButton*) FindWindow( ID_MUS_BT_SYMBOLES );
	m_buttons[WG_TOOLS_NUMBER] = (wxBitmapButton*) FindWindow( ID_MUS_BT_INSERT );
}


void WgToolRow::UpdateTools( bool edition )
{
	if ( edition == m_previous_edition )
		return;

	//int i;
	if ( edition )
	{ 
		//for(i = 0; i < WG_TOOLS_NUMBER; i++) // only if we want to disable
		//	m_buttons[i]->Disable();
		//m_buttons[m_type]->Enable();
		m_buttons[WG_TOOLS_NUMBER]->SetBitmapLabel( Btn( WG_TOOLS_NUMBER ) );
	}
	else
	{ 
		//for(i = 0; i < WG_TOOLS_NUMBER; i++)
		//	m_buttons[i]->Enable();
		m_buttons[WG_TOOLS_NUMBER]->SetBitmapLabel( Btn_selected( WG_TOOLS_NUMBER ) );
	}
	m_previous_edition = edition;
}

//----------------------------------------------------------------------------
// WgToolPanel
//----------------------------------------------------------------------------

// WDR: event table for WgToolPanel

BEGIN_EVENT_TABLE(WgToolPanel,wxPanel)
	EVT_COMMAND( ID_MUS_BT_INSERT, wxEVT_COMMAND_BUTTON_CLICKED, WgToolPanel::OnChangeMode )
    EVT_COMMAND_RANGE( ID_MUS_BT_NOTES, ID_MUS_BT_SYMBOLES, wxEVT_COMMAND_BUTTON_CLICKED, WgToolPanel::OnChangeTool )
    EVT_COMMAND_RANGE( ID_MUS_BT_N0, ID_MUS_BT_N7, wxEVT_COMMAND_BUTTON_CLICKED, WgToolPanel::OnNote )
    EVT_COMMAND_RANGE( ID_MUS_BT_R0, ID_MUS_BT_CT, wxEVT_COMMAND_BUTTON_CLICKED, WgToolPanel::OnNote )
	EVT_COMMAND_RANGE( ID_MUS_BT_LG_D, ID_MUS_BT_UPDOWN, wxEVT_COMMAND_BUTTON_CLICKED, WgToolPanel::OnNote )
    EVT_COMMAND_RANGE( ID_MUS_BT_G1, ID_MUS_BT_F5, wxEVT_COMMAND_BUTTON_CLICKED, WgToolPanel::OnKey )
    EVT_COMMAND_RANGE( ID_MUS_BT_MTPP, ID_MUS_BT_M2, wxEVT_COMMAND_BUTTON_CLICKED, WgToolPanel::OnSign )
    EVT_COMMAND_RANGE( ID_MUS_BT_PNT, ID_MUS_BT_BAR, wxEVT_COMMAND_BUTTON_CLICKED, WgToolPanel::OnSymbole )
END_EVENT_TABLE()

WgToolPanel::WgToolPanel( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{

	m_rows[WG_TOOLS_NOTES] = new WgToolRow( this, -1, WG_TOOLS_NOTES );
	m_rows[WG_TOOLS_KEYS] = new WgToolRow( this, -1, WG_TOOLS_KEYS );
	m_rows[WG_TOOLS_SIGNS] = new WgToolRow( this, -1, WG_TOOLS_SIGNS );
	m_rows[WG_TOOLS_OTHER] = new WgToolRow( this, -1, WG_TOOLS_OTHER );

	m_rows[WG_TOOLS_NOTES]->Show( true );
	m_rows[WG_TOOLS_KEYS]->Show( false );
	m_rows[WG_TOOLS_SIGNS]->Show( false );
	m_rows[WG_TOOLS_OTHER]->Show( false );

	m_previous_edition = false;
	m_previous_tools = WG_TOOLS_NOTES;

	SetTools( WG_TOOLS_NOTES, true );

	this->Fit();


    m_w = NULL;
}

void WgToolPanel::SetWgWindow( WgWindow *w )
{
    m_w = w;
}

void WgToolPanel::SetTools( int tools, bool edition )
{
	if ( tools != m_previous_tools ) // change tools
	{ 
		m_rows[m_previous_tools]->Show( false );
		m_rows[tools]->UpdateTools( edition );
		m_rows[tools]->Show( true );
		this->Refresh();
	}
	else if ( m_previous_edition != edition )  // just change mode
	{
		m_rows[tools]->UpdateTools( edition );
		this->Refresh();
	}	

	m_previous_tools = tools;
	m_previous_edition = edition;
}

void WgToolPanel::SendEvent( wxKeyEvent kevent )
{
    if (!m_w)
        return;

    kevent.SetId( m_w->GetId() );
    kevent.SetEventObject( m_w );
    m_w->ProcessEvent( kevent );
    m_w->SetFocus();
}

void WgToolPanel::OnChangeTool( wxCommandEvent &event )
{
    if (!m_w)
        return;

    int value = 0;
    switch ( event.GetId() )
    {
    case (ID_MUS_BT_NOTES): value = WG_TOOLS_NOTES; break;
    case (ID_MUS_BT_KEY): value = WG_TOOLS_KEYS; break;
    case (ID_MUS_BT_SIGNS): value = WG_TOOLS_SIGNS; break;
    case (ID_MUS_BT_SYMBOLES): value = WG_TOOLS_OTHER; break;
    }

	m_w->SetInsertMode( true );
	m_w->SetToolType( value );
    m_w->SetFocus();   
}

void WgToolPanel::OnChangeMode( wxCommandEvent &event )
{
    if (!m_w)
        return;

	m_w->SetInsertMode( m_w->m_editElement ); 
	m_w->SetFocus();
}


void WgToolPanel::OnSymbole( wxCommandEvent &event )
{
    int value = '0';
    switch ( event.GetId() )
    {
    case (ID_MUS_BT_DIESE): value = 'D'; break;
    case (ID_MUS_BT_BECAR): value = 'H'; break;
    case (ID_MUS_BT_BEMOL): value = 'B'; break;
    case (ID_MUS_BT_DDIESE): value = 'F'; break;
    case (ID_MUS_BT_DBEMOL): value = 'N'; break;

    case (ID_MUS_BT_PNT): value = 'P'; break;
    case (ID_MUS_BT_BAR): value = '|'; break;
    }
        
    wxKeyEvent kevent;
    kevent.SetEventType( wxEVT_KEY_DOWN );
    kevent.m_keyCode = value;
    SendEvent( kevent );  
}


void WgToolPanel::OnSign( wxCommandEvent &event )
{
    int value = '0';
    switch ( event.GetId() )
    {
    case (ID_MUS_BT_MTPP): value = 'Q'; break;
    case (ID_MUS_BT_MTPDP): value = 'W'; break;
    case (ID_MUS_BT_MTP): value = 'E'; break;
    case (ID_MUS_BT_MTPD): value = 'R'; break;

    case (ID_MUS_BT_MTIP): value = 'A'; break;
    case (ID_MUS_BT_MTIDP): value = 'S'; break;
    case (ID_MUS_BT_MTI): value = 'D'; break;
    case (ID_MUS_BT_MTID): value = 'F'; break;

    case (ID_MUS_BT_MTI2P): value = 'Y'; break;
    case (ID_MUS_BT_MTI2DP): value = 'X'; break;
    case (ID_MUS_BT_MTI2): value = 'C'; break;
    case (ID_MUS_BT_MTI2D): value = 'V'; break;

    case (ID_MUS_BT_M32): value = '1'; break;
    case (ID_MUS_BT_M2): value = '2'; break;
    case (ID_MUS_BT_M3): value = '3'; break;
    }

    wxKeyEvent kevent;
    kevent.SetEventType( wxEVT_KEY_DOWN );
    kevent.m_keyCode = value;
    SendEvent( kevent );       
}

void WgToolPanel::OnKey( wxCommandEvent &event )
{
    int value = '0';
    switch ( event.GetId() )
    {
    case (ID_MUS_BT_G1): value = '1'; break;
    case (ID_MUS_BT_G2): value = '2'; break;
    case (ID_MUS_BT_U1): value = '3'; break;
    case (ID_MUS_BT_U2): value = '4'; break;
    case (ID_MUS_BT_U3): value = '5'; break;
    case (ID_MUS_BT_U4): value = '6'; break;
    case (ID_MUS_BT_U5): value = '7'; break;
    case (ID_MUS_BT_F3): value = '8'; break;
    case (ID_MUS_BT_F4): value = '9'; break;
    case (ID_MUS_BT_F5): value = '0'; break;
    }

    wxKeyEvent kevent;
    kevent.SetEventType( wxEVT_KEY_DOWN );
    kevent.m_keyCode = value;
    SendEvent( kevent );    
}

void WgToolPanel::OnNote( wxCommandEvent &event )
{
	wxKeyEvent kevent;
    kevent.SetEventType( wxEVT_KEY_DOWN );

    int value = WXK_NUMPAD0;
    switch ( event.GetId() )
    {
    case (ID_MUS_BT_N1): value = WXK_NUMPAD1; break;
    case (ID_MUS_BT_N2): value = WXK_NUMPAD2; break;
    case (ID_MUS_BT_N3): value = WXK_NUMPAD3; break;
    case (ID_MUS_BT_N4): value = WXK_NUMPAD4; break;
    case (ID_MUS_BT_N5): value = WXK_NUMPAD5; break;
    case (ID_MUS_BT_N6): value = WXK_NUMPAD6; break;
    case (ID_MUS_BT_N7): value = WXK_NUMPAD7; break;

    case (ID_MUS_BT_R1): value = WXK_NUMPAD1; break;
    case (ID_MUS_BT_R2): value = WXK_NUMPAD2; break;
    case (ID_MUS_BT_R3): value = WXK_NUMPAD3; break;
    case (ID_MUS_BT_R4): value = WXK_NUMPAD4; break;
    case (ID_MUS_BT_R5): value = WXK_NUMPAD5; break;
    case (ID_MUS_BT_R6): value = WXK_NUMPAD6; break;
    case (ID_MUS_BT_CT): value = 'C'; break;
	
	case (ID_MUS_BT_LG_D): 
		{	
			kevent.m_keyCode = WXK_NUMPAD1;
			SendEvent( kevent ); // breve
			value = 'L'; break; // change ligature flag
		}
	case (ID_MUS_BT_LG_U): 
		{	
			kevent.m_keyCode = WXK_NUMPAD2;
			SendEvent( kevent ); // semi-breve
			value = 'L'; break; // change ligature flag
		}
	case (ID_MUS_BT_COLOR): value = 'I'; break; // coloration
	case (ID_MUS_BT_UPDOWN): value = 'A'; break; // reverse hampe
    }

 
    if ( in ( event.GetId(), ID_MUS_BT_R0, ID_MUS_BT_R6 ) )
        kevent.m_controlDown = true;
    kevent.m_keyCode = value;
    SendEvent( kevent );
}

