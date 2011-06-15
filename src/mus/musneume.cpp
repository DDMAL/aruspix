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
#include <uuid/uuid.h>
#include <exception>

//----------------------------------------------------------------------------
// MusNeumeElement
//----------------------------------------------------------------------------

/**
 * Take an MEIElement <note> and the pitch and octave of the first note in a <neume>
 * create an element that is the difference in pitch.
 */
MusNeumeElement::MusNeumeElement() : MusElement() {
	TYPE = NEUME_ELMT;
	m_meiref = NULL;
	m_pitch_difference = 0;
	ornament = NONE;
	m_element_type = NEUME_ELEMENT_PUNCTUM;
}

MusNeumeElement::MusNeumeElement(MeiElement &element, int firstpitch, int firstoct) : MusElement() {
    TYPE = NEUME_ELMT;
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
        int octave = atoi((o->getValue()).c_str());
		if (0 <= octave <= 7) {
			m_pitch_difference = (StrToPitch(meipitch) + octave*7) - (firstoct*7 + firstpitch);
		} else {
			throw "octave out of range";
		}
    } else {
        throw "missing pitch or octave";
    }
	for (vector<MeiElement*>::iterator i = m_meiref->getChildren().begin(); i != m_meiref->getChildren().end(); i++) {
		if ((*i)->getName() == "dot") {
			this->ornament = DOT;
		}
	}
}

MusNeumeElement::MusNeumeElement(int _pitchDifference)
{
    TYPE = NEUME_ELMT;
	m_pitch_difference = _pitchDifference;
	ornament = NONE;
	m_meiref = NULL;
    m_element_type = NEUME_ELEMENT_PUNCTUM;
}

// Duplicate an existing pitch
MusNeumeElement::MusNeumeElement( const MusNeumeElement &other) : MusElement(other) {
    m_pitch_difference = other.m_pitch_difference;
    m_meiref = other.m_meiref;
    m_element_type = other.m_element_type;
	ornament = other.ornament;
}

int MusNeumeElement::getPitchDifference() {
    return m_pitch_difference;
}

MeiElement* MusNeumeElement::getMeiElement() {
    return m_meiref;
}

NeumeElementType MusNeumeElement::getElementType()
{
    return m_element_type;
}

void MusNeumeElement::updateMeiElement(string pitch, int oct) {
    if (m_meiref->getAttributes().size() > 0) {
		if (m_meiref->hasAttribute("pname")) {
			m_meiref->getAttribute("pname")->setValue(pitch);
		} else {
			m_meiref->addAttribute(MeiAttribute("pname",pitch));
		}
		MeiAttribute *octattr = m_meiref->getAttribute("oct");
		char buf[8];
		snprintf(buf, 2, "%d", oct);
		if (octattr == NULL) {
			m_meiref->addAttribute(MeiAttribute("oct",string(buf)));
		} else {
		octattr->setValue(string(buf));
		}
	} else {
		m_meiref->addAttribute(MeiAttribute("pname",pitch));
		char buf[8];
		snprintf(buf, 2, "%d", oct);
		m_meiref->addAttribute(MeiAttribute("oct",string(buf)));
	}
}

void MusNeumeElement::setMeiElement(MeiElement *element) {
	this->m_meiref = element;
}

void MusNeumeElement::deleteMeiRef() {
	if (m_meiref->hasParent()) {
		m_meiref->getParent().removeChild(m_meiref);
	}
	if (m_meiref->getZone()->hasParent()) {
		m_meiref->getZone()->getParent().removeChild(m_meiref->getZone());
	}
	delete m_meiref->getZone();
	delete m_meiref;
}

NeumeOrnament MusNeumeElement::getOrnament() {
	return ornament;
}

//----------------------------------------------------------------------------
// MusNeume
//----------------------------------------------------------------------------

MusNeume::MusNeume() : MusElement() {
    TYPE = NEUME;
    m_type = NEUME_TYPE_PUNCTUM; //note: for all practical purposes, this can always be punctum.
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
    m_meiref = NULL; //this is necessary to avoid garbage when things aren't called from MEIs.
	ornament = NONE;
	newmeielement = false;
	inclinatum = false;
	quilisma = false;
}

