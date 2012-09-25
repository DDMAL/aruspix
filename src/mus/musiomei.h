/////////////////////////////////////////////////////////////////////////////
// Name:        musiomei.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IOMEI_H__
#define __MUS_IOMEI_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"

#include "musdoc.h"
#include "muslayer.h"
#include "musapp.h"

#include <mei/meielement.h>
#include <mei/exceptions.h>
#include <mei/meidocument.h>

#include <mei/header.h> 
#include <mei/cmn.h>
#include <mei/shared.h>
#include <mei/mensural.h>
#include <mei/layout.h>
#include <mei/critapp.h>

#include <uuid/uuid.h>

class MusBarline;
class MusBeam;
class MusClef;
class MusMensur;
class MusNote;
class MusRest;
class MusSymbol;
class MusLayout;
class MusPage;
class MusSystem;
class MusLaidOutStaff;
class MusLaidOutLayer;
class MusLaidOutLayerElement;

using namespace mei;

//----------------------------------------------------------------------------
// MusMeiOutput
//----------------------------------------------------------------------------

/**
 * This class is a file output stream for writing MEI files.
 * It uses the libmei C++ library.
 * Not implemented.
*/
class MusMeiOutput: public MusFileOutputStream
{
public:
    /** @name Constructors and destructor */
    ///@{
    MusMeiOutput( MusDoc *doc, wxString filename );
    virtual ~MusMeiOutput();
    ///@}
    
    /**
     * The main method for exporting the file to MEI.
     */ 
    virtual bool ExportFile( );
    
    /** @name Writing element methods
     * Overriding methods for writing the MEI file.
     */
    ///@{
    virtual bool WriteDoc( MusDoc *doc );
    // logical
    virtual bool WriteDiv( MusDiv *div ); 
    virtual bool WriteScore( MusScore *score ); 
    virtual bool WritePartSet( MusPartSet *parts );
    virtual bool WritePart( MusPart *part );
    virtual bool WriteSection( MusSection *section );
    virtual bool WriteMeasure( MusMeasure *measure );
    virtual bool WriteStaff( MusStaff *staff );
    virtual bool WriteLayer( MusLayer *layer );
    virtual bool WriteLayerElement( MusLayerElement *element );
    // layout
    virtual bool WriteLayout( MusLayout *layout );
    virtual bool WritePage( MusPage *page );
    virtual bool WriteSystem( MusSystem *system );
    virtual bool WriteLaidOutStaff( MusLaidOutStaff *laidOutStaff );
    virtual bool WriteLaidOutLayer( MusLaidOutLayer *laidOutLayer );
    virtual bool WriteLaidOutLayerElement( MusLaidOutLayerElement *laidOutLayerElement );
    // app
    virtual bool WriteLayerApp( MusLayerApp *app );
    virtual bool WriteLayerRdg( MusLayerRdg *rdg );
    ///@}
    
    /**
     * Method that closes the MEI Layer element.
     * Because we have a to keep a pointer to the current MEI <layer> ,
     * we need to change it and put it back to the parent MEI <layer> when
     * reaching the end of the <rdg>
     * This is achieved by changing the m_currentLayer pointer.
     */
    virtual bool EndLayerRdg( MusLayerRdg *rdg );

private:
    
    /**
     * Write a MusBarline. 
     * Callded from WriteLayerElement.
     */
    void WriteMeiBarline( BarLine *meiBarline, MusBarline *barline );
    
    /**
     * Write a MusBeam. 
     * Callded from WriteLayerElement.
     */
    void WriteMeiBeam( Beam *meiBeam, MusBeam *beam );
    
    /**
     * Write a MusClef.  
     * Callded from WriteLayerElement.
     */
    void WriteMeiClef( Clef *meiClef, MusClef *clef );
    
    /**
     * Write a MusMensur. 
     * Callded from WriteLayerElement.
     */
    void WriteMeiMensur( Mensur *meiMensur, MusMensur *mensur );
    
    /**
     * Write a MusNote. 
     * Callded from WriteLayerElement.
     */
    void WriteMeiNote( Note *meiNote, MusNote *note );
    
    /**
     * Write a MusRest. 
     * Callded from WriteLayerElement.
     */
    void WriteMeiRest( Rest *meiRest, MusRest *rest );
    
    /**
     * Write a MusSymbol. 
     * The appropriate MeiElement is created by the method and returned.
     * Callded from WriteLayerElement.
     */
    MeiElement *WriteMeiSymbol( MusSymbol *symbol );    
	
