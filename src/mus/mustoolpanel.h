/////////////////////////////////////////////////////////////////////////////
// Name:        mustoolpanel.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_TOOLPANEL_H__
#define __MUS_TOOLPANEL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "mustoolpanel.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/aui/auibar.h"

//#include "mus_wdr.h"

#define MUS_TOOLS_NOTES 0
#define MUS_TOOLS_CLEFS 1
#define MUS_TOOLS_SIGNS 2
#define MUS_TOOLS_OTHER 3
#define NEUME_TOOLS_NOTES 4
#define NEUME_TOOLS_CLEFS 5
#define NEUME_TOOLS_OTHER 6

#define MUS_TOOLS_NUMBER 7 // Nombre de type de symbols

#define MUS_MODE_EDIT 0
#define MUS_MODE_INSERT 1

#define MENSURAL_MODE 0
#define NEUMES_MODE 1

#define MUS_MODES_NUMBER 7

class MusWindow;


enum {
    // tools
    ID_MS_BT_NOTES = 20500,
    ID_MS_BT_CLEFS,
    ID_MS_BT_SIGNS,
    ID_MS_BT_SYMBOLS,
    ID_MS_BT_CHMOD,
    //
    ID_MS_BT_TEXT,
    //
    ID_MS_BT_INSERT,
    
    // note
    ID_MS_BT_N0,
    ID_MS_BT_N1,
    ID_MS_BT_N2,
    ID_MS_BT_N3,
    ID_MS_BT_N4,
    ID_MS_BT_N5,
    ID_MS_BT_N6,
    ID_MS_BT_N7,
    // rest
    ID_MS_BT_R0,
    ID_MS_BT_R1,
    ID_MS_BT_R2,
    ID_MS_BT_R3,
    ID_MS_BT_R4,
    ID_MS_BT_R5,
    ID_MS_BT_R6,
    ID_MS_BT_CT,
    // note - flags
    ID_MS_BT_LG_D,
    ID_MS_BT_LG_U,
    ID_MS_BT_COLOR,
    ID_MS_BT_UPDOWN,
    
    // clefs   
    ID_MS_BT_G1,
    ID_MS_BT_G2,
    ID_MS_BT_U1,
    ID_MS_BT_U2,
    ID_MS_BT_U3,
    ID_MS_BT_U4,
    ID_MS_BT_U5,
    ID_MS_BT_F3,
    ID_MS_BT_F4,
    ID_MS_BT_F5,
    
    // proportions
    ID_MS_BT_MTPP,
    ID_MS_BT_MTPDP,
    ID_MS_BT_MTP,
    ID_MS_BT_MTPD,
    ID_MS_BT_MTIP,
    ID_MS_BT_MTIDP,
    ID_MS_BT_MTI,
    ID_MS_BT_MTID,
    ID_MS_BT_MTI2P,
    ID_MS_BT_MTI2DP,
    ID_MS_BT_MTI2,
    ID_MS_BT_MTI2D,
    ID_MS_BT_M32,
    ID_MS_BT_M3,
    ID_MS_BT_M2,

    // varia
    ID_MS_BT_PNT,
    ID_MS_BT_DIESE,
    ID_MS_BT_BECAR,
    ID_MS_BT_BEMOL,
    ID_MS_BT_DDIESE,
    ID_MS_BT_DBEMOL,
    ID_MS_BT_BAR,
    ID_MS_BT_RDOTS,

    // neumes
    ID_MS_BT_N_NOTES,
    ID_MS_BT_N_KEY,
    ID_MS_BT_N_SYMBOLES,
    
    //
    ID_MS_BT_LAST // Used for RANGE EVT IDS
};


// WDR: class declarations


//----------------------------------------------------------------------------
// MusToolRow
//----------------------------------------------------------------------------

class MusToolRow: public wxAuiToolBar
{
public:
    // constructors and destructors
    MusToolRow( wxWindow *parent, wxWindowID id );

    void UpdateTools( int type );
    
    // WDR: method declarations for MusToolRow
private:
    // WDR: member variable declarations for MusToolRow
	int m_type; // row type;
	//bool m_previous_edition;
	//wxBitmapButton *m_buttons[MUS_TOOLS_NUMBER + 1]; // +1 pour le bouton insert

public:
    
private:
    // WDR: handler declarations for MusToolRow

private:
};


//----------------------------------------------------------------------------
// MusToolPanel
//----------------------------------------------------------------------------

class MusToolPanel: public wxPanel
{
public:
    // constructors and destructors
    MusToolPanel( wxWindow *parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL );

    void SetMusWindow( MusWindow *w );
    void SetTools( int tools, bool edition );
    
    static wxBitmap GetToolbarBitmap( wxString name );
    
    // WDR: method declarations for MusToolPanel

private:
    void SendEvent( wxKeyEvent event );
    
private:
    // WDR: member variable declarations for MusToolPanel
    MusWindow *m_w;
	// panels
    MusToolRow *m_tools;
    // sizers used to vertical / horizontal change
	bool m_edition;
	int m_current_tools;
	int m_notation_mode;

public:
    
private:
    // WDR: handler declarations for MusToolPanel
	void OnChangeNotationMode( wxCommandEvent &event );
	void OnChangeMode( wxCommandEvent &event );
	void OnChangeTool( wxCommandEvent &event );
    void OnSymbol( wxCommandEvent &event );
    void OnSign( wxCommandEvent &event );
    void OnKey( wxCommandEvent &event );
    void OnNote( wxCommandEvent &event );
    void OnText( wxCommandEvent &event );
    // toggle buttons handling
    void MusToolPanel::OnUpdateUI( wxUpdateUIEvent &event );

private:
    DECLARE_EVENT_TABLE()
};


#endif
