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

class RecMusWindow;
class RecMusController;
class RecFile;




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
    //void SetMusViewAndController( RecMusWindow *recMusWindow, RecMusController *recMusController );
    RecFile *GetRecFile( ) { return m_recFilePtr; }
	void LoadRecImage( int colId, wxString filename, int staff, int pos, int flag );
    
    
protected:
        //CmpEnv *m_envPtr;
    // to synchronize view
    RecFile *m_recFilePtr;

public:
    
private:
    
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
    
        void SetEnv( CmpEnv *env );
    void SetCirclePen( const wxPen *pen, int width );
    void DrawCircle( );
    void ScrollSource( double x, double y );
    
protected:
        CmpEnv *m_envPtr;
    RecFile *m_recFilePtr;
    
private:
        wxPoint m_circleCenter;
    wxPen m_pen;

private:
    DECLARE_CLASS(CmpImWindow)
    DECLARE_EVENT_TABLE()
};

	#endif // AX_COMPARISON
#endif // AX_RECOGNITION

#endif

