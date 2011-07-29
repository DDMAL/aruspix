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

using std::vector;

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
	parent = NULL;
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
		this->pitch = StrToPitch(meipitch);
		this->oct = octave;
		if (0 <= octave <= 7) {
			this->m_pitch_difference = (this->pitch + this->oct*7) - (firstoct*7 + firstpitch);
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

MusNeumeElement::MusNeumeElement(int _pitchDifference) : MusElement()
{
    TYPE = NEUME_ELMT;
	m_pitch_difference = _pitchDifference;
	ornament = NONE;
	m_meiref = NULL;
	parent = NULL;
    m_element_type = NEUME_ELEMENT_PUNCTUM;
}

// Duplicate an existing pitch
MusNeumeElement::MusNeumeElement( const MusNeumeElement &other) : MusElement(other) {
    m_meiref = other.m_meiref;
    m_element_type = other.m_element_type;
	ornament = other.ornament;
	parent = other.parent;
	SetPitch(this->pitch,this->oct);
}

MusNeume *MusNeumeElement::getParent() {
	return parent;
}

int MusNeumeElement::getPitchDifference() {
    return m_pitch_difference;
}

MeiElement* MusNeumeElement::getMeiRef() {
    return m_meiref;
}

NeumeElementType MusNeumeElement::getElementType()
{
    return m_element_type;
}

void MusNeumeElement::setElementType(NeumeElementType type) {
	m_element_type = type;
}

void MusNeumeElement::updateMeiElement(string pitch, int oct) { //still need to handle dots
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

void MusNeumeElement::Draw(AxDC *dc, MusStaff *staff) {
	wxASSERT_MSG( dc , "DC cannot be NULL");
    wxASSERT_MSG( m_r, "MusRC cannot be NULL ");
    if ( !Check() )
        return;
    
    if (!m_r->m_eraseElement && (this == m_r->m_currentElement))
        m_r->m_currentColour = AxRED;
	
    int pTaille = staff->pTaille;
    int xn = this->xrel + this->offset;
    int bby = staff->yrel - /*m_r->_interl[pTaille]*(staff->portNbLine - 1)*/ m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    int ledge = m_r->ledgerLine[pTaille][2];
    
	parent->Init(m_r);
	parent->leg_line(dc, ynn, bby, xn, ledge, pTaille); //draw ledger lines
	
	char symb;
	switch (m_element_type) {
		case NEUME_ELEMENT_PUNCTUM: symb = nPUNCTUM; break;
		case NEUME_ELEMENT_PUNCTUM_WHITE: symb = nWHITEPUNCT; break;
		case NEUME_ELEMENT_INCLINATUM: symb = nDIAMOND; break;
		case NEUME_ELEMENT_QUILISMA: symb = nQUILISMA; break;
		case NEUME_ELEMENT_PES: symb = nPES; break;
		case NEUME_ELEMENT_CEPHALICUS: symb = nCEPHALICUS; break;
		case NEUME_ELEMENT_CUSTOS: symb = nNEXT_PITCH; break;
		case NEUME_ELEMENT_LIQUESCENT_UP: symb = nLIQUES_UP; break;
		case NEUME_ELEMENT_LIQUESCENT_DOWN: symb = nLIQUES_DOWN; break;
		case NEUME_ELEMENT_EPIPHONUS: symb = nPODATUS_EP; break;
	}
    
	m_r->festa_string( dc, xn, ynn + 19, symb, staff, this->dimin); //draw element
	
	m_r->m_currentColour = AxBLACK;
    return;
}

void MusNeumeElement::SetPitch(int pitch, int oct) {
	if ( this->TYPE != NEUME_ELMT )
        return;
    
    if ( this->m_pitch_difference == (pitch + oct*7) - (parent->oct*7 + parent->pitch) ) {
		return;
	}
    
	this->m_pitch_difference = (pitch + oct*7) - (parent->oct*7 + parent->pitch);
	
    parent->SetPitch(parent->pitch, parent->oct);
	
    if (m_r) {
        m_r->DoRefresh();
    }
}

void MusNeumeElement::setParent(MusNeume *_parent) {
	parent = _parent;
}

//----------------------------------------------------------------------------
// MusNeume
//----------------------------------------------------------------------------

MusNeume::MusNeume() : MusElement() {
    TYPE = NEUME;
    m_type = NEUME_TYPE_PUNCTUM; //note: for all practical purposes, this can always be punctum.
    m_meiref = NULL; //this is necessary to avoid garbage when things aren't called from MEIs.
	m_meistaffzone = NULL;
	m_meifirstpitch = NULL;
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
	m_meifirstpitch = neume.m_meifirstpitch;
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
	inclinatum = false;
	quilisma = false;
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
                this->pitch = StrToPitch(p->getValue());
                int octave = atoi(o->getValue().c_str());
				if ( 0 <= octave <= 7 ) {
					this->oct = octave;
				} else {
					throw "octave out of range";
				}
				for (vector<MeiElement*>::iterator i = note.getChildren().begin(); i != note.getChildren().end(); i++) {
					if ( (*i)->getName() == "dot" ) {
						this->ornament = DOT;
					}
				}
                fail = false;
            }
        }
        if (fail) {
            throw "no pitches for neume";
        }
    } else if (element.getName() == "custos") {
        m_meiref = &element;
		m_meifirstpitch = &element;
        setType(NEUME_TYPE_CUSTOS);
        p = m_meiref->getAttribute("pname");
        o = m_meiref->getAttribute("oct");
        if (p == NULL || o == NULL) {
            throw "missing pitch or octave";
        }
        int octave = atoi(o->getValue().c_str());
		if ( 0 <= octave <= 7 ) {
			this->oct = octave;
		} else {
			throw "octave out of range";
		}
        this->pitch = StrToPitch(p->getValue());
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
        if (element.getAttribute("inclinatum") != NULL && element.getAttribute("inclinatum")->getValue() == "true") { //this is not true in general, but works for liber purposes.
            this->setType(NEUME_TYPE_PUNCTUM_INCLINATUM);
        }
        for (vector<MeiElement*>::iterator i = element.getChildren().begin(); i != element.getChildren().end(); i++) {
            if ((*i)->getName() == "note") {
				(*i)->setZone(this->m_meiref->getZone());
				if ( i == element.getChildren().begin() ) {
					setMeiFirstPitch(*i);
				} else {
					MusNeumeElement *note = new MusNeumeElement(**i, pitch, oct);
					AppendNote(note);
				}
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

void MusNeume::setMeiFirstPitch(MeiElement *element) {
	this->m_meifirstpitch = element;
}

void MusNeume::updateMeiRef() { //still need to handle dots
	if (!m_meiref) {
		m_meiref = new MeiElement ("");
	}
	if (m_type != NEUME_TYPE_CUSTOS) {
		if (m_meiref->getName() != "neume") {
			m_meiref->setName("neume");
		}
		if (!m_meiref->hasChild("nc")) {
			//ad the note container
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
			
			//add the first pitch
			MeiElement *firstpitch = new MeiElement("note");
			uuid_generate(uuidGenerated);
			uuid_unparse(uuidGenerated, uuidbuff);
			id = string(uuidbuff);
			std::transform(id.begin(), id.end(), id.begin(), ::tolower);
			id = "m-" + id;
			this->m_meifirstpitch = firstpitch;
			firstpitch->setId(id);
			nc->addChild(firstpitch);
			if (firstpitch->getAttributes().size() > 0) {
				if (firstpitch->hasAttribute("pname")) {
					firstpitch->getAttribute("pname")->setValue(PitchToStr(this->pitch%7));
				} else {
					firstpitch->addAttribute(MeiAttribute("pname",PitchToStr(this->pitch%7)));
				}
				MeiAttribute *octattr = firstpitch->getAttribute("oct");
				char buf[8];
				snprintf(buf, 2, "%d", this->oct);
				if (octattr == NULL) {
					firstpitch->addAttribute(MeiAttribute("oct",string(buf)));
				} else {
					octattr->setValue(string(buf));
				}
			} else {
				firstpitch->addAttribute(MeiAttribute("pname",PitchToStr(this->pitch%7)));
				char buf[8];
				snprintf(buf, 2, "%d", this->oct);
				firstpitch->addAttribute(MeiAttribute("oct",string(buf)));
			}
			
			//add the remaining pitches
			for (vector<MusNeumeElement*>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
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
				(*i)->setMeiElement(child);
				(*i)->updateMeiElement(PitchToStr((*i)->pitch%7), (*i)->oct);
			}
		} else {
			if (this->newmeielement) {
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
		//erase all attributes
		for (vector<MeiAttribute>::iterator i = m_meiref->getAttributes().begin(); i != m_meiref->getAttributes().end(); i++) {
			std::string name = i->getName();
			if (name != "facs") {
				i = m_meiref->getAttributes().erase(i);
				i--;
			}
		}
	} else { //m_type == NEUME_TYPE_CUSTOS
		MeiAttribute *pitchattr = m_meiref->getAttribute("pname");
		MeiAttribute *octattr = m_meiref->getAttribute("oct");
		char buf[8];
		snprintf(buf, 2, "%d", this->oct);
		if (pitchattr) {
			pitchattr->setValue(PitchToStr(this->pitch%7));
		} else {
			m_meiref->addAttribute(MeiAttribute("pname",PitchToStr(this->pitch%7)));
		}
		if (octattr) {
			octattr->setValue(string(buf));
		} else {
			m_meiref->addAttribute(MeiAttribute("oct",string(buf)));
		}
	}
	
	this->SetPitch(this->pitch, this->oct);
	
	if (m_meiref->getChildren().size() > 0) {
		m_meiref->getChildren()[0]->getAttributes().clear();
	}
	
	//add the correct attributes
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
	
	int lowest = this->dec_y, highest = this->dec_y;
	for (vector<MusNeumeElement*>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
		if ((*i)->dec_y > highest) {
			highest = (*i)->dec_y;
		}
		if ((*i)->dec_y < lowest) {
			lowest = (*i)->dec_y;
		}
	}
	
	switch (this->m_type) { //here is where we figure out width, height and yoffs
		case NEUME_TYPE_ANCUS: break;
		case NEUME_TYPE_CEPHALICUS: height = this->dec_y - m_pitches[0]->dec_y + 9; width = 20; break;
		case NEUME_TYPE_CLIVIS: height = this->dec_y - m_pitches[0]->dec_y + 19; width = (m_pitches[0]->xrel - this->xrel + PUNCT_WIDTH)*2; break;
		case NEUME_TYPE_EPIPHONUS: height = m_pitches[0]->dec_y - this->dec_y + 19; width = PUNCT_WIDTH*2; break;
		case NEUME_TYPE_PODATUS: height = m_pitches[0]->dec_y - this->dec_y + 19; width = PUNCT_WIDTH*2; break;
		case NEUME_TYPE_PORRECTUS: height = this->dec_y - m_pitches[m_pitches.size() - 2]->dec_y + 19; width = (m_pitches.back()->xrel - this->xrel + PUNCT_WIDTH)*2; break;
		case NEUME_TYPE_PUNCTUM_INCLINATUM: height = 25; width = PUNCT_WIDTH; break;
		case NEUME_TYPE_PUNCTUM_WHITE:
		case NEUME_TYPE_PUNCTUM: height = 18; width = 19; break;
		case NEUME_TYPE_SALICUS: height = (m_pitches.back()->dec_y - this->dec_y + 19)*(5.0/3.0); width = (m_pitches.back()->xrel - this->xrel)*2; break;
		case NEUME_TYPE_SCANDICUS: height = (m_pitches.back()->dec_y - this->dec_y + 19)*(5.0/3.0); width = (m_pitches.back()->xrel - this->xrel + PUNCT_WIDTH)*2; break;
		case NEUME_TYPE_SCANDICUS_FLEXUS:
		case NEUME_TYPE_PORRECTUS_FLEXUS:
		case NEUME_TYPE_TORCULUS_RESUPINUS:
		case NEUME_TYPE_COMPOUND:
			height = highest - lowest + 19; width = (m_pitches.back()->xrel - this->xrel + PUNCT_WIDTH)*2;
			break;
		case NEUME_TYPE_TORCULUS_LIQUESCENT: break; //no implementation yet
		case NEUME_TYPE_TORCULUS:
			lowest = (this->dec_y > m_pitches[1]->dec_y) ? this->dec_y : m_pitches[1]->dec_y;
			height = m_pitches[0]->dec_y - lowest + 19; break;
		case NEUME_TYPE_VIRGA_LIQUESCENT: break; //no implementation, probably deprecated
		case NEUME_TYPE_VIRGA: height = 35; width = PUNCT_WIDTH*2; break;
		case NEUME_TYPE_CUSTOS: width = 11; height = 48; break;
		default: break;
	}
	int ymid = (atoi(this->m_meistaffzone->getAttribute("uly")->getValue().c_str()) + atoi(this->m_meistaffzone->getAttribute("lry")->getValue().c_str()))/2;
	int uly = ymid - double(highest + 100 /* <--this is the dec_y of a note on the middle space */ + yoffs)*(5.0/3.0); // 5/3 is the ratio of an aruspix staff to an MEI staff height.
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

vector<MusNeumeElement*>& MusNeume::getPitches() {
    return m_pitches;
}


//same as MusNote, except for the MEI stuff.
void MusNeume::SetPitch( int pitch, int oct )
{
    if ( this->TYPE != NEUME )
        return;
    
    this->oct = oct;
    this->pitch = pitch;
    
	if (m_meifirstpitch) { //this subroutine takes place a lot; maybe it should be refactored as SetPitch(MeiElement *element, string, int) or something of the sort
		if (m_meifirstpitch->getAttributes().size() > 0) {
			if (m_meifirstpitch->hasAttribute("pname")) {
				m_meifirstpitch->getAttribute("pname")->setValue(PitchToStr(this->pitch%7));
			} else {
				m_meifirstpitch->addAttribute(MeiAttribute("pname",PitchToStr(this->pitch%7)));
			}
			MeiAttribute *octattr = m_meifirstpitch->getAttribute("oct");
			char buf[8];
			snprintf(buf, 2, "%d", this->oct);
			if (octattr == NULL) {
				m_meifirstpitch->addAttribute(MeiAttribute("oct",string(buf)));
			} else {
				octattr->setValue(string(buf));
			}
		} else {
			m_meifirstpitch->addAttribute(MeiAttribute("pname",PitchToStr(this->pitch%7)));
			char buf[8];
			snprintf(buf, 2, "%d", this->oct);
			m_meifirstpitch->addAttribute(MeiAttribute("oct",string(buf)));
		}
	}
	
    for (vector<MusNeumeElement*>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
        int octave = this->oct;
		int insertpitch = filtrpitch(this->pitch + (*i)->getPitchDifference(), &octave);
		(*i)->pitch = insertpitch;
		(*i)->oct = octave;
		if ( (*i)->getMeiRef() ) {
			(*i)->updateMeiElement(PitchToStr((*i)->pitch%7), (*i)->oct);
		}
    }
    if (m_r) {
        m_r->DoRefresh();
    }
}

void MusNeume::AppendNote(MusNeumeElement* note) {
	note->setParent(this);
	m_pitches.push_back(note);
}

void MusNeume::CalcPitches(MusStaff *staff) //this is the method that determines the logical structure of a neume! Should be split into a Validate() method and a CalcPitches method
{
	int oct = this->oct - 4; //for some reason, notes are automatically drawn 4 octaves too high?
	this->dec_y = staff->y_neume(this->pitch, staff->testcle( this->xrel ), oct);
	
	for (vector<MusNeumeElement*>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
		(*i)->Init(m_r);
	}
	
	//set x-positions for each note of the neume and make sure they have the right number of notes.
	switch (m_type) { //figure out some way to tidy this up -- do three-note (torculus, ancus, torculus liquescent, porrectus), then four-note (torculus resupinus, porrectus flexus, scandicus flexus) then multi-note (salicus, compound, scandicus)
		case (NEUME_TYPE_CUSTOS): //1-note neumes. If they have any MusNeumeElements, delete them.
		case (NEUME_TYPE_VIRGA):
		case (NEUME_TYPE_PUNCTUM):
		case (NEUME_TYPE_PUNCTUM_INCLINATUM):
		case (NEUME_TYPE_PUNCTUM_WHITE):
			for (vector<MusNeumeElement*>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
				if ( (*i)->getMeiRef() ) { //maybe refactor as RemoveNote(MeiElement *note) method?
					(*i)->deleteMeiRef();
				}
				delete (*i);
				m_pitches.erase(i);
			} break;
		case (NEUME_TYPE_VIRGA_LIQUESCENT): break; //no implementation
        case (NEUME_TYPE_PODATUS): //2-note neumes: If they have more than one MusNeumeElement, delete it, if they have less than one, add one, and make sure it's the right kind.
		case (NEUME_TYPE_CLIVIS):
		case (NEUME_TYPE_CEPHALICUS):
        case (NEUME_TYPE_EPIPHONUS):
			if (m_pitches.size() < 1) {
				for (unsigned i = 0; i < 1 - m_pitches.size(); i++) {
					AppendNote(new MusNeumeElement());
				}
			}
			for (vector<MusNeumeElement*>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
				int index = i - m_pitches.begin();
				if (index > 0) {
					if ( (*i)->getMeiRef() ) { //maybe refactor as RemoveNote(MeiElement *note) method?
						(*i)->deleteMeiRef();
					}
					delete (*i);
					m_pitches.erase(i);
				} else {
					//this subroutine happens pretty often too, maybe it could be refactored?
					(*i)->offset = this->offset;
					(*i)->xrel = 0;
					switch (m_type) {
						case NEUME_TYPE_CLIVIS: (*i)->xrel += ( (*i)->getPitchDifference() == 0 ) ? CLIVIS_X_SAME : CLIVIS_X_DIFF;
						case NEUME_TYPE_CEPHALICUS:
						case NEUME_TYPE_EPIPHONUS:
						case NEUME_TYPE_PODATUS: (*i)->xrel += this->xrel; break;
					}
					switch (m_type) {
						case NEUME_TYPE_CLIVIS:
						case NEUME_TYPE_PODATUS: (*i)->setElementType(NEUME_ELEMENT_PUNCTUM); break;
						case NEUME_TYPE_EPIPHONUS: (*i)->setElementType(NEUME_ELEMENT_LIQUESCENT_DOWN); break;
						case NEUME_TYPE_CEPHALICUS: (*i)->setElementType(NEUME_ELEMENT_LIQUESCENT_UP); break;
					}
					
				}
			} break;
		case (NEUME_TYPE_TORCULUS):
        case (NEUME_TYPE_TORCULUS_LIQUESCENT):
		case (NEUME_TYPE_ANCUS):
			if (m_pitches.size() < 2) {
				for (unsigned i = 0; i < 2 - m_pitches.size(); i++) {
					AppendNote(new MusNeumeElement());
				}
			}
			for (vector<MusNeumeElement*>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
				int index = i - m_pitches.begin();
				if (index > 1) {
					if ( (*i)->getMeiRef() ) { //maybe refactor as RemoveNote(MeiElement *note) method?
						(*i)->deleteMeiRef();
					}
					delete (*i);
					m_pitches.erase(i);
				} else {
					(*i)->xrel = (m_type == NEUME_TYPE_TORCULUS) ? this->xrel + (index + 1)*CLIVIS_X_DIFF : this->xrel + CLIVIS_X_DIFF;
					(*i)->offset = this->offset;
					NeumeElementType netype;
					if (m_type == NEUME_TYPE_TORCULUS) {
						netype = NEUME_ELEMENT_PUNCTUM;
					} else {
						switch (index) {
							case 0: netype = NEUME_ELEMENT_CEPHALICUS; break;
							case 1: netype = NEUME_ELEMENT_LIQUESCENT_UP; break;
							default: break;
						}
					}
					(*i)->setElementType(netype);
				}
			} break;
		case (NEUME_TYPE_PORRECTUS_FLEXUS):
		case (NEUME_TYPE_TORCULUS_RESUPINUS):
        case (NEUME_TYPE_PORRECTUS): {
			int max = (m_type == NEUME_TYPE_PORRECTUS) ? 2 : 3;
			if ((int)m_pitches.size() < max) {
				for (unsigned i = 0; i < max - m_pitches.size(); i++) {
					AppendNote(new MusNeumeElement());
				}
			}
			int diff = 3*PUNCT_WIDTH;
			int difference = (m_type == NEUME_TYPE_TORCULUS_RESUPINUS) ? m_pitches[1]->getPitchDifference() - m_pitches[0]->getPitchDifference() : m_pitches[0]->getPitchDifference();
			switch ( difference ) {
				case -1: diff += -5; break;
				case -2: diff += -2; break;
				case -3: diff += 3; break;
				case -4: diff += 3; break;
				default: break;
			}
			for (vector<MusNeumeElement*>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
				int index = i - m_pitches.begin();
				if (index > max - 1) {
					if ( (*i)->getMeiRef() ) { //maybe refactor as RemoveNote(MeiElement *note) method?
						(*i)->deleteMeiRef();
					}
					delete (*i);
					m_pitches.erase(i);
				} else {
					(*i)->offset = this->offset;
					switch (index) {
						case 0: (*i)->xrel = (m_type == NEUME_TYPE_TORCULUS_RESUPINUS) ? this->xrel + CLIVIS_X_DIFF : this->xrel + diff + PUNCT_WIDTH; (*i)->setElementType(NEUME_ELEMENT_PUNCTUM); break;
						case 1: 
							switch (m_type) {
								case NEUME_TYPE_PORRECTUS_FLEXUS: (*i)->xrel = this->xrel + diff + PUNCT_WIDTH; break;
								case NEUME_TYPE_TORCULUS_RESUPINUS: (*i)->xrel = this->xrel + diff + PUNCT_WIDTH + CLIVIS_X_DIFF; break;
								case NEUME_TYPE_PORRECTUS: (*i)->xrel = this->xrel + diff; break;
								default: break;
							} (*i)->setElementType(NEUME_ELEMENT_PUNCTUM); break;
						case 2: (*i)->xrel = (m_type == NEUME_TYPE_TORCULUS_RESUPINUS) ? this->xrel + diff + CLIVIS_X_DIFF : this->xrel + diff + PUNCT_WIDTH + CLIVIS_X_DIFF; (*i)->setElementType(NEUME_ELEMENT_PUNCTUM); break;
						default: break;
					}
				}
			} break; }
		case (NEUME_TYPE_COMPOUND):
        case (NEUME_TYPE_SCANDICUS_FLEXUS):
		case (NEUME_TYPE_SCANDICUS):
		case (NEUME_TYPE_SALICUS):
			for (vector<MusNeumeElement*>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
				int index = i - m_pitches.begin();
				(*i)->offset = this->offset;
				if (m_type == NEUME_TYPE_SALICUS && index == (int)m_pitches.size() - 1) {
					(*i)->xrel = this->xrel + CLIVIS_X_DIFF*index; //last note of a salicus is always directly above the previous note
					(*i)->setElementType(NEUME_ELEMENT_PUNCTUM);
				} else {
					(*i)->xrel = this->xrel + CLIVIS_X_DIFF*(index + 1);
					if (m_type == NEUME_TYPE_SALICUS && index == (int)m_pitches.size() - 2) {
						(*i)->setElementType(NEUME_ELEMENT_PES); //second last note of a salicus is always a pes
					} else {
						(*i)->setElementType(NEUME_ELEMENT_PUNCTUM);
					}
				}
			} break;
	}
}

void MusNeume::SetColour()
{
	if (!m_r->m_eraseElement && (this == m_r->m_currentElement))
        m_r->m_currentColour = AxRED;
    else if (!m_r->m_eraseElement && (this->m_cmp_flag == CMP_MATCH))
        m_r->m_currentColour = AxLIGHT_GREY;
    else if (!m_r->m_eraseElement && (this->m_cmp_flag == CMP_DEL))
        m_r->m_currentColour = AxGREEN;
    else if (!m_r->m_eraseElement && (this->m_cmp_flag == CMP_SUBST))
        m_r->m_currentColour = AxBLUE;
}

void MusNeume::Draw( AxDC *dc, MusStaff *staff)
{
    wxASSERT_MSG( dc , "DC cannot be NULL");
    wxASSERT_MSG( m_r, "MusRC cannot be NULL ");
    if ( !Check() )
        return;
    
    SetColour();
	
	CalcPitches(staff);
	
	for (vector<MusNeumeElement*>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
		(*i)->dec_y = staff->y_neume((*i)->pitch, staff->testcle(this->xrel), (*i)->oct - 4);
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
    SetColour();
	dc->SetPen( m_r->m_currentColour, m_r->ToRendererX( m_p->EpLignesPortee ), wxSOLID );
    dc->SetBrush(m_r->m_currentColour , wxTRANSPARENT );
    dc->DrawLine( m_r->ToRendererX(x1) , m_r->ToRendererY (y1) , m_r->ToRendererX(x2) , m_r->ToRendererY (y2) );
    dc->ResetPen();
    dc->ResetBrush();
}

void MusNeume::DrawAncus( AxDC *dc, MusStaff *staff )
{
    int pTaille = staff->pTaille;
    int xn = this->xrel + this->offset;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    int ledge = m_r->ledgerLine[pTaille][2];
    
	leg_line(dc, ynn, bby, xn, ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
	m_r->festa_string( dc, xn, ynn + 19, '3', staff, this->dimin);
	
    for (vector<MusNeumeElement*>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
		SetColour();
		(*i)->Init(m_r);
		(*i)->Draw(dc,staff);
	}
}

void MusNeume::DrawCustos( AxDC *dc, MusStaff *staff )
{
    int pTaille = staff->pTaille;
    int xn = this->xrel + this->offset;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    int ledge = m_r->ledgerLine[pTaille][2];
    
	leg_line(dc, ynn, bby, xn, ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nNEXT_PITCH, staff, this->dimin);
}

void MusNeume::DrawEpiphonus( AxDC *dc, MusStaff *staff )
{
	int pTaille = staff->pTaille;
    int xn = this->xrel + this->offset;
	int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
	int ledge = m_r->ledgerLine[pTaille][2];
    
	leg_line(dc, ynn, bby, xn, ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nPODATUS_EP, staff, this->dimin);
	
    for (vector<MusNeumeElement*>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
		SetColour();
		(*i)->Init(m_r);
		(*i)->Draw(dc,staff);
	}
    
	int ynn2 = m_pitches[0]->dec_y + staff->yrel;
	xn += 9;
	NeumeLine(dc, staff, xn, xn, ynn, ynn2);
}

void MusNeume::DrawCephalicus( AxDC *dc, MusStaff *staff )
{
    int pTaille = staff->pTaille;
    int xn = this->xrel + this->offset;
	int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
	int ledge = m_r->ledgerLine[pTaille][2];
    
	leg_line(dc, ynn, bby, xn, ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nCEPHALICUS, staff, this->dimin);
	m_r->festa_string( dc, xn, ynn + 19, '3', staff, this->dimin); //stem
	
	for (vector<MusNeumeElement*>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
		SetColour();
		(*i)->Init(m_r);
		(*i)->Draw(dc,staff);
	}
	
	int ynn2 = m_pitches[0]->dec_y + staff->yrel;
	xn += 9;
	NeumeLine(dc, staff, xn, xn, ynn, ynn2);
}

void MusNeume::DrawPunctum( AxDC *dc, MusStaff *staff ) 
{
    int pTaille = staff->pTaille;
    int xn = this->xrel + this->offset;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    int ledge = m_r->ledgerLine[pTaille][2];
    
	leg_line(dc, ynn, bby, xn, ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
}

void MusNeume::DrawPunctumInclinatum( AxDC *dc, MusStaff *staff ) 
{
	int pTaille = staff->pTaille;
    int xn = this->xrel + this->offset;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    int ledge = m_r->ledgerLine[pTaille][2];
    
	leg_line(dc, ynn, bby, xn, ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nDIAMOND, staff, this->dimin);
}

void MusNeume::DrawPunctumWhite( AxDC *dc, MusStaff *staff ) 
{
	int pTaille = staff->pTaille;
    int xn = this->xrel + this->offset;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    int ledge = m_r->ledgerLine[pTaille][2];
    
	leg_line(dc, ynn, bby, xn, ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nWHITEPUNCT, staff, this->dimin);
}

void MusNeume::DrawVirga( AxDC *dc, MusStaff *staff )
{
    int pTaille = staff->pTaille;
    int xn = this->xrel + this->offset;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    int ledge = m_r->ledgerLine[pTaille][2];
    
	leg_line(dc, ynn, bby, xn, ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
	
	xn += PUNCT_WIDTH;
	
	m_r->festa_string( dc, xn, ynn + 19, '#', staff, this->dimin); //stem
}

void MusNeume::DrawSalicus( AxDC *dc, MusStaff *staff )
{
    int pTaille = staff->pTaille;
    int xn = this->xrel + this->offset;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    int ledge = m_r->ledgerLine[pTaille][2];
    
	leg_line(dc, ynn, bby, xn, ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
    
	for (vector<MusNeumeElement*>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
		SetColour();
		(*i)->Init(m_r);
		(*i)->Draw(dc,staff);
	}
    
	int xn1 = m_pitches.back()->xrel + m_pitches.back()->offset + 9;
    int ynn1 = m_pitches[m_pitches.size() - 2]->dec_y + staff->yrel;
	int ynn2 = m_pitches.back()->dec_y + staff->yrel;
    NeumeLine( dc, staff, xn1, xn1, ynn1, ynn2);
}

void MusNeume::DrawPodatus( AxDC *dc, MusStaff *staff ) 
{
	//first is pes, second is punctum
	
	int pTaille = staff->pTaille;
    int xn = this->xrel + this->offset;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    int ledge = m_r->ledgerLine[pTaille][2];
    
	leg_line(dc, ynn, bby, xn, ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nPES, staff, this->dimin);
	
	for (vector<MusNeumeElement*>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
		SetColour();
		(*i)->Init(m_r);
		(*i)->Draw(dc,staff);
	}
	
    xn += 9;
	int ynn2 = m_pitches[0]->dec_y + staff->yrel;
	
    NeumeLine( dc, staff, xn, xn, ynn, ynn2);
}

void MusNeume::DrawClivis( AxDC *dc, MusStaff *staff ) 
{
	int pTaille = staff->pTaille;
	int xn1 = this->xrel + this->offset;
	int xn2 = m_pitches[0]->xrel + m_pitches[0]->offset + 1;
    int ynn1 = this->dec_y + staff->yrel;
	int ynn2 = m_pitches[0]->dec_y + staff->yrel;
	int bby = staff->yrel - m_r->_portee[pTaille];
	int ledge = m_r->ledgerLine[pTaille][2];
	
	leg_line(dc, ynn1, bby, xn1, ledge, pTaille);
	m_r->festa_string( dc, xn1, ynn1 + 19, nPUNCTUM, staff, this->dimin);
	m_r->festa_string( dc, xn1, ynn1 + 19, '3', staff, this->dimin);
    
	for (vector<MusNeumeElement*>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
		SetColour();
		(*i)->Init(m_r);
		(*i)->Draw(dc,staff);
	}
    
    NeumeLine( dc, staff, xn2, xn2, ynn1, ynn2);
}

void MusNeume::DrawPorrectus( AxDC *dc, MusStaff *staff ) 
{
	int pTaille = staff->pTaille;
    int xn = this->xrel + this->offset;
	int xn2 = m_pitches[0]->xrel + m_pitches[0]->offset;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    int ledge = m_r->ledgerLine[pTaille][2];
	
    leg_line( dc, ynn,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, '3', staff, this->dimin);
    
	wxString slope;
	switch ( m_pitches[0]->getPitchDifference() ) {
		case -1: slope = nPORRECT_1; break;
		case -2: slope = nPORRECT_2; break;
		case -3: slope = nPORRECT_3; break;
		case -4: slope = nPORRECT_4; break;
		default: break;
	}
    
    m_r->festa_string( dc, xn, ynn + 19, slope, staff, this->dimin);
    
	int ynn2 = m_pitches[0]->dec_y + staff->yrel;
	int ynn3 = m_pitches[1]->dec_y + staff->yrel;
	
    NeumeLine( dc, staff, xn2, xn2, ynn2, ynn3);
	
	m_pitches[1]->Draw(dc,staff);
}

void MusNeume::DrawPorrectusFlexus( AxDC *dc, MusStaff *staff)
{
	//in fact, maybe call DrawPorrectus and then just draw the extra note and line at the end? Will test it.
	int pTaille = staff->pTaille;
    int xn = this->xrel + this->offset;
	int xn2 = m_pitches[0]->xrel + m_pitches[0]->offset;
	int xn3 = m_pitches[2]->xrel + m_pitches[2]->offset;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    int ledge = m_r->ledgerLine[pTaille][2];
	
    leg_line( dc, ynn,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, '3', staff, this->dimin);
    
	wxString slope;
	switch ( m_pitches[0]->getPitchDifference() ) {
		case -1: slope = nPORRECT_1; break;
		case -2: slope = nPORRECT_2; break;
		case -3: slope = nPORRECT_3; break;
		case -4: slope = nPORRECT_4; break;
		default: break;
	}
    
    m_r->festa_string( dc, xn, ynn + 19, slope, staff, this->dimin);
    
	int ynn2 = m_pitches[0]->dec_y + staff->yrel;
	int ynn3 = m_pitches[1]->dec_y + staff->yrel;
	
    NeumeLine( dc, staff, xn2, xn2, ynn2, ynn3 );
	
	m_pitches[1]->Draw(dc,staff);
	
	SetColour();	
	m_pitches[2]->Draw(dc,staff);
	
	int ynn4 = m_pitches[2]->dec_y + staff->yrel;	
	NeumeLine( dc, staff, xn3, xn3, ynn3, ynn4 );
}

void MusNeume::DrawScandicus( AxDC *dc, MusStaff *staff ) 
{
	int pTaille = staff->pTaille;
    int xn = this->xrel + this->offset;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    int ledge = m_r->ledgerLine[pTaille][2];
    
	leg_line(dc, ynn, bby, xn, ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
	
	for (vector<MusNeumeElement*>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
		SetColour();
		(*i)->Init(m_r);
		(*i)->Draw(dc,staff);
	}
}

void MusNeume::DrawScandicusFlexus( AxDC *dc, MusStaff *staff )
{
	int pTaille = staff->pTaille;
    int xn = this->xrel + this->offset;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    int ledge = m_r->ledgerLine[pTaille][2];
    
	leg_line(dc, ynn, bby, xn, ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
	
	for (vector<MusNeumeElement*>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
		SetColour();
		(*i)->Init(m_r);
		(*i)->Draw(dc,staff);
	}
}

void MusNeume::DrawTorculus( AxDC *dc, MusStaff *staff ) 
{
    int pTaille = staff->pTaille;
    int xn = this->xrel + this->offset;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    int ledge = m_r->ledgerLine[pTaille][2];
    
	leg_line(dc, ynn, bby, xn, ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
	
	m_pitches[0]->Init(m_r);
	m_pitches[0]->Draw(dc,staff);
    
	int ynn2 = m_pitches[0]->dec_y + staff->yrel;
	xn += CLIVIS_X_DIFF;	
	NeumeLine( dc, staff, xn, xn, ynn, ynn2);
	
	m_pitches[1]->Init(m_r);
	m_pitches[1]->Draw(dc,staff);
	
	int ynn3 = m_pitches[1]->dec_y + staff->yrel;
	xn += CLIVIS_X_DIFF;
    NeumeLine( dc, staff, xn, xn, ynn2, ynn3);
}

void MusNeume::DrawTorculusLiquescent( AxDC *dc, MusStaff *staff )
{
    /*// magic happens here
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
	m_pitches.at(2).xrel = xn;*/
}

void MusNeume::DrawTorculusResupinus( AxDC *dc, MusStaff *staff )
{
    int pTaille = staff->pTaille;
    int xn = this->xrel + this->offset;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    int ledge = m_r->ledgerLine[pTaille][2];
	
    leg_line( dc, ynn,bby,xn,ledge, pTaille);
    m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
	
	wxString slope;
	switch ( m_pitches[1]->getPitchDifference() - m_pitches[0]->getPitchDifference() ) {
		case -1: slope = nPORRECT_1; break;
		case -2: slope = nPORRECT_2; break;
		case -3: slope = nPORRECT_3; break;
		case -4: slope = nPORRECT_4; break;
		default: break;
	}
	int xn2 = m_pitches[0]->xrel + m_pitches[0]->offset;
    int ynn2 = m_pitches[0]->dec_y + staff->yrel;
	
	m_r->festa_string( dc, xn2, ynn2 + 19, '3', staff, this->dimin);
	m_r->festa_string( dc, xn2, ynn2 + 19, slope, staff, this->dimin);
    
	int xn3 = m_pitches[1]->xrel + m_pitches[1]->offset;
	int ynn3 = m_pitches[1]->dec_y + staff->yrel;
	int ynn4 = m_pitches[2]->dec_y + staff->yrel;
	
    NeumeLine( dc, staff, xn3, xn3, ynn3, ynn4 );
	m_pitches[2]->Draw(dc,staff);
}

void MusNeume::DrawCompound( AxDC *dc, MusStaff *staff ) 
{
	int pTaille = staff->pTaille;
    int xn = this->xrel + this->offset;
    int bby = staff->yrel - m_r->_portee[pTaille];
    int ynn = this->dec_y + staff->yrel;
    int ledge = m_r->ledgerLine[pTaille][2];
    
	leg_line(dc, ynn, bby, xn, ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
	
	for (vector<MusNeumeElement*>::iterator i = m_pitches.begin(); i != m_pitches.end(); i++) {
		SetColour();
		(*i)->Init(m_r);
		(*i)->Draw(dc,staff);
	}
}

void MusNeume::DrawDots(AxDC *dc, MusStaff *staff) {
	int xn = (m_pitches.size() > 0) ? m_pitches.back()->xrel + PUNCT_WIDTH : this->xrel + PUNCT_WIDTH;
	vector< vector<int> > pitchoct;
	for (int i = -1; i < m_pitches.size(); i++) {
		if(i == -1) {
			int onspace = (this->dec_y/(m_r->_espace[staff->pTaille]))%2;
			int dot = (this->ornament == DOT) ? 1 : 0;
			vector<int> array;
			array.push_back(this->pitch);
			array.push_back(this->oct);
			array.push_back(dot);
			array.push_back(onspace);
			pitchoct.push_back(array);
		} else {
			int onspace = (m_pitches[i]->dec_y/(m_r->_espace[staff->pTaille]))%2;
			int dot = (m_pitches[i]->getOrnament() == DOT) ? 1 : 0;
			vector<int> array;
			array.push_back(m_pitches[i]->pitch);
			array.push_back(m_pitches[i]->oct);
			array.push_back(dot);
			array.push_back(onspace);
			pitchoct.push_back(array);
		}
	}
	for (int j = 0; j < pitchoct.size(); j++) {
		if (pitchoct[j][2] == 1) {
			if (pitchoct[j][3] == 1) {
				int ynn = staff->y_neume(pitchoct[j][0], staff->testcle(this->xrel), pitchoct[j][1]);
				m_r->festa_string( dc, xn, ynn + 19, nDOT, staff, this->dimin);
			} else {
				bool noteabove = false;
				int pitchElement = pitchoct[j][0] + 1;
				int octElement = pitchoct[j][1];
				while (pitchElement > 7) {
					octElement++;
					pitchElement -= 7;
				}
				for (int k = 0; k < pitchoct.size(); k++) {
					if (pitchoct[k][0] == pitchElement && pitchoct[k][1] == octElement) {
						noteabove = true;
					}
				}
				if (!noteabove) {
					int ynn = staff->y_neume(pitchElement, staff->testcle(this->xrel), octElement);
					m_r->festa_string( dc, xn, ynn + 19, nDOT, staff, this->dimin);
				} else {
					bool notebelow = false;
					pitchElement = pitchoct[j][0] - 1;
					octElement = pitchoct[j][1];
					while (pitchElement < 1) {
						octElement--;
						pitchElement += 7;
					}
					for (int l = 0; l < pitchoct.size(); l++) {
						if (pitchoct[l][0] == pitchElement && pitchoct[l][1] == octElement) {
							notebelow = true;
						}
					}
					if (!notebelow) {
						int ynn = staff->y_neume(pitchElement, staff->testcle(this->xrel), octElement);
						m_r->festa_string( dc, xn, ynn + 19, nDOT, staff, this->dimin);
					}
				}
			}
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