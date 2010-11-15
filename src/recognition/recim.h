/////////////////////////////////////////////////////////////////////////////
// Name:        recim.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __recim_H__
#define __recim_H__

#ifdef AX_RECOGNITION

#ifdef __GNUG__
    #pragma interface "recim.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "app/aximagecontroller.h"
#include "app/axscrolledwindow.h"

class RecEnv;

class RecMusWindow;
class RecMusController;
class RecFile;


// WDR: class declarations


//----------------------------------------------------------------------------
// RecImController
//----------------------------------------------------------------------------

class RecImController: public AxImageController
{
public:
    // constructors and destructors
    RecImController();
    RecImController( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER, int flags = CONTROLLER_ALL );
    virtual ~RecImController();
	void SetEnv( RecEnv *env );
    void SetMusViewAndController( RecMusWindow *recMusWindow, RecMusController *recMusController );
    void SetRecFile( RecFile *recFile );
	// selection
	void CancelMagicSelection( );
    void SyncSelectionBitmap( ); // genere l'image superposee de la selection
    // selection bitmap	
    void GetSelectionBitmap( wxBitmap *bmp, int *x, int *y ); // used by OnPaint
    
    // WDR: method declarations for RecImController
    virtual void CloseDraggingSelection(wxPoint start, wxPoint end);
	virtual void LeftDClick( wxPoint point );
	
protected:
    // WDR: member variable declarations for RecImController
    //RecEnv *m_envPtr;
    // to synchronize view
    RecFile *m_recFilePtr;
    RecMusWindow *m_musViewPtr;
    RecMusController *m_musControlPtr;

public:
    wxPoint m_start;
    wxPoint m_end;
	
	// selection bitmap
	bool m_magic_selection;
	AxImage m_selection_img;
	int m_selection_img_x, m_selection_img_y;
	//wxPoint m_selection_point;
	wxBitmap m_selection_bmp;
    int m_selection_bmp_x, m_selection_bmp_y;
    
private:
    // WDR: handler declarations for RecImController
    void OnChangeClassification( wxCommandEvent &event );

private:
    DECLARE_CLASS(RecImController)
    DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------------------
// RecImWindow
//----------------------------------------------------------------------------

class RecImWindow: public AxScrolledWindow
{
public:
    // constructors and destructors
    RecImWindow();
    RecImWindow( AxImageController *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxScrolledWindowStyle );
    virtual ~RecImWindow();
    
    // WDR: method declarations for RecImWindow
    void SetEnv( RecEnv *env );
    void SetMusViewAndController( RecMusWindow *recMusWindow, RecMusController *recMusController );
	void SetRecFile( RecFile *recFile );
    
protected:
    // WDR: member variable declarations for RecImWindow
    RecEnv *m_envPtr;
    RecFile *m_recFilePtr;
    RecMusWindow *m_musViewPtr;
    RecMusController *m_musControlPtr;
    
private:
    wxMenu m_popupMenuClassification;
    // WDR: handler declarations for RecImWindow
    void OnPaint( wxPaintEvent &event );
    void OnMouse( wxMouseEvent &event );
    void OnScroll( wxScrollWinEvent &event );

private:
    DECLARE_CLASS(RecImWindow)
    DECLARE_EVENT_TABLE()
};

#endif //AX_RECOGNITION

#endif

