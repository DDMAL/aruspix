/////////////////////////////////////////////////////////////////////////////
// Name:        musneume.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin, Chris Niven
// All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musneume.h"
#include "musdef.h"

#include <math.h>

//----------------------------------------------------------------------------
// MusNeumeElement
//----------------------------------------------------------------------------

/**
 * Take an MEIElement <note> and the pitch and octave of the first note in a <neume>
 * create an element that is the difference in pitch.
 */
MusNeumeElement::MusNeumeElement(MeiElement &element, int firstpitch, int firstoct) : MusLayerElement(), MusPitchInterface() {
    m_meiref = &element;
    MeiAttribute *p = m_meiref->getAttribute("pname");
    MeiAttribute *o = m_meiref->getAttribute("oct");
    //fake oct attribute
    if (o == NULL) {
        //m_meiref->addAttribute(MeiAttribute("oct","4")); // BROKEN!!! ax2  - Commented by LP - does not work with the new version of libmei
        o = m_meiref->getAttribute("oct");
    }
    if (p && o) {
        string meipitch = p->getValue();
        int octave = atoi((o->getValue()).c_str());
        if (meipitch == "c") {
            m_pitch_difference = (octave*7 + 1) -  (firstoct*7 + firstpitch);
        } else if (meipitch == "d") {
            m_pitch_difference = (octave*7 + 2) -  (firstoct*7 + firstpitch);
        } else if (meipitch == "e") {
            m_pitch_difference = (octave*7 + 3) -  (firstoct*7 + firstpitch);
        } else if (meipitch == "f") {
            m_pitch_difference = (octave*7 + 4) -  (firstoct*7 + firstpitch);
        } else if (meipitch == "g") {
            m_pitch_difference = (octave*7 + 5) -  (firstoct*7 + firstpitch);
        } else if (meipitch == "a") {
            m_pitch_difference = (octave*7 + 6) -  (firstoct*7 + firstpitch);
        } else if (meipitch == "b") {
            m_pitch_difference = (octave*7 + 7) -  (firstoct*7 + firstpitch);
        }
    } else {
        throw "missing pitch or octave";
    }
    // // BROKEN!!! ax2  - Commented by LP - does not work with the new version of libmei
    /*
	for (vector<MeiElement*>::iterator i = m_meiref->getChildren().begin(); i != m_meiref->getChildren().end(); i++) {
		if ((*i)->getName() == "dot") {
			this->ornament = DOT;
		}
	}
    */
}

/*MusNeumeElement::MusNeumeElement(int _pitchDifference)
{
    m_pitch_difference = _pitchDifference;
    m_element_type = NEUME_ELEMENT_PUNCTUM;
}*/

// Duplicate an existing pitch
MusNeumeElement::MusNeumeElement( const MusNeumeElement &other) : MusLayerElement(other), MusPitchInterface(other) {
    m_pitch_difference = other.m_pitch_difference;
    m_meiref = other.m_meiref;
    m_element_type = other.m_element_type;
	ornament = other.ornament;
    
    // ax2 - member previously in MusElement
	m_pname = other.m_pname;
    m_oct = other.m_oct;    
}

int MusNeumeElement::getPitchDifference() {
    return m_pitch_difference;
}

MeiElement &MusNeumeElement::getMeiElement() {
    return *m_meiref;
}

NeumeElementType MusNeumeElement::getElementType()
{
    return m_element_type;
}

void MusNeumeElement::updateMeiRef(string pitch, int oct) {
    m_meiref->getAttribute("pname")->setValue(pitch);
    char buf[8];
    snprintf(buf, 2, "%d", oct);
    m_meiref->getAttribute("oct")->setValue(string(buf));
}

void MusNeumeElement::deleteMeiRef() {
	if (m_meiref->hasParent()) {
		//m_meiref->getParent().removeChild(m_meiref); // // BROKEN!!! ax2  - Commented by LP - does not work with the new version of libmei
		delete m_meiref;
	}
}

NeumeOrnament MusNeumeElement::getOrnament() {
	return ornament;
}

//----------------------------------------------------------------------------
// MusNeume
//----------------------------------------------------------------------------

