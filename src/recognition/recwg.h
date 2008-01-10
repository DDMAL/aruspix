/////////////////////////////////////////////////////////////////////////////
// Name:        recmus.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __recmus_H__
#define __recmus_H__

#ifdef AX_RECOGNITION

#ifdef __GNUG__
    #pragma interface "recmus.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mus/muswindow.h"

//#include "rec_wdr.h"

class RecEnv;
class RecWgWindow;

class RecImWindow;
class RecImController;
class RecFile;

class RecMLFBmp;
class RecTypModel;

// WDR: class declarations

//----------------------------------------------------------------------------
// RecWgController
//----------------------------------------------------------------------------

class RecWgController: public wxPanel
{
public:
    // constructors and destructors
    RecWgController();
    RecWgController( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    
    // WDR: method declarations for RecWgController
	void Init( RecEnv *env, RecWgWindow *window );
	void SetImViewAndController( RecImWindow *recImWindow, RecImController *recImController );
	void SetRecFile( RecFile *recFile );
	// sync
	void SyncZoom();
    void SyncStaffBitmap( ); // genere l'image superposee de la portee wolfgang	
    // staff bitmap	
    void GetStaffBitmap( wxBitmap *bmp, int *x, int *y ); // used by RecImWindow::OnPaint
    void InverseShowStaffBitmap( );
    void CancelShowStaffBitmap( );
	void LoadBitmapsForFont( ); // load bitmaps for staff superimposition
	bool ShowStaffBitmap( ) { return m_staffbmp_show; }
    
protected:
    // WDR: member variable declarations for RecWgController
	RecEnv *m_envPtr;
    RecWgWindow *m_viewPtr;
	// to synchronize view
	RecFile *m_recFilePtr;
	RecImWindow *m_imViewPtr;
	RecImController *m_imControlPtr;
	
    // staff bitmap
    bool m_staffbmp_show;
    wxBitmap m_staffbmp;
    int m_staffbmp_x, m_staffbmp_y;
    RecMLFBmp *m_mlfBmp;
    int m_lastX, m_lastWidth; // keep last current element x and width to optimize redraw;

	
    
private:
    // WDR: handler declarations for RecWgController
    void OnSize( wxSizeEvent &event );

private:
    DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------------------
// RecWgWindow
//----------------------------------------------------------------------------

class RecWgWindow: public MusWindow
{
public:
    // constructors and destructors
    RecWgWindow();
    RecWgWindow( RecWgController *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxScrolledWindowStyle, bool center = true );
    virtual ~RecWgWindow();
    void SetEnv( RecEnv *env );
	void SetImViewAndController( RecImWindow *recImWindow, RecImController *recImController );
	void SetRecFile( RecFile *recFile );

    // edition
    virtual void OnBeginEditionClef(); // 
    virtual void OnEndEditionClef(); //
    virtual void OnEndEdition();
	virtual void OnSyncScroll( int x, int y );
	virtual void OnSize( wxSizeEvent &event );
    
    // WDR: method declarations for RecWgWindow
    
protected:
    // WDR: member variable declarations for RecWgWindow
    bool m_shiftDown;
    RecEnv *m_envPtr;
	RecWgController *m_wgControlPtr;
    bool m_edition; // true if OnBeginEdition() has been called -> retranspose current staff
	RecFile *m_recFilePtr;
	RecImWindow *m_imViewPtr;
	RecImController *m_imControlPtr;
    
private:
    // WDR: handler declarations for RecWgWindow
    void OnMouse( wxMouseEvent &event );
    void OnScroll( wxScrollWinEvent &event );
    void OnKeyUp( wxKeyEvent &event );
    void OnKeyDown( wxKeyEvent &event );

private:
    DECLARE_CLASS(RecWgWindow)
    DECLARE_EVENT_TABLE()
};

#endif //AX_RECOGNITION

#endif

