/////////////////////////////////////////////////////////////////////////////
// Name:        musneume.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin, Chris Niven
// All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "musneume.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "musneume.h"
#include "musrc.h"
#include "musstaff.h"
#include "muspage.h"
#include "musdef.h"
#include "neumedef.h"

#include <math.h>

#include <mei/mei.h>
#include <exception>

//----------------------------------------------------------------------------
// MusNeumeElement
//----------------------------------------------------------------------------

/**
 * Take an MEIElement <note> and the pitch and octave of the first note in a <neume>
 * create an element that is the difference in pitch.
 */
MusNeumeElement::MusNeumeElement(MeiElement &element, int firstpitch, int firstoct) {
    m_meiref = &element;
    MeiAttribute *p = m_meiref->getAttribute("pname");
    MeiAttribute *o = m_meiref->getAttribute("oct");
    //fake oct attribute
    if (o == NULL) {
        m_meiref->addAttribute(MeiAttribute("oct","4"));
        o = m_meiref->getAttribute("oct");
    }
    if (p && o) {
        string meipitch = p->getValue();
        int octave = atoi((o->getValue()).c_str()); //this code needs testing
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
            m_pitch_difference = ((octave - 1)*7 + 6) -  (firstoct*7 + firstpitch); // use octave - 1 to correct for standard pitch notation
        } else if (meipitch == "b") {
            m_pitch_difference = ((octave - 1)*7 + 7) -  (firstoct*7 + firstpitch); //ditto
        }
    } else {
        throw "missing pitch or octave";
    }
}

/*MusNeumeElement::MusNeumeElement(int _pitchDifference)
{
    m_pitch_difference = _pitchDifference;
    m_element_type = NEUME_ELEMENT_PUNCTUM;
}*/

// Duplicate an existing pitch
MusNeumeElement::MusNeumeElement( const MusNeumeElement &other) {
    m_pitch_difference = other.m_pitch_difference;
    m_meiref = other.m_meiref;
    m_element_type = other.m_element_type;
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
    getMeiElement().getAttribute("pname")->setValue(pitch);
    char buf[8];
    snprintf(buf, 2, "%d", oct);
    getMeiElement().getAttribute("oct")->setValue(string(buf));
}

//----------------------------------------------------------------------------
// MusNeume
//----------------------------------------------------------------------------

MusNeume::MusNeume() : MusElement() {
    TYPE = NEUME;
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
}

// Copy an existing neume
MusNeume::MusNeume( const MusNeume &neume) :
    MusElement(neume) {
    TYPE = neume.TYPE;
    m_type = neume.m_type;
    m_pitches = neume.m_pitches;
    m_meiref = neume.m_meiref;
}

/**
 * Create a neume from an MeiElement.
 */