MusNeume::MusNeume() : MusLayerElement(), MusPitchInterface() {
    //TYPE = NEUME;
    m_type = NEUME_TYPE_CUSTOS; //note: for all practical purposes, this can always be punctum.
    // For testing only
    /*MusNeumeElement first = MusNeumeElement(0);
    MusNeumeElement next = MusNeumeElement(1);
    MusNeumeElement third = MusNeumeElement(-1);
    MusNeumeElement fourth = MusNeumeElement(1);
    MusNeumeElement fifth = MusNeumeElement(3);
    m_pitches.push_back(first);
    m_pitches.push_back(next);
    m_pitches.push_back(third);
    m_pitches.push_back(fourth);
    m_pitches.push_back(fifth);*/
    m_meiref = 0; //this is necessary to avoid garbage when things aren't called from MEIs.
	ornament = NONE;
}

// Copy an existing neume
MusNeume::MusNeume( const MusNeume &neume) :
    MusLayerElement(neume), MusPitchInterface(neume)
{
    //TYPE = neume.TYPE;
    m_type = neume.m_type;
    m_pitches = neume.m_pitches;
    m_meiref = neume.m_meiref;
	ornament = neume.ornament;
}

/**
 * Create a neume from an MeiElement.
 */
MusNeume::MusNeume(MeiElement &element) : MusLayerElement(), MusPitchInterface() {
    //TYPE = NEUME;
    MeiAttribute *p = NULL;
    MeiAttribute *o = NULL;
    vector<MeiElement*> children;
    if (element.getName() == "neume") {
        m_meiref = &element;
        MeiAttribute *nameattr = m_meiref->getAttribute("name");
        wxString type = "";
        if (nameattr) {
            type = (nameattr->getValue()).c_str();
        }
        setType(type);

        // Find the first note in the first nc
        children = m_meiref->getChildren();
        bool fail = true;
        if (children.size() > 0 && children[0]->getName() == "nc") {
            vector<MeiElement*> firstnotes = children[0]->getChildren();
            if (firstnotes.size() > 0) {
                MeiElement note = *firstnotes[0];
                p = note.getAttribute("pname");
                o = note.getAttribute("oct");
                //fake oct attribute
                if (o == NULL) {
                    //note.addAttribute(MeiAttribute("oct","4")); // // BROKEN!!! ax2  - Commented by LP - does not work with the new version of libmei
                    o = note.getAttribute("oct");
                }
                if (p == NULL || o == NULL) {
                    throw "missing pitch or octave";
                }
                m_pname = this->StrToPitch(p->getValue());
                m_oct = atoi(o->getValue().c_str());
                fail = false;
            }
        }
        if (fail) {
            throw "no pitches for neume";
        }
    } else if (element.getName() == "custos") {
        m_meiref = &element;
        setType(element.getName().c_str());
        p = m_meiref->getAttribute("pname");
        o = m_meiref->getAttribute("oct");
        if (p == NULL || o == NULL) {
            throw "missing pitch or octave";
        }
        m_oct = atoi(o->getValue().c_str());
        m_pname = this->StrToPitch(p->getValue());
    } else {
        throw "invalid type for a neume";
    }
    
    // Now that we have a neume, populate it with each set of notes
    // // BROKEN!!! ax2  - Commented by LP - does not work with the new version of libmei
    /*
    if (m_meiref->getName() == "neume") {
        for (vector<MeiElement*>::iterator i = m_meiref->getChildren().begin(); i != m_meiref->getChildren().end(); i++) {
            if ((*i)->getName() == "nc") {
                readNoteContainer(**i, m_pname, m_oct);
            }
        }
    }
    */
}

void MusNeume::readNoteContainer(MeiElement &element, int pitch, int oct) {
    if (element.getName() == "nc") {
        if (element.getAttribute("inclinatum") != NULL && element.getAttribute("inclinatum")->getValue() == "true") {
            this->setType(NEUME_TYPE_PUNCTUM_INCLINATUM);
        }
        // // BROKEN!!! ax2  - Commented by LP - does not work with the new version of libmei
        /*
        for (vector<MeiElement*>::iterator i = element.getChildren().begin(); i != element.getChildren().end(); i++) {
            if ((*i)->getName() == "note") {
                MusNeumeElement note = MusNeumeElement(**i, pitch, oct);
                m_pitches.push_back(note);
            }
        }
        */
    }
}

