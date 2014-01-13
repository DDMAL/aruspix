/////////////////////////////////////////////////////////////////////////////
// Name:        musdoc.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_DOC_H__
#define __MUS_DOC_H__

#include "vrv.h"
#include "devicecontext.h"
#include "vrvdef.h"
#include "io.h"
#include "object.h"
#include "scoredef.h"

namespace vrv {

class Page;    
    
enum DocType {
    Raw = 0,
    Rendering,
    Transcription
};


//----------------------------------------------------------------------------
// Doc
//----------------------------------------------------------------------------

/** 
 * This class is a hold the data and corresponds to the model of a MVC design pattern.
 */
class Doc: public Object
{

public:
    // constructors and destructors
    Doc();
    virtual ~Doc();
	
	void AddPage( Page *page );
    
    /*
     * Clear the content of the document.
     */ 
    void Reset( DocType type );
    
    /**
     * Refreshes the views from Doc.
     */
    virtual void Refresh();
    
    /**
     * Getter for the DocType.
     * The setter is Doc::Reset.
     */
    DocType GetType() { return m_type; };
    
    /**
     * @name Setters for the page dimensions and margins
     */
    ///@{
    void SetPageHeight( int pageHeight ) { m_pageHeight = pageHeight; };
    void SetPageWidth( int pageWidth ) { m_pageWidth = pageWidth; };
    void SetPageLeftMar( int pageLeftMar ) { m_pageLeftMar = pageLeftMar; };
    void SetPageRightMar( int pageRightMar ) { m_pageRightMar = pageRightMar; };
    void SetPageTopMar( int pageTopMar ) { m_pageTopMar = pageTopMar; };
    ///@}
    
    /**
     * Saves the document using the specified output stream.
     * Creates a functors that will parse the full tree.
     */
    virtual int Save( FileOutputStream *output );

    
    /**
     * Performs the layout of the entire document
     */
    void Layout( );
    
    /**
     * To be implemented.
     */
    void RefreshViews( ) {};
	
	/**
     * Set the paper size for the (when drawing it)
     * By default, the page size of the document is taken.
     * If a page is given, the size of the page is taken.
     * calculFormatPapier() in Wolfgang
     */
	void SetRendPage( Page *page );

    /**
     * Initializes fonts from the resources (music and lyrics)
     * The method is called when the doc is reset.
     */
	virtual void UpdateFontValues();
        
    //----------//
    // Functors //
    //----------//
    
    /**
     * Functor method for saving the page
     */
    virtual int Save( ArrayPtrVoid params );
    
private:
    /**
     * Calculates the Leipzig font size according to the m_interlDefin reference value.
     * The size is calculated using  LEIPZIG_UNITS_PER_EM and LEIPZIG_WHOLE_NOTE_HEAD_HEIGHT.
     * See musdef.h for more information about these values.
     */
    int CalcLeipzigFontSize( );
    
public:
    

    /** 
     * The full name of the file. Exact use to be clarified.
     */
    std::string m_fname;
    
    /**
     * The object with the default values.
     * This could be saved somewhere as preferences (todo).
     */
    MusEnv m_env;
    
    /** 
     * The source id. Exact use to be clarified.
     */
    std::string m_source;
    
    /**
     * Holds the top scoreDef.
     * In a standard MEI file, this is the <scoreDef> encoded before the first <section>.
     */
    ScoreDef m_scoreDef;
    
    /*
     * The following values are set in the Doc::SetRenderedPage.
     * They are all current values to be used when rendering a page in a View and 
     * reset for every page. However, most of them are based on the m_staffDefin values
     * and will remain the same. This can be optimized.
     * The pages dimensions and margins are based on the page ones, the document ones or 
     * the default in the following order and if available.
     */
  
    /** The page currently being rendered */
    Page *m_rendPage;
    /** Editor step (10 by default) */
    int m_rendStep1;
    /** Editor medium step (3 * m_rendStep1) */
    int m_rendStep2;
    /** Editor large step (6 * m_rendStep1) */
    int m_rendStep3;
    /** Half a the space between to staff lines for normal and small staff (10 and 8 by default) */
    int m_rendHalfInterl[2];
    /** Space between to staff lines for normal and small staff (20 and 16 by default) */
    int m_rendInterl[2];
    /** Height of a five line staff for normal and small staff (80 and 64 by default) */
    int m_rendStaffSize[2];
    /** Height of an octave for normal and small staff (70 and 56 by default) */
    int m_rendOctaveSize[2];
    /** Font height (100 par defaut) */
    int m_rendFontHeight;
    /** Font height with ascent for normal and small staff and normal and grace size */
	int m_rendFontHeightAscent[2][2];
    /** Normal and small staff ration (16 / 20 by default) */
    int m_rendSmallStaffRatio[2];
    /** Normal and grace size (3 / 4 by default) */
    int m_rendGraceRatio[2];
    /** Height of a beam for normal and small staff (10 and 6 by default) */
    int m_rendBeamWidth[2];
    /** Height of a beam spacing (white) for normal and small staff (10 and 6 by default) */
    int m_rendBeamWhiteWidth[2];
    /** Current font height with ascent for normal and small staff and normal and grace size */
    int m_rendFontSize[2][2];
    /** Note radius for normal and small staff and normal and grace size */
    int m_rendNoteRadius[2][2];
    /** Ledger line length for normal and small staff and normal and grace size */
    int m_rendLedgerLine[2][3];
    /** Brevis width for normal and small staff */
    int m_rendBrevisWidth[2];
    /** Accident width for normal and small staff and normal and grace size */
    int m_rendAccidWidth[2][2];
    /** A vertical unit corresponding to the 1/4 of an interline */
    float m_rendVerticalUnit1[2];
    /** A vertical unit corresponding to the 1/8 of an interline */
    float m_rendVerticalUnit2[2];
    /** Current Leipzig font for normal and small staff and normal and grace size */
    FontMetricsInfo m_rendFonts[2][2];				
    /** Leipzig font */
    FontMetricsInfo m_rendLeipzigFont;
    /** Current lyric font for normal and small staff and normal and grace size */
    FontMetricsInfo m_rendLyricFonts[2];
    /** Lyric font by default */
    FontMetricsInfo m_rendLyricFont;
    /** The current page height */
    int m_rendPageHeight;
    /** The current page height */
    int m_rendPageWidth;
    /** The current page witdth */
    int m_rendPageLeftMar;
    /** The current page left margin */
    int m_rendPageRightMar;
    /** The current page right margin */
    int m_rendPageTopMar;
    /** the current beam minimal slope */
	float m_rendBeamMinSlope;
    /** the current beam maximal slope */
    float m_rendBeamMaxSlope;
    
private:
    /**
     * The type of document indicates how to deal with the layout information.
     * A Transcription document types means that the layout information is included
     * and that no layout algorithm should be applied.
     */
    DocType m_type;
    
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
	
};

} // namespace vrv

#endif