MusNeume::MusNeume(MeiElement &element) : MusElement() {
    TYPE = NEUME;
    MeiAttribute *p = NULL;
    MeiAttribute *o = NULL;
    vector<MeiElement> children;
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
        if (children.size() > 0 && children[0].getName() == "nc") {
            vector<MeiElement> firstnotes = children[0].getChildren();
            if (firstnotes.size() > 0) {
                MeiElement note = firstnotes[0];
                p = note.getAttribute("pname");
                o = note.getAttribute("oct");
                //fake oct attribute
                if (o == NULL) {
                    note.addAttribute(MeiAttribute("oct","4"));
                    o = note.getAttribute("oct");
                }
                if (p == NULL || o == NULL) {
                    throw "missing pitch or octave";
                }
                pitch = this->StrToPitch(p->getValue());
                oct = atoi(o->getValue().c_str());
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
        oct = atoi(o->getValue().c_str());
        pitch = this->StrToPitch(p->getValue());
    } else {
        throw "invalid type for a neume";
    }
    
    //In standard notation, A is represented as 1 -> G:7, however in aruspix we
    // start with C:1, B:7. Fiddle with oct for now.
    if (pitch > 5) {
        oct--;
    }
    // Now that we have a neume, populate it with each set of notes
    if (m_meiref->getName() == "neume") {
        for (vector<MeiElement>::iterator i = m_meiref->getChildren().begin(); i != m_meiref->getChildren().end(); i++) {
            if (i->getName() == "nc") {
                readNoteContainer(*i, pitch, oct);
            }
        }
    }
}

void MusNeume::readNoteContainer(MeiElement &element, int pitch, int oct) {
    if (element.getName() == "nc") {
        for (vector<MeiElement>::iterator i = element.getChildren().begin(); i != element.getChildren().end(); i++) {
            if (i->getName() == "note") {
                MusNeumeElement note = MusNeumeElement(*i, pitch, oct);
                m_pitches.push_back(note);
            }
        }
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

vector<MusNeumeElement> MusNeume::getPitches() {
    return m_pitches;
}


//same as MusNote, except for the MEI stuff.
void MusNeume::SetPitch( int pitch, int oct )
{
    if ( this->TYPE != NEUME )
        return;
    
    if ((this->pitch == pitch) && (this->oct == oct ))
        return;
    
    this->oct = oct;
    this->pitch = pitch;
    
    for (vector<MusNeumeElement>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
        int thispitch = (i->getPitchDifference() + pitch) % 7;
        
        int octave = oct;
        int testpitch = pitch + i->getPitchDifference();
        if (testpitch > 7) {
            octave += floor((testpitch - 7)/7) + 1;
        } else if (testpitch < 1) {
            octave -= floor((1 - testpitch)/7) + 1;
        }
        if (thispitch == 0 || thispitch == 6) { //to correct for standard pitch notation.
            octave++;
        }
        i->updateMeiRef(PitchToStr(pitch), octave);
    }
    if (m_r) {
        m_r->DoRefresh();
    }
}

void MusNeume::Draw( AxDC *dc, MusStaff *staff)
{
    wxASSERT_MSG( dc , "DC cannot be NULL");
    wxASSERT_MSG( m_r, "MusRC cannot be NULL ");
    if ( !Check() )
        return;
    
    int oct = this->oct - 4; //for some reason, notes are automatically drawn 4 octaves too high?
    
    if (!m_r->m_eraseElement && (this == m_r->m_currentElement))
        m_r->m_currentColour = AxRED;
    else if (!m_r->m_eraseElement && (this->m_cmp_flag == CMP_MATCH))
        m_r->m_currentColour = AxLIGHT_GREY;
    else if (!m_r->m_eraseElement && (this->m_cmp_flag == CMP_DEL))
        m_r->m_currentColour = AxGREEN;
    else if (!m_r->m_eraseElement && (this->m_cmp_flag == CMP_SUBST))
        m_r->m_currentColour = AxBLUE;
    
    this->dec_y = staff->y_neume(this->pitch, staff->testcle( this->xrel ), oct);
    switch (m_type) {
        case (NEUME_TYPE_PUNCTUM): this->DrawPunctum(dc, staff); break;
        case (NEUME_TYPE_PUNCTUM_INCLINATUM): this->DrawPunctumInclinatum(dc, staff); break;
        case (NEUME_TYPE_VIRGA): this->DrawVirga(dc, staff); break;
        //case (NEUME_TYPE_VIRGA_LIQUESCENT): this->DrawVirgaLiquescent(dc, staff); break;
        case (NEUME_TYPE_PODATUS): this->DrawPodatus(dc, staff); break;
        case (NEUME_TYPE_CLIVIS): this->DrawClivis(dc, staff); break;
        case (NEUME_TYPE_PORRECTUS): this->DrawPorrectus(dc, staff); break;
        case (NEUME_TYPE_PORRECTUS_FLEXUS): this->DrawPorrectusFlexus(dc, staff); break;
        case (NEUME_TYPE_SCANDICUS): this->DrawScandicus(dc, staff); break;
        case (NEUME_TYPE_SCANDICUS_FLEXUS): this->DrawScandicusFlexus(dc, staff); break;
        case (NEUME_TYPE_TORCULUS): this->DrawTorculus(dc, staff); break;
        case (NEUME_TYPE_TORCULUS_LIQUESCENT): this->DrawTorculusLiquescent(dc, staff); break;
        case (NEUME_TYPE_TORCULUS_RESUPINUS): this->DrawTorculusResupinus(dc, staff); break;
        case (NEUME_TYPE_COMPOUND): this->DrawCompound(dc, staff); break;
        case (NEUME_TYPE_ANCUS): this->DrawAncus(dc, staff); break;
        case (NEUME_TYPE_CEPHALICUS): this->DrawCephalicus(dc, staff); break;
        case (NEUME_TYPE_EPIPHONUS): this->DrawEpiphonus(dc, staff); break;
        case (NEUME_TYPE_SALICUS): this->DrawSalicus(dc, staff); break;
        case (NEUME_TYPE_CUSTOS): this->DrawCustos(dc, staff); break;
        default: break;
    }
    
    m_r->m_currentColour = AxBLACK;
    return;
}

void MusNeume::NeumeLine( AxDC *dc, MusStaff *staff, int x1, int x2, int y1, int y2)
{
    dc->SetPen( m_r->m_currentColour, m_r->ToRendererX( m_p->EpLignesPortee ), wxSOLID );
    dc->SetBrush(m_r->m_currentColour , wxTRANSPARENT );
    dc->DrawLine( m_r->ToRendererX(x1) , m_r->ToRendererY (y1) , m_r->ToRendererX(x2) , m_r->ToRendererY (y2) );
    dc->ResetPen();
    dc->ResetBrush();
}

void MusNeume::DrawAncus( AxDC *dc, MusStaff *staff )
{
    // magic happens here
    int pTaille = staff->pTaille;
    
    int xn = this->xrel;
    //int xl = this->xrel;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    //printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
    //     ynn, this->dec_y, staff->yrel );
    
    xn += this->offset;
    
    int ledge = m_r->ledgerLine[pTaille][2];
    
    leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, '3', staff, this->dimin);
    m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
    xn += CLIVIS_X_DIFF;
    int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
    leg_line( dc, ynn2,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn2 + 19, nCEPHALICUS, staff, this->dimin); //notehead : 'K'
    int ynn3 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(2)).getPitchDifference());
    m_r->festa_string( dc, xn, ynn3 + 19, nLIQUES_UP, staff, this->dimin); //liquescent: 'e'
}

