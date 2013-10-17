/////////////////////////////////////////////////////////////////////////////
// Name:        musiomei.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IOMEI_H__
#define __MUS_IOMEI_H__

#include "musapp.h"
#include "musdoc.h"
#include "muslayer.h"

#ifndef USE_FAKE_UUIDS
#include <uuid/uuid.h>
#else
#include "musuuid.h"
#endif

// TINYXML
#if defined (__WXMSW__)
    #include "tinyxml.h"
#else
    #include "tinyxml/tinyxml.h"
#endif

class MusBarline;
class MusBeam;
class MusClef;
class MusDoc;
class MusLayer;
class MusLayerApp;
class MusLayerElement;
class MusMensur;
class MusNote;
class MusPage;
class MusRest;
class MusStaff;
class MusSymbol;
class MusSystem;
class MusTuplet;


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
    MusMeiOutput( MusDoc *doc, std::string filename );
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
    virtual bool WritePage( MusPage *page );
    virtual bool WriteSystem( MusSystem *system );
    virtual bool WriteStaff( MusStaff *staff );
    virtual bool WriteMeasure( MusMeasure *measure );
    virtual bool WriteLayer( MusLayer *layer );
    virtual bool WriteLayerElement( MusLayerElement *element );
    // app
    virtual bool WriteLayerApp( MusLayerApp *app );
    virtual bool WriteLayerRdg( MusLayerRdg *rdg );
    ///@}

private:
    
    /**
     * Write a MusBarline. 
     * Callded from WriteLayerElement.
     */
    void WriteMeiBarline( TiXmlElement *meiBarline, MusBarline *barline );
    
    /**
     * Write a MusBeam. 
     * Callded from WriteLayerElement.
     */
    void WriteMeiBeam( TiXmlElement *meiBeam, MusBeam *beam );
    
    /**
     * Write a MusClef.  
     * Callded from WriteLayerElement.
     */
    void WriteMeiClef( TiXmlElement *meiClef, MusClef *clef );
    
    /**
     * Write a MusMensur. 
     * Callded from WriteLayerElement.
     */
    void WriteMeiMensur( TiXmlElement *meiMensur, MusMensur *mensur );
    
    /**
     * Write a MusNote. 
     * Callded from WriteLayerElement.
     */
    void WriteMeiNote( TiXmlElement *meiNote, MusNote *note );
    
    /**
     * Write a MusRest. 
     * Callded from WriteLayerElement.
     */
    void WriteMeiRest( TiXmlElement *meiRest, MusRest *rest );
    
    /**
     * Write a MusTuplet. 
     * Callded from WriteLayerElement.
     */
    void WriteMeiTuplet( TiXmlElement *meiTuplet, MusTuplet *tuplet );
    
    /**
     * Write a MusSymbol. 
     * The appropriate MeiElement is created by the method and returned.
     * Callded from WriteLayerElement.
     */
    TiXmlElement *WriteMeiSymbol( MusSymbol *symbol ); 
    
    /**
     * Write a sameAs attribute
     * The method has to be called by classed that support it (e.g., MusLayerElement)
     */
    void WriteSameAsAttr( TiXmlElement *meiElement, MusObject *element );
	
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
    std::string DocTypeToStr(DocType type);
    ///@}

    
public:


private:
    std::string m_filename;
    TiXmlElement *m_mei;
    /** @name Members for pointers to the current element */
    ///@{
    TiXmlElement *m_score;
    TiXmlElement *m_page;
    TiXmlElement *m_system;
    TiXmlElement *m_staff;
    TiXmlElement *m_measure;
    /** The pointer for the layer within a staff */
    TiXmlElement *m_layer;
    /** The pointer for the rdg within an app (MusLayerRdg) */
    TiXmlElement *m_rdgLayer;
    /** The pointer for a beam */
    TiXmlElement *m_beam;
    /** The pointer for a tuplet */
    TiXmlElement *m_tuplet;
    // app
    TiXmlElement *m_app;
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
    MusMeiInput( MusDoc *doc, std::string filename );
    virtual ~MusMeiInput();
    
    virtual bool ImportFile( );
    virtual bool ImportString(std::string mei);
    
private:
    bool ReadMei( TiXmlElement *root );
    bool ReadMeiHeader( TiXmlElement *meihead );
    /** Reads the content of a <layer> or of a <rdg> for <app> within <layer> */
    bool ReadMeiPage( TiXmlElement *page );
    bool ReadMeiSystem( TiXmlElement *system );
    bool ReadMeiStaff( TiXmlElement *staff );
    bool ReadMeiMeasure( TiXmlElement *measure );
    bool ReadMeiLayer( TiXmlElement *layer );
    bool ReadMeiLayerElement( TiXmlElement *XmlElement );
    MusLayerElement *ReadMeiBarline( TiXmlElement *barline );
    MusLayerElement *ReadMeiBeam( TiXmlElement *beam );
    MusLayerElement *ReadMeiClef( TiXmlElement *clef );
    MusLayerElement *ReadMeiMensur( TiXmlElement *mensur );
    MusLayerElement *ReadMeiNote( TiXmlElement *note );
    MusLayerElement *ReadMeiRest( TiXmlElement *rest );
    MusLayerElement *ReadMeiTuplet( TiXmlElement *tuplet );
    MusLayerElement *ReadMeiAccid( TiXmlElement *accid );
    MusLayerElement *ReadMeiCustos( TiXmlElement *custos );
    MusLayerElement *ReadMeiDot( TiXmlElement *dot );
    /** Reads <app> elements. For now, only <app> within <layer> are taken into account */
    MusLayerElement *ReadMeiApp( TiXmlElement *app );
    bool ReadMeiRdg( TiXmlElement *rdg );
    
    /**
     * Read a sameAs attribute
     * The method has to be called by classed that support it (e.g., MusLayerElement)
     */
    void ReadSameAsAttr( TiXmlElement *element, MusObject *object );
    
    /**
     * Add the LayerElement to the appropriate parent (e.g., MusLayer, MusLayerRdg)
     */
    void AddLayerElement( MusLayerElement *element );
    
    /**
     * Read unsupported element and try to convert them
     */
    bool ReadUnsupported( TiXmlElement *element );
    
	//
    void SetMeiUuid( TiXmlElement *element, MusObject *object );
    void StrToUuid(std::string uuid, uuid_t dest);
	int StrToDur(std::string dur);
	int StrToOct(std::string oct);
	int StrToPitch(std::string pitch ); 
    unsigned char StrToAccid(std::string accid);
    ClefId StrToClef(std::string shape, std::string line);
    MensurSign StrToMensurSign(std::string sign);
    DocType StrToDocType(std::string type);

    
public:
    
private:
    std::string m_filename;
    MusPage *m_page;
    MusSystem *m_system;
	MusStaff *m_staff;
    MusMeasure *m_measure;
	MusLayer *m_layer;
    MusLayerRdg *m_layerRdg;
    MusBeam *m_beam;
    MusTuplet *m_tuplet;
    MusObject *m_currentLayer;
    MusLayerApp *m_layerApp;
    
    // unsupported elements
    MusMeasure *m_contentBasedMeasure;
};


#endif
