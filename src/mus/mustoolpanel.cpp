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

// WDR: class implementations


wxSizer *NotesPanel1( wxWindow *parent, bool call_fit, bool set_sizer )
{
    wxBoxSizer *item0 = new wxBoxSizer( wxHORIZONTAL );
    ToolNotesPaneBoxSizer = item0;

    item0->Add( 2, 1, 0, wxALIGN_CENTER|wxALL, 0 );
	
    wxToolBar *toolbar = new wxToolBar( parent, ID_MS_BT_INSERT, wxDefaultPosition, wxDefaultSize );
    item0->Add( toolbar, 0, wxALIGN_CENTER, 0 );
    
	/*
    toolbar->InsertTool( 0, -, _T("New"), m_framePtr->GetToolbarBitmap( "book_new.png" ), wxNullBitmap, wxITEM_NORMAL, _("New book"), _("New book") );
    toolbar->InsertTool( 1, ID4_OPEN_BOOK, _T("Open"), m_framePtr->GetToolbarBitmap( "book_open.png" ), wxNullBitmap, wxITEM_NORMAL, _("Open book"), _("Open book") );
    toolbar->InsertSeparator( 2 );
    
    toolbar->AddTool( ID4_ZOOM_OUT, _T("Zoom out"), m_framePtr->GetToolbarBitmap( "viewmag-.png" ), wxNullBitmap, wxITEM_NORMAL, _("Zoom out"), _("Zoom out") );
    toolbar->AddTool( ID4_ZOOM_IN, _T("Zoom in"), m_framePtr->GetToolbarBitmap( "viewmag+.png" ), wxNullBitmap, wxITEM_NORMAL, _("Zoom in"), _("Zoom in") );
    toolbar->AddTool( ID4_ADJUST, _T("Fit"), m_framePtr->GetToolbarBitmap( "viewmagfit.png" ), wxNullBitmap, wxITEM_CHECK, _("Adjust"), _("Adjust to fit the window") );
    //toolbar->AddTool( ID4_ADJUST_V, _T(""), BitmapsFunc4( 10 ), wxNullBitmap, wxITEM_CHECK, _("Adjust vertically"), _("Adjust to fit the window vertically") );
    //toolbar->AddTool( ID4_ADJUST_H, _T(""), BitmapsFunc4( 11 ), wxNullBitmap, wxITEM_CHECK, _("Adjust horizontally"), _("Adjust to fit the window horizontally") );
    //toolbar->AddSeparator();
    //toolbar->AddTool( ID4_SHOW_STAFF_BMP, _("Staff correspondence"), BitmapsFunc4( 12 ), wxNullBitmap, wxITEM_CHECK, _("Staff correspondence"), _("Show staff correspondence on image") );
    toolbar->AddSeparator();
    toolbar->AddTool( ID4_PROCESS, _T("Run"), m_framePtr->GetToolbarBitmap( "noatun.png" ), wxNullBitmap, wxITEM_NORMAL, _("Process"), _("Process the current page") );
    //parent->AddTool( ID4_RECOGNIZE, _T(""), BitmapsFunc4( 5 ), wxNullBitmap, wxITEM_NORMAL, _("Recognize"), _("Recognize current page") );
	*/
    toolbar->AddSeparator();
    //toolbar->AddTool( ID4_BATCH, _T("Batch"), m_framePtr->GetToolbarBitmap( "bookcase.png" ), wxNullBitmap, wxITEM_NORMAL, _("Batch pre-processing"), _("Execute batch pre-processing") );
    
    toolbar->Realize();

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}

//----------------------------------------------------------------------------
// MusToolRow
//----------------------------------------------------------------------------

// WDR: event table for MusToolRow

MusToolRow::MusToolRow( wxWindow *parent, wxWindowID id, int type ) :
    wxPanel( parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL )
{
	m_type = type;
	switch (type)
	{  
	case (MUS_TOOLS_NOTES): NotesPanel1( this, true, true ); break;
	case (MUS_TOOLS_CLEFS): ClefsPanel( this, true, true ); break;
	case (MUS_TOOLS_SIGNS): SignsPanel( this, true, true ); break;
	case (MUS_TOOLS_OTHER): SymbolsPanel( this, true, true ); break;
	}

	m_buttons[MUS_TOOLS_NOTES] = (wxBitmapButton*) FindWindow( ID_MS_BT_NOTES );
	m_buttons[MUS_TOOLS_CLEFS] = (wxBitmapButton*) FindWindow( ID_MS_BT_KEY );
	m_buttons[MUS_TOOLS_SIGNS] = (wxBitmapButton*) FindWindow( ID_MS_BT_SIGNS );
	m_buttons[MUS_TOOLS_OTHER] = (wxBitmapButton*) FindWindow( ID_MS_BT_SYMBOLES );
	m_buttons[MUS_TOOLS_NUMBER] = (wxBitmapButton*) FindWindow( ID_MS_BT_INSERT );
}
 

