/////////////////////////////////////////////////////////////////////////////
// Name:        musrc.h
// Author:      Laurent Pugin
// Created:     2010
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_RENDERER_H__
#define __MUS_RENDERER_H__

#include "musbeam.h"
#include "musdc.h"
#include "musdef.h"
#include "mustie.h"
#include "mustuplet.h"

class MusDoc;
class MusLayer;
class MusLayerElement;
class MusPage;
class MusStaff;
class MusSystem;

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
    virtual void DoLyricCursor( int x, int y, MusDC *dc, std::string lyric ) {}
    virtual void DoReset() {}
    virtual void OnPageChange() {};

	// navigation
	void Next( bool forward );
	bool HasNext( bool forward );
	void LoadPage( int nopage );
	
	// simply returns the value of the last note-type element (mensural or neume)
	bool GetNotationMode();
	
	// convenience method that should be changed after refactoring
	bool IsNoteSelected();
	    
    void SetDoc( MusDoc *doc );
    
    /** x value in the Renderer */
	int ToRendererX( int i );
	/** x value in the Logical world */
	int ToLogicalX( int i );
	/** y value in the Renderer */
	int ToRendererY( int i );
	/** y value in the Logical world  */
	int ToLogicalY( int i );
	
	static void SwapY( int *y1, int *y2 ) { int tmp = *y1; *y1 = *y2; *y2 = tmp; }
    
    /**
     * Set the current page to *page.
     * This method is dangerous because it can potentially be a page that do not belong to the layout.
     * It should be check (currently not done)
     * A safer option would be to pass the page number.
     */
	void SetPage( MusPage *page );

	/* musrc_graph.cpp */
	void v_bline ( MusDC *dc, int y1, int y2, int x1, int nbr);
	void v_bline2 ( MusDC *dc, int y1, int y2, int x1, int nbr);
	void h_bline ( MusDC *dc, int x1, int x2, int y1, int nbr);
	void festa_string ( MusDC *dc, int x, int y, const std::string str, 
					   MusStaff *staff, int dimin ); 
	void DrawLeipzigFont ( MusDC *dc, int x, int y, unsigned char c, 
				  MusStaff *staff, bool dimin );
    void DrawTieBezier(MusDC *dc, int x, int y, int x1, int height, int width, bool direction);
	//void putfontfast ( MusDC *dc, int x, int y, unsigned char c );
	void putstring ( MusDC *dc, int x, int y, std::string s, int centrer, int staffSize = 0);
	void putlyric ( MusDC *dc, int x, int y, std::string s, int staffSize = 0, bool cursor = false);
	void box( MusDC *dc, int x1, int y1, int x2, int y2);
	void rect_plein2( MusDC *dc, int x1, int y1, int x2, int y2);
	int hGrosseligne ( MusDC *dc, int x1, int y1, int x2, int y2, int decal);
	int DrawDot ( MusDC *dc, int x, int b, int decal, MusStaff *staff );
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
	void DrawBarline ( MusDC *dc, MusSystem *system, int x, int cod, bool porteeAutonome, MusStaff *pportee);
	void DrawSpecialBarline( MusDC *dc, MusSystem *system, int x, BarlineType code, bool porteeAutonome, MusStaff *pportee);
	void DrawPartialBarline ( MusDC *dc, MusSystem *system, int x, MusStaff *pportee);
	void DrawStaff( MusDC *dc, MusStaff *staff, MusSystem *system );
	void DrawStaffLines( MusDC *dc, MusStaff *staff, MusSystem *system );
    int CalculatePitchPosY ( MusStaff *staff, char pname, int dec_clef, int oct);
	int CalculateNeumePosY ( MusStaff *staff, char note, int dec_clef, int oct);
    int CalculateRestPosY ( MusStaff *staff, char duration);
	void DrawLayer( MusDC *dc, MusLayer *layer, MusStaff *staff );
	void DrawSlur( MusDC *dc, MusLayer *layer, int x1, int y1, int x2, int y2, bool up, int height = -1);
    int CalculatePitchCode ( MusLayer *layer, int y_n, int x_pos, int *octave );
    /* musrc_element.cpp */
    void DrawAcciaccaturaSlash(MusDC *dc, MusLayerElement *element);
    void DrawElement( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );
    void DrawDurationElement( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );
    void DrawBeamElement(MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawBarline( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );  
    void DrawClef( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );
    void DrawMensur( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );
    void DrawFermata(MusDC *dc, MusLayerElement *element, MusStaff *staff);
    void DrawMensurCircle( MusDC *dc, int x, int yy, MusStaff *staff );
    void DrawMensurDot( MusDC *dc, int x, int yy, MusStaff *staff ); 
    void DrawMensurFigures( MusDC *dc, int x, int y, int num, int numBase, MusStaff *staff); 
    void DrawMensurHalfCircle( MusDC *dc, int x, int yy, MusStaff *staff );
    void DrawMensurReversedHalfCircle( MusDC *dc, int x, int yy, MusStaff *staff ); 
    void DrawMensurSlash( MusDC *dc, int x, int yy, MusStaff *staff );  
    void DrawNote( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );  
    void DrawRest( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );
    void DrawSymbol( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff, MusLayerElement *parent = NULL );
    void DrawSymbolAccid( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );
    void DrawSymbolCustos( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );
    void DrawSymbolDot( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );
    void DrawTie( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );
    void DrawTupletElement( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );
    void DrawTuplet( MusDC *dc, MusTuplet *tuplet, MusLayer *layer, MusStaff *staff);
    void DrawTrill(MusDC *dc, MusLayerElement *element, MusStaff *staff);
    void DrawLigature( MusDC *dc, int y, MusLayerElement *element, MusLayer *layer, MusStaff *staff );  
    void DrawLedgerLines( MusDC *dc, int y_n, int y_p, int xn, unsigned int smaller, int staffSize);
    void DrawSpecialRest ( MusDC *dc, int a, MusLayerElement *element, MusStaff *staff);
    void DrawLongRest ( MusDC *dc, int a, int b, MusStaff *staff);
    void DrawBreveRest ( MusDC *dc, int a, int b, MusStaff *staff);
    void DrawWholeRest ( MusDC *dc, int a, int b, int valeur, unsigned char dots, unsigned int smaller, MusStaff *staff);
    void DrawQuarterRest ( MusDC *dc, int a, int b, int valeur, unsigned char dots, unsigned int smaller, MusStaff *staff);
    void DrawDots ( MusDC *dc, int x1, int y1, int offy, unsigned char dots, MusStaff *staff );
    void CalculateLigaturePosX ( MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawKeySig( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );
    void DrawLayerApp( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );
    /* musrc_beam.cpp */
    void DrawBeam(  MusDC *dc, MusLayer *layer, MusBeam *beam, MusStaff *staff );
    /* musrc_beam_original.cpp */
    void DrawBeamOriginal(  MusDC *dc, MusLayer *layer, MusStaff *staff );
    
    /* musrc_neumes.cpp - musneume */
    void DrawNeume( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void NeumeLine( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff, int x1, int x2, int y1, int y2);
    void DrawAncus( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawCephalicus( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawPunctum( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawPunctumInclinatum( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawVirga( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawVirgaLiquescent( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawPodatus( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawClivis( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawEpiphonus( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawPorrectus( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawPorrectusFlexus( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawSalicus( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawScandicus( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawScandicusFlexus( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawTorculus( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawTorculusLiquescent( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawTorculusResupinus( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawCompound( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawCustos( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
	void DrawNeumeDots(MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawNeumeLedgerLines( MusDC *dc, int y_n, int y_p, int xn, unsigned int smaller, int staffSize);
	/* musrc_neumes.cpp - musneumesymbol */
    void DrawNeumeSymbol(MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
	void DrawNeumeClef(MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff);
	void DrawComma(MusDC *dc, MusLayerElement *element, MusStaff *staff, bool cueSize);
	void DrawFlat(MusDC *dc, MusLayerElement *element, MusStaff *staff, bool cueSize);
	void DrawNatural(MusDC *dc, MusLayerElement *element, MusStaff *staff, bool cueSize);
	void DrawDivFinal(MusDC *dc, MusLayerElement *element, MusStaff *staff, bool cueSize);
	void DrawDivMajor(MusDC *dc, MusLayerElement *element, MusStaff *staff, bool cueSize);
	void DrawDivMinor(MusDC *dc, MusLayerElement *element, MusStaff *staff, bool cueSize);
	void DrawDivSmall(MusDC *dc, MusLayerElement *element, MusStaff *staff, bool cueSize);
    
private:
	void UpdateStavesPos();
    
public:
    /** Document */
    MusDoc *m_doc;
    /** Page affichee */
    MusPage *m_page;
    /** No Page affichee */
    int m_npage;
	/** format max utile; en principe, celui de la feuille **/
	//int m_pageMaxY, m_pageMaxX;

	std::string m_str;
    
	// static
	static MusPoint point_[4];
	static MusPoint bcoord[2*(BEZIER_NB_POINTS+1)];
    
	int discontinu;

	//const wxColour *m_currentColour;
    int m_currentColour;
    
    // element currently selected
	MusLayerElement *m_currentElement;
    MusLayer *m_currentLayer;
	MusStaff *m_currentStaff;
    MusSystem *m_currentSystem;
    
	int m_notation_mode; // neumes or mensural notation mode
	bool m_lyricMode;
	bool m_inputLyric;
	MusEditorMode m_editorMode; // Edit or insert
    
private:

    // static for ligatures
    static int s_drawingLigX[2], s_drawingLigY[2];	// pour garder coord. des ligatures    
    static bool s_drawingLigObliqua;	// marque le 1e passage pour une oblique


};



#endif
