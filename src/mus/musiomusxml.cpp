//
//  musiomusxml.cpp
//  aruspix
//
//  Created by Rodolfo Zitellini on 10/08/12.
//  Copyright (c) 2012 com.aruspix.www. All rights reserved.
//

#include <iostream>

/////////////////////////////////////////////////////////////////////////////
// Name:        musiomei.cpp
// Author:      Laurent Pugin
// Created:     2008
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////



#include <algorithm>
using std::min;
using std::max;

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wx.h"

#include "wx/filename.h"
#include "wx/txtstrm.h"

#include "musiomusxml.h"

#include "musbarline.h"
#include "musbeam.h"
#include "musclef.h"
#include "musmensur.h"
#include "musneume.h"
#include "musneumesymbol.h"
#include "musnote.h"
#include "musrest.h"
#include "mussymbol.h"

#include "muslaidoutlayerelement.h"

#include "tinyxml.h"

//#include "app/axapp.h"

#include <vector>
using std::vector;

//----------------------------------------------------------------------------
// MusXMLOutput
//----------------------------------------------------------------------------

MusXMLOutput::MusXMLOutput( MusDoc *doc, wxString filename ) :
// This is pretty bad. We open a bad fileoutputstream as we don't use it
MusFileOutputStream( doc, -1 )
{
    m_filename = filename;
    m_measure_count = 0;

}

MusXMLOutput::~MusXMLOutput()
{
}

bool MusXMLOutput::ExportFile( )
{

    m_xml_doc = new TiXmlDocument;
    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "UTF-8", "no" );
    m_xml_doc->LinkEndChild( decl );

    // do this or finale will barf, versione 3.0 for now
    TiXmlUnknown *unk = new TiXmlUnknown;
    unk->SetValue("!DOCTYPE score-partwise PUBLIC \"-//Recordare//DTD MusicXML 3.0 Partwise//EN\" \"http://www.musicxml.org/dtds/partwise.dtd\"");    
    m_xml_doc->LinkEndChild(unk);
    
    // this starts the call of all the functors
    m_doc->Save( this );
        
    m_xml_doc->SaveFile( m_filename );
    
	return true;    
}

bool MusXMLOutput::WriteDoc( MusDoc *doc )
{
    printf("Doc\n");
    
    // Write the partwise declaration
    // the MusicXML "score-partwise" does not map to our MusScore
    
    m_xml_score = new TiXmlElement("score-partwise");
    
    // hardcode a voice for noew
    TiXmlElement *plist =  new TiXmlElement("part-list");
    TiXmlElement *spart = new TiXmlElement("score-part");
    spart->SetAttribute("id", "P1");
    TiXmlElement *pname = new TiXmlElement("part-name");
    TiXmlText *pname_text = new TiXmlText("Example Music");
    pname->LinkEndChild(pname_text);
    
    spart->LinkEndChild(pname);
    plist->LinkEndChild(spart);
    
    m_xml_score->LinkEndChild(plist);
    m_xml_doc->LinkEndChild(m_xml_score);
    
    
    return true;
}

bool MusXMLOutput::WriteDiv( MusDiv *div )
{
    printf("Div\n");
    return true;
}

bool MusXMLOutput::WriteScore( MusScore *score )
//bool MusXMLOutput::WriteMeiScore( Score *meiScore, MusScore *score )
{
    printf("Score\n");
    return true;
}

bool MusXMLOutput::WritePartSet( MusPartSet *partSet )
//bool MusXMLOutput::WriteMeiParts( Parts * meiParts, MusPartSet *partSet )
{
    printf("PartSet\n");
    return true;
}

bool MusXMLOutput::WritePart( MusPart *part )
//bool MusXMLOutput::WriteMeiPart( Part *meiPart, MusPart *part )
{
    printf("Part\n");
    return true;
}

bool MusXMLOutput::WriteSection( MusSection *section )
//bool MusXMLOutput::WriteMeiSection( Section *meiSection, MusSection *section )
{
    printf("Section\n");
    
    m_xml_part = new TiXmlElement("part");
    m_xml_part->SetAttribute("id", "P1");
    
    m_xml_score->LinkEndChild(m_xml_part);
    
    return true;
}

bool MusXMLOutput::WriteMeasure( MusMeasure *measure )
//bool MusXMLOutput::WriteMeiMeasure( Measure *meiMeasure, MusMeasure *measure )
{
    m_measure_count++;
    char mstring[1024];
    printf("Measure %i\n", m_measure_count);
    
    // FIXME FInd a better solution, placeholder
    sprintf(mstring, "%i", m_measure_count);
    
    m_xml_measure = new TiXmlElement("measure");
    m_xml_measure->SetAttribute("number", mstring);
    
    // in first measure set the divisions value in <attributes>
    if (m_measure_count == 1) {
        TiXmlElement *attributes = new TiXmlElement("attributes"); // this should be global
        TiXmlElement *divisions = new TiXmlElement("divisions");
        TiXmlText *divval = new TiXmlText("4"); // no more than sixteenths for now
        
        divisions->LinkEndChild(divval);
        attributes->LinkEndChild(divisions);
        m_xml_measure->LinkEndChild(attributes);
    }
    
    m_xml_part->LinkEndChild(m_xml_measure);
    
    return true;
}

