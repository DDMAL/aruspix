/////////////////////////////////////////////////////////////////////////////
// Name:        muswindow.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_WINDOW_H__
#define __MUS_WINDOW_H__

#ifdef __GNUG__
    #pragma interface "muswindow.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "musrc.h"

#include "app/axundo.h"

class MusToolPanel;
class MusKeyboardEditor;

// kind of operation, maybe used in subclasses 
enum
{
	MUS_NONE = 0,
	MUS_NOTE_INSERT,
	MUS_NOTE_DEL,
	MUS_NOTE_PITCH,
	MUS_NOTE_VALUE,
	MUS_NOTE_STEM,
	MUS_NOTE_LIGATURE,
	MUS_NOTE_COLORATION,
	MUS_SYMBOL_INSERT,
	MUS_SYMBOL_DEL,
	MUS_ELEMENT_MOVE
};

// levels of undo
// save either staff, page or file depending on operation
enum
{
	MUS_UNDO_STAFF = 0,
	MUS_UNDO_PAGE,
	MUS_UNDO_FILE
};


//----------------------------------------------------------------------------
// MusWindow
//----------------------------------------------------------------------------

class MusWindow: public wxScrolledWindow, public MusRC, public AxUndo
{
public:
    // constructors and destructors
    MusWindow();
    MusWindow( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxScrolledWindowStyle, bool center = true );
	virtual ~MusWindow();
    
    // edition
    virtual void DoRefresh() { Refresh(); }
    virtual void DoResize() { Resize(); }
    virtual void DoLyricCursor( int x, int y, AxDC *dc, wxString lyric );
    virtual void DoReset();

	// copy paste
	void Copy();
	void Cut();
	void Paste();
	bool CanCopy() { return (m_currentElement != NULL); }
	bool CanCut() { return (m_currentElement != NULL); }
	bool CanPaste() { return (m_bufferElement != NULL); };
	
	// undo
	virtual void Load( AxUndoFile *undoPtr );
    virtual void Store( AxUndoFile *undoPtr );

	// sync scroll
	virtual void OnSyncScroll( int x, int y ) {} // called when scroll synchronzation is needed

	void InitDC( wxDC *dc);

    bool CanGoto();
	void Goto( );
	// scroll
	void UpdateScroll(); // update scroll position if moving keys

	// set the tool panel ptr
	void SetToolPanel( MusToolPanel *toolpanel );
	// get the current tool
	int GetToolType( );
	void SetToolType( int type );
			
	void SetInsertMode( bool insert );
    
	/**
		Redimensionne la fenetre en fonction de la taille du parent et du zoom de la taille de la page
	  */
	void Resize( );
	/**
     Met a jour la table des fontes en fonction de nTailleFont et du zoom
	 Doit etre appelee apres changement de zoom ou de definition de page
     */
    
public:
 	
	/** format max utile; en principe, celui de la feuille **/
	/*
    int wymax, wxmax;
	int wxg, wxd, wyg, wyd;
	int winwxg, winwyg;
	float beamPenteMin, beamPenteMx;
	int pageFormatHor, pageFormatVer;
	int margeMorteHor, margeMorteVer;
	int portNoIndent, portIndent;
	wxRect drawRect;
	int mrgG;
	int discontinu;
	int mesureNum, mesureDen;
	float MesVal;
	// static
	static wxPoint point_[4];
	static wxPoint bcoord[2*(PTCONTROL+1)];
    */

	/** indique si la page doit etre centree asi elle n'occupe pas toute la fenetre */
	bool m_center;

	MusElement *m_newElement;
	MusElement *m_bufferElement;
	MusElement *m_lastEditedElement;

	// keep elements statically - m_newElement will point on them
	MusNote m_note;
	MusSymbol m_symbol;
	MusNeume m_neume;

	int m_insertx;
	int m_insertcode;
	int m_insertoct;
	int m_dragging_x;
	int m_dragging_y_offset;
	bool m_has_been_dragged;
	MusToolPanel *m_toolpanel;
	int m_lyricCursor;
	
    static int s_flats[];
    static int s_sharps[];
    
private:
	void SyncToolPanel(); // synchronize tool dlg (m_currentElement, mode ... )

	bool KeyboardEntry(wxKeyEvent &event);
    void LyricEntry( wxKeyEvent &event );
    
    void OnSize( wxSizeEvent &event );
    void OnPaint( wxPaintEvent &event );
	void OnMouseMotion( wxMouseEvent &event );
	void OnMouseLeftDown( wxMouseEvent &event );
	void OnMouseLeftUp( wxMouseEvent &event );
	void OnMouseDClick( wxMouseEvent &event );
	void OnMouseLeave( wxMouseEvent &event );
	void OnKeyUp( wxKeyEvent &event );
	void OnKeyDown( wxKeyEvent &event );
    void OnChar( wxKeyEvent &event );
	void OnPopupMenuNote( wxCommandEvent &event );
	void OnPopupMenuSymbole( wxCommandEvent &event );
    void OnMidiInput( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(MusWindow)
};



#endif
