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
#define MUS_TOOLS_PROPORTIONS 2
#define MUS_TOOLS_OTHER 3
// neumes
#define NEUME_TOOLS_NOTES 4
#define NEUME_TOOLS_CLEFS 5
#define NEUME_TOOLS_OTHER 6
// cmn
// TODO

#define MUS_TOOLS_NUMBER 7 // Nombre de type de symbols

#define MUS_MODE_EDIT 0
#define MUS_MODE_INSERT 1

// #define MENSURAL_MODE 0 moved to musdef.h as MUS_MENSURAL_MODE, etc.
// #define NEUMES_MODE 1

//#define MUS_MODES_NUMBER 7

class MusWindow;


enum {
    ID_MS_BT_CHANGE_TOOL_START = 20500,  // not a button, just used for RANGE EVT IDS
    // tools for mensural notation
    ID_MS_BT_NOTES,
    ID_MS_BT_CLEFS,
    ID_MS_BT_SIGNS,
    ID_MS_BT_SYMBOLS,
    // tools for neumatic notation
    ID_MS_BT_NEUMES,
    ID_MS_BT_CLEFS_NEUMES,
    ID_MS_BT_SYMBOLS_NEUMES,
    // tools for cmn notation
    /*
    ID_MS_BT_NOTES_CMN,
    ID_MS_BT_CLEFS_CMN,
    ID_MS_BT_SIGNS_CMN,
    ID_MS_BT_SYMBOLS_CMN,
    ID_MS_BT_TEXT_CMN,
    */
    //
    ID_MS_BT_CHANGE_TOOL_END, // not a button, just used for RANGE EVT IDS
    //
    ID_MS_BT_INSERT,
    ID_MS_BT_TEXT, 
    
    
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
    ID_MS_BT_M32_NUM,
    ID_MS_BT_M32_DEN,
    ID_MS_BT_M3,
    ID_MS_BT_M2, 

    // varia
    ID_MS_BT_DOT,
    ID_MS_BT_SHARP,
    ID_MS_BT_NATURAL,
    ID_MS_BT_FLAT,
    ID_MS_BT_DSHARP,
    ID_MS_BT_DFLAT,
    ID_MS_BT_BAR,
    ID_MS_BT_RDOTS,

    // neumes  - is it used?
    /*
    ID_MS_BT_N_NOTES,
    ID_MS_BT_N_KEY,
    ID_MS_BT_N_SYMBOLES,
    */
    
    //
    ID_MS_BT_LAST // Used for RANGE EVT IDS
};




//----------------------------------------------------------------------------
// MusToolRow
//----------------------------------------------------------------------------

class MusToolRow: public wxAuiToolBar
{
public:
    // constructors and destructors
    MusToolRow( wxWindow *parent, wxWindowID id );

    void UpdateTools( int type, int notation_mode );
    
    private:
    int m_type; // row type;
    int m_notation_mode;
	//bool m_previous_edition;
	//wxBitmapButton *m_buttons[MUS_TOOLS_NUMBER + 1]; // +1 pour le bouton insert

public:
    
private:
    DECLARE_EVENT_TABLE()
    
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
    
    
private:
    void SendEvent( wxKeyEvent event, bool set_focus = true );
    
private:
        MusWindow *m_w;
	// panels
    MusToolRow *m_tools;
    // sizers used to vertical / horizontal change
	bool m_edition;
	int m_current_tools;
	int m_notation_mode;

public:
    
private:
    void OnChangeNotationMode( wxCommandEvent &event );
	void OnChangeMode( wxCommandEvent &event );
    void OnMeasure( wxCommandEvent &event );
	void OnChangeTool( wxCommandEvent &event );
    void OnSymbol( wxCommandEvent &event );
    void OnSign( wxCommandEvent &event );
    void OnKey( wxCommandEvent &event );
    void OnNote( wxCommandEvent &event );
    void OnText( wxCommandEvent &event );
    // toggle buttons handling
    void OnUpdateUI( wxUpdateUIEvent &event );

private:
    DECLARE_EVENT_TABLE()
};


#endif
