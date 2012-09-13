/////////////////////////////////////////////////////////////////////////////
// Name:        musrc.h
// Author:      Laurent Pugin
// Created:     2010
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_RENDERER_H__
#define __MUS_RENDERER_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/dynarray.h"

#include "musbeam.h"
#include "musdef.h"
#include "musdc.h"

class MusLayout;
class MusPage;
class MusSystem;
class MusLaidOutStaff;
class MusLaidOutLayer;
class MusLaidOutLayerElement;

//----------------------------------------------------------------------------
// MusRC
//----------------------------------------------------------------------------

/** 
 * This class is a rendering context and corresponds to the view of a MVC design pattern.
 */
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
    virtual void DoLyricCursor( int x, int y, MusDC *dc, wxString lyric ) {}
    virtual void DoReset() {}

	// navigation
	void Next( bool forward );
	bool HasNext( bool forward );
	void LoadPage( int nopage );
	
	// simply returns the value of the last note-type element (mensural or neume)
	bool GetNotationMode();
	
	// convenience method that should be changed after refactoring
	bool IsNoteSelected();
	    
    void SetLayout( MusLayout *layout );
    
    /** x value in the Renderer */
	int ToRendererX( int i );
	/** x value in the Logical world */
	int ToLogicalX( int i );
	/** y value in the Renderer */
	int ToRendererY( int i );
	/** y value in the Logical world  */
	int ToLogicalY( int i );
	
	static void SwapY( int *y1, int *y2 ) { int tmp = *y1; *y1 = *y2; *y2 = tmp; }
    
	void SetPage( MusPage *page );

	/* musrc_graph.cpp */
	void v_bline ( MusDC *dc, int y1, int y2, int x1, int nbr);
	void v_bline2 ( MusDC *dc, int y1, int y2, int x1, int nbr);
	void h_bline ( MusDC *dc, int x1, int x2, int y1, int nbr);
	void festa_string ( MusDC *dc, int x, int y, const wxString str, 
					   MusLaidOutStaff *staff, int dimin ); 
	void DrawLeipzigFont ( MusDC *dc, int x, int y, unsigned char c, 
				  MusLaidOutStaff *staff, bool dimin );
    void DrawTieBezier(MusDC *dc, int x, int y, int x1, int height, int width, bool direction);
	//void putfontfast ( MusDC *dc, int x, int y, unsigned char c );
	void putstring ( MusDC *dc, int x, int y, wxString s, int centrer, int staffSize = 0);
	void putlyric ( MusDC *dc, int x, int y, wxString s, int staffSize = 0, bool cursor = false);
	void box( MusDC *dc, int x1, int y1, int x2, int y2);
	void rect_plein2( MusDC *dc, int x1, int y1, int x2, int y2);
	int hGrosseligne ( MusDC *dc, int x1, int y1, int x2, int y2, int decal);
	int DrawDot ( MusDC *dc, int x, int b, int decal, MusLaidOutStaff *staff );
	/* musrc_bezier.cpp */
	static int CC(int ,int );
	static long BBlend(int ,int ,long );
	static int InitBezier(int );
	static void Bezier(long *,long *,long ,int );
	static void calcBez ( MusPoint *ptcoord, int _nbint );
	static void pntswap (MusPoint *x1, MusPoint *x2);
    
    /* musrc_page.cpp */
	void DrawPage( MusDC *dc, MusPage *page, bool background = true );
    void DrawSystem( MusDC *dc, MusSystem *system );
	void DrawGroups( MusDC *dc, MusSystem *system );
	void DrawBracket ( MusDC *dc, MusSystem *system, int x, int y1, int y2, int cod, int staffSize);
	void DrawBrace ( MusDC *dc, MusSystem *system, int x, int y1, int y2, int staffSize);
	void DrawBarline ( MusDC *dc, MusSystem *system, int x, int cod, bool porteeAutonome, MusLaidOutStaff *pportee);
	void DrawSpecialBarline( MusDC *dc, MusSystem *system, int x, BarlineType code, bool porteeAutonome, MusLaidOutStaff *pportee);
	void DrawPartialBarline ( MusDC *dc, MusSystem *system, int x, MusLaidOutStaff *pportee);
	void DrawStaff( MusDC *dc, MusLaidOutStaff *staff, MusSystem *system );
	void DrawStaffLines( MusDC *dc, MusLaidOutStaff *staff, MusSystem *system );
    int CalculatePitchPosY ( MusLaidOutStaff *staff, char pname, int dec_clef, int oct);
	int CalculateNeumePosY ( MusLaidOutStaff *staff, char note, int dec_clef, int oct);
    int CalculateRestPosY ( MusLaidOutStaff *staff, char duration);
	void DrawLayer( MusDC *dc, MusLaidOutLayer *layer, MusLaidOutStaff *staff );
	void DrawSlur( MusDC *dc, MusLaidOutLayer *layer, int x1, int y1, int x2, int y2, bool up, int height = -1);
    int CalculatePitchCode ( MusLaidOutLayer *layer, int y_n, int x_pos, int *octave );
    /* musrc_element.cpp */
    void DrawAcciaccaturaSlash(MusDC *dc, MusLaidOutLayerElement *element);
    void DrawElement( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff );
    void DrawDurationElement( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff );
    void DrawBeamElement(MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawBarline( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff );  
    void DrawClef( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff );
    void DrawMensur( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff );
    void DrawFermata(MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutStaff *staff);
    void DrawMensurCircle( MusDC *dc, int x, int yy, MusLaidOutStaff *staff );
    void DrawMensurDot( MusDC *dc, int x, int yy, MusLaidOutStaff *staff ); 
    void DrawMensurFigures( MusDC *dc, int x, int y, int num, int numBase, MusLaidOutStaff *staff); 
    void DrawMensurHalfCircle( MusDC *dc, int x, int yy, MusLaidOutStaff *staff );
    void DrawMensurReversedHalfCircle( MusDC *dc, int x, int yy, MusLaidOutStaff *staff ); 
    void DrawMensurSlash( MusDC *dc, int x, int yy, MusLaidOutStaff *staff );  
    void DrawNote( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff, bool inBeam = false );  
    void DrawRest( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff );
    void DrawSymbol( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff, MusLaidOutLayerElement *parent = NULL );
    void DrawSymbolAccid( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff );
    void DrawSymbolCustos( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff );
    void DrawSymbolDot( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff );
    void DrawTie( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff );
    void DrawTuplet( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawLigature( MusDC *dc, int y, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff );  
    void DrawLedgerLines( MusDC *dc, int y_n, int y_p, int xn, unsigned int smaller, int staffSize);
    void DrawSpecialRest ( MusDC *dc, int a, MusLaidOutLayerElement *element, MusLaidOutStaff *staff);
    void DrawLongRest ( MusDC *dc, int a, int b, MusLaidOutStaff *staff);
    void DrawBreveRest ( MusDC *dc, int a, int b, MusLaidOutStaff *staff);
    void DrawWholeRest ( MusDC *dc, int a, int b, int valeur, unsigned char dots, unsigned int smaller, MusLaidOutStaff *staff);
    void DrawQuarterRest ( MusDC *dc, int a, int b, int valeur, unsigned char dots, unsigned int smaller, MusLaidOutStaff *staff);
    void DrawDots ( MusDC *dc, int x1, int y1, int offy, unsigned char dots, MusLaidOutStaff *staff );
    void CalculateLigaturePosX ( MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawKeySig( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff );
    /* musrc_beam.cpp */
    void DrawBeam(  MusDC *dc, MusLaidOutLayer *layer, MusBeam *beam, MusLaidOutStaff *staff );
    /* musrc_beam_original.cpp */
    void DrawBeamOriginal(  MusDC *dc, MusLaidOutLayer *layer, MusLaidOutStaff *staff );
    
    /* musrc_neumes.cpp - musneume */
    void DrawNeume( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void NeumeLine( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff, int x1, int x2, int y1, int y2);
    void DrawAncus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawCephalicus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawPunctum( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawPunctumInclinatum( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawVirga( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawVirgaLiquescent( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawPodatus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawClivis( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawEpiphonus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawPorrectus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawPorrectusFlexus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawSalicus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawScandicus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawScandicusFlexus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawTorculus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawTorculusLiquescent( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawTorculusResupinus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawCompound( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawCustos( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
	void DrawNeumeDots(MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
    void DrawNeumeLedgerLines( MusDC *dc, int y_n, int y_p, int xn, unsigned int smaller, int staffSize);
	/* musrc_neumes.cpp - musneumesymbol */
    void DrawNeumeSymbol(MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
	void DrawNeumeClef(MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff);
	void DrawComma(MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutStaff *staff, bool cueSize);
	void DrawFlat(MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutStaff *staff, bool cueSize);
	void DrawNatural(MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutStaff *staff, bool cueSize);
	void DrawDivFinal(MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutStaff *staff, bool cueSize);
	void DrawDivMajor(MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutStaff *staff, bool cueSize);
	void DrawDivMinor(MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutStaff *staff, bool cueSize);
	void DrawDivSmall(MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutStaff *staff, bool cueSize);
    
public:
    /** Layout */
    MusLayout *m_layout;
    /** Page affichee */
    MusPage *m_page;
    /** No Page affichee */
    int m_npage;
	/** format max utile; en principe, celui de la feuille **/
	//int m_pageMaxY, m_pageMaxX;

	wxString m_str;
    
	// static
	static MusPoint point_[4];
	static MusPoint bcoord[2*(BEZIER_NB_POINTS+1)];
    
	int discontinu;

	//const wxColour *m_currentColour;
    int m_currentColour;
    
    // element currently selected
	MusLaidOutLayerElement *m_currentElement;
    MusLaidOutLayer *m_currentLayer;
	MusLaidOutStaff *m_currentStaff;
    MusSystem *m_currentSystem;
    
	int m_notation_mode; // neumes or mensural notation mode
	bool m_lyricMode;
	bool m_inputLyric;
	MusEditorMode m_editorMode; // Edit or insert
	    
private:
	void UpdateStavesPos();

private:

    // static for ligatures
    static int s_drawingLigX[2], s_drawingLigY[2];	// pour garder coord. des ligatures    
    static bool s_drawingLigObliqua;	// marque le 1e passage pour une oblique

};



#endif