// Copy an existing neume
MusNeume::MusNeume( const MusNeume &neume) :
    MusElement(neume) {
    TYPE = neume.TYPE;
    m_type = neume.m_type;
    m_pitches = neume.m_pitches;
    m_meiref = neume.m_meiref;
	m_meistaffzone = neume.m_meistaffzone;
	ornament = neume.ornament;
	newmeielement = neume.newmeielement;
	inclinatum = neume.inclinatum;
	quilisma = neume.quilisma;	
}

/**
 * Create a neume from an MeiElement.
 */
MusNeume::MusNeume(MeiElement &element) : MusElement() {
    TYPE = NEUME;
	newmeielement = false;
    MeiAttribute *p = NULL;
    MeiAttribute *o = NULL;
    vector<MeiElement*> children;
    if (element.getName() == "neume") {
        m_meiref = &element;
        MeiAttribute *nameattr = m_meiref->getAttribute("name");
		MeiAttribute *variantattr = m_meiref->getAttribute("variant");
        wxString type = "", variant = "";
        if (nameattr) {
            type = (nameattr->getValue()).c_str();
        }
		if (variantattr) {
			variant = (variantattr->getValue()).c_str();
		}
        setType(type, variant);

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
                    note.addAttribute(MeiAttribute("oct","4"));
                    o = note.getAttribute("oct");
                }
                if (p == NULL || o == NULL) {
                    throw "missing pitch or octave";
                }
                pitch = StrToPitch(p->getValue());
                int octave = atoi(o->getValue().c_str());
				if ( 0 <= octave <= 7 ) {
					oct = octave;
				} else {
					throw "octave out of range";
				}
                fail = false;
            }
        }
        if (fail) {
            throw "no pitches for neume";
        }
    } else if (element.getName() == "custos") {
        m_meiref = &element;
        setType(NEUME_TYPE_CUSTOS);
        p = m_meiref->getAttribute("pname");
        o = m_meiref->getAttribute("oct");
        if (p == NULL || o == NULL) {
            throw "missing pitch or octave";
        }
        int octave = atoi(o->getValue().c_str());
		if ( 0 <= octave <= 7 ) {
			oct = octave;
		} else {
			throw "octave out of range";
		}
        pitch = this->StrToPitch(p->getValue());
    } else {
        throw "invalid type for a neume";
    }
    
    // Now that we have a neume, populate it with each set of notes
    if (m_meiref->getName() == "neume") {
        for (vector<MeiElement*>::iterator i = m_meiref->getChildren().begin(); i != m_meiref->getChildren().end(); i++) {
            if ((*i)->getName() == "nc") {
                readNoteContainer(**i, pitch, oct);
            }
        }
    }
}

void MusNeume::readNoteContainer(MeiElement &element, int pitch, int oct) {
    if (element.getName() == "nc") {
        if (element.getAttribute("inclinatum") != NULL && element.getAttribute("inclinatum")->getValue() == "true") {
            this->setType(NEUME_TYPE_PUNCTUM_INCLINATUM);
        }
        for (vector<MeiElement*>::iterator i = element.getChildren().begin(); i != element.getChildren().end(); i++) {
            if ((*i)->getName() == "note") {
                MusNeumeElement note = MusNeumeElement(**i, pitch, oct);
                m_pitches.push_back(note);
            }
        }
    }
}

/**
 * The type of this neume (from MEI) e.g. 'torculus', 'podatus'
 */