void MusNeume::DrawCustos( AxDC *dc, MusStaff *staff )
{
    // magic happens here
    int pTaille = staff->pTaille;
    
    int xn = this->xrel;
    //int xl = this->xrel;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    //printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
    //     ynn, this->dec_y, staff->yrel );
    
    xn += this->offset;
    
    int ledge = m_r->ledgerLine[pTaille][2];
    
    leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, nNEXT_PITCH, staff, this->dimin);
}

void MusNeume::DrawEpiphonus( AxDC *dc, MusStaff *staff )
{
    //podatus_ep for first note
    //liques_down for second
    
    // magic happens here
    int pTaille = staff->pTaille;
    
    int xn = this->xrel;
    //int xl = this->xrel;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    //printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
    //     ynn, this->dec_y, staff->yrel );
    
    xn += this->offset;
    
    int ledge = m_r->ledgerLine[pTaille][2];
    
    leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, nPODATUS_EP, staff, this->dimin);
    int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
    m_r->festa_string( dc, xn, ynn2 + 19, nLIQUES_DOWN, staff, this->dimin);
}

void MusNeume::DrawCephalicus( AxDC *dc, MusStaff *staff )
{
    // magic happens here
    int pTaille = staff->pTaille;
    
    int xn = this->xrel;
    //int xl = this->xrel;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    //printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
    //     ynn, this->dec_y, staff->yrel );
    
    xn += this->offset;
    
    int ledge = m_r->ledgerLine[pTaille][2];
    
    //stem: '3'
    leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, '3', staff, this->dimin);
    m_r->festa_string( dc, xn, ynn + 19, nCEPHALICUS, staff, this->dimin); //notehead : 'K'
    int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
    m_r->festa_string( dc, xn, ynn2 + 19, nLIQUES_UP, staff, this->dimin); //liquescent: 'e'
}