    /** @name Methods for converting members into MEI attributes. */
    ///@{
    std::string UuidToMeiStr( MusObject *element );
	std::string DurToStr(int dur);
	std::string OctToStr(int oct);
	std::string PitchToStr(int pitch);
    std::string AccidToStr(unsigned char accid);
    std::string ClefLineToStr(ClefId clefId);
    std::string ClefShapeToStr(ClefId clefId);
    std::string MensurSignToStr(MensurSign sign);
    ///@}

    
public:


private:
    wxString m_filename;
    MeiElement *m_mei;
    /** @name Members for pointers to the current element */
    ///@{
    // logical
    Music *m_music;
    Body *m_body;
    Mdiv *m_div;
    Score *m_score;
    Parts *m_parts;
    Part *m_part;
    Section *m_section;
    Measure *m_measure;
    Staff *m_staff;
    /** The pointer for the layer within a staff */
    Layer *m_layer;
    /** The pointer for the layer within an app (MusLayerRdg) */
    Rdg *m_rdgLayer;
    /** The pointer to the current layer (either m_layer or m_rdgLayer) */
    MeiElement *m_currentLayer;
    // layout
    Layouts *m_layouts;
    Layout *m_layout;
    Page *m_page;
    System *m_system;
    LaidOutStaff *m_laidOutStaff;
    LaidOutLayer *m_laidOutLayer;
    // app
    App *m_app;
    ///@}
};


//----------------------------------------------------------------------------
// MusMeiInput
//----------------------------------------------------------------------------

/**
 * This class is a file input stream for reading MEI files.
 * It uses the libmei C++ library.
 * Under development.
*/
class MusMeiInput: public MusFileInputStream
{
public:
    // constructors and destructors
    MusMeiInput( MusDoc *doc, wxString filename );
    virtual ~MusMeiInput();
    
    
    bool ImportFile( );    
    
private:
    bool ReadMeiHeader( MeiHead *meihead );
    // logical
    bool ReadMeiDiv( Mdiv *mdiv );
    bool ReadMeiScore( Score *score );
    bool ReadMeiParts( Parts * parts );
    bool ReadMeiPart( Part *part );
    bool ReadMeiSection( Section *section );
    bool ReadMeiMeasure( Measure *measure );
    bool ReadMeiStaff( Staff *staff );
    bool ReadMeiLayer( Layer *layer );
    bool ReadMeiBarline( BarLine *barline );
    bool ReadMeiBeam( Beam *beam );
    bool ReadMeiClef( Clef *clef );
    bool ReadMeiMensur( Mensur *mensur );
    bool ReadMeiNote( Note *note );
    bool ReadMeiRest( Rest *rest );
    bool ReadMeiSymbol( Accid *accid );
    bool ReadMeiSymbol( Custos *custos );
    bool ReadMeiSymbol( Dot *dot );
    // layout
    bool ReadMeiLayout( Layout *layout );
    bool ReadMeiPage( Page *page );
    bool ReadMeiSystem( System *system );
    bool ReadMeiLaidOutStaff( LaidOutStaff *laidOutStaff );
    bool ReadMeiLaidOutLayer( LaidOutLayer *laidOutLayer );
    bool ReadMeiLaidOutElement( LaidOutElement *laidOutElement );
    // app
    bool ReadMeiApp( App *app );
    bool ReadMeiRdg( Rdg *rdg );
	//
    void SetMeiUuid( MeiElement *element, MusObject *object );
    void StrToUuid(std::string uuid, uuid_t dest);
	int StrToDur(std::string dur);
	int StrToOct(std::string oct);
	int StrToPitch(std::string pitch ); 
    unsigned char StrToAccid(std::string accid);
    ClefId StrToClef(std::string shape, std::string line);
    MensurSign StrToMensurSign(std::string sign);

    
public:
    
private:
    wxString m_filename;
    // logical
	MusDiv *m_div;
	MusScore *m_score;
	MusPartSet *m_parts;
	MusPart *m_part;
	MusSection *m_section;
	MusMeasure *m_measure;
	MusStaff *m_staff;
	MusLayer *m_layer;
    // layout
    MusLayout *m_layout;
    MusPage *m_page;
    MusSystem *m_system;
    MusLaidOutStaff *m_laidOutStaff;
    MusLaidOutLayer *m_laidOutLayer;
};


#endif
