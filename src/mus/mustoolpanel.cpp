/////////////////////////////////////////////////////////////////////////////
// Name:        mustoolpanel.cpp
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

#include "app/axapp.h"


// WDR: class implementations

//----------------------------------------------------------------------------
// MusToolRow
//----------------------------------------------------------------------------

// WDR: event table for MusToolRow

MusToolRow::MusToolRow( wxWindow *parent, wxWindowID id ) :
    wxAuiToolBar( parent, id, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT)
{

    SetToolBitmapSize(wxSize(32,32));
    SetFont(*wxSMALL_FONT);

    AddTool(ID_MS_BT_INSERT, wxT(""), MusToolPanel::GetToolbarBitmap( "tool_insert.png" ), "", wxITEM_CHECK);
    AddSeparator();   
    //AddSeparator();       
    AddTool(ID_MS_BT_NOTES, wxT("Notes"), MusToolPanel::GetToolbarBitmap( "tool_notes.png" ), "", wxITEM_CHECK);
    AddTool(ID_MS_BT_CLEFS, wxT("Clefs"), MusToolPanel::GetToolbarBitmap( "tool_clefs.png" ), "", wxITEM_CHECK);
    AddTool(ID_MS_BT_SIGNS, wxT("Prop."), MusToolPanel::GetToolbarBitmap( "tool_mes.png" ), "", wxITEM_CHECK);
    AddTool(ID_MS_BT_SYMBOLS, wxT("Varia"), MusToolPanel::GetToolbarBitmap( "tool_varia.png" ), "", wxITEM_CHECK);
    AddTool(ID_MS_BT_TEXT, wxT("Text"), MusToolPanel::GetToolbarBitmap( "tool_text.png" ), "");
    AddTool(ID_MS_BT_CHMOD, wxT("Neumes"), MusToolPanel::GetToolbarBitmap( "tool_neumes.png" ), "", wxITEM_CHECK);
    AddSeparator(); 
    AddStretchSpacer(100);

    Realize();

}


void MusToolRow::UpdateTools( int type )
{
    if (type == m_type) {
        return;
    }
    m_type = type;
    
    int tc = GetToolCount();
    int i;
    for (i = tc -1; i > MUS_MODES_NUMBER; i--) {
        DeleteByIndex(i);
    }
	
    switch(m_type) 
    {
    case (MUS_TOOLS_NOTES): 
        AddTool(ID_MS_BT_N0, "", MusToolPanel::GetToolbarBitmap( "note_0.png" ));
        AddTool(ID_MS_BT_N1, "", MusToolPanel::GetToolbarBitmap( "note_1.png" ));
        AddTool(ID_MS_BT_N2, "", MusToolPanel::GetToolbarBitmap( "note_2.png" ));
        AddTool(ID_MS_BT_N3, "", MusToolPanel::GetToolbarBitmap( "note_3.png" ));
        AddTool(ID_MS_BT_N4, "", MusToolPanel::GetToolbarBitmap( "note_4.png" ));
        AddTool(ID_MS_BT_N5, "", MusToolPanel::GetToolbarBitmap( "note_5.png" ));
        AddTool(ID_MS_BT_N6, "", MusToolPanel::GetToolbarBitmap( "note_6.png" ));
        AddTool(ID_MS_BT_N7, "", MusToolPanel::GetToolbarBitmap( "note_7.png" ));
        // add more...
        break;
	case (MUS_TOOLS_CLEFS):
        AddTool(ID_MS_BT_G1, "", MusToolPanel::GetToolbarBitmap( "clef_g1.png" ));
        AddTool(ID_MS_BT_G2, "", MusToolPanel::GetToolbarBitmap( "clef_g2.png" ));
        AddTool(ID_MS_BT_U1, "", MusToolPanel::GetToolbarBitmap( "clef_u1.png" ));
        AddTool(ID_MS_BT_U2, "", MusToolPanel::GetToolbarBitmap( "clef_u2.png" ));
        AddTool(ID_MS_BT_U3, "", MusToolPanel::GetToolbarBitmap( "clef_u3.png" ));
        AddTool(ID_MS_BT_U4, "", MusToolPanel::GetToolbarBitmap( "clef_u4.png" ));
        AddTool(ID_MS_BT_U5, "", MusToolPanel::GetToolbarBitmap( "clef_u5.png" ));
        AddTool(ID_MS_BT_F3, "", MusToolPanel::GetToolbarBitmap( "clef_f3.png" ));
        AddTool(ID_MS_BT_F4, "", MusToolPanel::GetToolbarBitmap( "clef_f4.png" ));
        AddTool(ID_MS_BT_F5, "", MusToolPanel::GetToolbarBitmap( "clef_f5.png" ));
        break;
    case (MUS_TOOLS_SIGNS):
        AddTool(ID_MS_BT_MTPP, "", MusToolPanel::GetToolbarBitmap( "mes_mtpp.png" ));
        // add more...
        break;
	case (MUS_TOOLS_OTHER):
        AddTool(ID_MS_BT_PNT, "", MusToolPanel::GetToolbarBitmap( "symb_dot.png" ));
        // add more...
        break;
	case (NEUME_TOOLS_NOTES):
        AddTool(ID_MS_BT_MTP, "", MusToolPanel::GetToolbarBitmap( "mes_mtp.png" ));
        // add more...
        break;
    }
    Realize();
    Fit();
}


