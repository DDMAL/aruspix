/////////////////////////////////////////////////////////////////////////////
// Name:        cmpim.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __cmpim_H__
#define __cmpim_H__

#ifdef AX_RECOGNITION
	#ifdef AX_COMPARISON

#ifdef __GNUG__
    #pragma interface "cmpim.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "app/aximagecontroller.h"
#include "app/axscrolledwindow.h"

class CmpEnv;

class RecWgWindow;
class RecWgController;
class RecFile;


// WDR: class declarations


//----------------------------------------------------------------------------
// CmpImController
//----------------------------------------------------------------------------

class CmpImController: public AxImageController
{
public:
    // constructors and destructors
    CmpImController();
    CmpImController( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER, int flags = CONTROLLER_ALL );
    virtual ~CmpImController();
	void SetEnv( CmpEnv *env );
    //void SetWgViewAndController( RecWgWindow *recWgWindow, RecWgController *recWgController );
    RecFile *GetRecFile( ) { return m_recFilePtr; }
	void LoadRecImage( int colId, wxString filename, int staff, int segment, int pos, int flag );
    
    // WDR: method declarations for CmpImController
	
protected:
    // WDR: member variable declarations for CmpImController
    CmpEnv *m_envPtr;
    // to synchronize view
    RecFile *m_recFilePtr;

public:
    
private:
    // WDR: handler declarations for CmpImController

private:
    DECLARE_CLASS(CmpImController)
    DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------------------
// CmpImWindow
//----------------------------------------------------------------------------

class CmpImWindow: public AxScrolledWindow
{
public:
    // constructors and destructors
    CmpImWindow();
    CmpImWindow( CmpImController *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxScrolledWindowStyle );
    virtual ~CmpImWindow();
    
    // WDR: method declarations for CmpImWindow
    void SetEnv( CmpEnv *env );
    void SetCirclePen( const wxPen *pen, int width );
    void DrawCircle( );
    void ScrollSource( double x, double y );
    
protected:
    // WDR: member variable declarations for CmpImWindow
    CmpEnv *m_envPtr;
    RecFile *m_recFilePtr;
    
private:
    // WDR: handler declarations for CmpImWindow
    wxPoint m_circleCenter;
    wxPen m_pen;

private:
    DECLARE_CLASS(CmpImWindow)
    DECLARE_EVENT_TABLE()
};

	#endif // AX_COMPARISON
#endif // AX_RECOGNITION

#endif

