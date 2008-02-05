/////////////////////////////////////////////////////////////////////////////
// Name:        cmpmus.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __cmpmus_H__
#define __cmpmus_H__

#ifdef AX_RECOGNITION
	#ifdef AX_COMPARISON

#ifdef __GNUG__
    #pragma interface "cmpmus.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mus/muswindow.h"

//#include "cmp_wdr.h"

class CmpEnv;
class CmpMusWindow;

class CmpImWindow;
class CmpImController;
class CmpFile;



// WDR: class declarations

//----------------------------------------------------------------------------
// CmpMusController
//----------------------------------------------------------------------------

class CmpMusController: public wxPanel
{
public:
    // constructors and destructors
    CmpMusController();
    CmpMusController( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    
    // WDR: method declarations for CmpMusController
	void Init( CmpEnv *env, CmpMusWindow *window );
	void SetImViewAndController( CmpImWindow *cmpImWindow1, CmpImController *cmpImController1,
		CmpImWindow *cmpImWindow2, CmpImController *cmpImController2 );
	void SetCmpFile( CmpFile *cmpFile );
    
protected:
    // WDR: member variable declarations for CmpMusController
	CmpEnv *m_envPtr;
    CmpMusWindow *m_viewPtr;
	// to synchronize view
	CmpFile *m_cmpFilePtr;
	CmpImWindow *m_imViewPtr1, *m_imViewPtr2;
	CmpImController *m_imControlPtr1, *m_imControlPtr2;
	
private:
    // WDR: handler declarations for CmpMusController
    void OnSize( wxSizeEvent &event );

private:
    DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------------------
// CmpMusWindow
//----------------------------------------------------------------------------

class CmpMusWindow: public MusWindow
{
public:
    // constructors and destructors
    CmpMusWindow();
    CmpMusWindow( CmpMusController *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxScrolledWindowStyle, bool center = true );
    virtual ~CmpMusWindow();
    void SetEnv( CmpEnv *env );
	void SetImViewAndController( CmpImWindow *cmpImWindow1, CmpImController *cmpImController1,
		CmpImWindow *cmpImWindow2, CmpImController *cmpImController2 );
	void SetCmpFile( CmpFile *cmpFile );

    // edition
    //virtual void OnBeginEditionClef(); // 
    //virtual void OnEndEditionClef(); //
    //virtual void OnEndEdition();
	virtual void OnSyncScroll( int x, int y );
	virtual void OnSize( wxSizeEvent &event );
    
    // WDR: method declarations for CmpMusWindow
    
protected:
    // WDR: member variable declarations for CmpMusWindow
    bool m_shiftDown;
    CmpEnv *m_envPtr;
	CmpMusController *m_musControlPtr;
    //bool m_edition; // true if OnBeginEdition() has been called -> retranspose current staff
	CmpFile *m_cmpFilePtr;
	CmpImWindow *m_imViewPtr1, *m_imViewPtr2;
	CmpImController *m_imControlPtr1, *m_imControlPtr2;
	// variable to control alternance in images : change if we change the staff
	int m_lastStaff, m_lastController;
    
private:
    // WDR: handler declarations for CmpMusWindow
    void OnMouse( wxMouseEvent &event );
    void OnScroll( wxScrollWinEvent &event );
    void OnKeyUp( wxKeyEvent &event );
    void OnKeyDown( wxKeyEvent &event );

private:
    DECLARE_CLASS(CmpMusWindow)
    DECLARE_EVENT_TABLE()
};

	#endif // AX_COMPARISON
#endif // AX_RECOGNITION

#endif

