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

#include <mei/meielement.h>
#include <mei/exceptions.h>
#include <mei/meidocument.h>

#include <mei/header.h> 
#include <mei/cmn.h>
#include <mei/shared.h>
#include <mei/mensural.h>
#include <mei/layout.h>

class MusBarline;
class MusBeam;
class MusClef;
class MusMensur;
class MusNote;
class MusRest;
class MusSymbol;

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
    // constructors and destructors
    MusMeiOutput( MusDoc *doc, wxString filename );
    virtual ~MusMeiOutput();
    
    virtual bool ExportFile( );
    
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

private:
    /*bool WriteMeiHeader( MeiHead *meiHead );
    bool WriteMeiDiv( Mdiv *meiDiv, MusDiv *div );
    bool WriteMeiScore( Score *meiScore, MusScore *score );
    bool WriteMeiParts( Parts * meiParts, MusPartSet *partSet );
    bool WriteMeiPart( Part *meiPart, MusPart *part );
    bool WriteMeiSection( Section *meiSection, MusSection *section );
    bool WriteMeiMeasure( Measure *meiMeasure, MusMeasure *measure );
    bool WriteMeiStaff( Staff *meiStaff, MusStaff *staff );
    bool WriteMeiLayer( Layer *meiLayer, MusLayer *layer );*/
    void WriteMeiBarline( BarLine *meiBarline, MusBarline *barline );
    void WriteMeiBeam( Beam *meiBeam, MusBeam *beam );
    void WriteMeiClef( Clef *meiClef, MusClef *clef );
    void WriteMeiMensur( Mensur *meiMensur, MusMensur *mensur );
    void WriteMeiNote( Note *meiNote, MusNote *note );
    void WriteMeiRest( Rest *meiRest, MusRest *rest );
    /**
     * Write a MusSymbol. The appropriate MeiElement is created by the method
     * and returned.
     */
    MeiElement *WriteMeiSymbol( MusSymbol *symbol );    
	//
    std::string GetMeiUuid( MusObject *element );
	std::string DurToStr(int dur);
	std::string OctToStr(int oct);
	std::string PitchToStr(int pitch);
    std::string AccidToStr(unsigned char accid);
    std::string ClefLineToStr(ClefId clefId);
    std::string ClefShapeToStr(ClefId clefId);
    std::string MensurSignToStr(MensurSign sign);

    
public:


private:
    wxString m_filename;
    MeiElement *m_mei;
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
    Layer *m_layer;
    // layout
    Layouts *m_layouts;
    Layout *m_layout;
    Page *m_page;
    System *m_system;
    LaidOutStaff *m_laidOutStaff;
    LaidOutLayer *m_laidOutLayer;
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
	//
	int StrToDur(std::string dur);
	int StrToOct(std::string oct);
	int StrToPitch(std::string pitch ); 
    unsigned char StrToAccid(std::string accid);
    ClefId StrToClef(std::string line, std::string shape);
    MensurSign StrToMensurSign(std::string sign);

    
public:
    
private:
    wxString m_filename;
	MusDiv *m_div;
	MusScore *m_score;
	MusPartSet *m_parts;
	MusPart *m_part;
	MusSection *m_section;
	MusMeasure *m_measure;
	MusStaff *m_staff;
	MusLayer *m_layer;
};


#endif
