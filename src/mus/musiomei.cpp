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


#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/filename.h"
#include "wx/txtstrm.h"

#include "musiomei.h"

#include "musbarline.h"
#include "musclef.h"
#include "musmensur.h"
#include "musneume.h"
#include "musnote.h"
#include "musrest.h"
#include "mussymbol.h"

#include "app/axapp.h"

#include <vector>
using std::vector;

//----------------------------------------------------------------------------
// MusMeiOutput
//----------------------------------------------------------------------------

MusMeiOutput::MusMeiOutput( MusDoc *doc, wxString filename ) :
    // This is pretty bad. We open a bad fileoutputstream as we don't use it
	MusFileOutputStream( doc, -1 )
{
}

MusMeiOutput::~MusMeiOutput()
{
}

bool MusMeiOutput::ExportFile( )
{
	return true;    
}

std::string MusMeiOutput::OctToStr(int oct)
{
	char buf[3];
	snprintf(buf, 2, "%d", oct);
	return std::string(buf);
	
	// For some reason, #include <sstream> does not work with xcode 3.2
	//std::ostringstream oss;
	//oss << oct;
	//return oss.str();
}


std::string MusMeiOutput::PitchToStr(int pitch)
{
    string value;
    switch (pitch) {
        case 0: value = "b"; break;
        case 1: value = "c"; break;
        case 2: value = "d"; break;
        case 3: value = "e"; break;
        case 4: value = "f"; break;
        case 5: value = "g"; break;
        case 6: value = "a"; break;
        default: break;
    }
	return value;
}



//----------------------------------------------------------------------------
// MusMeiInput
//----------------------------------------------------------------------------

MusMeiInput::MusMeiInput( MusDoc *doc, wxString filename ) :
    // This is pretty bad. We open a bad fileoinputstream as we don't use it
	MusFileInputStream( doc, -1 )
{
    m_filename = filename;
	
	m_div = NULL;
	m_score = NULL;
	m_parts = NULL;
	m_part = NULL;
	m_section = NULL;
	m_measure = NULL;
	m_staff = NULL;
	m_layer = NULL;
}

MusMeiInput::~MusMeiInput()
{
}

bool MusMeiInput::ImportFile( )
{
    try {
        mei::MeiDocument *doc = XmlImport::documentFromFile( m_filename.c_str() );
        if ( !doc ) {
            return false;
        }
        MeiElement *root = doc->getRootElement();
        // header
        if ( root->hasChildren("meiHead") ) {
             vector<MeiElement*> children = root->getChildrenByName("meiHead");
             if ( dynamic_cast<MeiHead*> (children[0]) ) {
                ReadHeader(dynamic_cast<MeiHead*> (children[0]));
             }
        }
        // music
        MeiElement *music = NULL;
        MeiElement *body = NULL;
        if ( root->hasChildren("music") ) {
            music = root->getChildrenByName("music")[0];
        }
        if ( music && music->hasChildren("body") ) {
            body = music->getChildrenByName("body")[0];
        }
		if ( body && body->hasChildren("mdiv") ) {
			vector<MeiElement*> children = body->getChildrenByName("mdiv");
			for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
				MeiElement *e = *iter;
				m_div = new MusDiv( );
				if (ReadDiv(dynamic_cast<Mdiv*>(e))) {
					m_doc->AddDiv( m_div );
				}
				else {
					delete m_div;
				}
				m_div = NULL;
			}
        }
        return true;
    }
    catch( char * str ) {
        wxLogError("%s", str );
        return false;
   }
}

bool MusMeiInput::ReadHeader( MeiHead *meihead )
{
    return true;
}

bool MusMeiInput::ReadDiv( Mdiv *mdiv )
{		
	if ( mdiv && mdiv->hasChildren("score") ) {
		vector<MeiElement*> children = mdiv->getChildrenByName("score");
		if ( dynamic_cast<Score*> (children[0]) ) {
			m_score = new MusScore( );
			if (ReadScore(dynamic_cast<Score*> (children[0]))) {
				m_div->AddScore(m_score);
				return true;
			}
			else {
				delete m_score;
				return false;
			}
		}
	} else if ( mdiv && mdiv->hasChildren("parts") ) {
		vector<MeiElement*> children = mdiv->getChildrenByName("parts");
		if ( dynamic_cast<Parts*> (children[0]) ) {
			m_parts = new MusPartSet( );
			if (ReadParts(dynamic_cast<Parts*> (children[0]))) {
				m_div->AddPartSet( m_parts );
				return true;
			}
			else {
				delete m_parts;
				return false;
			}
		}
	}
    return false;
}

bool MusMeiInput::ReadScore( Score *score )
{
	if ( score && score->hasChildren("section") ) {
		vector<MeiElement*> children = score->getChildrenByName("section");
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
			m_section = new MusSection( );
			if (ReadSection(dynamic_cast<Section*>(e))) {
				m_score->AddSection( m_section );
			}
			else {
				delete m_section;
			}
			m_section = NULL;
		}
		// success only if at least one section was added to the score
		return (m_score->m_sections.GetCount() > 0);
	}
	return false;
}

bool MusMeiInput::ReadParts( Parts * parts )
{
	if ( parts && parts->hasChildren("part") ) {
		vector<MeiElement*> children = parts->getChildrenByName("part");
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
			m_part = new MusPart(  );
			if (ReadPart(dynamic_cast<Part*>(e))) {
				m_parts->AddPart( m_part );
			}
			else {
				delete m_part;
			}
			m_part = NULL;
		}
		// success only if at least one part was added to the parts
		return (m_parts->m_parts.GetCount() > 0);
	}
	return false;
}

