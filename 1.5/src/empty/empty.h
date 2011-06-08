/////////////////////////////////////////////////////////////////////////////
// Name:        empty.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#ifndef __EMPTY_H__
#define __EMPTY_H__

#ifdef AX_EMPTY

#ifdef __GNUG__
    #pragma interface "empty.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "app/axenv.h"

#include "empty_wdr.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// PanelEmpty
//----------------------------------------------------------------------------

class PanelEmpty: public wxPanel
{
public:
    // constructors and destructors
    PanelEmpty( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    
    // WDR: method declarations for PanelEmpty
    
private:
    // WDR: member variable declarations for PanelEmpty
    
private:
    // WDR: handler declarations for PanelEmpty
    void OnTree( wxTreeEvent &event );

private:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// EnvEmpty
//----------------------------------------------------------------------------

class EnvEmpty: public AxEnv
{
public:
    // constructors and destructors
    EnvEmpty::EnvEmpty();
    virtual EnvEmpty::~EnvEmpty();
    
    // WDR: method declarations for EnvEmpty
    virtual void LoadWindow();

private:
    // WDR: member variable declarations for EnvEmpty
    
private:
    // WDR: handler declarations for EnvEmpty
	void OnUpdateUI( wxUpdateUIEvent &event );
    void OnMenuItem( wxCommandEvent &event );

private:
    DECLARE_DYNAMIC_CLASS(EnvEmpty)

    DECLARE_EVENT_TABLE()
};

#endif //AX_EMPTY

#endif