//----------------------------------------------------------------------------
// MusToolPanel
//----------------------------------------------------------------------------

// WDR: event table for MusToolPanel

BEGIN_EVENT_TABLE(MusToolPanel,wxPanel)
	EVT_MENU( ID_MS_BT_INSERT, MusToolPanel::OnChangeMode )
    EVT_MENU_RANGE( ID_MS_BT_NOTES, ID_MS_BT_CHMOD, MusToolPanel::OnChangeTool )
    EVT_MENU_RANGE( ID_MS_BT_N0, ID_MS_BT_N7, MusToolPanel::OnNote )
    EVT_MENU_RANGE( ID_MS_BT_R0, ID_MS_BT_CT, MusToolPanel::OnNote )
	EVT_MENU_RANGE( ID_MS_BT_LG_D, ID_MS_BT_UPDOWN, MusToolPanel::OnNote )
    EVT_MENU_RANGE( ID_MS_BT_G1, ID_MS_BT_F5, MusToolPanel::OnKey )
    EVT_MENU_RANGE( ID_MS_BT_MTPP, ID_MS_BT_M2, MusToolPanel::OnSign )
    EVT_MENU_RANGE( ID_MS_BT_PNT, ID_MS_BT_BAR, MusToolPanel::OnSymbol )
    EVT_MENU( ID_MS_BT_TEXT, MusToolPanel::OnText )
    EVT_UPDATE_UI_RANGE(ID_MS_BT_NOTES, ID_MS_BT_INSERT, MusToolPanel::OnUpdateUI)
END_EVENT_TABLE()

MusToolPanel::MusToolPanel( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
    // now just one row, which is actually a tool bar
    // when calling UpdateTools, the corresponding buttons are loaded
    m_tools = new MusToolRow( this, -1 );
    this->Fit();

    m_w = NULL;
}

wxBitmap MusToolPanel::GetToolbarBitmap( wxString name )
{
	wxString subdir = wxString::Format("/mus/" );
	wxBitmap bitmap ( wxGetApp().m_resourcesPath + subdir + name , wxBITMAP_TYPE_PNG );
	return bitmap;
}

void MusToolPanel::SetMusWindow( MusWindow *w )
{
    m_w = w;
}

