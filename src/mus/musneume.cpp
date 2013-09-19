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


/*MusNeumeElement::MusNeumeElement(int _pitchDifference)
{
    m_pitch_difference = _pitchDifference;
    m_element_type = NEUME_ELEMENT_PUNCTUM;
}*/

// Duplicate an existing pitch
MusNeumeElement::MusNeumeElement( const MusNeumeElement &other) : MusLayerElement(other), MusPitchInterface(other) {
    m_pitch_difference = other.m_pitch_difference;
    m_element_type = other.m_element_type;
	ornament = other.ornament;
    
    // ax2 - member previously in MusElement
	m_pname = other.m_pname;
    m_oct = other.m_oct;    
}

int MusNeumeElement::getPitchDifference() {
    return m_pitch_difference;
}

NeumeElementType MusNeumeElement::getElementType()
{
    return m_element_type;
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
	ornament = NONE;
}

// Copy an existing neume
MusNeume::MusNeume( const MusNeume &neume) :
    MusLayerElement(neume), MusPitchInterface(neume)
{
    //TYPE = neume.TYPE;
    m_type = neume.m_type;
    m_pitches = neume.m_pitches;
	ornament = neume.ornament;
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
        wxString t = type.mb_str();
        throw "unknown neume type " + t;
    }
}

void MusNeume::setType(NeumeType type) {
    m_type = type;
}

NeumeType MusNeume::getType() {
    return m_type;
}

vector<MusNeumeElement> MusNeume::getPitches() {
    return m_pitches;
}


wxString MusNeume::PitchToStr(int pitch)
{
    wxString value;
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

int MusNeume::StrToPitch(wxString pitch)
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
        int octave = oct;
        int testpitch = pitch + i->getPitchDifference();
        if (testpitch > 7) {
            octave += floor((testpitch - 7)/7) + 1;
        } else if (testpitch < 1) {
            octave -= floor((1 - testpitch)/7) + 1;
        }
    }
    /* - no more MusRC access in ax2
    if (m_r) {
        m_r->DoRefresh();
    }
    */
}