void MusNeume::DrawPunctum( AxDC *dc, MusStaff *staff ) 
{
    // magic happens here
    int pTaille = staff->pTaille;
    
    int xn = this->xrel;
    //int xl = this->xrel;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    //printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
    //     ynn, this->dec_y, staff->yrel );
    
    xn += this->offset;
    
    int ledge = m_r->ledgerLine[pTaille][2];
    leg_line( dc, ynn,bby,this->xrel,ledge, pTaille); //draw ledger lines
    m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin); //draw punctum
}

void MusNeume::DrawPunctumInclinatum( AxDC *dc, MusStaff *staff ) 
{
    // magic happens here
    int pTaille = staff->pTaille;
    
    int xn = this->xrel;
    //int xl = this->xrel;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    //printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
    //     ynn, this->dec_y, staff->yrel );
    
    xn += this->offset;
    
    int ledge = m_r->ledgerLine[pTaille][2];
    leg_line( dc, ynn,bby,this->xrel,ledge, pTaille); //draw ledger lines
    m_r->festa_string( dc, xn, ynn + 19, nDIAMOND, staff, this->dimin); //draw punctum inclinatum
}

void MusNeume::DrawVirga( AxDC *dc, MusStaff *staff ) 
{
    // magic happens here
    int pTaille = staff->pTaille;
    
    int xn = this->xrel;
    //int xl = this->xrel;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    //printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
    //     ynn, this->dec_y, staff->yrel );
    
    xn += this->offset;
    
    int ledge = m_r->ledgerLine[pTaille][2];
    leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
    m_r->festa_string( dc, xn + PUNCT_WIDTH, ynn + 19, '#', staff, this->dimin); //bottom right stem
}

void MusNeume::DrawSalicus( AxDC *dc, MusStaff *staff )
{
    // magic happens here
    int pTaille = staff->pTaille;
    
    int xn = this->xrel;
    //int xl = this->xrel;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    //printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
    //     ynn, this->dec_y, staff->yrel );
    
    xn += this->offset;
    
    int ledge = m_r->ledgerLine[pTaille][2];
    
    leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
    int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
    xn += CLIVIS_X_SAME;
    leg_line( dc, ynn2,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn2 + 19, nPES, staff, this->dimin);
    int ynn3 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(2)).getPitchDifference());
    leg_line( dc, ynn3,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn3 + 19, nPUNCTUM, staff, this->dimin);
    this->NeumeLine( dc, staff, xn + 9, xn + 9, ynn2, ynn3);
}

void MusNeume::DrawPodatus( AxDC *dc, MusStaff *staff ) 
{
    // magic happens here
    int pTaille = staff->pTaille;
    
    int xn = this->xrel;
    //int xl = this->xrel;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    //printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
    //     ynn, this->dec_y, staff->yrel );
    
    xn += this->offset;
    
    int ledge = m_r->ledgerLine[pTaille][2];
    leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, nPES, staff, this->dimin);
    int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
    leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
    this->NeumeLine( dc, staff, xn + 9, xn + 9, ynn, ynn2);
}

void MusNeume::DrawClivis( AxDC *dc, MusStaff *staff ) 
{
    // magic happens here
    int pTaille = staff->pTaille;
    
    int xn = this->xrel;
    //int xl = this->xrel;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    //printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
    //     ynn, this->dec_y, staff->yrel );
    
    xn += this->offset;
    
    int ledge = m_r->ledgerLine[pTaille][2];
    int ynn2, dx;
    leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
    m_r->festa_string( dc, xn, ynn + 19, '3', staff, this->dimin);
    ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
    if (ynn2 == ynn)
        dx = CLIVIS_X_SAME;
    else dx = CLIVIS_X_DIFF;
    leg_line( dc, ynn2,bby,this->xrel + dx,ledge, pTaille);
    m_r->festa_string( dc, xn + dx, ynn2 + 19, nPUNCTUM, staff, this->dimin);
}