void MusToolRow::UpdateTools( bool edition )
{
	if ( edition == m_previous_edition )
		return; 

	//int i;  
	if ( edition )
	{ 
		//for(i = 0; i < MUS_TOOLS_NUMBER; i++) // only if we want to disable
		//	m_buttons[i]->Disable();
		//m_buttons[m_type]->Enable();
		m_buttons[MUS_TOOLS_NUMBER]->SetBitmapLabel( Btn( MUS_TOOLS_NUMBER ) );
	}  
	else 
	{ 
		//for(i = 0; i < MUS_TOOLS_NUMBER; i++)
		//	m_buttons[i]->Enable();
		m_buttons[MUS_TOOLS_NUMBER]->SetBitmapLabel( Btn_selected( MUS_TOOLS_NUMBER ) );
	}
	m_previous_edition = edition;
}

//----------------------------------------------------------------------------
// MusToolPanel
//----------------------------------------------------------------------------

// WDR: event table for MusToolPanel

BEGIN_EVENT_TABLE(MusToolPanel,wxPanel)
	EVT_COMMAND( ID_MS_BT_INSERT, wxEVT_COMMAND_BUTTON_CLICKED, MusToolPanel::OnChangeMode )
    EVT_COMMAND_RANGE( ID_MS_BT_NOTES, ID_MS_BT_SYMBOLES, wxEVT_COMMAND_BUTTON_CLICKED, MusToolPanel::OnChangeTool )
    EVT_COMMAND_RANGE( ID_MS_BT_N0, ID_MS_BT_N7, wxEVT_COMMAND_BUTTON_CLICKED, MusToolPanel::OnNote )
    EVT_COMMAND_RANGE( ID_MS_BT_R0, ID_MS_BT_CT, wxEVT_COMMAND_BUTTON_CLICKED, MusToolPanel::OnNote )
	EVT_COMMAND_RANGE( ID_MS_BT_LG_D, ID_MS_BT_UPDOWN, wxEVT_COMMAND_BUTTON_CLICKED, MusToolPanel::OnNote )
    EVT_COMMAND_RANGE( ID_MS_BT_G1, ID_MS_BT_F5, wxEVT_COMMAND_BUTTON_CLICKED, MusToolPanel::OnKey )
    EVT_COMMAND_RANGE( ID_MS_BT_MTPP, ID_MS_BT_M2, wxEVT_COMMAND_BUTTON_CLICKED, MusToolPanel::OnSign )
    EVT_COMMAND_RANGE( ID_MS_BT_PNT, ID_MS_BT_BAR, wxEVT_COMMAND_BUTTON_CLICKED, MusToolPanel::OnSymbole )
END_EVENT_TABLE()

MusToolPanel::MusToolPanel( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{

	m_rows[MUS_TOOLS_NOTES] = new MusToolRow( this, -1, MUS_TOOLS_NOTES );
	m_rows[MUS_TOOLS_CLEFS] = new MusToolRow( this, -1, MUS_TOOLS_CLEFS );
	m_rows[MUS_TOOLS_SIGNS] = new MusToolRow( this, -1, MUS_TOOLS_SIGNS );
	m_rows[MUS_TOOLS_OTHER] = new MusToolRow( this, -1, MUS_TOOLS_OTHER );

	m_rows[MUS_TOOLS_NOTES]->Show( true );
	m_rows[MUS_TOOLS_CLEFS]->Show( false );
	m_rows[MUS_TOOLS_SIGNS]->Show( false );
	m_rows[MUS_TOOLS_OTHER]->Show( false );

	m_previous_edition = false;
	m_previous_tools = MUS_TOOLS_NOTES;

	SetTools( MUS_TOOLS_NOTES, true );

	this->Fit();


    m_w = NULL;
}

void MusToolPanel::SetWgWindow( MusWindow *w )
{
    m_w = w;
}

void MusToolPanel::SetTools( int tools, bool edition )
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
    case (ID_MS_BT_KEY): value = MUS_TOOLS_CLEFS; break;
    case (ID_MS_BT_SIGNS): value = MUS_TOOLS_SIGNS; break;
    case (ID_MS_BT_SYMBOLES): value = MUS_TOOLS_OTHER; break;
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


void MusToolPanel::OnSymbole( wxCommandEvent &event )
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