void MusNeume::setType(wxString type, wxString variant)
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
		if (variant == "flexus") {
			m_type = NEUME_TYPE_PORRECTUS_FLEXUS;
		} else if (variant == "") {
			m_type = NEUME_TYPE_PORRECTUS;
		} else {
			string t = variant.mb_str();
			throw "unknown variant on a neume: " + t;
		}
    } else if (type == "scandicus") {
		if (variant == "flexus") {
			m_type = NEUME_TYPE_SCANDICUS_FLEXUS;
		} else if (variant == "") {
			m_type = NEUME_TYPE_SCANDICUS;
		} else {
			string t = variant.mb_str();
			throw "unknown variant on a neume: " + t;
		}
    } else if (type == "torculus") {
		if (variant == "resupinus") {
			m_type = NEUME_TYPE_TORCULUS_RESUPINUS;
		} else if (variant == "") {
			m_type = NEUME_TYPE_TORCULUS;
		} else {
			string t = variant.mb_str();
			throw "unknown variant on a neume: " + t;
		}
    } else if (type == "compound") {
        m_type = NEUME_TYPE_COMPOUND;
    } else if (type == "cephalicus") { //maybe liquescent clivis
        m_type = NEUME_TYPE_CEPHALICUS;
    } else if (type == "custos") {
        m_type = NEUME_TYPE_CUSTOS;
    } else if (type == "ancus") { //maybe irrelevant
        m_type = NEUME_TYPE_ANCUS;
    } else if (type == "epiphonus" || (type == "podatus" && variant == "liquescent")) { //liquescent isn't represented yet
        m_type = NEUME_TYPE_EPIPHONUS;
    } else if (type == "salicus") { //maybe scandicus?
        m_type = NEUME_TYPE_SALICUS;
    } else if (type == "cavum") {
		m_type = NEUME_TYPE_PUNCTUM_WHITE;
	}else {
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

MeiElement* MusNeume::getMeiRef() {
    return m_meiref;
}

void MusNeume::setMeiRef(MeiElement *element) {
	this->m_meiref = element;
}

void MusNeume::updateMeiRef() {
	if (m_type != NEUME_TYPE_CUSTOS) {
		if (m_meiref->getName() != "neume") {
			m_meiref->setName("neume");
		}
		if (!m_meiref->hasChild("nc")) {
			MeiElement *nc = new MeiElement("nc");
			std::string id;
			char uuidbuff[36];
			uuid_t uuidGenerated;
			uuid_generate(uuidGenerated);
			uuid_unparse(uuidGenerated, uuidbuff);
			id = string(uuidbuff);
			std::transform(id.begin(), id.end(), id.begin(), ::tolower);
			id = "m-" + id;
			nc->setId(id);
			m_meiref->addChild(nc);
			for (vector<MusNeumeElement>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
				MeiElement *child = new MeiElement("note");
				std::string id;
				char uuidbuff[36];
				uuid_t uuidGenerated;
				uuid_generate(uuidGenerated);
				uuid_unparse(uuidGenerated, uuidbuff);
				id = string(uuidbuff);
				std::transform(id.begin(), id.end(), id.begin(), ::tolower);
				id = "m-" + id;
				child->setId(id);
				nc->addChild(child);
				i->setMeiElement(child);
				i->updateMeiElement(PitchToStr(this->pitch), this->oct);
			}
		} else {
			if (this->newmeielement) { //not sure if this creates a memory leak, since the children were initially created with "new"?
				for (vector<MeiElement*>::iterator i = m_meiref->getChildren().begin(); i != m_meiref->getChildren().end(); i++) {
					for (vector<MeiElement*>::iterator it = (*i)->getChildren().begin(); it != (*i)->getChildren().end(); it++) {
						delete (*it);
					}
					(*i)->getChildren().clear();
					delete (*i);
				}
				m_meiref->getChildren().clear();
				updateMeiRef();
			}
		}
	}
	this->SetPitch(this->pitch, this->oct);
	for (vector<MeiAttribute>::iterator i = m_meiref->getAttributes().begin(); i != m_meiref->getAttributes().end(); i++) {
		std::string name = i->getName();
		if (name != "facs") {
			i = m_meiref->getAttributes().erase(i);
			i--;
		}
	}
	switch (m_type) {
		case NEUME_TYPE_ANCUS: m_meiref->addAttribute(MeiAttribute("name","ancus")); break;
		case NEUME_TYPE_CEPHALICUS: m_meiref->addAttribute(MeiAttribute("name","cephalicus")); break;
		case NEUME_TYPE_CLIVIS: m_meiref->addAttribute(MeiAttribute("name","clivis")); break;
		case NEUME_TYPE_EPIPHONUS: m_meiref->addAttribute(MeiAttribute("name","epiphonus")); break;
		case NEUME_TYPE_PODATUS: m_meiref->addAttribute(MeiAttribute("name","podatus")); break;
		case NEUME_TYPE_PORRECTUS_FLEXUS: m_meiref->addAttribute(MeiAttribute("variant","flexus"));
		case NEUME_TYPE_PORRECTUS: m_meiref->addAttribute(MeiAttribute("name","porrectus")); break;
		case NEUME_TYPE_PUNCTUM_INCLINATUM: m_meiref->getChildren()[0]->addAttribute(MeiAttribute("inclinatum","true"));
		case NEUME_TYPE_PUNCTUM: m_meiref->addAttribute(MeiAttribute("name","punctum")); break;
		case NEUME_TYPE_PUNCTUM_WHITE: m_meiref->addAttribute(MeiAttribute("name","cavum")); break;
		case NEUME_TYPE_SALICUS: m_meiref->addAttribute(MeiAttribute("name","salicus")); break;
		case NEUME_TYPE_SCANDICUS_FLEXUS: m_meiref->addAttribute(MeiAttribute("variant","flexus"));
		case NEUME_TYPE_SCANDICUS: m_meiref->addAttribute(MeiAttribute("name","scandicus")); break;
		case NEUME_TYPE_TORCULUS_RESUPINUS: m_meiref->addAttribute(MeiAttribute("variant","resupinus"));
		case NEUME_TYPE_TORCULUS_LIQUESCENT: //no implementation yet
		case NEUME_TYPE_TORCULUS: m_meiref->addAttribute(MeiAttribute("name","torculus")); break;
		case NEUME_TYPE_VIRGA_LIQUESCENT: //no implementation, probably deprecated
		case NEUME_TYPE_VIRGA: m_meiref->addAttribute(MeiAttribute("name","virga")); break;
		case NEUME_TYPE_COMPOUND: m_meiref->addAttribute(MeiAttribute("name","compound")); break;
		case NEUME_TYPE_CUSTOS: m_meiref->setName("custos"); break;
		default: break;
	}
}

void MusNeume::updateMeiZone() {
	MeiElement *zone = m_meiref->getZone();
	int width = 0, height = 0;
	int yoffs = 19; //all neumes are drawn 19px higher than their dec_y
	switch (this->m_type) { //here is where we figure out width, height and yoffs
		case NEUME_TYPE_ANCUS: break;
		case NEUME_TYPE_CEPHALICUS: height = m_pitches[0].dec_y - m_pitches[1].dec_y + 9; width = 20; break;
		case NEUME_TYPE_CLIVIS: height = m_pitches[0].dec_y - m_pitches[1].dec_y + 19; width = (m_pitches[1].xrel - m_pitches[0].xrel + PUNCT_WIDTH)*2; break;
		case NEUME_TYPE_EPIPHONUS: height = m_pitches[1].dec_y - m_pitches[0].dec_y + 19; width = PUNCT_WIDTH*2; break;
		case NEUME_TYPE_PODATUS: height = m_pitches[1].dec_y - m_pitches[0].dec_y + 19; width = PUNCT_WIDTH*2; break;
		case NEUME_TYPE_PORRECTUS: height = m_pitches[0].dec_y - m_pitches[m_pitches.size() - 2].dec_y + 19; width = (m_pitches.back().xrel - m_pitches[0].xrel + PUNCT_WIDTH)*2; break;
		case NEUME_TYPE_PUNCTUM_INCLINATUM: height = 25; width = PUNCT_WIDTH; break;
		case NEUME_TYPE_PUNCTUM_WHITE:
		case NEUME_TYPE_PUNCTUM: height = 18; width = 19; break;
		case NEUME_TYPE_SALICUS: height = m_pitches.back().dec_y - m_pitches[0].dec_y + 19; width = (m_pitches.back().xrel - m_pitches[0].xrel)*2; break;
		case NEUME_TYPE_SCANDICUS: height = m_pitches.back().dec_y - m_pitches[0].dec_y + 19; width = (m_pitches.back().xrel - m_pitches[0].xrel + PUNCT_WIDTH)*2; break;
		case NEUME_TYPE_SCANDICUS_FLEXUS:
		case NEUME_TYPE_PORRECTUS_FLEXUS:
		case NEUME_TYPE_TORCULUS_RESUPINUS: 
			int lowest = m_pitches[0].dec_y, highest = m_pitches[0].dec_y;
			for (vector<MusNeumeElement>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
				if (i->dec_y > highest) {
					highest = i->dec_y;
				}
				if (i->dec_y < lowest) {
					lowest = i->dec_y;
				}
			}
			height = highest - lowest + 19; width = (m_pitches.back().xrel - m_pitches[0].xrel + PUNCT_WIDTH)*2;
			break;
		case NEUME_TYPE_TORCULUS_LIQUESCENT: break; //no implementation yet
		case NEUME_TYPE_TORCULUS:
			lowest = (m_pitches[0].dec_y > m_pitches[1].dec_y) ? m_pitches[0].dec_y : m_pitches[2].dec_y;
			height = m_pitches[1].dec_y - lowest + 19; break;
		case NEUME_TYPE_VIRGA_LIQUESCENT: break; //no implementation, probably deprecated
		case NEUME_TYPE_VIRGA: height = 35; width = PUNCT_WIDTH*2; break;
		case NEUME_TYPE_COMPOUND: break;
		case NEUME_TYPE_CUSTOS: width = 11; height = 48; break;
		default: break;
	}
	int ymid = (atoi(this->m_meistaffzone->getAttribute("uly")->getValue().c_str()) + atoi(this->m_meistaffzone->getAttribute("lry")->getValue().c_str()))/2;
	int uly = ymid - double(this->dec_y + 100 /* <--this is the dec_y of a note on the middle space */ + yoffs)*(5.0/3.0); //5/3 is the ratio of an aruspix staff to an MEI staff height.
	char buf[16];
	
	MeiAttribute *lryattr = zone->getAttribute("lry");
	int lry = uly + height;
	snprintf(buf, 16, "%d", lry);
	if (lryattr != NULL) {
		lryattr->setValue(string(buf));
	} else {
		zone->addAttribute(MeiAttribute("lry",string(buf)));
	}
	
	MeiAttribute *ulxattr = zone->getAttribute("ulx");
	int ulx = (this->xrel + this->offset)*2; //the horizontal scale of aruspix is approximately 1/2 that of the images we're dealing with
	snprintf(buf, 16, "%d", ulx);
	if (ulxattr != NULL) {
		ulxattr->setValue(string(buf));
	} else {
		zone->addAttribute(MeiAttribute("ulx",string(buf)));
	}
	
	MeiAttribute *ulyattr = zone->getAttribute("uly");
	snprintf(buf, 16, "%d", uly);
	if (ulyattr != NULL) {
		ulyattr->setValue(string(buf));
	} else {
		zone->addAttribute(MeiAttribute("uly",string(buf)));
	}
	
	MeiAttribute *lrxattr = zone->getAttribute("lrx");
	int lrx = ulx + width;
	snprintf(buf, 16, "%d", lrx);
	if (lrxattr != NULL) {
		lrxattr->setValue(string(buf));
	} else {
		zone->addAttribute(MeiAttribute("lrx",string(buf)));
	}
}

void MusNeume::setMeiStaffZone(MeiElement *element) {
	this->m_meistaffzone = element;
}

void MusNeume::deleteMeiRef() {
	if (m_meiref->hasParent()) {
		m_meiref->getParent().removeChild(m_meiref);
	}
	if (m_meiref->getZone()->hasParent()) {
		m_meiref->getZone()->getParent().removeChild(m_meiref->getZone());
	}
	delete m_meiref->getZone();
	delete m_meiref;
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
		if (i->getMeiElement() != NULL) {
			i->updateMeiElement(PitchToStr(thispitch), octave);
		}
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
    
	//set y-positions for each note of the neume
    this->dec_y = staff->y_neume(this->pitch, staff->testcle( this->xrel ), oct);
	for (vector<MusNeumeElement>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
		int oct2 = oct;
		i->dec_y = staff->y_neume(filtrpitch(this->pitch + i->getPitchDifference(), &oct2), staff->testcle(this->xrel), oct2);
	}
	
    switch (m_type) {
        case (NEUME_TYPE_PUNCTUM): this->DrawPunctum(dc, staff); break;
        case (NEUME_TYPE_PUNCTUM_INCLINATUM): this->DrawPunctumInclinatum(dc, staff); break;
		case (NEUME_TYPE_PUNCTUM_WHITE): this->DrawPunctumWhite(dc, staff); break;
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
	
	if (this->newmeielement) {
		updateMeiZone();
	}
	
	DrawDots(dc, staff);
    
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
	m_pitches.at(0).xrel = xn;
    xn += CLIVIS_X_DIFF;
	m_pitches.at(1).xrel = xn;
    int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
    leg_line( dc, ynn2,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn2 + 19, nCEPHALICUS, staff, this->dimin); //notehead : 'K'
    int ynn3 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(2)).getPitchDifference());
	m_pitches.at(2).xrel = xn;
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
	m_pitches.at(0).xrel = xn;
    int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
	m_pitches.at(1).xrel = xn;
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
	m_pitches.at(0).xrel = xn;
    int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
	m_pitches.at(1).xrel = xn;
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
	m_pitches.at(0).xrel = xn;
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
	m_pitches.at(0).xrel = xn;
}

