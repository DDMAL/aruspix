/*
 *  musneumesymbol.cpp
 *  aruspix
 *
 *  Created by Alastair Porter on 11-03-29.
 *  Copyright 2011 com.aruspix.www. All rights reserved.
 *
 */

#include "musneumesymbol.h"
#include "musdef.h"
#include "musclef.h" // included for the ids - we should either use musclef for neume clef or move the neume clef ids in musneumesymbol

#include <algorithm>

MusNeumeSymbol::MusNeumeSymbol() :
    MusLayerElement(), MusPositionInterface()
{
	m_meiref = 0;
}

MusNeumeSymbol::MusNeumeSymbol(const MusNeumeSymbol &symbol) :
    MusLayerElement(symbol), MusPositionInterface(symbol)
{
    symbolType = symbol.symbolType;
	m_meiref = symbol.m_meiref;
	value = symbol.value;
    m_pname = symbol.m_pname;
}

MusNeumeSymbol::MusNeumeSymbol(MeiElement &meielement) :
    MusLayerElement(), MusPositionInterface()
{
	m_meiref = &meielement;
	if (m_meiref->getName() == "clef") {
        MeiAttribute *shape = m_meiref->getAttribute("shape");
        MeiAttribute *line = m_meiref->getAttribute("line");
		if (shape != NULL && line != NULL) {
            string shstr = shape->getValue();
            std::transform(shstr.begin(), shstr.end(), shstr.begin(), ::toupper);
			if (shstr == "C") {
                symbolType = NEUME_SYMB_CLEF_C;
				if (line->getValue() == "1") {
					value = nC1;
				} else if (line->getValue() == "2") {
					value = nC2;
				} else if (line->getValue() == "3") {
					value = nC3;
				} else if (line->getValue() == "4") {
					value = nC4;
				} else {
                    throw "unknown line for a C clef";
                }
			} else if (shstr == "F") {
                symbolType = NEUME_SYMB_CLEF_F;
				if (line->getValue() == "1") {
					value = nF1;
                } else if (line->getValue() == "2") {
					value = nF2;
                } else if (line->getValue() == "3") {
					value = nF3;
                } else if (line->getValue() == "4") {
					value = nF4;
                } else {
                    throw "unknown line for an F clef";
                }
			} else {
                throw "unknown clef";
            }
		} else {
			throw "missing shape or line attribute on clef";
		}
	} else if (m_meiref->getName() == "division") {
        MeiAttribute *form = m_meiref->getAttribute("form");
        if (form != NULL) {
            if (form->getValue() == "final") {
                symbolType = NEUME_SYMB_DIVISION_FINAL;
            } else if (form->getValue() == "major") {
                symbolType = NEUME_SYMB_DIVISION_MAJOR;
            } else if (form->getValue() == "minor") {
                symbolType = NEUME_SYMB_DIVISION_MINOR;
            } else if (form->getValue() == "small") {
                symbolType = NEUME_SYMB_DIVISION_SMALL;
            } else if (form->getValue() == "comma") {
                symbolType = NEUME_SYMB_COMMA;
            }
        } else {
            throw "missing form for division";
        }
    } else if (m_meiref->getName() == "accid") {
        MeiAttribute *accid = m_meiref->getAttribute("accid");
        if (accid != NULL) {
            if (accid->getValue() == "f") {
                symbolType = NEUME_SYMB_FLAT;
            } else if (accid->getValue() == "n") {
                symbolType = NEUME_SYMB_NATURAL;
            } else {
                throw "unknown accidental";
            }
        } else {
            throw "missing accid attribute on accidental";
        }
        MeiAttribute *pname = m_meiref->getAttribute("pname");
        MeiAttribute *o = m_meiref->getAttribute("oct");
        if (pname && o) {
            m_oct = atoi((o->getValue()).c_str());
            m_pname = this->StrToPitch(pname->getValue());
        } else {
            throw "missing pitch or octave on accidental";
        }
    } else {
        throw "unknown type of symbol";
    }
}