void MusNeume::DrawPorrectus( AxDC *dc, MusStaff *staff ) 
{
    // magic happens here
    int pTaille = staff->pTaille;
    
    int xn = this->xrel;
    //int xl = this->xrel;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    //printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
    //     ynn, this->dec_y, staff->yrel );
    
    xn += this->offset;
    
    int ledge = m_r->ledgerLine[pTaille][2];
    leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, '3', staff, this->dimin);
    wxString slope; int dx;
    if ((this->m_pitches.at(1)).getPitchDifference()==-1) {
        slope = nPORRECT_1;
        dx = -5;
    }
    else if ((this->m_pitches.at(1)).getPitchDifference()==-2) {
        slope = nPORRECT_2;
        dx = -2;
    }
    else if ((this->m_pitches.at(1)).getPitchDifference()==-3) {
        slope = nPORRECT_3;
        dx = 3;
    }
    else if ((this->m_pitches.at(1)).getPitchDifference()==-4) {
        slope = nPORRECT_4;
        dx = 3;
    }
    m_r->festa_string( dc, xn, ynn + 19, slope, staff, this->dimin);
    xn += 3*PUNCT_WIDTH + dx;
    int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(2)).getPitchDifference());
    leg_line( dc, ynn,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn2 + 19, nPUNCTUM, staff, this->dimin);
    xn += PUNCT_WIDTH - 1;
    int ynn3 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
    this->NeumeLine( dc, staff, xn, xn, ynn2, ynn3);
}

void MusNeume::DrawPorrectusFlexus( AxDC *dc, MusStaff *staff)
{
    // magic happens here
    int pTaille = staff->pTaille;    
    
    int xn = this->xrel;
    //int xl = this->xrel;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    //printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
    //     ynn, this->dec_y, staff->yrel );
    
    xn += this->offset;
    
    int ledge = m_r->ledgerLine[pTaille][2];
    leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, '3', staff, this->dimin);
    wxString slope; int dx;
    if ((this->m_pitches.at(1)).getPitchDifference()==-1) {
        slope = nPORRECT_1;
        dx = -5;
    }
    else if ((this->m_pitches.at(1)).getPitchDifference()==-2) {
        slope = nPORRECT_2;
        dx = -2;
    }
    else if ((this->m_pitches.at(1)).getPitchDifference()==-3) {
        slope = nPORRECT_3;
        dx = 3;
    }
    else if ((this->m_pitches.at(1)).getPitchDifference()==-4) {
        slope = nPORRECT_4;
        dx = 3;
    }
    m_r->festa_string( dc, xn, ynn + 19, slope, staff, this->dimin);
    xn += 4*PUNCT_WIDTH + dx;
    int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(2)).getPitchDifference());
    leg_line( dc, ynn2,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn2 + 19, nPUNCTUM, staff, this->dimin);
    xn += PUNCT_WIDTH - 1;
    int ynn3 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(3)).getPitchDifference());
    leg_line( dc, ynn3,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn3 + 19, nPUNCTUM, staff, this->dimin);
    this->NeumeLine( dc, staff, xn + 1, xn, ynn2, ynn3);
}

