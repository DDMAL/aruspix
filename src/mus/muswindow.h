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
#include "wx/dynarray.h"

#include "mus.h"

#include "app/axundo.h"

class MusPage;
class MusFile;
class MusElement;
class MusStaff;
class MusToolPanel;
class MusKeyboardEditor;

#include "musnote.h"
#include "musneume.h"
#include "mussymbol.h"

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

static int bem [] = {F2, F3, F3, F4, F4, F5, F6, F6, F7, F7, F8, F8};
static int die [] = {F2, F2, F3, F3, F4, F5, F5, F6, F6, F7, F7, F8};

// WDR: class declarations

//----------------------------------------------------------------------------
// MusWindow
//----------------------------------------------------------------------------

class MusWindow: public wxScrolledWindow, public AxUndo
{

public:
    // constructors and destructors
    MusWindow();
    MusWindow( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxScrolledWindowStyle, bool center = true );
	virtual ~MusWindow();

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

	// edition
	virtual void OnBeginEdition() {} // pure virtual fonctions - may be used in subclass
	virtual void OnEndEdition() {}
	virtual void OnBeginEditionClef() {} // pure virtual fonctions - may be used in subclass
	virtual void OnEndEditionClef() {}
	// sync scroll
	virtual void OnSyncScroll( int x, int y ) {} // called when scroll synchronzation is needed

	void InitDC(wxDC *dc);

	// zoom
	void Zoom( bool zoomIn );
	bool CanZoom( bool zoomIn );
	void SetZoom( int percent );
    int GetZoom( ) { return 100 * zoomNum / zoomDen; };
	// navigation
	void Next( bool forward );
	bool HasNext( bool forward );
	void Goto( );
	bool CanGoto( );
	void Goto( int nopage );
	// scroll
	void UpdateScroll(); // update scroll position if moving keys

	// set the tool panel ptr
	void SetToolPanel( MusToolPanel *toolpanel );
	// get the current tool
	int GetToolType( );
	void SetToolType( int type );
//	void SetNotationMode( bool mode );
	
	// simply returns the value of the last note-type element (mensural or neume)
	bool GetNotationMode();
	
	// convenience method that should be changed after refactoring
	bool MusWindow::IsNoteSelected();
	
	void SetInsertMode( bool insert );
	
	void SetKeyboardEntryMode( bool mode );
    
    // WDR: method declarations for MusWindow
	void SetFile( MusFile *file );
	/**
		Redimensionne la fenetre en fonction de la taille du parent et du zoom de la taille de la page
	  */
	void Resize( );
	/**
     Met ‡ jour la table des fontes en fonction de nTailleFont et du zoom
	 Doit etre appelee apres changement de zoom ou de definition de page
     */
	void UpdateZoomValues();
    /**
     Initialise les donnees de visualisation par page
     */
	void UpdatePageValues();
    /**
     Initialise les donnees de polices (music and lyrics)
     */
	void UpdateFontValues();
    /**
     Retourne la valeur correspondante de i pour le zoom qui a cours
     */
	int ToZoom( int i );
	/** inverse */
	int ToReel( int i );
	/** Zoom avec inversion des coordonnees */
	int ToZoomY( int i );
	/** inverse  */
	int ToReelY( int i );
	
	static void SwapY( int *y1, int *y2 ) { int tmp = *y1; *y1 = *y2; *y2 = tmp; }

