/////////////////////////////////////////////////////////////////////////////
// Name:        aximagecontroller.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __aximagecontroller_H__
#define __aximagecontroller_H__

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "aximagecontroller.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "aximcontrol_wdr.h"

#include "aximage.h"

class AxEnv;
class AxScrolledWindow;

#define CONTROLLER_ALL  0 
#define CONTROLLER_NO_TOOLBAR  (1<<0)
#define CONTROLLER_NO_NAVIGATION  (1<<1) 
#define CONTROLLER_NO_ZOOM  (1<<2) 
#define CONTROLLER_NO_ADJUST  (1<<3)
#define CONTROLLER_NO_MULTIPAGE  (1<<4)

#define CONTROLLER_IDS_MIN 21000
#define CONTROLLER_IDS_MAX 21999

#define ID_POPUP_MENU_GOTO 21996 
#define ID_POPUP_MENU_NAVIGATION 21997
#define ID_POPUP_MENU_BY_PAGES 21998
#define ID_POPUP_MENU_BY_FILES 21999 

/*
#define IMAGE_FILES \
        "Image files|*.bmp;*.png;*.jpg;*jpeg;*.gif;*.tif;*.tiff;*.pcx;*.ico;*.cur|" \
        "BMP files (*.bmp)|*.bmp|" \
        "PNG files (*.png)|*.png|" \
        "JPEG files (*.jpg)|*.jpg;*.jpeg|" \
        "GIF files (*.gif)|*.gif|" \
        "TIFF files (*.tif)|*.tif;*.tiff|" \
        "PCX files (*.pcx)|*.pcx|" \
        "ICO files (*.ico)|*.ico|" \
        "CUR files (*.cur)|*.cur|" \
        "All files (*.*)|*.*"
*/

#define IMAGE_FILES \
        "TIFF files|*.tif;*.tiff|" \
        "All files (*.*)|*.*"

// WDR: class declarations

//----------------------------------------------------------------------------
// AxImageController
//----------------------------------------------------------------------------

class AxImageController: public wxPanel, public AxImage
{
public:
    // constructors and destructors
    AxImageController();
    AxImageController( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER, int flags = CONTROLLER_ALL );
    virtual ~AxImageController();
    
    // WDR: method declarations for AxImageController
	// initialisation
	void Init( AxEnv *env, AxScrolledWindow *window );
	// virtual
    virtual void CloseDraggingSelection(wxPoint start, wxPoint end) {};
	virtual void LeftDClick( wxPoint point ) {};
	// actions 
    void Paste( );
    void Copy( );
	void Cut( );
    void Open( const wxString fname = "", const int index = 0 );
    void Close( bool yield = true );
    void Next( );
    void Previous( );
    void Goto( );
    void SetImage( AxImage image );
	void ResetImage( AxImage image ); // same title, same zoom
	void UpdateTitleMsg( ); // default text ( name , index, zoom )
    // methodes d'etat
	bool HasNavigation( );
	bool IsMultipage( ) { return m_multipage; } // true if navigation is currently by page ( not by file )
    bool CanPaste( );
    bool CanCopy( );
	bool CanCut( );
    bool HasPrevious( );
    bool HasNext( );
    bool CanGoto( );
	bool HasFilename( ) { return !m_filename.IsEmpty(); }
	wxString GetTitleMsg( ) { return m_titleMsg; }
	wxString GetFilename( ) { return m_filename; }



protected:
    virtual void OpenPage( bool yield = true );
	void SetMultipage( bool multi );
	void LoadDirectoryFilenames( );

private:

public:
	
protected:
    // WDR: member variable declarations for AxImageController
	AxEnv *m_envPtr;
    AxScrolledWindow *m_viewPtr;
    wxFlexGridSizer *m_sizerPtr;
	int m_flags; // controller flags
	bool m_multipage; // if true navigation in multipage files else by directory
	wxToolBar *m_toolBarPtr;
    wxString m_filename;
	wxString m_shortname;
    int m_nbOfPages;
    int m_currentPage;
	int m_nbOfFiles;
	int m_currentFile;
	wxArrayString m_files;
	wxString m_titleMsg; // contenu pour la barre de titre
    
private:
    // WDR: handler declarations for AxImageController
    void OnUpdateUI( wxUpdateUIEvent &event );
    void OnCommand( wxCommandEvent &event );

private:
    DECLARE_CLASS(AxImageController)
    DECLARE_EVENT_TABLE()
};

#endif