void MusNeume::DrawScandicus( AxDC *dc, MusStaff *staff ) 
{
    // magic happens here
    int pTaille = staff->pTaille;
    
    int xn = this->xrel;
    //int xl = this->xrel;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    //printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
    //     ynn, this->dec_y, staff->yrel );
    
    xn += this->offset;
    
    int ledge = m_r->ledgerLine[pTaille][2];
    leg_line( dc, ynn,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
    xn += PUNCT_WIDTH;
    ynn = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
    leg_line( dc, ynn,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
    xn += PUNCT_WIDTH;
    ynn = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(2)).getPitchDifference());
    leg_line( dc, ynn,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
}

void MusNeume::DrawScandicusFlexus( AxDC *dc, MusStaff *staff )
{
    // magic happens here
    int pTaille = staff->pTaille;
    
    int xn = this->xrel;
    //int xl = this->xrel;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    //printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
    //     ynn, this->dec_y, staff->yrel );
    
    xn += this->offset;
    
    int ledge = m_r->ledgerLine[pTaille][2];
    leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, nPES, staff, this->dimin);
    int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
    leg_line( dc, ynn2,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn2 + 19, nPUNCTUM, staff, this->dimin);
    this->NeumeLine( dc, staff, xn + 9, xn + 9, ynn, ynn2);
    xn += CLIVIS_X_SAME;
    int ynn3 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(2)).getPitchDifference());
    leg_line( dc, ynn3,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn3 + 19, '3', staff, this->dimin);
    m_r->festa_string( dc, xn, ynn3 + 19, nPUNCTUM, staff, this->dimin);
    xn += PUNCT_WIDTH - 2;
    int ynn4 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(3)).getPitchDifference());
    leg_line( dc, ynn4,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn4 + 19, nPUNCTUM, staff, this->dimin);
    this->NeumeLine( dc, staff, xn + 1, xn + 1, ynn3, ynn4);
}

void MusNeume::DrawTorculus( AxDC *dc, MusStaff *staff ) 
{
    // magic happens here
    int pTaille = staff->pTaille;
    
    int xn = this->xrel;
    //int xl = this->xrel;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    //printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
    //     ynn, this->dec_y, staff->yrel );
    
    xn += this->offset;
    
    int ledge = m_r->ledgerLine[pTaille][2];
    leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
    int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
    this->NeumeLine( dc, staff, xn + 9, xn + 9, ynn, ynn2);
    xn += PUNCT_WIDTH - 2;
    leg_line( dc, ynn2,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn2 + 19, nPUNCTUM, staff, this->dimin);
    int ynn3 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(2)).getPitchDifference());
    xn += PUNCT_WIDTH - 2;
    this->NeumeLine( dc, staff, xn + 1, xn + 1, ynn2, ynn3);
    leg_line( dc, ynn3,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn3 + 19, nPUNCTUM, staff, this->dimin);
}

void MusNeume::DrawTorculusLiquescent( AxDC *dc, MusStaff *staff )
{
    // magic happens here
    int pTaille = staff->pTaille;
    
    int xn = this->xrel;
    //int xl = this->xrel;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    //printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
    //     ynn, this->dec_y, staff->yrel );
    
    xn += this->offset;
    
    int ledge = m_r->ledgerLine[pTaille][2];
    leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
    int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
    this->NeumeLine( dc, staff, xn + 9, xn + 9, ynn, ynn2);
    xn += PUNCT_WIDTH - 2;
    leg_line( dc, ynn2,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn2 + 19, nCEPHALICUS, staff, this->dimin); //notehead : 'K'
    int ynn3 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(2)).getPitchDifference());
    m_r->festa_string( dc, xn, ynn3 + 19, nLIQUES_UP, staff, this->dimin); //liquescent: 'e'
}

void MusNeume::DrawTorculusResupinus( AxDC *dc, MusStaff *staff )
{
    // magic happens here
    int pTaille = staff->pTaille;
    
    int xn = this->xrel;
    //int xl = this->xrel;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    //printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
    //     ynn, this->dec_y, staff->yrel );
    
    xn += this->offset;
    
    int ledge = m_r->ledgerLine[pTaille][2];
    
    leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, nPES, staff, this->dimin);
    
    xn += PUNCT_WIDTH;
    int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
    leg_line( dc, ynn2,bby,this->xrel,ledge, pTaille);
    wxString slope; int dx;
    if (((this->m_pitches.at(1)).getPitchDifference())-((this->m_pitches.at(2)).getPitchDifference())==1) {
        slope = nPORRECT_1;
        dx = -5;
    }
    else if (((this->m_pitches.at(1)).getPitchDifference())-((this->m_pitches.at(2)).getPitchDifference())==2) {
        slope = nPORRECT_2;
        dx = -2;
    }
    else if (((this->m_pitches.at(1)).getPitchDifference())-((this->m_pitches.at(2)).getPitchDifference())==3) {
        slope = nPORRECT_3;
        dx = 3;
    }
    else if (((this->m_pitches.at(1)).getPitchDifference())-((this->m_pitches.at(2)).getPitchDifference())==4) {
        slope = nPORRECT_4;
        dx = 3;
    }
    m_r->festa_string( dc, xn, ynn2 + 19, slope, staff, this->dimin);
    xn += 3*PUNCT_WIDTH + dx;
    int ynn3 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(3)).getPitchDifference());
    leg_line( dc, ynn3,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn3 + 19, nPUNCTUM, staff, this->dimin);
    xn += PUNCT_WIDTH - 1;
    int ynn4 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(2)).getPitchDifference());
    this->NeumeLine( dc, staff, xn, xn, ynn3, ynn4);
}

