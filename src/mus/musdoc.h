/////////////////////////////////////////////////////////////////////////////
// Name:        musdoc.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_DOC_H__
#define __MUS_DOC_H__

#include "mus.h"
#include "musdc.h"
#include "musdef.h"
#include "musio.h"
#include "musobject.h"

class MusPage;

enum DocType {
    Raw = 0,
    Rendering,
    Transcription
};


//----------------------------------------------------------------------------
// MusDoc
//----------------------------------------------------------------------------

/** 
 * This class is a hold the data and corresponds to the model of a MVC design pattern.
 */
class MusDoc: public MusObject
{
	//friend class MusFileOutputStream;
	friend class MusFileInputStream;

public:
    // constructors and destructors
    MusDoc();
    virtual ~MusDoc();
	
	void AddPage( MusPage *page );
    
    /*
     * Clear the content of the document.
     */ 
    void Reset( DocType type );
    
    /*
     * Clear the paper size of the document.
     */ 
    void ResetPaperSize( );
    
    /**
     * Refreshes the views from MusDoc.
     */
    virtual void Refresh();
    
    /**
     * This method reset the MusLayerElement, MusSection and MusMeasure pointers and check the elements exist.
     * It has to be call when a layout (pages) was copied from another file in memory
     * because we need the MusLayerElement, MusSection and MusMeasure pointers to be redirected to the correct object.
     * The methods find the object in the logical tree with the UUID.
     * The MusLayerElement is deleted from the layout if the MusLayerElement is not found.
     * Empty MusLayer, MusStaff, MusSystem etc. objects remain.
     */
    
    virtual int Save( MusFileOutputStream *output );
    
    // moulinette
    void GetNumberOfVoices( int *min_voice, int *max_voice );
    MusStaff *GetVoice( int i );
    
public:
    /** nom complet du fichier */
    std::string m_fname;
    /** Page width (MEI scoredef@page.width) - currently not saved */
    int m_pageWidth;
    /** Page height (MEI scoredef@page.height) - currently not saved */
    int m_pageHeight;
    /** Page left margin (MEI scoredef@page.leftmar) - currently not saved */
    short m_pageLeftMar;
    /** Page right margin (MEI scoredef@page.rightmar) - currently not saved */
    short m_pageRightMar;
    /** Page top margin (MEI scoredef@page.topmar) - currently not saved */
    short m_pageTopMar;
    
    MusEnv m_env;
    
public:
    
    DocType GetType() { return m_type; };
    
    /** The parent MusDoc setter */
    void SetDoc( MusDoc *doc );
    /* Claculate spacing in the music */
    void Layout( bool trim = false );
    
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
    
	int GetPageCount() const { return (int)m_children.size(); };
    
    // functors
    virtual int Save( ArrayPtrVoid params );
    
private:
    // method calculating the font size
    int CalcMusicFontSize( );
    int CalcNeumesFontSize( );
    
public:
    
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
	//int m_pageWidth;
    //int m_pageHeight;
	//int m_pageLeftMar;
    //int m_pageRightMar;
    ///@}
    
    /** The source id */
    std::string m_source;
	
    //int mesureNum, mesureDen;
	//float MesVal;
    
    /** indique si la definition de page poue laquelle fontes actuelles est a jour */
    int m_charDefin;
    
    
private:
    DocType m_type;
    
	
};


#endif
