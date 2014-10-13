/////////////////////////////////////////////////////////////////////////////
// Name:        mustoolpanel.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <algorithm>
using std::min;
using std::max;

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "mustoolpanel.h"
#include "muswindow.h"
#include "musstaff.h"
//#include "muselement.h"

#include "app/axapp.h"



//----------------------------------------------------------------------------
// MusToolRow
//----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(MusToolRow,wxAuiToolBar)
END_EVENT_TABLE()

MusToolRow::MusToolRow( wxWindow *parent, wxWindowID id ) :
    wxAuiToolBar( parent, id, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT) //  | wxAUI_TB_PLAIN_BACKGROUND) // we might it add when switching to wx2.9
{
    m_notation_mode = MUS_MENSURAL_MODE;

    SetToolBitmapSize(wxSize(32,32));
    SetFont(*wxSMALL_FONT);

    // for some reason, we need to have at least one item to get the right height 
    AddTool(ID_MS_BT_BAR, "", MusToolPanel::GetToolbarBitmap( "padding.png" ));
    //AddSpacer(100);
    AddStretchSpacer(100);
    Realize();
    Fit();
    // remove the item
    //DeleteByIndex(0);
    Clear();
    //Fit();
}


void MusToolRow::UpdateTools( int type, int notation_mode )
{
//    if (type == m_type) {
//        return;
//    }
    m_type = type;
    
    // clear the entire toolbar
    Clear();
    DestroyChildren(); // we need to destroy the controls
    
    switch(notation_mode) 
    {
    case (MUS_MENSURAL_MODE):
        AddTool(ID_MS_BT_INSERT, wxT(""), MusToolPanel::GetToolbarBitmap( "tool_insert.png" ), "", wxITEM_CHECK);
        AddSeparator();   
        //AddSeparator();       
        AddTool(ID_MS_BT_NOTES, wxT("Notes"), MusToolPanel::GetToolbarBitmap( "tool_notes.png" ), "", wxITEM_CHECK);
        AddTool(ID_MS_BT_CLEFS, wxT("Clefs"), MusToolPanel::GetToolbarBitmap( "tool_clefs.png" ), "", wxITEM_CHECK);
        AddTool(ID_MS_BT_SIGNS, wxT("Prop."), MusToolPanel::GetToolbarBitmap( "tool_mes.png" ), "", wxITEM_CHECK);
        AddTool(ID_MS_BT_SYMBOLS, wxT("Varia"), MusToolPanel::GetToolbarBitmap( "tool_varia.png" ), "", wxITEM_CHECK);
        AddTool(ID_MS_BT_TEXT, wxT("Text"), MusToolPanel::GetToolbarBitmap( "tool_text.png" ), "", wxITEM_CHECK);
        AddSeparator(); 
        break;
    case (MUS_NEUMATIC_MODE):
        AddTool(ID_MS_BT_INSERT, wxT(""), MusToolPanel::GetToolbarBitmap( "tool_insert.png" ), "", wxITEM_CHECK);
        AddSeparator();   
        //AddSeparator();       
        AddTool(ID_MS_BT_NEUMES, wxT("Neumes"), MusToolPanel::GetToolbarBitmap( "tool_neumes.png" ), "", wxITEM_CHECK);
        AddTool(ID_MS_BT_SYMBOLS_NEUMES, wxT("Symbols"), MusToolPanel::GetToolbarBitmap( "clef_neume_c.png" ), "", wxITEM_CHECK);
        AddSeparator(); 
        break;
    case (MUS_CMN_MODE):
    /*
        AddTool(ID_MS_BT_INSERT, wxT(""), MusToolPanel::GetToolbarBitmap( "tool_insert.png" ), "", wxITEM_CHECK);
        AddSeparator();   
        //AddSeparator();       
        AddTool(ID_MS_BT_NOTES, wxT("Notes"), MusToolPanel::GetToolbarBitmap( "tool_notes.png" ), "", wxITEM_CHECK);
        AddTool(ID_MS_BT_CLEFS, wxT("Clefs"), MusToolPanel::GetToolbarBitmap( "tool_clefs.png" ), "", wxITEM_CHECK);
        AddTool(ID_MS_BT_SIGNS, wxT("Prop."), MusToolPanel::GetToolbarBitmap( "tool_mes.png" ), "", wxITEM_CHECK);
        AddTool(ID_MS_BT_SYMBOLS, wxT("Varia"), MusToolPanel::GetToolbarBitmap( "tool_varia.png" ), "", wxITEM_CHECK);
        AddTool(ID_MS_BT_TEXT, wxT("Text"), MusToolPanel::GetToolbarBitmap( "tool_text.png" ), "", wxITEM_CHECK);
        AddSeparator(); 
    */
        break;
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
        // rests
        AddSeparator(); 
        AddTool(ID_MS_BT_R0, "", MusToolPanel::GetToolbarBitmap( "rest_0.png" ));
        AddTool(ID_MS_BT_R1, "", MusToolPanel::GetToolbarBitmap( "rest_1.png" ));
        AddTool(ID_MS_BT_R2, "", MusToolPanel::GetToolbarBitmap( "rest_2.png" ));
        AddTool(ID_MS_BT_R3, "", MusToolPanel::GetToolbarBitmap( "rest_3.png" ));
        AddTool(ID_MS_BT_R4, "", MusToolPanel::GetToolbarBitmap( "rest_4.png" ));
        AddTool(ID_MS_BT_R5, "", MusToolPanel::GetToolbarBitmap( "rest_5.png" ));
        AddTool(ID_MS_BT_R6, "", MusToolPanel::GetToolbarBitmap( "rest_6.png" ));
        // ligatures
        AddSeparator(); 
        AddTool(ID_MS_BT_LG_D, "", MusToolPanel::GetToolbarBitmap( "note_lig1.png" ));
        AddTool(ID_MS_BT_LG_U, "", MusToolPanel::GetToolbarBitmap( "note_lig2.png" ));
        // coloration
        AddSeparator(); 
        AddTool(ID_MS_BT_COLOR, "", MusToolPanel::GetToolbarBitmap( "note_inv.png" ));
        // stem direction
        AddSeparator(); 
        AddTool(ID_MS_BT_UPDOWN, "", MusToolPanel::GetToolbarBitmap( "note_stem.png" ));
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
    case (MUS_TOOLS_PROPORTIONS):
        AddTool(ID_MS_BT_MTPP, "", MusToolPanel::GetToolbarBitmap( "mes_mtpp.png" ));
        AddTool(ID_MS_BT_MTPDP, "", MusToolPanel::GetToolbarBitmap( "mes_mtpdp.png" ));
        AddTool(ID_MS_BT_MTP, "", MusToolPanel::GetToolbarBitmap( "mes_mtp.png" ));
        AddTool(ID_MS_BT_MTPD, "", MusToolPanel::GetToolbarBitmap( "mes_mtpd.png" ));
        // 
        AddSeparator();
        AddTool(ID_MS_BT_MTIP, "", MusToolPanel::GetToolbarBitmap( "mes_mtip.png" ));
        AddTool(ID_MS_BT_MTIDP, "", MusToolPanel::GetToolbarBitmap( "mes_mtidp.png" ));
        AddTool(ID_MS_BT_MTI, "", MusToolPanel::GetToolbarBitmap( "mes_mti.png" ));
        AddTool(ID_MS_BT_MTID, "", MusToolPanel::GetToolbarBitmap( "mes_mtid.png" ));
        // 
        AddSeparator();
        AddTool(ID_MS_BT_MTI2P, "", MusToolPanel::GetToolbarBitmap( "mes_mti2p.png" ));
        AddTool(ID_MS_BT_MTI2DP, "", MusToolPanel::GetToolbarBitmap( "mes_mti2dp.png" ));
        AddTool(ID_MS_BT_MTI2, "", MusToolPanel::GetToolbarBitmap( "mes_mti2.png" ));
        AddTool(ID_MS_BT_MTI2D, "", MusToolPanel::GetToolbarBitmap( "mes_mtid.png" ));
        // other
        AddSeparator();
        AddTool(ID_MS_BT_M32, "", MusToolPanel::GetToolbarBitmap( "mes_m32.png" ));
        AddControl( new wxTextCtrl( this, ID_MS_BT_M32_NUM, wxString::Format("%d", MusMensur::s_num), wxDefaultPosition, wxSize( 30, 20 )), "Num" );
        AddControl( new wxTextCtrl( this, ID_MS_BT_M32_DEN, wxString::Format("%d", MusMensur::s_numBase), wxDefaultPosition, wxSize( 30, 20 )), "Den" );  
        //AddSeparator();
        //AddTool(ID_MS_BT_M3, "", MusToolPanel::GetToolbarBitmap( "mes_m3.png" ));
        //AddTool(ID_MS_BT_M2, "", MusToolPanel::GetToolbarBitmap( "mes_m2.png" ));
        break;
	case (MUS_TOOLS_OTHER):
        AddTool(ID_MS_BT_DOT, "", MusToolPanel::GetToolbarBitmap( "symb_dot.png" ));
        // other
        AddSeparator();
        AddTool(ID_MS_BT_SHARP, "", MusToolPanel::GetToolbarBitmap( "symb_sharp.png" ));
        AddTool(ID_MS_BT_NATURAL, "", MusToolPanel::GetToolbarBitmap( "symb_natural.png" ));
        AddTool(ID_MS_BT_FLAT, "", MusToolPanel::GetToolbarBitmap( "symb_flat.png" ));  
        // other
        AddSeparator();
        AddTool(ID_MS_BT_DSHARP, "", MusToolPanel::GetToolbarBitmap( "symb_2sharp.png" ));
        AddTool(ID_MS_BT_DFLAT, "", MusToolPanel::GetToolbarBitmap( "symb_2flat.png" ));  
        // other
        AddSeparator();
        AddTool(ID_MS_BT_BAR, "", MusToolPanel::GetToolbarBitmap( "symb_bar.png" ));
        AddTool(ID_MS_BT_RDOTS, "", MusToolPanel::GetToolbarBitmap( "symb_rdots.png" ));
        // custos
        AddSeparator(); 
        AddTool(ID_MS_BT_CT, "", MusToolPanel::GetToolbarBitmap( "note_c.png" ));
        break;
	case (NEUME_TOOLS_NOTES):
        AddTool(ID_NU_BT_N0, "", MusToolPanel::GetToolbarBitmap( "neume_punctum.png" ));
        AddTool(ID_NU_BT_N1, "", MusToolPanel::GetToolbarBitmap( "neume_diamond.png" ));
        AddTool(ID_NU_BT_N2, "", MusToolPanel::GetToolbarBitmap( "neume_cephalicus.png" ));
        AddTool(ID_NU_BT_N3, "", MusToolPanel::GetToolbarBitmap( "neume_punctum_up.png" ));
        AddTool(ID_NU_BT_N4, "", MusToolPanel::GetToolbarBitmap( "neume_quilisma.png" ));
        AddTool(ID_NU_BT_N5, "", MusToolPanel::GetToolbarBitmap( "neume_virga.png" ));
        AddTool(ID_NU_BT_CUSTOS, "", MusToolPanel::GetToolbarBitmap( "neume_custos.png" ));
		AddSeparator();
		AddTool(ID_NU_BT_N_HE, "", MusToolPanel::GetToolbarBitmap( "neume_he.png" ));
		AddTool(ID_NU_BT_N_VE, "", MusToolPanel::GetToolbarBitmap( "neume_ve.png" ));
		AddTool(ID_NU_BT_N_DOT, "", MusToolPanel::GetToolbarBitmap( "neume_dot.png" ));
        break;
	case (NEUME_TOOLS_SYMBOLS):
        AddTool(ID_NU_BT_C0, "", MusToolPanel::GetToolbarBitmap( "clef_neume_c1.png" ));
        AddTool(ID_NU_BT_C1, "", MusToolPanel::GetToolbarBitmap( "clef_neume_c2.png" ));
        AddTool(ID_NU_BT_C2, "", MusToolPanel::GetToolbarBitmap( "clef_neume_c3.png" ));
        AddTool(ID_NU_BT_C3, "", MusToolPanel::GetToolbarBitmap( "clef_neume_f1.png" ));
        AddTool(ID_NU_BT_C4, "", MusToolPanel::GetToolbarBitmap( "clef_neume_f2.png" ));
		AddTool(ID_NU_BT_COMMA, "", MusToolPanel::GetToolbarBitmap( "symb_neume_comma.png" ));
		AddTool(ID_NU_BT_FLAT, "", MusToolPanel::GetToolbarBitmap( "symb_neume_flat.png" ));
		AddTool(ID_NU_BT_SHARP, "", MusToolPanel::GetToolbarBitmap( "symb_neume_sharp.png" ));
		AddTool(ID_NU_BT_DIV_FINAL, "", MusToolPanel::GetToolbarBitmap( "symb_neume_div_final.png" ));
		AddTool(ID_NU_BT_DIV_MAJOR, "", MusToolPanel::GetToolbarBitmap( "symb_neume_div_major.png" ));
		AddTool(ID_NU_BT_DIV_MINOR, "", MusToolPanel::GetToolbarBitmap( "symb_neume_div_minor.png" ));
		AddTool(ID_NU_BT_DIV_SMALL, "", MusToolPanel::GetToolbarBitmap( "symb_neume_div_small.png" ));
        break;
    }
    //AddSpacer(10000); // fill in the space, arbitrary value
    Realize();
    Fit();
}


