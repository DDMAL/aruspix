/////////////////////////////////////////////////////////////////////////////
// Name:        supim.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __supim_H__
#define __supim_H__

#ifdef AX_SUPERIMPOSITION

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "app/aximagecontroller.h"
#include "app/axscrolledwindow.h"


class SupEnv;
class SupFile;



//----------------------------------------------------------------------------
// SupImSrcWindow
//----------------------------------------------------------------------------

class SupImSrcWindow: public AxScrolledWindow
{
public:
    // constructors and destructors
    SupImSrcWindow();
    SupImSrcWindow( AxImageController *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxScrolledWindowStyle );
    virtual ~SupImSrcWindow();
    
    void SetCirclePen( const wxPen *pen, int width );
    void DrawCircle( );
    void ScrollSource( double x, double y );
    
private:
    wxPoint m_circleCenter;
    wxPen m_pen;
    
private:
    
private:
    DECLARE_CLASS(SupImSrcWindow)
    DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------------------
// SupImWindow
//----------------------------------------------------------------------------

class SupImWindow: public AxScrolledWindow
{
public:
    // constructors and destructors
    SupImWindow();
    SupImWindow( AxImageController *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxScrolledWindowStyle );
    virtual ~SupImWindow();
    
    void SynchronizeScroll( int x, int y );
    
private:
        
private:
    void OnMouse( wxMouseEvent &event );
    void OnPaint( wxPaintEvent &event );

private:
    DECLARE_CLASS(SupImWindow)
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// SupImController
//----------------------------------------------------------------------------

class SupImController: public AxImageController
{
public:
    // constructors and destructors
    SupImController();
    SupImController( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER, int flags = CONTROLLER_ALL );
    virtual ~SupImController();
    
    void UpdateBrightness( );
    
    virtual void CloseDraggingSelection(wxPoint start, wxPoint end);
    void SetControllers( AxImageController *controller1, AxImageController *controller2 );
    void SetViews( SupImSrcWindow *view1, SupImSrcWindow *view2 );
    void ScrollSources( double x, double y );
    void DrawCircles( bool clear = false );
    void ResetImage( AxImage image );
	void SetSupFile( SupFile *supFile ) { m_supFilePtr = supFile; }
    void SetInitialPoints( );
    wxPoint ToLogical( wxPoint p );
    wxPoint ToRender( wxPoint p );
    
private:
    _imImage *m_greenIm;
    _imImage *m_redIm;
    AxImageController *m_imControl1Ptr;
    AxImageController *m_imControl2Ptr;
    SupImSrcWindow *m_viewSrc1Ptr;
    SupImSrcWindow *m_viewSrc2Ptr;

public:
    int m_redContrast, m_redBrightness, m_greenContrast, m_greenBrightness;
    SupFile *m_supFilePtr;
	
protected:
    //SupEnv *m_envPtr;
    // to synchronize view
    
private:
    
private:
    DECLARE_CLASS(SupImController)
    DECLARE_EVENT_TABLE()
};

#endif // AX_SUPERIMPOSITION

#endif