MusNeumeSymbolType MusNeumeSymbol::getType()
{
	return symbolType;
}

void MusNeumeSymbol::calcoffs (int *offst, int value)
{	
	*offst = 0;
	switch(value)
	{	
		case nC1 : *offst = 2; break;
		case nC2 : *offst = 4; break;
		case nC3 : *offst = 6; break;
		case nC4 : *offst = 8; break;
		case nF1 : *offst = 6; break;
		case nF2 : *offst = 8; break;
		case nF3 : *offst = 10; break;
		case nF4 : *offst = 12; break;
		default: break;
	}
	return;
}

void MusNeumeSymbol::SetValue(int value, MusLaidOutStaff *staff, int vflag)
{		
	switch (value)
	{
			
		case ('1'):
			this->symbolType = NEUME_SYMB_CLEF_C;
			this->value = nC2;
			break;
		case ('2'):
			this->symbolType = NEUME_SYMB_CLEF_C;
			this->value = nC3;
			break;
		case ('3'):
			this->symbolType = NEUME_SYMB_CLEF_C;
			this->value = nC4;
			break;
		case ('4'):
			this->symbolType = NEUME_SYMB_CLEF_F;
			this->value = nF3;
			break;
		case ('5'):
			this->symbolType = NEUME_SYMB_CLEF_F;
			this->value = nF4;
			break;
		case ('6'): this->symbolType = NEUME_SYMB_COMMA; break;
		case ('F'): this->symbolType = NEUME_SYMB_FLAT; break;
		case ('N'): this->symbolType = NEUME_SYMB_NATURAL; break;
		case ('7'): this->symbolType = NEUME_SYMB_DIVISION_FINAL; break;
		case ('8'): this->symbolType = NEUME_SYMB_DIVISION_MAJOR; break;
		case ('9'): this->symbolType = NEUME_SYMB_DIVISION_MINOR; break;
		case ('0'): this->symbolType = NEUME_SYMB_DIVISION_SMALL; break;
	}
	
	if (m_meiref)
	{
		switch (this->symbolType)
        {
            case (NEUME_SYMB_CLEF_F):
            case (NEUME_SYMB_CLEF_C):
                switch (this->value)
                {
                    case (nC1): updateMeiRefClef("C", "1"); break;
                    case (nC2): updateMeiRefClef("C", "2"); break;
                    case (nC3): updateMeiRefClef("C", "3"); break;
                    case (nC4): updateMeiRefClef("C", "4"); break;
                    case (nF1): updateMeiRefClef("F", "1"); break;
                    case (nF2): updateMeiRefClef("F", "2"); break;
                    case (nF3): updateMeiRefClef("F", "3"); break;
                    case (nF4): updateMeiRefClef("F", "4"); break;
                }
                break;
            case (NEUME_SYMB_DIVISION_FINAL):
                updateMeiRefDiv("final");
                break;
            case (NEUME_SYMB_DIVISION_MAJOR):
                updateMeiRefDiv("major");
                break;
            case (NEUME_SYMB_DIVISION_MINOR):
                updateMeiRefDiv("minor");
                break;
            case (NEUME_SYMB_DIVISION_SMALL):
                updateMeiRefDiv("small");
                break;
            case (NEUME_SYMB_COMMA):
                updateMeiRefDiv("comma");
                break;
            case (NEUME_SYMB_FLAT):
                updateMeiRefAccid("f", this->m_pname, this->m_oct);
                break;
            case (NEUME_SYMB_NATURAL):
                updateMeiRefAccid("n", this->m_pname, this->m_oct);
                break;
            default: break;
        }
	}
}

int MusNeumeSymbol::getValue()
{
	return value;
}