//----------------------------------------------------------------------------
// MusToolPanel
//----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(MusToolPanel,wxPanel)
	EVT_MENU( ID_MS_BT_INSERT, MusToolPanel::OnChangeMode )
    EVT_MENU_RANGE( ID_MS_BT_CHANGE_TOOL_START, ID_MS_BT_CHANGE_TOOL_END, MusToolPanel::OnChangeTool )
    EVT_MENU_RANGE( ID_MS_BT_N0, ID_MS_BT_N7, MusToolPanel::OnNote )
    EVT_MENU_RANGE( ID_MS_BT_R0, ID_MS_BT_R6, MusToolPanel::OnNote )
	EVT_MENU_RANGE( ID_MS_BT_LG_D, ID_MS_BT_UPDOWN, MusToolPanel::OnNote )
    EVT_MENU_RANGE( ID_MS_BT_G1, ID_MS_BT_F5, MusToolPanel::OnKey )
    EVT_MENU_RANGE( ID_MS_BT_MTPP, ID_MS_BT_M2, MusToolPanel::OnSign )
    EVT_MENU_RANGE( ID_MS_BT_DOT, ID_MS_BT_CT, MusToolPanel::OnSymbol )
    EVT_MENU_RANGE( ID_NU_BT_N0, ID_NU_BT_N5, MusToolPanel::OnNeume )
    EVT_MENU_RANGE( ID_NU_BT_C0, ID_NU_BT_C4, MusToolPanel::OnNeumeClef )
	EVT_MENU_RANGE( ID_NU_BT_COMMA, ID_NU_BT_DIV_SMALL, MusToolPanel::OnNeumeSymbol )
    EVT_MENU( ID_MS_BT_TEXT, MusToolPanel::OnText )
    EVT_UPDATE_UI_RANGE(ID_MS_BT_CHANGE_TOOL_START, ID_MS_BT_TEXT, MusToolPanel::OnUpdateUI)
    // measure controls
    EVT_TEXT(ID_MS_BT_M32_NUM, MusToolPanel::OnMeasure)
    EVT_TEXT(ID_MS_BT_M32_DEN, MusToolPanel::OnMeasure)
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
    m_current_tools = -1;
}

