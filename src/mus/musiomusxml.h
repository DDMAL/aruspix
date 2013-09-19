//
//  musiomusxml.h
//  aruspix
//
//  Created by Rodolfo Zitellini on 10/08/12.
//  Copyright (c) 2012 com.aruspix.www. All rights reserved.
//

#ifndef aruspix_musiomusxml_h
#define aruspix_musiomusxml_h

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wx/wfstream.h"

#include "musdoc.h"
#include "muslayer.h"
#include "mustie.h"

#include <tinyxml.h>

class MusBarline;
class MusBeam;
class MusClef;
class MusMensur;
class MusNote;
class MusRest;
class MusSymbol;


//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------

class MusXMLOutput: public MusFileOutputStream
{
public:
    // constructors and destructors
    MusXMLOutput( MusDoc *doc, wxString filename );
    virtual ~MusXMLOutput();
    
    virtual bool ExportFile( );
    
    virtual bool WriteDoc( MusDoc *doc );
    // logical
    virtual bool WriteStaff( MusStaff *staff );
    virtual bool WriteLayer( MusLayer *layer );
    virtual bool WriteLayerElement( MusLayerElement *element );
    // layout
    virtual bool WriteLayout( MusDoc *layout );
    virtual bool WritePage( MusPage *page );
    virtual bool WriteSystem( MusSystem *system );
    virtual bool WriteLaidOutStaff( MusStaff *laidOutStaff );
    virtual bool WriteLaidOutLayer( MusLayer *laidOutLayer );
    virtual bool WriteLaidOutLayerElement( MusLayerElement *laidOutLayerElement );
    
private:
    
public:
    void WriteClef(MusLayerElement *element);
    void WriteKey(MusLayerElement *element);
    void WriteTime(MusLayerElement *element);
    void WriteNoteOrRest(MusLayerElement *element);
    void WriteMultiMeasureRest(MusRest *r);
    void CreateAttributes();
    void SetTie(TiXmlElement *xml_note, bool last);
    void CreateRestsForMultiMeasure();
    
private:
    wxString m_filename;
    
    TiXmlElement *m_xml_score;
    TiXmlElement *m_xml_part;
    TiXmlElement *m_xml_measure;
    TiXmlElement *m_xml_attributes;
    TiXmlElement *m_xml_measure_style;
    TiXmlElement *m_xml_last_note;
    TiXmlDocument *m_xml_doc;
    TiXmlElement *m_xml_current_clef;
    
    MusMensur *m_current_time;
    MusBeam *m_current_beam;
    bool m_in_beam;
    bool m_tied;
    int m_multimeasure_rests;
    
    int m_measure_count;
};

#endif