	/** musgraph */
	void v_bline ( wxDC *dc, int y1, int y2, int x1, int nbr);
	void v_bline2 ( wxDC *dc, int y1, int y2, int x1, int nbr);
	void h_bline ( wxDC *dc, int x1, int x2, int y1, int nbr);
	void putfont ( wxDC *dc, int x, int y, unsigned char c, 
				  MusStaff *staff, int dimin, int font_flag );
	void putfontfast ( wxDC *dc, int x, int y, unsigned char c );
	void putstring ( wxDC *dc, int x, int y, wxString s, int centrer, int pTaille = 0);
	void putlyric ( wxDC *dc, int x, int y, wxString s, int pTaille = 0, bool cursor = false);
	void box( wxDC *dc, int x1, int y1, int x2, int y2);
	void rect_plein2( wxDC *dc, int x1, int y1, int x2, int y2);
	int hGrosseligne ( wxDC *dc, int x1, int y1, int x2, int y2, int decal);
	int pointer ( wxDC *dc, int x, int b, int decal, MusStaff *staff );
	/** musbezier */
	static int CC(int ,int );
	static long BBlend(int ,int ,long );
	static int InitBezier(int );
	static void Bezier(long *,long *,long ,int );
	static void calcBez ( wxPoint *ptcoord, int _nbint );
	static void pntswap (wxPoint *x1, wxPoint *x2);

    
public:
    // WDR: member variable declarations for MusWindow
    /** Fichier  */
    MusFile *m_f;
    /** FileHeader du fichier contenant la page */
    MusFileHeader *m_fh;
    /** Page affichee */
    MusPage *m_page;
    /** No Page affichee */
    int m_npage;
    /** valeur du pas (10 par defaut) */
    int _pas;
    /** valeur du bond (60 par defaut) */
    int _bond;
    /** valeur du pas3 (3 * pas par defaut) */
    int _pas3;
    /** valeurs d'un espace (demi-note) grand et petit (10 et 8 par defaut)*/
    int _espace[2];
    /** valeurs d'un interligne grand et petit (20 et 16 par defaut)*/
    int _interl[2];
    /** valeurs d'une portee grande et petite (80 et 64 par defaut)*/
    int _portee[2];
    /** valeurs d'une octave grand et petit (70 et 56 par defaut)*/
    int _octave[2];
    /** hauteur de la fonte (100 par defaut) */
    int hautFont;
	int hautFontCorr[2][2]; // avec correction par platforme
    /** rapport entre grande et petite portees (16 / 20 par defaut) */
    int RapportPortee[2];
    /** rapport en element normal et element diminue (3 / 4 par defaut)*/
    int RapportDimin[2];
    /** valeurs d'une barre de valeur grande et petite (10 et 6 par defaut)*/
    int DELTANbBAR[2];
    /** valeurs d'un espace blanc entre les barres de valeur grand et petit (6 et 4 par defaut)*/
    int DELTABLANC[2];
    /** tailles des fontes normales et diminues sur des portees grandes et petites */
    int nTailleFont[2][2];
    /** tailles des notes normales et diminues sur des portees grandes et petites */
    int rayonNote[2][2];
    /** tailles des lignes aditionnelles normales et diminues sur des portees grandes et petites */
    int ledgerLine[2][3];
    /** tailles des fontes normales et diminues sur des portees grandes et petites */
    int largeurBreve[2];
    /** tailles des alterations normales et diminues sur des portees grandes et petites */
    int largAlter[2][2];
    /** ???? */
    float v_unit[2];
    /** ???? */
    float v4_unit[2];
    /** ???? */
    int DELTABAR;
    /** contient la positions des clefs */
    MusPosKey kPos[MAXPORTNBRE+1];
    /** fontes actuelles mises a jour selon la taille du zoom */
    wxFont m_activeFonts[2][2];				
    /** fonte Leipzig par defaut */
    wxFont m_ftLeipzig;
	
	wxFont m_activeChantFonts[2][2];
	/** FestaDiesA for neume notation */
	wxFont m_ftFestaDies;
    /** fontes actuelles mises a jour selon la taille du zoom */
    wxFont m_activeLyricFonts[2];
    /** fonte Leipzig par defaut */
    wxFont m_ftLyrics;
	
	

	/** format max utile; en principe, celui de la feuille **/
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


    /** valeur du numerateur de zoom */
    int zoomNum;
    /** denominateur du zoom */
    int zoomDen;
    /** indique si la definition de page poue laquelle fontes actuelles est a jour */
    int m_charDefin;
	/** indique si la page doit etre centree asi elle n'occupe pas toute la fenetre */
	bool m_center;

	
	wxString m_str;
	wxBitmap m_fontBitmaps[256][2][2];
	wxMemoryDC m_memDC;

	const wxColour *m_currentColour;
	wxColour m_black;
	MusElement *m_currentElement;
	MusElement *m_newElement;
	MusElement *m_bufferElement;
	MusElement *m_lastEditedElement;

	// keep elements statically - m_newElement will point on them
	MusNote m_note;
	MusSymbol m_symbol;
	MusNeume m_neume;

	MusStaff *m_currentStaff;
	bool efface;
	bool m_editElement; // edition mode or insertion mode
	bool m_notation_mode; // neumes or mensural notation mode
	int m_insertx;
	int m_insertcode;
	int m_insertoct;
	int m_dragging_x;
	int m_dragging_y_offset;
	bool m_has_been_dragged;
	MusToolPanel *m_toolpanel;
	bool m_lyricMode;
	bool m_inputLyric;
	int m_lyricCursor;
	
	// keyboard entry mode
	bool m_keyEntryMode;
	MusKeyboardEditor *m_keyEditor;

    
private:
	void SetPage( MusPage *page );
	/*
     Calcul la taille de la page = calculFormatPapier()
     */
	void PaperSize( );

	void UpdateStavesPos();

	void SyncToolPanel(); // synchronize tool dlg (m_currentElement, mode ... )


	bool KeyboardEntry(wxKeyEvent &event);	//parse key input and send MIDI messages
	
    // WDR: handler declarations for MusWindow
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
