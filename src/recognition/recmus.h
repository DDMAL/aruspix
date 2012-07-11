/////////////////////////////////////////////////////////////////////////////
// Name:        recmus.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __recmus_H__
#define __recmus_H__

#ifdef AX_RECOGNITION

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mus/muswindow.h"

//#include "rec_wdr.h"

class RecEnv;
class RecMusWindow;

class RecImWindow;
class RecImController;
class RecFile;

class RecMLFBmp;
class RecTypModel;

// WDR: class declarations

//----------------------------------------------------------------------------
// RecMusController
//----------------------------------------------------------------------------

class RecMusController: public wxPanel
{
public:
    // constructors and destructors
    RecMusController();
    RecMusController( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    
    // WDR: method declarations for RecMusController
	void Init( RecEnv *env, RecMusWindow *window );
	void SetImViewAndController( RecImWindow *recImWindow, RecImController *recImController );
	void SetRecFile( RecFile *recFile );
	// sync
	void SyncZoom();
    void SyncStaffBitmap( ); // genere l'image superposee de la portee wolfgang	
    // staff bitmap	
	void SetForMEI();
    void GetStaffBitmap( wxBitmap *bmp, int *x, int *y ); // used by RecImWindow::OnPaint
    void InverseShowStaffBitmap( );
    void CancelShowStaffBitmap( );
	void LoadBitmapsForFont( ); // load bitmaps for staff superimposition
	bool ShowStaffBitmap( ) { return m_staffbmp_show; }
    
protected:
    // WDR: member variable declarations for RecMusController
	RecEnv *m_envPtr;
    RecMusWindow *m_viewPtr;
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
    // WDR: handler declarations for RecMusController
    void OnSize( wxSizeEvent &event );

private:
    DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------------------
// RecMusWindow
//----------------------------------------------------------------------------

class RecMusWindow: public MusWindow
{
public:
    // constructors and destructors
    RecMusWindow();
    RecMusWindow( RecMusController *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxScrolledWindowStyle, bool center = true );
    virtual ~RecMusWindow();
    void SetEnv( RecEnv *env );
	void SetImViewAndController( RecImWindow *recImWindow, RecImController *recImController );
	void SetRecFile( RecFile *recFile );

    // edition
    /**
     * Transposes the content of the current layer to UT1 before editing a clef
     */
    virtual void OnBeginEditionClef();
    /**
     * Transposes the content of the current layer its the actual value
     */
    virtual void OnEndEditionClef(); //
    virtual void OnEndEdition();
	virtual void OnSyncScroll( int x, int y );
	virtual void OnSize( wxSizeEvent &event );
    
    // WDR: method declarations for RecMusWindow
    
protected:
    // WDR: member variable declarations for RecMusWindow
    bool m_shiftDown;
    RecEnv *m_envPtr;
	RecMusController *m_musControlPtr;
    bool m_edition; // true if OnBeginEdition() has been called -> retranspose current staff
	RecFile *m_recFilePtr;
	RecImWindow *m_imViewPtr;
	RecImController *m_imControlPtr;
    
private:
    // WDR: handler declarations for RecMusWindow
    void OnMouse( wxMouseEvent &event );
    void OnScroll( wxScrollWinEvent &event );
    void OnKeyUp( wxKeyEvent &event );
    void OnKeyDown( wxKeyEvent &event );

private:
    DECLARE_CLASS(RecMusWindow)
    DECLARE_EVENT_TABLE()
};

#endif //AX_RECOGNITION

#endif