/**
 * The type of this neume (from MEI) e.g. 'torculus', 'podatus'
 */
void MusNeume::setType(wxString type)
{
    if (type == "punctum") {
        m_type = NEUME_TYPE_PUNCTUM;
    } else if (type == "virga") {
        m_type = NEUME_TYPE_VIRGA;
    } else if (type == "podatus") {
        m_type = NEUME_TYPE_PODATUS;
    } else if (type == "clivis") {
        m_type = NEUME_TYPE_CLIVIS;
    } else if (type == "porrectus") {
        m_type = NEUME_TYPE_PORRECTUS;
    } else if (type == "scandicus") {
        m_type = NEUME_TYPE_SCANDICUS;
    } else if (type == "torculus") {
        m_type = NEUME_TYPE_TORCULUS;
    } else if (type == "compound") {
        m_type = NEUME_TYPE_COMPOUND;
    } else if (type == "cephalicus") { //maybe liquescent clivis
        m_type = NEUME_TYPE_CEPHALICUS;
    } else if (type == "custos") {
        m_type = NEUME_TYPE_CUSTOS;
    } else if (type == "ancus") { //maybe irrelevant
        m_type = NEUME_TYPE_ANCUS;
    } else if (type == "epiphonus") { //maybe podatus liquescent
        m_type = NEUME_TYPE_EPIPHONUS;
    } else if (type == "porrectus flexus") { //maybe formatted differently?
        m_type = NEUME_TYPE_PORRECTUS_FLEXUS;
    } else if (type == "salicus") { //maybe scandicus?
        m_type = NEUME_TYPE_SALICUS;
    } else if (type == "scandicus flexus") {
        m_type = NEUME_TYPE_SCANDICUS_FLEXUS;
    } else if (type == "torculus resupinus") {
        m_type = NEUME_TYPE_TORCULUS_RESUPINUS;
        //torculus liquescent?
    } else {
        string t = type.mb_str();
        throw "unknown neume type " + t;
    }
}

void MusNeume::setType(NeumeType type) {
    m_type = type;
}

NeumeType MusNeume::getType() {
    return m_type;
}

MeiElement &MusNeume::getMeiElement() {
    return *m_meiref;
}

void MusNeume::deleteMeiRef() {
	if (m_meiref->hasParent()) {
		//m_meiref->getParent().removeChild(m_meiref); // // BROKEN!!! ax2  - Commented by LP - does not work with the new version of libmei
		delete m_meiref;
	} else {
		delete m_meiref;
	}
}

vector<MusNeumeElement> MusNeume::getPitches() {
    return m_pitches;
}


std::string MusNeume::PitchToStr(int pitch)
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

int MusNeume::StrToPitch(std::string pitch)
{
    int value;
    if (pitch == "c") {
        value = 1;
    } else if (pitch == "d") {
        value = 2;
    } else if (pitch == "e") {
        value = 3;
    } else if (pitch == "f") {
        value = 4;
    } else if (pitch == "g") {
        value = 5;
    } else if (pitch == "a") {
        value = 6;
    } else if (pitch == "b") {
        value = 7;
    } else {
        value = -1;
    }
    return value;
}

//same as MusNote1, except for the MEI stuff.
void MusNeume::SetPitch( int pitch, int oct )
{
    if ((this->m_pname == pitch) && (this->m_oct == oct ))
        return;
    
    this->m_oct = oct;
    this->m_pname = pitch;
    
    for (vector<MusNeumeElement>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
        int thispitch = (i->getPitchDifference() + pitch) % 7;
        
        int octave = oct;
        int testpitch = pitch + i->getPitchDifference();
        if (testpitch > 7) {
            octave += floor((testpitch - 7)/7) + 1;
        } else if (testpitch < 1) {
            octave -= floor((1 - testpitch)/7) + 1;
        }
        i->updateMeiRef(PitchToStr(thispitch), octave);
    }
    /* - no more MusRC access in ax2
    if (m_r) {
        m_r->DoRefresh();
    }
    */
}

