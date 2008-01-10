/////////////////////////////////////////////////////////////////////////////
// Name:        wgviewer.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __WG_VIEWER_H__
#define __WG_VIEWER_H__

#ifdef AX_WGVIEWER

#ifdef __GNUG__
    #pragma interface "wgviewer.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "app/axapp.h"
#include "app/axenv.h"

#include "wgviewer_wdr.h"

class MusFile;
class MusWindow;
class MusToolPanel;

enum
{
	ID5_WGWINDOW = ENV_IDS_LOCAL_WGV
};

// WDR: class declarations

//----------------------------------------------------------------------------
// WgvPanel
//----------------------------------------------------------------------------

class WgvPanel: public wxPanel
{
public:
    // constructors and destructors
    WgvPanel( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    
    // WDR: method declarations for WgvPanel
    wxPanel* GetWgPanel()  { return (wxPanel*) FindWindow( ID5_MUSPANEL ); }
    MusToolPanel* GetWgToolPanel()  { return (MusToolPanel*) FindWindow( ID5_TOOLPANEL ); }
    
private:
    // WDR: member variable declarations for WgvPanel
    
private:
    // WDR: handler declarations for WgvPanel
    void OnTree( wxTreeEvent &event );
    void OnSize( wxSizeEvent &event );

private:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// WgvEnv
//----------------------------------------------------------------------------

class WgvEnv: public AxEnv
{
public:
    // constructors and destructors
    WgvEnv::WgvEnv();
    virtual WgvEnv::~WgvEnv();
    
    // WDR: method declarations for WgvEnv
    virtual void LoadWindow();
	virtual void RealizeToolbar( );

private:
    // WDR: member variable declarations for WgvEnv
    wxPanel *m_panelPtr;
    MusWindow *m_wgViewPtr;
    MusFile *m_filePtr;
    
private:
    // WDR: handler declarations for WgvEnv
	void OnSize( wxSizeEvent &event );
    void OnTools( wxCommandEvent &event );
    void OnSave( wxCommandEvent &event );
    void OnOpenMLF( wxCommandEvent &event );
	void OnSaveMLF( wxCommandEvent &event );
    void OnSaveModel( wxCommandEvent &event );
    void OnZoomOut( wxCommandEvent &event );
    void OnZoomIn( wxCommandEvent &event );
    void OnGoto( wxCommandEvent &event );
    void OnPrevious( wxCommandEvent &event );
    void OnNext( wxCommandEvent &event );
    void OnUpdateUI( wxUpdateUIEvent &event );
    void OnOpen( wxCommandEvent &event );
	void OnUndo( wxCommandEvent &event );
	void OnRedo( wxCommandEvent &event );

private:
    DECLARE_DYNAMIC_CLASS(WgvEnv)

    DECLARE_EVENT_TABLE()
};

#endif //AX_WGVIEWER

#endif