void MusNeume::DrawCompound( AxDC *dc, MusStaff *staff ) 
{
    // magic happens here
    int pTaille = staff->pTaille;
    
    int xn = this->xrel;
    //int xl = this->xrel;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    //printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
    //     ynn, this->dec_y, staff->yrel );
    
    xn += this->offset;
    
    int ledge = m_r->ledgerLine[pTaille][2];
    leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
    int dx = (PUNCT_WIDTH + 1)*(this->m_pitches.size());
    int y1 = 0, y2 = 0;
    for (vector<MusNeumeElement>::iterator i = this->m_pitches.begin() + 1; i != this->m_pitches.end(); i++) {
        MusNeumeElement e = *i;
        if (e.getPitchDifference() > y1)
            y1 = e.getPitchDifference();
        if (e.getPitchDifference() < y2)
            y2 = e.getPitchDifference();
    }
    y1 = ynn + (m_r->_espace[pTaille])*y1;
    y2 = ynn + (m_r->_espace[pTaille])*y2;
    m_r->box( dc, xn, y1, xn + dx, y2 );
}

//Also same as MusNote. Could use an update, since it fails to draw ledger lines immediately below the staff.
void MusNeume::leg_line( AxDC *dc, int y_n, int y_p, int xn, unsigned int smaller, int pTaille)
{
    int yn, ynt, yh, yb, test, v_decal = m_r->_interl[pTaille];
    int dist, xng, xnd;
    register int i;
    
    
    yh = y_p + m_r->_espace[pTaille]; yb = y_p- m_r->_portee[pTaille]- m_r->_espace[pTaille];
    
    if (!in(y_n,yh,yb))                           // note hors-portee?
    {
        xng = xn - smaller;
        xnd = xn + smaller;
        
        dist = ((y_n > yh) ? (y_n - y_p) : y_p - m_r->_portee[pTaille] - y_n);
        ynt = ((dist % m_r->_interl[pTaille] > 0) ? (dist - m_r->_espace[pTaille]) : dist);
        test = ynt/ m_r->_interl[pTaille];
        if (y_n > yh)
        {   yn = ynt + y_p;
            v_decal = - m_r->_interl[pTaille];
        }
        else
            yn = y_p - m_r->_portee[pTaille] - ynt;
        
        //hPen = (HPEN)SelectObject (hdc, CreatePen (PS_SOLID, _param.EpLignesPORTEE+1, workColor2));
        //xng = toZoom(xng);
        //xnd = toZoom(xnd);
        
        dc->SetPen( m_r->m_currentColour, m_r->ToRendererX( m_p->EpLignesPortee ), wxSOLID );
        dc->SetBrush(m_r->m_currentColour , wxTRANSPARENT );
        
        for (i = 0; i < test; i++)
        {
            dc->DrawLine( m_r->ToRendererX(xng) , m_r->ToRendererY ( yn ) , m_r->ToRendererX(xnd) , m_r->ToRendererY ( yn ) );
            //m_r->h_line ( dc, xng, xnd, yn, m_r->_param.EpLignesPORTEE);
            //yh =  toZoom(yn);
            //MoveToEx (hdc, xng, yh, NULL);
            //LineTo (hdc, xnd, yh);
            
            yn += v_decal;
        }
        
        dc->ResetPen();
        dc->ResetBrush();
    }
    return;
}
