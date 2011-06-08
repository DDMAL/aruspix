/////////////////////////////////////////////////////////////////////////////
// Name:        edt.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __EDT_H__
#define __EDT_H__

#ifdef AX_EDT

#ifdef __GNUG__
    #pragma interface "edt.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "app/axapp.h"
#include "app/axenv.h"

#include "edt_wdr.h"

class MusFile;
class MusWindow;
class MusToolPanel;

enum
{
	ID5_MUSWINDOW = ENV_IDS_LOCAL_EDT
};

// WDR: class declarations

//----------------------------------------------------------------------------
// EdtPanel
//----------------------------------------------------------------------------

class EdtPanel: public wxPanel
{
public:
    // constructors and destructors
    EdtPanel( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    
    // WDR: method declarations for EdtPanel
    wxPanel* GetMusPanel()  { return (wxPanel*) FindWindow( ID5_MUSPANEL ); }
    MusToolPanel* GetMusToolPanel()  { return (MusToolPanel*) FindWindow( ID5_TOOLPANEL ); }
    
private:
    // WDR: member variable declarations for EdtPanel
    
private:
    // WDR: handler declarations for EdtPanel
    void OnTree( wxTreeEvent &event );
    void OnSize( wxSizeEvent &event );

private:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// EdtEnv
//----------------------------------------------------------------------------

class EdtEnv: public AxEnv
{
public:
    // constructors and destructors
    EdtEnv::EdtEnv();
    virtual EdtEnv::~EdtEnv();
    
    // WDR: method declarations for EdtEnv
    virtual void LoadWindow();
	virtual void RealizeToolbar( );

private:
    // WDR: member variable declarations for EdtEnv
    wxPanel *m_panelPtr;
    MusWindow *m_musViewPtr;
    MusFile *m_filePtr;
    
private:
    // WDR: handler declarations for EdtEnv
	void OnSize( wxSizeEvent &event );
    void OnTools( wxCommandEvent &event );
    void OnSave( wxCommandEvent &event );
    void OnOpenMLF( wxCommandEvent &event );
	void OnSaveMLF( wxCommandEvent &event );
    void OnOpenWWG( wxCommandEvent &event );
    void OnSaveWWG( wxCommandEvent &event );
    void OnSaveSVG( wxCommandEvent &event );
    void OnSaveModel( wxCommandEvent &event );
    void OnZoomOut( wxCommandEvent &event );
    void OnZoomIn( wxCommandEvent &event );
    void OnGoto( wxCommandEvent &event );
    void OnPrevious( wxCommandEvent &event );
    void OnNext( wxCommandEvent &event );
    void OnUpdateUI( wxUpdateUIEvent &event );
	void OnUndo( wxCommandEvent &event );
	void OnRedo( wxCommandEvent &event );

private:
    DECLARE_DYNAMIC_CLASS(EdtEnv)

    DECLARE_EVENT_TABLE()
};

#endif //AX_EDT

#endif