bool MusMeiInput::ReadPart( Part *part )
{
	if ( part && part->hasChildren("section") ) {
		vector<MeiElement*> children = part->getChildrenByName("section");
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
			m_section = new MusSection( );
			if (ReadSection(dynamic_cast<Section*>(e))) {
				m_part->AddSection( m_section );
			}
			else {
				delete m_section;
			}
			m_section = NULL;
		}
		// success only if at least one section was added to the score
		return (m_part->m_sections.GetCount() > 0);
	}
	return false;
}

bool MusMeiInput::ReadSection( Section *section )
{
	if ( section && section->hasChildren("measure") ) {
		vector<MeiElement*> children = section->getChildrenByName("measure");
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
			m_measure = new MusMeasure( );
			if (ReadMeasure(dynamic_cast<Measure*>(e))) {
				m_section->AddSectionElement( m_measure );
			}
			else {
				delete m_measure;
			}
			m_measure = NULL;
		}
		// success only if at least one measure was added to the section
		return (m_section->m_sectionElements.GetCount() > 0);
	} else if ( section && section->hasChildren("staff") ) {
		vector<MeiElement*> children = section->getChildrenByName("staff");
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
			m_staff= new MusStaff( );
			if (ReadStaff(dynamic_cast<Staff*>(e))) {
				m_section->AddSectionElement( m_staff );
			}
			else {
				delete m_staff;
			}
			m_staff = NULL;
		}
		// success only if at least one staff was added to the section
		return (m_section->m_sectionElements.GetCount() > 0);
	}
    return false;
}

bool MusMeiInput::ReadMeasure( Measure *measure )
{
	if ( measure && measure->hasChildren("staff") ) {
		vector<MeiElement*> children = measure->getChildrenByName("staff");
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
			m_staff = new MusStaff( );
			if (ReadStaff(dynamic_cast<Staff*>(e))) {
				m_measure->AddMeasureElement( m_staff );
			}
			else {
				delete m_staff;
			}
			m_staff = NULL;
		}
		// success only if at least one staff was added to the measure
		return (m_measure->m_measureElements.GetCount() > 0);
	}
    return false;
}

bool MusMeiInput::ReadStaff( Staff *staff )
{
	if ( staff && staff->hasChildren("layer") ) {
		vector<MeiElement*> children = staff->getChildrenByName("layer");
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
			m_layer = new MusLayer( );
			if (ReadLayer(dynamic_cast<Layer*>(e))) {
				m_staff->AddStaffElement( m_layer );
			}
			else {
				delete m_layer;
			}
			m_layer = NULL;
		}
		// success only if at least one layer was added to the staff
		return (m_staff->m_staffElements.GetCount() > 0);
	}
    return false;
}

bool MusMeiInput::ReadLayer( Layer *layer )
{
	if ( layer && layer->hasChildren() ) {
		vector<MeiElement*> children = layer->getChildren();
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
			if (e->getName()=="note") {
				if (!ReadNote(dynamic_cast<Note*>(e))) {
					return false;
				}
			}
			else {
				wxLogDebug("LayerElement %s ignored", e->getName().c_str() );
			}

		}
		// success in any case
		return true;
	}
    return false;
}

bool MusMeiInput::ReadBarline( BarLine *barline )
{
    return true;
}

bool MusMeiInput::ReadClef( Clef *clef )
{
    return true;
}

bool MusMeiInput::ReadMensur( Mensur *mensur )
{
    return true;
}

bool MusMeiInput::ReadNote( Note *note )
{
	MusNote *musNote = new MusNote();
	// pitch
	if ( note->m_Pitch.hasPname() ) {
		musNote->m_pname = StrToPitch( note->m_Pitch.getPname()->getValue() );
	}
	// oct
	if ( note->m_Octave.hasOct() ) {
		musNote->m_oct = StrToOct( note->m_Octave.getOct()->getValue() );
	}
	// duration
	
	m_layer->AddLayerElement( musNote );
    return true;
}

bool MusMeiInput::ReadRest( Rest *rest )
{
    return true;
}

bool MusMeiInput::ReadSymbol( MeiElement *symbol )
{
    return true;
}


int MusMeiInput::StrToDur(std::string dur)
{
    int value;
    if (dur == "longa") value = DUR_LG;
    else if (dur == "brevis") value = DUR_BR;
    else if (dur == "semibrevis") value = DUR_1;
    else if (dur == "minima") value = DUR_2;
    else if (dur == "semiminima") value = DUR_4;
    else if (dur == "fusa") value = DUR_8;
    else if (dur == "semifusa") value = DUR_16;
    else if (dur == "long") value = DUR_LG;
    else if (dur == "breve") value = DUR_BR;
    else if (dur == "1") value = DUR_1;
    else if (dur == "2") value = DUR_2;
    else if (dur == "4") value = DUR_4;
    else if (dur == "8") value = DUR_8;
    else if (dur == "16") value = DUR_16;
	else {
		wxLogWarning("Unknow duration '%s'", dur.c_str());
        value = DUR_4;
	}
    return value;
}

int MusMeiInput::StrToOct(std::string oct)
{
	return atoi(oct.c_str());
}

int MusMeiInput::StrToPitch(std::string pitch)
{
    int value;
    if (pitch == "c") value = PITCH_C;
    else if (pitch == "d") value = PITCH_D;
    else if (pitch == "e") value = PITCH_E;
    else if (pitch == "f") value = PITCH_F;
    else if (pitch == "g") value = PITCH_G;
    else if (pitch == "a") value = PITCH_A;
    else if (pitch == "b") value = PITCH_B;
    else {
		wxLogWarning("Unknow pname '%s'", pitch.c_str());
        value = PITCH_C;
    }
    return value;
}



