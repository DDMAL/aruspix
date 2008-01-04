/////////////////////////////////////////////////////////////////////////////
// Name:        display.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#ifdef AX_DISPLAY

#ifdef __GNUG__
    #pragma interface "display.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "app/axapp.h"
#include "app/axenv.h"
#include "app/aximagecontroller.h"
#include "app/axscrolledwindow.h"

#include "display_wdr.h"

enum
{
	ID3_CONTROLLER =  ENV_IDS_LOCAL_DSP,
	ID3_CONTROLLER1,
	ID3_CONTROLLER2,
	ID3_VIEW,
	ID3_VIEW1,
	ID3_VIEW2
};

// slider de barre de menu valeurs maximal decroissante
#define ID3_SLIDER_R ENV_IDS_MAX_DSP - 1
#define ID3_SLIDER_G ENV_IDS_MAX_DSP


struct _imImage;

// WDR: class declarations

//----------------------------------------------------------------------------
// DspSrcWindow
//----------------------------------------------------------------------------

class DspSrcWindow: public AxScrolledWindow
{
public:
    // constructors and destructors
    DspSrcWindow();
    DspSrcWindow( AxImageController *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxScrolledWindowStyle );
    virtual ~DspSrcWindow();
    
    // WDR: method declarations for DspSrcWindow
    void SetCirclePen( const wxPen *pen, int width );
    void DrawCircle( );
    void ScrollSource( double x, double y );
    
private:
    // WDR: member variable declarations for DspSrcWindow
    wxPoint m_circleCenter;
    wxPen m_pen;
    
private:
    // WDR: handler declarations for DspSrcWindow

private:
    DECLARE_CLASS(DspSrcWindow)
    DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------------------
// DspResWindow
//----------------------------------------------------------------------------

class DspResWindow: public AxScrolledWindow
{
public:
    // constructors and destructors
    DspResWindow();
    DspResWindow( AxImageController *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxScrolledWindowStyle );
    virtual ~DspResWindow();
    
    // WDR: method declarations for DspResWindow
    void SynchronizeScroll( int x, int y );
    
private:
    // WDR: member variable declarations for DspResWindow
    
private:
    // WDR: handler declarations for DspResWindow
    void OnMouse( wxMouseEvent &event );

private:
    DECLARE_CLASS(DspResWindow)
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// DspImController
//----------------------------------------------------------------------------

class DspImController: public AxImageController
{
public:
    // constructors and destructors
    DspImController();
    DspImController( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER, int flags = CONTROLLER_ALL );
    virtual ~DspImController();
    
    // WDR: method declarations for DspImController
    void UpdateBrightness( );
    virtual void CloseDraggingSelection(wxPoint start, wxPoint end);
    void SetControllers( AxImageController *controller1, AxImageController *controller2 );
    void SetViews( DspSrcWindow *view1, DspSrcWindow *view2 );
    void ScrollSources( double x, double y );
    void DrawCircles( bool clear = false );

private:
    virtual void OpenPage( bool yield = true );
    
private:
    // WDR: member variable declarations for DspImController
    _imImage *m_greenIm;
    _imImage *m_redIm;
    AxImageController *m_imControl1Ptr;
    AxImageController *m_imControl2Ptr;
    DspSrcWindow *m_viewSrc1Ptr;
    DspSrcWindow *m_viewSrc2Ptr;

public:
    int m_red, m_green;
    
private:
    // WDR: handler declarations for DspImController

private:
    DECLARE_CLASS(DspImController)
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// DspPanel
//----------------------------------------------------------------------------

class DspPanel: public wxPanel
{
public:
    // constructors and destructors
    DspPanel( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    
    // WDR: method declarations for DspPanel
    wxSplitterWindow* GetSplitter1()  { return (wxSplitterWindow*) FindWindow( ID3_SPLITTER1 ); }
    //wxSplitterWindow* GetSplitter2()  { return (wxSplitterWindow*) FindWindow( ID1_SPLITTER2 ); }
    AxImageController* GetController()  { return (AxImageController*) FindWindow( ID3_CONTROLLER ); }
    AxImageController* GetController2()  { return (AxImageController*) FindWindow( ID3_CONTROLLER2 ); }
    AxImageController* GetController1()  { return (AxImageController*) FindWindow( ID3_CONTROLLER1 ); }
    
private:
    // WDR: member variable declarations for DspPanel
    
private:
    // WDR: handler declarations for DspPanel
    void OnSashPosition( wxSplitterEvent &event );

private:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// DspEnv
//----------------------------------------------------------------------------

class DspEnv: public AxEnv
{
public:
    // constructors and destructors
    DspEnv::DspEnv();
    virtual DspEnv::~DspEnv();
    
    // WDR: method declarations for DspEnv
    virtual void LoadWindow();
    static void LoadConfig();
    static void SaveConfig();
    virtual void RestoreToolBar();
	virtual void RealizeToolbar( );
    virtual void ParseCmd( wxCmdLineParser *parser );
    virtual void UpdateTitle( );

    // toolbar sliders
    wxSlider* GetRedSlider();
	wxSlider* GetGreenSlider();

private:
    // WDR: member variable declarations for DspEnv
    DspImController *m_imControlPtr;
    AxImageController *m_imControl1Ptr;
    AxImageController *m_imControl2Ptr;
    AxScrolledWindow *m_viewPtr;
    AxScrolledWindow *m_view1Ptr;
    AxScrolledWindow *m_view2Ptr;

public:
    static int s_sashPosition;

    
private:
    // WDR: handler declarations for DspEnv
    void OnClose( wxCommandEvent &event );
    void OnSashPosition( wxSplitterEvent &event );
    void OnRedGreen( wxScrollEvent &event );
	//void OnRedGreen( wxCommandEvent &event );
    void OnPaste( wxCommandEvent &event );
    void OnCopy( wxCommandEvent &event );
    void OnCut( wxCommandEvent &event );
    void OnOpen( wxCommandEvent &event );
    void OnUpdateUI( wxUpdateUIEvent &event );

private:
    DECLARE_DYNAMIC_CLASS(DspEnv)

    DECLARE_EVENT_TABLE()
};


#endif //AX_SUPERIMPOSITION

#endif

