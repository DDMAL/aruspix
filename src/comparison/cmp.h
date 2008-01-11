/////////////////////////////////////////////////////////////////////////////
// Name:        cmp.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __CMP_H__
#define __CMP_H__

#ifdef AX_RECOGNITION
	#ifdef AX_COMPARISON

#ifdef __GNUG__
    #pragma interface "cmp.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "app/axapp.h"
#include "app/axenv.h"

#include "cmp_wdr.h"

enum
{
    ID6_CONTROLLER1 = ENV_IDS_LOCAL_CMP,
    ID6_VIEW1,
	ID6_CONTROLLER2,
    ID6_VIEW2,
    ID6_DISPLAY,
    ID6_WGWINDOW,
    
    ID6_POPUP_TREE_LOAD,
    ID6_POPUP_TREE_EDIT,
    ID6_POPUP_TREE_AX_DESACTIVATE,
	ID6_POPUP_TREE_ASSEMBLE,
};

class CmpImController;
class CmpMusController;
class CmpImWindow;
class CmpMusWindow;

class CmpFile;
class CmpCtrl;
//class RecFile;
class CmpCtrlPanel;
class CmpEnv;
class CmpCollation;

// WDR: class declarations

/*
//----------------------------------------------------------------------------
// RecSplitterWindow
//----------------------------------------------------------------------------

class RecSplitterWindow: public wxSplitterWindow
{
public:
    // constructors and destructors
    RecSplitterWindow( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0 );
    void SetEnv( CmpEnv *env, wxFlexGridSizer *sizer, MusToolPanel *toolpanel,   CmpMusController *musControlPtr );
    
    // WDR: method declarations for RecSplitterWindow
    void ChangeOrientation( );
    
protected:
    // WDR: member variable declarations for RecSplitterWindow
    CmpEnv *m_envPtr;
    MusToolPanel *m_toolpanel;
    wxFlexGridSizer *m_mussizer;
    CmpMusController *m_musControlPtr;
    
private:
    // WDR: handler declarations for RecSplitterWindow
    void OnSashDoubleClick( wxSplitterEvent &event );
    void OnSashChanged( wxSplitterEvent &event );

private:
    DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------------------
// RecPanel
//----------------------------------------------------------------------------

class RecPanel: public wxPanel
{
public:
    // constructors and destructors
    RecPanel( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    
    // WDR: method declarations for RecPanel
    RecSplitterWindow* GetSplitter1()  { return (RecSplitterWindow*) FindWindow( ID4_SPLITTER1 ); }
    AxImageController* GetController()  { return (AxImageController*) FindWindow( ID4_CONTROLLER ); }
    
private:
    // WDR: member variable declarations for RecPanel
    
private:
    // WDR: handler declarations for RecPanel

private:
    DECLARE_EVENT_TABLE()
};
*/


//----------------------------------------------------------------------------
// CmpEnv
//----------------------------------------------------------------------------

class CmpEnv: public AxEnv
{
public:
    // constructors and destructors
    CmpEnv::CmpEnv();
    virtual CmpEnv::~CmpEnv();
    
    // WDR: method declarations for CmpEnv
    virtual void LoadWindow();
    static void LoadConfig();
    static void SaveConfig();
    virtual void ParseCmd( wxCmdLineParser *parser );
    virtual void UpdateTitle( );
    virtual void RealizeToolbar( );
    virtual void Open( wxString filename, int type );
    virtual bool CloseAll( );
    //
    bool ResetFile(); // must be called when new file is created or opened
    bool ResetCmpFile(); // must be called when new file is created or opened
	void ViewCollation( CmpCollation *collation );
    void OpenFile( wxString filename);
    void UpdateViews( int flags );
	CmpCollation *GetCollationPtr() { return m_cmpCollationPtr; }

private:
    // WDR: member variable declarations for CmpEnv
    wxSplitterWindow *m_splitterPtr;
    wxSplitterWindow *m_vsplitterPtr;
	wxSplitterWindow *m_isplitterPtr;
    // im
    CmpImController *m_imControlPtr1, *m_imControlPtr2;
    CmpImWindow *m_imViewPtr1, *m_imViewPtr2;
    // mus    
    CmpMusController *m_musControlPtr;
    CmpMusWindow *m_musViewPtr;
	CmpCollation *m_cmpCollationPtr;
    //MusToolPanel *m_toolpanel;
    //wxPanel *m_musPanelPtr; // panel complet: contient toolpanel et recwgcontoller
    // recognition elements
    //RecFile *m_recFilePtr;
    // 
    CmpCtrl *m_cmpCtrlPtr; // tree
    CmpFile *m_cmpFilePtr;
    CmpCtrlPanel *m_cmpCtrlPanelPtr;

public:
    static int s_book_sash;
	static int s_view_sash;
	// tree
    static bool s_expand_root;
	static bool s_expand_book;
    static bool s_expand_cmp;
    
private:

    // WDR: handler declarations for CmpEnv
    void OnCmpEdit( wxCommandEvent &event );
    void OnCmpLoad( wxCommandEvent &event );
    void OnOpen( wxCommandEvent &event );
    void OnTools( wxCommandEvent &event );
    void OnNew( wxCommandEvent &event );
    void OnSave( wxCommandEvent &event );
    void OnSaveAs( wxCommandEvent &event );
    void OnClose( wxCommandEvent &event );
    void OnAdjust( wxCommandEvent &event );
    void OnZoom( wxCommandEvent &event );
    void OnGoto( wxCommandEvent &event );
    void OnPrevious( wxCommandEvent &event );
    void OnNext( wxCommandEvent &event );
    void OnPaste( wxCommandEvent &event );
    void OnCopy( wxCommandEvent &event );
    void OnCut( wxCommandEvent &event );
    void OnUndo( wxCommandEvent &event );
    void OnRedo( wxCommandEvent &event );
    void OnCollate( wxCommandEvent &event );
    virtual void OnUpdateUI( wxUpdateUIEvent &event );

private:
    DECLARE_DYNAMIC_CLASS(CmpEnv)

    DECLARE_EVENT_TABLE()
};

	#endif //AX_COMPARISON
#endif //AX_RECOGNITION

#endif

