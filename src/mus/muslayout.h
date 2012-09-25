/////////////////////////////////////////////////////////////////////////////
// Name:        layout.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_LAYOUT_H__
#define __MUS_LAYOUT_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "musobject.h"
#include "musdc.h"

class MusScore;
//class MusPage;

enum LayoutType {
    Raw = 0,
    Rendering,
    Transcription
};


//----------------------------------------------------------------------------
// MusLayout
//----------------------------------------------------------------------------

/**
 * This class represents a laid-out score.
 * A MusLayout is contained in a MusDoc.
 * It contains MusPage objects.
*/
class MusLayout: public MusLayoutObject
{
public:
    // constructors and destructors
    MusLayout( LayoutType type );
    virtual ~MusLayout();
    
    virtual bool Check();
    
    virtual wxString MusClassName( ) { return "MusLayout"; };	    
	
	void AddPage( MusPage *page );
    
    void Clear();
    
    LayoutType GetType() { return m_type; };
    
    /** The parent MusDoc setter */
    void SetDoc( MusDoc *doc );
	/** Realize the layout */
	void Realize( MusScore *score );
    /* Claculate spacing in the music */
    void SpaceMusic();
    
    void RefreshViews( ) {};
	
	/**
     * Set the paper size for the layout (when drawing it)
     * By default, the page size of the document is taken.
     * If a page is given, the size of the page is taken.
     * calculFormatPapier() in Wolfgang
     */
	void PaperSize( MusPage *page = NULL );
    /**
     Initialise les donnees de visualisation par page
     */
	virtual void UpdatePageValues();
    /**
     Initialise les donnees de polices (music and lyrics)
     */
	virtual void UpdateFontValues();
    
    /**
     * Return the right X position of the according to the page and system margins.
     */
    int GetSystemRightX( MusSystem *system );
    
	int GetPageCount() const { return (int)m_pages.GetCount(); };
    
    // moulinette
    virtual void Process(MusFunctor *functor, wxArrayPtrVoid params );
    // functors
    void Save( wxArrayPtrVoid params );
    void Load( wxArrayPtrVoid params );
    
private:
    // method calculating the font size
    int CalcMusicFontSize( );
    int CalcNeumesFontSize( );
    
public:
    /** The MusPage objects of the layout */
    ArrayOfMusPages m_pages;
    
    MusDoc *m_doc;
    
    MusEnv m_env;
    
    // Previously in MusRC
    /** Editor step (10 by default) */
    int m_step1;
    /** Editor medium step (3 * m_step1) */
    int m_step2;
    /** Editor large step (6 * m_step1) */
    int m_step3;
    /** Half a the space between to staff lines for normal and small staff (10 and 8 by default) */
    int m_halfInterl[2];
    /** Space between to staff lines for normal and small staff (20 and 16 by default) */
    int m_interl[2];
    /** Height of a five line staff for normal and small staff (80 and 64 by default) */
    int m_staffSize[2];
    /** Height of an octave for normal and small staff (70 and 56 by default) */
    int m_octaveSize[2];
    /** Font height (100 par defaut) */
    int m_fontHeight;
    /** Font height with ascent for normal and small staff and normal and grace size */
	int m_fontHeightAscent[2][2];
    /** Normal and small staff ration (16 / 20 by default) */
    int m_smallStaffRatio[2];
    /** Normal and grace size (3 / 4 by default) */
    int m_graceRatio[2];
    /** Height of a beam for normal and small staff (10 and 6 by default) */
    int m_beamWidth[2];
    /** Height of a beam spacing (white) for normal and small staff (10 and 6 by default) */
    int m_beamWhiteWidth[2];
    /** Current font height with ascent for normal and small staff and normal and grace size */
    int m_fontSize[2][2];
    /** Note radius for normal and small staff and normal and grace size */
    int m_noteRadius[2][2];
    /** Ledger line length for normal and small staff and normal and grace size */
    int m_ledgerLine[2][3];
    /** Brevis width for normal and small staff */
    int m_brevisWidth[2];
    /** Accident width for normal and small staff and normal and grace size */
    int m_accidWidth[2][2];
    /** A vertical unit corresponding to the 1/4 of an interline */
    float m_verticalUnit1[2];
    /** A vertical unit corresponding to the 1/8 of an interline */
    float m_verticalUnit2[2];
    /** Spacing between barlines */
    int m_barlineSpacing;
    /** Current Leipzig font for normal and small staff and normal and grace size */
    MusFontInfo m_activeFonts[2][2];				
    /** Leipzig font */
    MusFontInfo m_ftLeipzig;
    /** Current FestaDiesA font for normal and small staff and normal and grace size */
	MusFontInfo m_activeChantFonts[2][2];
	/** FestaDiesA font for neume notation */
	MusFontInfo m_ftFestaDiesA;
    /** Current lyric font for normal and small staff and normal and grace size */
    MusFontInfo m_activeLyricFonts[2];
    /** Lyric font by default */
    MusFontInfo m_ftLyrics;
	
	float m_beamMinSlope, m_beamMaxSlope;
    /** @name Variables for rendering
     * Variables for rendering used only at runtime.
     * These variable are currently not saved in the MEI file.
     * The are initialized either with the document ones or the page ones.
     * This happen in PaperSize().
     * Eventually, we could also store the in the layout as a default for all pages in each layout.
     */
    ///@{
	int m_pageWidth;
    int m_pageHeight;
	int m_pageLeftMar;
    int m_pageRightMar;
    ///@}
	
    //int mesureNum, mesureDen;
	//float MesVal;
    
    /** indique si la definition de page poue laquelle fontes actuelles est a jour */
    int m_charDefin;

    
private:
    LayoutType m_type;
    
};


//----------------------------------------------------------------------------
// MusLayoutFunctor
//----------------------------------------------------------------------------

/**
 * This class is a Functor that processes MusLayout objects.
 * Needs testing.
 */
class MusLayoutFunctor: public MusFunctor
{
private:
    void (MusLayout::*fpt)( wxArrayPtrVoid params );   // pointer to member function
    
public:
    
    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    MusLayoutFunctor( void(MusLayout::*_fpt)( wxArrayPtrVoid ) ) { fpt=_fpt; };
	virtual ~MusLayoutFunctor() {};
    
    // override function "Call"
    void Call( MusLayout *ptr, wxArrayPtrVoid params )
    { (*ptr.*fpt)( params );};          // execute member function
};


#endif