bool MusXMLOutput::WriteStaff( MusStaff *staff )
//bool MusXMLOutput::WriteMeiStaff( Staff *meiStaff, MusStaff *staff )
{
    printf("Staff\n");
    return true;
}

bool MusXMLOutput::WriteLayer( MusLayer *layer )
//bool MusXMLOutput::WriteMeiLayer( Layer *meiLayer, MusLayer *layer )
{
    printf("Layer\n");
    return true;
}

bool MusXMLOutput::WriteLayerElement( MusLayerElement *element )
{
    printf("Layer Elem\n");
    char steps[] = {'C', 'D', 'E', 'F', 'G', 'A', 'B'};
    char buf[1024];
    
    // FIXME, do note and rest parsing in a more clever way
    
    if (dynamic_cast<MusNote*>(element)) {
        MusNote *n = dynamic_cast<MusNote*>(element);
        
        TiXmlElement *note = new TiXmlElement("note");
        
        // make pitch
        TiXmlElement *pitch = new TiXmlElement("pitch");
        TiXmlElement *step = new TiXmlElement("step");
        TiXmlElement *octave = new TiXmlElement("octave");
        
        sprintf(buf, "%c", steps[n->m_pname - 1]);
        TiXmlText *step_name = new TiXmlText(buf);
        step->LinkEndChild(step_name);
        
        sprintf(buf, "%i", n->m_oct);
        TiXmlText *octave_name = new TiXmlText(buf);
        octave->LinkEndChild(octave_name);
        
        pitch->LinkEndChild(step);
        pitch->LinkEndChild(octave);
        note->LinkEndChild(pitch);
        
        TiXmlElement *duration = new TiXmlElement("duration");
        TiXmlElement *type = new TiXmlElement("type");
        
        string t;
        int dur;
        
        switch (n->m_dur) {
            case DUR_1: dur = 16; t = "whole"; break;
            case DUR_2: dur = 8; t = "whole"; break;
            case DUR_4: dur = 4; t = "whole"; break;
            case DUR_8: dur = 2; t = "whole"; break;
            case DUR_16: dur = 1; t = "whole"; break;
                
            default:
                break;
        }
        
        sprintf(buf, "%i", dur);
        TiXmlText *dur_name = new TiXmlText(buf);
        duration->LinkEndChild(dur_name);
        
        note->LinkEndChild(duration);
        
        m_xml_measure->LinkEndChild(note);
    } else if (dynamic_cast<MusRest*>(element)) {
        MusRest *r = dynamic_cast<MusRest*>(element);
        
        TiXmlElement *note = new TiXmlElement("note");
        TiXmlElement *rest = new TiXmlElement("rest");
        
        note->LinkEndChild(rest);
        
        TiXmlElement *duration = new TiXmlElement("duration");
        
        string t;
        int dur;
        
        switch (r->m_dur) {
            case DUR_1: dur = 16; t = "whole"; break;
            case DUR_2: dur = 8; t = "whole"; break;
            case DUR_4: dur = 4; t = "whole"; break;
            case DUR_8: dur = 2; t = "whole"; break;
            case DUR_16: dur = 1; t = "whole"; break;
                
            default:
                break;
        }
        
        sprintf(buf, "%i", dur);
        TiXmlText *dur_name = new TiXmlText(buf);
        duration->LinkEndChild(dur_name);
        
        note->LinkEndChild(duration);
        
        m_xml_measure->LinkEndChild(note);
    }

    
    return true;
 }




bool MusXMLOutput::WriteLayout( MusLayout *layout )
{
    printf("Layout\n");
    return true;
}

bool MusXMLOutput::WritePage( MusPage *page )
{
    printf("Page\n");
    return true;
}

bool MusXMLOutput::WriteSystem( MusSystem *system )
{
    printf("System\n");
    return true;
}

bool MusXMLOutput::WriteLaidOutStaff( MusLaidOutStaff *laidOutStaff )
{
    printf("Laid staff\n");
    return true;
}

bool MusXMLOutput::WriteLaidOutLayer( MusLaidOutLayer *laidOutLayer )
{    printf("Laid layer\n");
    return true;

}

bool MusXMLOutput::WriteLaidOutLayerElement( MusLaidOutLayerElement *laidOutLayerElement )
{
    printf("Laid Layer Elem\n");
    return true;
}









