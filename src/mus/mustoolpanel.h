/////////////////////////////////////////////////////////////////////////////
// Name:        wgtoolpanel.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __wgtoolpanel_H__
#define __wgtoolpanel_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "mustoolpanel.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//#include "wx/bmpbuttn.h"

#include "mus_wdr.h"

#define MUS_TOOLS_NOTES 0
#define MUS_TOOLS_CLEFS 1
#define MUS_TOOLS_SIGNS 2
#define MUS_TOOLS_OTHER 3

#define MUS_TOOLS_NUMBER 4 // Nombre de type de symbols

#define MUS_MODE_EDIT 0
#define MUS_MODE_INSERT 1

class WgWindow;

// WDR: class declarations


//----------------------------------------------------------------------------
// WgToolRow
//----------------------------------------------------------------------------

class WgToolRow: public wxPanel
{
public:
    // constructors and destructors
    WgToolRow( wxWindow *parent, wxWindowID id, int type );

    void UpdateTools( bool edition );
    
    // WDR: method declarations for WgToolRow
private:
    // WDR: member variable declarations for WgToolRow
	int m_type; // row type;
	bool m_previous_edition;
	wxBitmapButton *m_buttons[MUS_TOOLS_NUMBER + 1]; // +1 pour le bouton insert

public:
    
private:
    // WDR: handler declarations for WgToolRow

private:
};


//----------------------------------------------------------------------------
// WgToolPanel
//----------------------------------------------------------------------------

class WgToolPanel: public wxPanel
{
public:
    // constructors and destructors
    WgToolPanel( wxWindow *parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL );

    void SetWgWindow( WgWindow *w );
    void SetTools( int tools, bool edition );
    
    // WDR: method declarations for WgToolPanel

private:
    void SendEvent( wxKeyEvent event );
    
private:
    // WDR: member variable declarations for WgToolPanel
    WgWindow *m_w;
	// panels
	WgToolRow *m_rows[4];
    // sizers used to vertical / horizontal change
	bool m_previous_edition;
	int m_previous_tools;


public:
    
private:
    // WDR: handler declarations for WgToolPanel
	void OnChangeMode( wxCommandEvent &event );
	void OnChangeTool( wxCommandEvent &event );
    void OnSymbole( wxCommandEvent &event );
    void OnSign( wxCommandEvent &event );
    void OnKey( wxCommandEvent &event );
    void OnNote( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};




#endif