wxBitmap MusToolPanel::GetToolbarBitmap( wxString name )
{
	wxString subdir = wxString::Format("/mus/" );
	wxBitmap bitmap ( wxGetApp().m_resourcesPath + subdir + name , wxBITMAP_TYPE_PNG );
	return bitmap;
}

void MusToolPanel::SetMusWindow( MusWindow *w )
{
    m_current_tools = -1; // forces the toolbar to be recreated 
    m_w = w;
	if (m_w) {
		m_notation_mode = m_w->m_notation_mode;
	}
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
        event.Check( m_current_tools == MUS_TOOLS_PROPORTIONS);
    } else if (id == ID_MS_BT_SYMBOLS) {
        event.Check( m_current_tools == MUS_TOOLS_OTHER);
    } else if (id == ID_MS_BT_TEXT) {
        event.Check( m_w && m_w->m_lyricMode );
    // neumatic notation
    } else if (id == ID_MS_BT_NEUMES) {
        event.Check( m_current_tools == NEUME_TOOLS_NOTES);
    } else if (id == ID_MS_BT_SYMBOLS_NEUMES) {
        event.Check( m_current_tools == NEUME_TOOLS_SYMBOLS);
    }
}

void MusToolPanel::SetTools( int tools, bool edition )
{
	if ( (tools == -1) || (tools != m_current_tools) ) // change tools
	{ 
		m_tools->UpdateTools( tools, m_notation_mode );
        //wxYield( );
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

void MusToolPanel::SendEvent( wxKeyEvent kevent, bool set_focus )
{
    if (!m_w)
        return;

    kevent.SetId( m_w->GetId() );
    kevent.SetEventObject( m_w );
    m_w->GetEventHandler()->ProcessEvent( kevent );
    if (set_focus)
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
    case (ID_MS_BT_SIGNS): value = MUS_TOOLS_PROPORTIONS; break;
    case (ID_MS_BT_SYMBOLS): value = MUS_TOOLS_OTHER; break;
    // neumes
	case (ID_MS_BT_NEUMES): value = NEUME_TOOLS_NOTES; break;
	case (ID_MS_BT_SYMBOLS_NEUMES): value = NEUME_TOOLS_SYMBOLS; break;
    }

	m_w->SetEditorMode( MUS_EDITOR_INSERT );
	m_w->SetToolType( value );
    m_w->SetFocus();   
}

void MusToolPanel::OnChangeMode( wxCommandEvent &event )
{
    if (!m_w)
        return;
	m_w->ToggleEditorMode();
	m_w->SetFocus();
}

void MusToolPanel::OnSymbol( wxCommandEvent &event )
{
    int value = '0';
    switch ( event.GetId() )
    {
    case (ID_MS_BT_SHARP): value = 'S'; break;
    case (ID_MS_BT_NATURAL): value = 'N'; break;
    case (ID_MS_BT_FLAT): value = 'F'; break;
    case (ID_MS_BT_DSHARP): value = 'X'; break;
    case (ID_MS_BT_DFLAT): value = 'D'; break;

    case (ID_MS_BT_DOT): value = '.'; break;
    case (ID_MS_BT_BAR): value = '|'; break;
            
    case (ID_MS_BT_CT): value = 'C'; break;
    }
        
    wxKeyEvent kevent;
    kevent.SetEventType( wxEVT_KEY_DOWN );
    kevent.m_keyCode = value;
    SendEvent( kevent );  
}


// pass the value back to the MusSymbol1 static
void MusToolPanel::OnMeasure( wxCommandEvent &event )
{
    MusMensur::s_num = atoi(((wxTextCtrl*)FindWindow(ID_MS_BT_M32_NUM))->GetValue());
    MusMensur::s_numBase = atoi(((wxTextCtrl*)FindWindow(ID_MS_BT_M32_DEN))->GetValue());
    wxKeyEvent kevent;
    kevent.SetEventType( wxEVT_KEY_DOWN );
    kevent.m_keyCode = '1';
    SendEvent( kevent, false );
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
    case (ID_MS_BT_M3): value = '2'; break;
    case (ID_MS_BT_M2): value = '3'; break;
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

 
    int r0 = ID_MS_BT_R0;
    int r6 = ID_MS_BT_R6;
    if ( in ( event.GetId(), r0, r6 ) )
        kevent.m_controlDown = true;
    kevent.m_keyCode = value;
    SendEvent( kevent );
}

void MusToolPanel::OnNeume( wxCommandEvent &event )
{
    wxKeyEvent kevent;
    kevent.SetEventType( wxEVT_KEY_DOWN );

    int value = WXK_NUMPAD0;
    switch ( event.GetId() )
    {
        case (ID_NU_BT_N0): value = WXK_NUMPAD0; break;
        case (ID_NU_BT_N1): value = WXK_NUMPAD1; break;
        case (ID_NU_BT_N2): value = WXK_NUMPAD2; break;
        case (ID_NU_BT_N3): value = WXK_NUMPAD3; break;
        case (ID_NU_BT_N4): value = WXK_NUMPAD4; break;
        case (ID_NU_BT_N5): value = WXK_NUMPAD5; break;
    }
    int n0 = ID_NU_BT_N0;
    int n5 = ID_NU_BT_N5;
    if (in(event.GetId(), n0, n5)) {
        kevent.m_controlDown = true;
    }
    kevent.m_keyCode = value;
    SendEvent(kevent);
}

void MusToolPanel::OnNeumeClef( wxCommandEvent &event ) {
    int value = '0';
    switch ( event.GetId() )
    {
        case (ID_NU_BT_C0): value = '1'; break;
        case (ID_NU_BT_C1): value = '2'; break;
        case (ID_NU_BT_C2): value = '3'; break;
        case (ID_NU_BT_C3): value = '4'; break;
        case (ID_NU_BT_C4): value = '5'; break;
    }
    
    wxKeyEvent kevent;
    kevent.SetEventType( wxEVT_KEY_DOWN );
    kevent.m_keyCode = value;
    SendEvent( kevent );    
}

void MusToolPanel::OnNeumeSymbol( wxCommandEvent &event ) {
    int value = '0';
    switch ( event.GetId() )
    {
        case (ID_NU_BT_COMMA): value = '6'; break;
        case (ID_NU_BT_FLAT): value = 'F'; break;
        case (ID_NU_BT_SHARP): value = 'N'; break;
        case (ID_NU_BT_DIV_FINAL): value = '7'; break;
        case (ID_NU_BT_DIV_MAJOR): value = '8'; break;
		case (ID_NU_BT_DIV_MINOR): value = '9'; break;
		case (ID_NU_BT_DIV_SMALL): value = '0'; break;
    }
    
    wxKeyEvent kevent;
    kevent.SetEventType( wxEVT_KEY_DOWN );
    kevent.m_keyCode = value;
    SendEvent( kevent );    
}