void MusNeume::DrawPunctumWhite( AxDC *dc, MusStaff *staff ) 
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
    m_r->festa_string( dc, xn, ynn + 19, nWHITEPUNCT, staff, this->dimin); //draw cavum
	m_pitches.at(0).xrel = xn;
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
	m_pitches.at(0).xrel = xn;
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
	m_pitches.at(0).xrel = xn;
	for (unsigned int i = 0; i < m_pitches.size() - 2; i++) {
		int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(i)).getPitchDifference());
		leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
		m_r->festa_string( dc, xn, ynn2 + 19, nPUNCTUM, staff, this->dimin);
		m_pitches.at(i).xrel = xn;
		xn += CLIVIS_X_SAME;
	}
    int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(m_pitches.size() - 2)).getPitchDifference());
    leg_line( dc, ynn2,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn2 + 19, nPES, staff, this->dimin);
	m_pitches.at(m_pitches.size() - 1).xrel = xn;
    int ynn3 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.back()).getPitchDifference());
    leg_line( dc, ynn3,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn3 + 19, nPUNCTUM, staff, this->dimin);
	m_pitches.back().xrel = xn;
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
	m_pitches.at(0).xrel = xn;
    int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
    leg_line( dc, ynn2,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn2 + 19, nPUNCTUM, staff, this->dimin);
	m_pitches.at(1).xrel = xn;
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
	m_pitches.at(0).xrel = xn;
    m_r->festa_string( dc, xn, ynn + 19, '3', staff, this->dimin);
    ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
    if (ynn2 == ynn) {
        dx = CLIVIS_X_SAME;
    } else dx = CLIVIS_X_DIFF;
    leg_line( dc, ynn2,bby,this->xrel + dx - 1,ledge, pTaille);
    m_r->festa_string( dc, xn + dx - 1, ynn2 + 19, nPUNCTUM, staff, this->dimin);
	m_pitches.at(1).xrel = xn + dx - 1;
	if (ynn2 != ynn) {
	NeumeLine(dc, staff, xn+dx, xn+dx, ynn, ynn2);
	}
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
	m_pitches.at(0).xrel = xn;
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
	m_pitches.at(2).xrel = xn;
    leg_line( dc, ynn,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn2 + 19, nPUNCTUM, staff, this->dimin);
    xn += PUNCT_WIDTH - 1;
	//no assignment of xrel to the second pitch as the bottom of a porrectus never has a dot.
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
	m_pitches.at(0).xrel = xn;
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
	m_pitches.at(2).xrel = xn;
    xn += PUNCT_WIDTH - 1;
    int ynn3 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(3)).getPitchDifference());
    leg_line( dc, ynn3,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn3 + 19, nPUNCTUM, staff, this->dimin);
	m_pitches.at(3).xrel = xn;
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
	
    for (vector<MusNeumeElement>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
		int ynn2 = ynn + (m_r->_espace[pTaille])*(i->getPitchDifference());
		leg_line( dc, ynn2,bby,xn,ledge, pTaille);
		m_r->festa_string( dc, xn, ynn2 + 19, nPUNCTUM, staff, this->dimin);
		i->xrel = xn;
		xn += PUNCT_WIDTH;
	}
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
	m_pitches.at(0).xrel = xn;
    int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
    leg_line( dc, ynn2,bby,this->xrel,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn2 + 19, nPUNCTUM, staff, this->dimin);
	m_pitches.at(1).xrel = xn;
    this->NeumeLine( dc, staff, xn + 9, xn + 9, ynn, ynn2);
    xn += CLIVIS_X_SAME;
    int ynn3 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(2)).getPitchDifference());
    leg_line( dc, ynn3,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn3 + 19, '3', staff, this->dimin);
    m_r->festa_string( dc, xn, ynn3 + 19, nPUNCTUM, staff, this->dimin);
	m_pitches.at(2).xrel = xn;
    xn += PUNCT_WIDTH - 2;
    int ynn4 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(3)).getPitchDifference());
    leg_line( dc, ynn4,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn4 + 19, nPUNCTUM, staff, this->dimin);
	m_pitches.at(3).xrel = xn;
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
	m_pitches.at(0).xrel = xn;
    int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
    this->NeumeLine( dc, staff, xn + 9, xn + 9, ynn, ynn2);
    xn += PUNCT_WIDTH - 2;
    leg_line( dc, ynn2,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn2 + 19, nPUNCTUM, staff, this->dimin);
	m_pitches.at(1).xrel = xn;
    int ynn3 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(2)).getPitchDifference());
    xn += PUNCT_WIDTH - 2;
    this->NeumeLine( dc, staff, xn + 1, xn + 1, ynn2, ynn3);
    leg_line( dc, ynn3,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn3 + 19, nPUNCTUM, staff, this->dimin);
	m_pitches.at(2).xrel = xn;
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
	m_pitches.at(0).xrel = xn;
    int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
    this->NeumeLine( dc, staff, xn + 9, xn + 9, ynn, ynn2);
    xn += PUNCT_WIDTH - 2;
    leg_line( dc, ynn2,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn2 + 19, nCEPHALICUS, staff, this->dimin); //notehead : 'K'
	m_pitches.at(1).xrel = xn;
    int ynn3 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(2)).getPitchDifference());
    m_r->festa_string( dc, xn, ynn3 + 19, nLIQUES_UP, staff, this->dimin); //liquescent: 'e'
	m_pitches.at(2).xrel = xn;
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
	m_pitches.at(0).xrel = xn;
    
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
	m_pitches.at(1).xrel = xn;
    int ynn3 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(3)).getPitchDifference());
    leg_line( dc, ynn3,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn3 + 19, nPUNCTUM, staff, this->dimin);
	m_pitches.at(3).xrel = xn;
	m_pitches.at(2).xrel = xn;
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

void MusNeume::DrawDots(AxDC *dc, MusStaff *staff) {
	for (vector<MusNeumeElement>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
		if (i->getOrnament() == DOT) {
			bool onspace = (i->dec_y/(m_r->_espace[staff->pTaille]))%2;
			int y = staff->yrel + i->dec_y + m_r->_interl[staff->pTaille];
			y += (!onspace) ? (m_r->_espace[staff->pTaille]) : 0;
			bool noteabove = false;
			for (vector<MusNeumeElement>::iterator it = m_pitches.begin(); it != m_pitches.end(); it++) {
				if (!onspace && (it->getPitchDifference() - i->getPitchDifference() == 1)) {
					noteabove = true;
				}
			}
			y -= (noteabove) ? (m_r->_interl[staff->pTaille]) : 0;
			m_r->festa_string( dc, m_pitches.back().xrel + PUNCT_WIDTH, y + 19, nDOT, staff, this->dimin);
		}
	}
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
