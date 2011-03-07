/////////////////////////////////////////////////////////////////////////////
// Name:        musrc.h
// Author:      Laurent Pugin
// Created:     2010
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_RENDERER_H__
#define __MUS_RENDERER_H__

#ifdef __GNUG__
    #pragma interface "musrc.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/dynarray.h"

#include "mus.h"

#include "app/axdc.h"

class MusPage;
class MusFile;
class MusElement;
class MusStaff;

#include "musnote.h"
#include "musneume.h"
#include "mussymbol.h"

//----------------------------------------------------------------------------
// MusRC
//----------------------------------------------------------------------------

class MusRC
{
public:
    // constructors and destructors
    MusRC();
	virtual ~MusRC();

	// edition
    virtual void OnBeginEdition() {}
    virtual void OnEndEdition() {}
    virtual void OnBeginEditionClef() {}
    virtual void OnEndEditionClef() {}
    virtual void DoRefresh() {}
    virtual void DoResize() {}
    virtual void DoLyricCursor( int x, int y, AxDC *dc, wxString lyric ) {}
    virtual void DoReset() {}

	// navigation
	void Next( bool forward );
	bool HasNext( bool forward );
	void LoadPage( int nopage );
	
	// simply returns the value of the last note-type element (mensural or neume)
	bool GetNotationMode();
	
	// convenience method that should be changed after refactoring
	bool IsNoteSelected();
	    
    void SetFile( MusFile *file );
	/**
     Met ‡ jour la table des fontes en fonction de la definition de page
	 Doit etre appelee apres changement de definition de page
     */
	void UpdatePageFontValues();
    /**
     Initialise les donnees de visualisation par page
     */
	virtual void UpdatePageValues();
    /**
     Initialise les donnees de polices (music and lyrics)
     */
	virtual void UpdateFontValues();
    
    /** x value in the Renderer */
	int ToRendererX( int i ) { return i; }; // the same
	/** x value in the Logical world */
	int ToLogicalX( int i )  { return i; };
	/** y value in the Renderer */
	int ToRendererY( int i )  { return m_pageMaxY - i; }; // flipped
	/** y value in the Logical world  */
	int ToLogicalY( int i )  { return m_pageMaxY - i; }; // flipped
	
	static void SwapY( int *y1, int *y2 ) { int tmp = *y1; *y1 = *y2; *y2 = tmp; }
    
	void SetPage( MusPage *page );
	/*
     Calcul la taille de la page = calculFormatPapier()
     */
	void PaperSize( );


	/** musgraph */
	void v_bline ( AxDC *dc, int y1, int y2, int x1, int nbr);
	void v_bline2 ( AxDC *dc, int y1, int y2, int x1, int nbr);
	void h_bline ( AxDC *dc, int x1, int x2, int y1, int nbr);
	void festa_string ( AxDC *dc, int x, int y, const wxString str, 
					   MusStaff *staff, int dimin ); 
	void putfont ( AxDC *dc, int x, int y, unsigned char c, 
				  MusStaff *staff, int dimin, int font_flag );
	//void putfontfast ( AxDC *dc, int x, int y, unsigned char c );
	void putstring ( AxDC *dc, int x, int y, wxString s, int centrer, int pTaille = 0);
	void putlyric ( AxDC *dc, int x, int y, wxString s, int pTaille = 0, bool cursor = false);
	void box( AxDC *dc, int x1, int y1, int x2, int y2);
	void rect_plein2( AxDC *dc, int x1, int y1, int x2, int y2);
	int hGrosseligne ( AxDC *dc, int x1, int y1, int x2, int y2, int decal);
	int pointer ( AxDC *dc, int x, int b, int decal, MusStaff *staff );
	/** musbezier */
	static int CC(int ,int );
	static long BBlend(int ,int ,long );
	static int InitBezier(int );
	static void Bezier(long *,long *,long ,int );
	static void calcBez ( AxPoint *ptcoord, int _nbint );
	static void pntswap (AxPoint *x1, AxPoint *x2);
    
    // method calculating the font size
    int CalcMusicFontSize( );
    int CalcNeumesFontSize( );
    
public:
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
	int hautFontAscent[2][2]; // avec correction par platforme
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
    MusPosClef kPos[MAXPORTNBRE+1];
    /** fontes actuelles mises a jour selon la taille du zoom */
    AxFontInfo m_activeFonts[2][2];				
    /** fonte Leipzig par defaut */
    AxFontInfo m_ftLeipzig;
	
	AxFontInfo m_activeChantFonts[2][2];
	/** FestaDiesA for neume notation */
	AxFontInfo m_ftFestaDiesA;
    /** fontes actuelles mises a jour selon la taille du zoom */
    AxFontInfo m_activeLyricFonts[2];
    /** fonte Leipzig par defaut */
    AxFontInfo m_ftLyrics;
	
	

	/** format max utile; en principe, celui de la feuille **/
	int m_pageMaxY, m_pageMaxX;
	float beamPenteMin, beamPenteMx;
	int pageFormatHor, pageFormatVer;
	//int margeMorteHor, margeMorteVer;
	//int portNoIndent, portIndent;
	int mrgG;
	int discontinu;
	int mesureNum, mesureDen;
	float MesVal;
	// static
	static AxPoint point_[4];
	static AxPoint bcoord[2*(PTCONTROL+1)];

    /** indique si la definition de page poue laquelle fontes actuelles est a jour */
    int m_charDefin;

	wxString m_str;

	//const wxColour *m_currentColour;
    int m_currentColour;
	MusElement *m_currentElement;

	MusStaff *m_currentStaff;
	int m_notation_mode; // neumes or mensural notation mode
	bool m_lyricMode;
	bool m_inputLyric;
	bool m_editElement; // edition mode or insertion mode
	bool m_eraseElement;
	    
private:
	void UpdateStavesPos();

private:

};



#endif