void MusToolPanel::OnUpdateUI( wxUpdateUIEvent &event )
{
    int id = event.GetId();
    if (id == ID_MS_BT_INSERT) {
        event.Check( !m_edition );
    } else if (id == ID_MS_BT_NOTES) {
        event.Check( m_current_tools == MUS_TOOLS_NOTES);
    } else if (id == ID_MS_BT_CLEFS) {
        event.Check( m_current_tools == MUS_TOOLS_CLEFS);
    } else if (id == ID_MS_BT_SIGNS) {
        event.Check( m_current_tools == MUS_TOOLS_SIGNS);
    } else if (id == ID_MS_BT_SYMBOLS) {
        event.Check( m_current_tools == MUS_TOOLS_OTHER);
    } else if (id == ID_MS_BT_CHMOD) {
        event.Check( m_current_tools == NEUME_TOOLS_NOTES);
    }
}

void MusToolPanel::SetTools( int tools, bool edition )
{
	if ( tools != m_current_tools ) // change tools
	{ 
		m_tools->UpdateTools( tools );
        this->Fit(); // resize
		this->Refresh();
	}
	else if ( m_edition != edition )  // just change mode
	{
		//m_tools->UpdateTools( edition );
		this->Refresh();
	}	

	m_current_tools = tools;
	m_edition = edition;
}

void MusToolPanel::SendEvent( wxKeyEvent kevent )
{
    if (!m_w)
        return;

    kevent.SetId( m_w->GetId() );
    kevent.SetEventObject( m_w );
    m_w->ProcessEvent( kevent );
    m_w->SetFocus();
}

void MusToolPanel::OnChangeTool( wxCommandEvent &event )
{
    if (!m_w)
        return;

    int value = 0;
    switch ( event.GetId() )
    {
    case (ID_MS_BT_NOTES): value = MUS_TOOLS_NOTES; break;
    case (ID_MS_BT_CLEFS): value = MUS_TOOLS_CLEFS; break;
    case (ID_MS_BT_SIGNS): value = MUS_TOOLS_SIGNS; break;
    case (ID_MS_BT_SYMBOLS): value = MUS_TOOLS_OTHER; break;
	case (ID_MS_BT_CHMOD): value = NEUME_TOOLS_NOTES; break;
    }

	m_w->SetInsertMode( true );
	m_w->SetToolType( value );
    m_w->SetFocus();   
}

void MusToolPanel::OnChangeMode( wxCommandEvent &event )
{
    if (!m_w)
        return;

	m_w->SetInsertMode( m_w->m_editElement ); 
	m_w->SetFocus();
}


void MusToolPanel::OnSymbol( wxCommandEvent &event )
{
    int value = '0';
    switch ( event.GetId() )
    {
    case (ID_MS_BT_DIESE): value = 'D'; break;
    case (ID_MS_BT_BECAR): value = 'H'; break;
    case (ID_MS_BT_BEMOL): value = 'B'; break;
    case (ID_MS_BT_DDIESE): value = 'F'; break;
    case (ID_MS_BT_DBEMOL): value = 'N'; break;

    case (ID_MS_BT_PNT): value = 'P'; break;
    case (ID_MS_BT_BAR): value = '|'; break;
    }
        
    wxKeyEvent kevent;
    kevent.SetEventType( wxEVT_KEY_DOWN );
    kevent.m_keyCode = value;
    SendEvent( kevent );  
}


void MusToolPanel::OnSign( wxCommandEvent &event )
{
    int value = '0';
    switch ( event.GetId() )
    {
    case (ID_MS_BT_MTPP): value = 'Q'; break;
    case (ID_MS_BT_MTPDP): value = 'W'; break;
    case (ID_MS_BT_MTP): value = 'E'; break;
    case (ID_MS_BT_MTPD): value = 'R'; break;

    case (ID_MS_BT_MTIP): value = 'A'; break;
    case (ID_MS_BT_MTIDP): value = 'S'; break;
    case (ID_MS_BT_MTI): value = 'D'; break;
    case (ID_MS_BT_MTID): value = 'F'; break;

    case (ID_MS_BT_MTI2P): value = 'Y'; break;
    case (ID_MS_BT_MTI2DP): value = 'X'; break;
    case (ID_MS_BT_MTI2): value = 'C'; break;
    case (ID_MS_BT_MTI2D): value = 'V'; break;

    case (ID_MS_BT_M32): value = '1'; break;
    case (ID_MS_BT_M2): value = '2'; break;
    case (ID_MS_BT_M3): value = '3'; break;
    }

    wxKeyEvent kevent;
    kevent.SetEventType( wxEVT_KEY_DOWN );
    kevent.m_keyCode = value;
    SendEvent( kevent );       
}

