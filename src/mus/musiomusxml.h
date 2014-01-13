/////////////////////////////////////////////////////////////////////////////
// Name:        musiomusxml.h
// Author:      Rodolfo Zitellini
// Created:     10/08/2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_IOMUSXML_H__
#define __MUS_IOMUSXML_H__

#include "musdoc.h"
#include "muslayer.h"
#include "mustie.h"

// TINYXML
#if defined (__WXMSW__)
    #include "tinyxml.h"
#else
    #include "tinyxml/tinyxml.h"
#endif

class Barline;
class Beam;
class Clef;
class Mensur;
class Note;
class Rest;
class MusSymbol;


//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------

class MusXMLOutput: public MusFileOutputStream
{
public:
    // constructors and destructors
    MusXMLOutput( MusDoc *doc, std::string filename );
    virtual ~MusXMLOutput();
    
    virtual bool ExportFile( );
    
    virtual bool WriteDoc( MusDoc *doc );
    // logical
    virtual bool WriteStaff( MusStaff *staff );
    virtual bool WriteLayer( MusLayer *layer );
    virtual bool WriteLayerElement( LayerElement *element );
    // layout
    virtual bool WriteLayout( MusDoc *layout );
    virtual bool WritePage( MusPage *page );
    virtual bool WriteSystem( MusSystem *system );
    virtual bool WriteLaidOutStaff( MusStaff *laidOutStaff );
    virtual bool WriteLaidOutLayer( MusLayer *laidOutLayer );
    virtual bool WriteLaidOutLayerElement( LayerElement *laidOutLayerElement );
    
private:
    
public:
    void WriteClef(LayerElement *element);
    void WriteKey(LayerElement *element);
    void WriteTime(LayerElement *element);
    void WriteNoteOrRest(LayerElement *element);
    void WriteMultiMeasureRest(Rest *r);
    void CreateAttributes();
    void SetTie(TiXmlElement *xml_note, bool last);
    void CreateRestsForMultiMeasure();
    
private:
    std::string m_filename;
    
    TiXmlElement *m_xml_score;
    TiXmlElement *m_xml_part;
    TiXmlElement *m_xml_measure;
    TiXmlElement *m_xml_attributes;
    TiXmlElement *m_xml_measure_style;
    TiXmlElement *m_xml_last_note;
    TiXmlDocument *m_xml_doc;
    TiXmlElement *m_xml_current_clef;
    
    Mensur *m_current_time;
    Beam *m_current_beam;
    bool m_in_beam;
    bool m_tied;
    int m_multimeasure_rests;
    
    int m_measure_count;
};

#endif // __MUS_IOMUSXML_H__
