//
//  musiomusxml.cpp
//  aruspix
//
//  Created by Rodolfo Zitellini on 10/08/12.
//  Copyright (c) 2012 com.aruspix.www. All rights reserved.
//

#include <iostream>

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
#include "muskeysig.h"
#include "musmensur.h"
#include "musneume.h"
#include "musneumesymbol.h"
#include "musnote.h"
#include "musrest.h"
#include "mussymbol.h"
#include "musmensur.h"

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
    m_xml_attributes = NULL;
    m_current_clef = NULL;
    m_xml_measure_style = NULL;

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
    //unk->SetValue("!DOCTYPE score-partwise PUBLIC \"-//Recordare//DTD MusicXML 3.0 Partwise//EN\" \"http://www.musicxml.org/dtds/partwise.dtd\"");
    unk->SetValue("!DOCTYPE score-timewise PUBLIC \"-//Recordare//DTD MusicXML 2.0 Timewise//EN\" \"http://www.musicxml.org/dtds/timewise.dtd\"");
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
    
    m_xml_score = new TiXmlElement("score-timewise");
    
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
    
    m_xml_score->LinkEndChild(m_xml_measure);
    
    //m_xml_part->LinkEndChild(m_xml_measure);
    
    return true;
}

bool MusXMLOutput::WriteStaff( MusStaff *staff )
//bool MusXMLOutput::WriteMeiStaff( Staff *meiStaff, MusStaff *staff )
{
    printf("Staff\n");
    
    m_xml_part = new TiXmlElement("part");
    m_xml_part->SetAttribute("id", "P1");
    
    m_xml_measure->LinkEndChild(m_xml_part);
    
    // in first measure set the divisions value in <attributes>
    if (m_measure_count == 1) {
        m_xml_attributes = new TiXmlElement("attributes");
        TiXmlElement *divisions = new TiXmlElement("divisions");
        TiXmlText *divval = new TiXmlText("4"); // no more than sixteenths for now
        
        divisions->LinkEndChild(divval);
        m_xml_attributes->LinkEndChild(divisions);
        m_xml_part->LinkEndChild(m_xml_attributes);
    }
    
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
    
    if (dynamic_cast<MusClef*>(element)) {
        WriteClef(element);
    } else if (dynamic_cast<MusKeySig*>(element)) {
        WriteKey(element);
    } else if (dynamic_cast<MusMensur*>(element)) {
        WriteTime(element);
    }
    
 //   printf("---- %s\n", element->MusClassName().c_str());
    
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
        
        // measure in partwise
        m_xml_part->LinkEndChild(note);
    } else if (dynamic_cast<MusRest*>(element)) {
        MusRest *r = dynamic_cast<MusRest*>(element);
        
        // handle multi measure rest
        if (r->m_dur == VALSilSpec) {
            WriteMultiMeasureRest(r);
            return true;
        }
        
        TiXmlElement *note = new TiXmlElement("note");
        TiXmlElement *rest = new TiXmlElement("rest");
        
        note->LinkEndChild(rest);
        
        TiXmlElement *duration = new TiXmlElement("duration");
        
        string t;
        int dur = 4;
        
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
        
        m_xml_part->LinkEndChild(note);
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


void MusXMLOutput::WriteClef(MusLayerElement *element) {
    wxString sign, line;
    
    // Create the attributes elem
    // or use existing one, all the attribute changes
    // go in the same <attributes>
    CreateAttributes();
    
    MusClef *clef = dynamic_cast<MusClef*>(element);
    
    switch (clef->m_clefId) {
        case SOL1: sign = "G"; line = "1"; break;
        case SOL2: sign = "G"; line = "2"; break;
        case UT1: sign = "C"; line = "1"; break;
        case UT2: sign = "C"; line = "2"; break;
        case UT3: sign = "C"; line = "3"; break;
        case UT4: sign = "C"; line = "4"; break;
        case UT5: sign = "C"; line = "5"; break;
        case FA3: sign = "F"; line = "3"; break;
        case FA4: sign = "F"; line = "4"; break;
        case FA5: sign = "F"; line = "5"; break;
        default: break;
    }
    
    //Create the <clef> element
    TiXmlElement *xclef = new TiXmlElement("clef");
    
    // Create the <sign> element and link to it it's text
    TiXmlElement *xsign = new TiXmlElement("sign");
    TiXmlText *xsignt = new TiXmlText(sign.c_str());
    xsign->LinkEndChild(xsignt);
    // Insert it into the <clef>
    xclef->LinkEndChild(xsign);
    
    // Create the <line> element and link to it it's text
    TiXmlElement *xline = new TiXmlElement("line");
    TiXmlText *xlinet = new TiXmlText(line.c_str());
    xline->LinkEndChild(xlinet);
    // Insert it into the <clef>
    xclef->LinkEndChild(xline); 
    
    // place clef into <attribute>
    m_xml_attributes->LinkEndChild(xclef);
    m_current_clef = xclef;
    
}

void MusXMLOutput::WriteKey(MusLayerElement *element) {
    MusKeySig* key = dynamic_cast<MusKeySig*>(element);
    
    // Check for attrib element as above
    // or use existing one, all the attribute changes
    // go in the same <attributes>
    CreateAttributes();
    
    // create tompost <key> elem
    TiXmlElement *xkey = new TiXmlElement("key");
    
    // Convert the number of alterations to string
    wxString n_alter;
    if (key->m_alteration == ACCID_FLAT)
        // flats are negative numbers
        n_alter << -key->m_num_alter;
    else
        n_alter << key->m_num_alter;
    
    //create <fifths> node with the number of alterations
    TiXmlElement *xfifths = new TiXmlElement("fifths");
    TiXmlText *xftxt = new TiXmlText(n_alter.c_str());
    xfifths->LinkEndChild(xftxt);
    // add it to the key elem
    xkey->LinkEndChild(xfifths);
    
    //I dont know what musicxml does with the <mode> tag
    //bmaybe it is just to annoy programmers using tinyxml
    //finale sets this always to major
    TiXmlElement *mode = new TiXmlElement("mode");
    TiXmlText *major = new TiXmlText("major");
    mode->LinkEndChild(major);
    // add it to the key elem
    xkey->LinkEndChild(mode);
    
    // Obviously the order in which <key> <clef> and <time>
    // are fixed. If in music notation practice we have CLEF then KEY
    // why should MusicXML follow? the genious architect decided to
    // force the order of <attributes> REVERSED, <key> BEFORE <clef>
    // wow.
    if (!m_current_clef) // no clef
        m_xml_attributes->LinkEndChild(xkey);
    else // insert before the clef
        m_xml_attributes->InsertBeforeChild(m_current_clef, *xkey);
    
}

void MusXMLOutput::WriteTime(MusLayerElement *element) {
    MusMensur *timesig = dynamic_cast<MusMensur*>(element);
    wxString number;
    
    CreateAttributes();
    
    TiXmlElement *xtime = new TiXmlElement("time");
    
    // add symbol attribute if necessay
    if (timesig->m_meterSymb == METER_SYMB_COMMON) {
        xtime->SetAttribute("symbol", "common");
        // if the number of beats was not specified, approximate it
        if (timesig->m_num == 0){
            timesig->m_num = 4;
            timesig->m_numBase = 4;
        }
    } else if (timesig->m_meterSymb == METER_SYMB_CUT) {
        xtime->SetAttribute("symbol", "cut");
        if (timesig->m_num == 0){
            timesig->m_num = 2;
            timesig->m_numBase = 2;
        }
    }
    
    // Create beat and beat type attribsb
    TiXmlElement *xbeats = new TiXmlElement("beats");
    TiXmlElement *xbtype = new TiXmlElement("beat-type");
    
    // convert number to text for beats
    number << timesig->m_num;
    TiXmlText *beat_text = new TiXmlText(number.c_str());
    xbeats->LinkEndChild(beat_text);
    
    // ditto as above
    number.Clear();
    number << timesig->m_numBase;
    TiXmlText *base_text = new TiXmlText(number.c_str());
    xbtype->LinkEndChild(base_text);
    
    // push it to xtime
    xtime->LinkEndChild(xbeats);
    xtime->LinkEndChild(xbtype);
    
    if (!m_current_clef) // no clef
        m_xml_attributes->LinkEndChild(xtime);
    else // insert before the clef
        m_xml_attributes->InsertBeforeChild(m_current_clef, *xtime);
}

void MusXMLOutput::WriteMultiMeasureRest(MusRest *r) {
    wxString num;
    num << r->m_multimeasure_dur;
    
    TiXmlElement *xmrest = new TiXmlElement("multiple-rest");
    TiXmlText *mdur = new TiXmlText(num.c_str());
    xmrest->LinkEndChild(mdur);
    
    
    // chech that <attributes> exists
    CreateAttributes();
    
    if (!m_xml_measure_style) {
        m_xml_measure_style = new TiXmlElement("measure-style");
        // link it into the current attributes
        m_xml_attributes->LinkEndChild(m_xml_measure_style);
    }
    
    // put the multiple measure thing into the measure style
    m_xml_measure_style->LinkEndChild(xmrest);
    
}

void MusXMLOutput::CreateAttributes() {
    // Make new attributes as necessary
    if (m_xml_attributes == NULL) {
        m_xml_attributes = new TiXmlElement("attributes");
        // put it into the current part
        m_xml_part->LinkEndChild(m_xml_attributes);
    }
}