void MusToolPanel::OnKey( wxCommandEvent &event )
{
    int value = '0';
    switch ( event.GetId() )
    {
    case (ID_MS_BT_G1): value = '1'; break;
    case (ID_MS_BT_G2): value = '2'; break;
    case (ID_MS_BT_U1): value = '3'; break;
    case (ID_MS_BT_U2): value = '4'; break;
    case (ID_MS_BT_U3): value = '5'; break;
    case (ID_MS_BT_U4): value = '6'; break;
    case (ID_MS_BT_U5): value = '7'; break;
    case (ID_MS_BT_F3): value = '8'; break;
    case (ID_MS_BT_F4): value = '9'; break;
    case (ID_MS_BT_F5): value = '0'; break;
    }

    wxKeyEvent kevent;
    kevent.SetEventType( wxEVT_KEY_DOWN );
    kevent.m_keyCode = value;
    SendEvent( kevent );    
}

void MusToolPanel::OnText( wxCommandEvent &event )
{
    wxKeyEvent kevent;
    kevent.SetEventType( wxEVT_KEY_DOWN );
    kevent.m_keyCode = 'T';
    SendEvent( kevent );    
}

void MusToolPanel::OnNote( wxCommandEvent &event )
{
	wxKeyEvent kevent;
    kevent.SetEventType( wxEVT_KEY_DOWN );

    int value = WXK_NUMPAD0;
    switch ( event.GetId() )
    {
    case (ID_MS_BT_N1): value = WXK_NUMPAD1; break;
    case (ID_MS_BT_N2): value = WXK_NUMPAD2; break;
    case (ID_MS_BT_N3): value = WXK_NUMPAD3; break;
    case (ID_MS_BT_N4): value = WXK_NUMPAD4; break;
    case (ID_MS_BT_N5): value = WXK_NUMPAD5; break;
    case (ID_MS_BT_N6): value = WXK_NUMPAD6; break;
    case (ID_MS_BT_N7): value = WXK_NUMPAD7; break;

    case (ID_MS_BT_R1): value = WXK_NUMPAD1; break;
    case (ID_MS_BT_R2): value = WXK_NUMPAD2; break;
    case (ID_MS_BT_R3): value = WXK_NUMPAD3; break;
    case (ID_MS_BT_R4): value = WXK_NUMPAD4; break;
    case (ID_MS_BT_R5): value = WXK_NUMPAD5; break;
    case (ID_MS_BT_R6): value = WXK_NUMPAD6; break;
    case (ID_MS_BT_CT): value = 'C'; break;
	
	case (ID_MS_BT_LG_D): 
		{	
			kevent.m_keyCode = WXK_NUMPAD1;
			SendEvent( kevent ); // breve
			value = 'L'; break; // change ligature flag
		}
	case (ID_MS_BT_LG_U): 
		{	
			kevent.m_keyCode = WXK_NUMPAD2;
			SendEvent( kevent ); // semi-breve
			value = 'L'; break; // change ligature flag
		}
	case (ID_MS_BT_COLOR): value = 'I'; break; // coloration
	case (ID_MS_BT_UPDOWN): value = 'A'; break; // reverse hampe
    }

 
    if ( in ( event.GetId(), ID_MS_BT_R0, ID_MS_BT_R6 ) )
        kevent.m_controlDown = true;
    kevent.m_keyCode = value;
    SendEvent( kevent );
}