void MusNeumeSymbol::updateMeiRefAccid(string accid, int pitch, int octave)
{
    if (m_meiref->getName() == "accid") {
        MeiAttribute *accidattr = m_meiref->getAttribute("accid");
        if (accidattr == NULL) {
            //m_meiref->addAttribute(MeiAttribute("accid", accid)); // BROKEN!!! ax2  - Commented by LP - does not work with the new version of libmei
        } else {
            accidattr->setValue(accid);
        }
        MeiAttribute *pnameattr = m_meiref->getAttribute("pname");
        if (pnameattr == NULL) {
            //m_meiref->addAttribute(MeiAttribute("pname", this->PitchToStr(m_pname))); // BROKEN!!! ax2  - Commented by LP - does not work with the new version of libmei
        } else {
            pnameattr->setValue(this->PitchToStr(m_pname));
        }
        MeiAttribute *octattr = m_meiref->getAttribute("oct");
        char buffer[1];
        sprintf(buffer, "%d", octave);
        string octstr;
        octstr.assign(buffer, 1);
        if (octattr == NULL) {
            //m_meiref->addAttribute(MeiAttribute("oct", octstr)); // BROKEN!!! ax2  - Commented by LP - does not work with the new version of libmei
        } else {
            octattr->setValue(octstr);
        }
    }
}

void MusNeumeSymbol::updateMeiRefDiv(string form)
{
    if (m_meiref->getName() == "division") {
        MeiAttribute *formattr = m_meiref->getAttribute("form");
        if (formattr == NULL) {
            //m_meiref->addAttribute(MeiAttribute("form", form)); // BROKEN!!! ax2  - Commented by LP - does not work with the new version of libmei
        } else {
            formattr->setValue(form);
        }
    }
}
                                       
void MusNeumeSymbol::updateMeiRefClef(string shape, string line) {
    if (m_meiref->getName() == "clef") {
        MeiAttribute *shapeattr = m_meiref->getAttribute("shape");
        if (shapeattr == NULL) {
            //m_meiref->addAttribute(MeiAttribute("shape", shape)); // BROKEN!!! ax2  - Commented by LP - does not work with the new version of libmei
        } else {
            shapeattr->setValue(shape);
        }
        MeiAttribute *lineattr = m_meiref->getAttribute("line");
        if (lineattr == NULL) {
            //m_meiref->addAttribute(MeiAttribute("line", line)); // BROKEN!!! ax2  - Commented by LP - does not work with the new version of libmei
        } else {
            lineattr->setValue(line);
        }
    }
}

void MusNeumeSymbol::deleteMeiRef() {
	if (m_meiref->hasParent()) {
		//m_meiref->getParent().removeChild(m_meiref); // BROKEN!!! ax2  - Commented by LP - does not work with the new version of libmei
	}
	delete m_meiref;
}

void MusNeumeSymbol::SetPitch(int pitch, int oct) //this is incomplete, there is no other default symbol at the moment.
{	
	if ((this->getType() == NEUME_SYMB_FLAT) || (this->getType() == NEUME_SYMB_NATURAL))
	{
		if ((this->m_pname == pitch) && (this->m_oct == oct ))
			return;
		
		this->m_oct = oct;
		this->m_pname = pitch;
		
        /* - no more MusRC access in ax2
		if (m_r)
			m_r->DoRefresh();
        */
        
        if (m_meiref)
        {
            updateMeiRefAccid(m_meiref->getAttribute("accid")->getValue(), pitch, oct);
        }
	}
}

// XXX: We should be careful here - what is the impact of the m_meiref?
// This is currently only used for creating stuff, which our mei support
// doesn't handle, so should be fine for now...
void MusNeumeSymbol::ResetToNeumeSymbol()
{
	MusNeumeSymbol reset;
	reset.symbolType = NEUME_SYMB_FLAT;
	reset.value = nC2;
	*this = reset;
}

void MusNeumeSymbol::ResetToClef()
{
	MusNeumeSymbol reset;
	reset.symbolType = NEUME_SYMB_CLEF_C;
	reset.value = nC2;
	*this = reset;
}

MusNeumeSymbolType MusNeumeSymbol::GetSymbolType() {
    return symbolType;
}

MeiElement *MusNeumeSymbol::getMeiRef() {
    return m_meiref;
}

std::string MusNeumeSymbol::PitchToStr(int pitch)
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

int MusNeumeSymbol::StrToPitch(std::string pitch)
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


