/////////////////////////////////////////////////////////////////////////////
// Name:        musrc.h
// Author:      Laurent Pugin
// Created:     2010
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_RENDERER_H__
#define __MUS_RENDERER_H__

#include <typeinfo>

#include "musdc.h"
#include "musdef.h"
#include "musscoredef.h"

class MusBeam;
class MusBarline;
class MusDoc;
class MusLayer;
class MusLayerElement;
class MusMeasure;
class MusPage;
class MusStaff;
class MusSystem;
class Tie;
class Tuplet;

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
    void DrawTieBezier(MusDC *dc, int x, int y, int x1, bool direction);
	//void putfontfast ( MusDC *dc, int x, int y, unsigned char c );
	void putstring ( MusDC *dc, int x, int y, std::string s, int centrer, int staffSize = 0);
	void putlyric ( MusDC *dc, int x, int y, std::string s, int staffSize = 0, bool cursor = false);
	void box( MusDC *dc, int x1, int y1, int x2, int y2);
	void rect_plein2( MusDC *dc, int x1, int y1, int x2, int y2);
	int hGrosseligne ( MusDC *dc, int x1, int y1, int x2, int y2, int decal);
	int DoDrawDot ( MusDC *dc, int x, int y );
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
	void DrawScoreDef( MusDC *dc, MusScoreDef *scoreDef, MusMeasure *measure, int x, MusBarline *barline = NULL );
	void DrawStaffGrp( MusDC *dc, MusMeasure *measure, MusStaffGrp *staffGrp, int x );
	void DrawBracket ( MusDC *dc, int x, int y1, int y2, int staffSize);
	void DrawBrace ( MusDC *dc, int x, int y1, int y2, int staffSize);
    void DrawBarlines( MusDC *dc, MusMeasure *measure, MusStaffGrp *staffGrp, int x, MusBarline *barline );
    void DrawBarline( MusDC *dc, int x, int y_top, int y_bottom, MusBarline *barline );
	void DrawBarlineDots ( MusDC *dc, int x, MusStaff *staff, MusBarline *barline );
	void DrawPartialBarline ( MusDC *dc, MusSystem *system, int x, MusStaff *pportee);
    void DrawMeasure( MusDC *dc, MusMeasure *measure, MusSystem *system );
    void DrawStaff( MusDC *dc, MusStaff *staff, MusMeasure *measure, MusSystem *system );
	void DrawStaffLines( MusDC *dc, MusStaff *staff, MusMeasure *measure, MusSystem *system );
    int CalculatePitchPosY ( MusStaff *staff, char pname, int dec_clef, int oct);
	int CalculateNeumePosY ( MusStaff *staff, char note, int dec_clef, int oct);
    int CalculateRestPosY ( MusStaff *staff, char duration);
    void DrawLayer( MusDC *dc, MusLayer *layer, MusStaff *staff,  MusMeasure *measure );
    void DrawLayerList( MusDC *dc, MusLayer *layer, MusStaff *staff, MusMeasure *measure, const std::type_info *elementType );
	void DrawSlur( MusDC *dc, MusLayer *layer, int x1, int y1, int x2, int y2, bool up, int height = -1);
    int CalculatePitchCode ( MusLayer *layer, int y_n, int x_pos, int *octave );
    
    /* musrc_element.cpp */
    /** @name Methods for drawing MusLayerElement containing other elements */
    ///@{
    void DrawElement( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusMeasure *measure, MusStaff *staff );
    void DrawBeamElement(MusDC *dc, MusLayerElement *element, MusLayer *layer, MusMeasure *measure, MusStaff *staff);
    void DrawTupletElement( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusMeasure *measure, MusStaff *staff );
    void DrawLayerApp( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusMeasure *measure, MusStaff *staff );
    ///@}
    
    void DrawDurationElement( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );
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
    void DrawMultiRest( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );
    void DrawNote( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );  
    void DrawRest( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );
    void DrawSymbol( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff, MusLayerElement *parent = NULL );
    void DrawSymbolAccid( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );
    void DrawSymbolCustos( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );
    void DrawSymbolDot( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );
    void DrawTie( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff, MusMeasure *measure );
    void DrawTuplet( MusDC *dc, Tuplet *tuplet, MusLayer *layer, MusStaff *staff);
    void DrawTrill(MusDC *dc, MusLayerElement *element, MusStaff *staff);
    void DrawLigature( MusDC *dc, int y, MusLayerElement *element, MusLayer *layer, MusStaff *staff );  
    void DrawLedgerLines( MusDC *dc, int y_n, int y_p, int xn, unsigned int smaller, int staffSize);
    void DrawLongRest ( MusDC *dc, int a, int b, MusStaff *staff);
    void DrawBreveRest ( MusDC *dc, int a, int b, MusStaff *staff);
    void DrawWholeRest ( MusDC *dc, int a, int b, int valeur, unsigned char dots, unsigned int smaller, MusStaff *staff);
    void DrawQuarterRest ( MusDC *dc, int a, int b, int valeur, unsigned char dots, unsigned int smaller, MusStaff *staff);
    void DrawDots ( MusDC *dc, int x1, int y1, int offy, unsigned char dots, MusStaff *staff );
    void CalculateLigaturePosX ( MusLayerElement *element, MusLayer *layer, MusStaff *staff);
    void DrawAcciaccaturaSlash(MusDC *dc, MusLayerElement *element);
    void DrawKeySig( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff );
    /* musrc_beam.cpp */
    void DrawBeam(  MusDC *dc, MusLayer *layer, MusBeam *beam, MusStaff *staff );
    
private:
    
    //Used for calculating tuplets
    bool GetTupletCoordinates(Tuplet* tuplet, MusLayer *layer, MusPoint* start, MusPoint* end, MusPoint *center);
    std::string IntToObliqueFigures(unsigned int number);
    bool OneBeamInTuplet(Tuplet* tuplet);
    
public:
    /** Document */
    MusDoc *m_doc;
    /** Page affichee */
    MusPage *m_page;
    /** No Page affichee */
    int m_npage;

	std::string m_str;
    
	// static
	static MusPoint point_[4];
	static MusPoint bcoord[2*(BEZIER_NB_POINTS+1)];

    int m_currentColour;
    
    // element currently selected
	MusLayerElement *m_currentElement;
    MusLayer *m_currentLayer;
    MusMeasure *m_currentMeasure;
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

    MusScoreDef m_drawingScoreDef;
    

};



#endif
