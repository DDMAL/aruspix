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
#include "neumedef.h"
#include "musrc.h"
#include "musstaff.h"

#include <mei/meiattribute.h>

MusNeumeSymbol::MusNeumeSymbol() :
    MusElement()
{
	TYPE = NEUME_SYMB;
	m_meiref = 0;
}

MusNeumeSymbol::MusNeumeSymbol(const MusNeumeSymbol &symbol) :
    MusElement(symbol)
{
    symbolType = symbol.symbolType;
	m_meiref = symbol.m_meiref;
	value = symbol.value;
}

MusNeumeSymbol::MusNeumeSymbol(MeiElement &meielement) :
    MusElement()
{
	m_meiref = &meielement;
	TYPE = NEUME_SYMB;
	if (m_meiref->getName() == "clef") {
        MeiAttribute *shape = m_meiref->getAttribute("shape");
        MeiAttribute *line = m_meiref->getAttribute("line");
		if (shape != NULL && line != NULL) {
			if (shape->getValue() == "C") {
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
			} else if (shape->getValue() == "F") {
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
		case nC1 : *offst = 0; break;
		case nC2 : *offst = 2; break;
		case nC3 : *offst = 4; break;
		case nC4 : *offst = 6; break;
		case nF1 : *offst = 4; break;
		case nF2 : *offst = 6; break;
		case nF3 : *offst = 8; break;
		case nF4 : *offst = 10; break;
		default: break;
	}
	return;
}

void MusNeumeSymbol::Draw ( AxDC *dc, MusStaff *staff)
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	wxASSERT_MSG( m_r, "MusRC cannot be NULL ");
	if ( !Check() )
		return;	
	
	if ( this->ElemInvisible )
		return;
	
	if (!m_r->m_eraseElement && (this == m_r->m_currentElement))
		m_r->m_currentColour = AxRED;
	else if (!m_r->m_eraseElement && (this->m_cmp_flag == CMP_MATCH))
		m_r->m_currentColour = AxLIGHT_GREY;
	else if (!m_r->m_eraseElement && (this->m_cmp_flag == CMP_DEL))
		m_r->m_currentColour = AxGREEN;
	else if (!m_r->m_eraseElement && (this->m_cmp_flag == CMP_SUBST))
		m_r->m_currentColour = AxBLUE;
	else if (!m_r->m_eraseElement && (this->m_cmp_flag == CMP_INS))
		m_r->m_currentColour = AxRED;
	
	
    dc->StartGraphic( "symbol", wxString::Format("s_%d_%d", staff->no, this->no) );
	
	int x = this->xrel + this->offset;
	
	int oct = this->oct - 4;
	
	if ((this->symbolType == NEUME_SYMB_FLAT) || (this->symbolType == NEUME_SYMB_NATURAL)) {
		this->dec_y = staff->y_neume(this->pitch, staff->testcle(x), oct) + m_r->_interl[staff->pTaille];
	}
	else if ((this->symbolType == NEUME_SYMB_COMMA) || (this->symbolType == NEUME_SYMB_DIVISION_FINAL) || (this->symbolType == NEUME_SYMB_DIVISION_MAJOR)
			 || (this->symbolType == NEUME_SYMB_DIVISION_MINOR) || (this->symbolType == NEUME_SYMB_DIVISION_SMALL))
	{
		this->dec_y = -m_r->_portee[staff->pTaille] - m_r->_interl[staff->pTaille]*2;
	}
	switch (this->symbolType)
	{
		case NEUME_SYMB_CLEF_C:
		case NEUME_SYMB_CLEF_F:
		calcoffs (&x,(int)this->value);
		this->dec_y = x;
		this->DrawClef( dc, m_r->kPos[staff->no].compte, staff); 
		break;
		case NEUME_SYMB_COMMA: this->DrawComma(dc, staff); break;
		case NEUME_SYMB_FLAT: this->DrawFlat(dc, staff); break;
		case NEUME_SYMB_NATURAL: this->DrawNatural(dc, staff); break;
		case NEUME_SYMB_DIVISION_FINAL: this->DrawDivFinal(dc, staff); break;
		case NEUME_SYMB_DIVISION_MAJOR: this->DrawDivMajor(dc, staff); break;
		case NEUME_SYMB_DIVISION_MINOR: this->DrawDivMinor(dc, staff); break;
		case NEUME_SYMB_DIVISION_SMALL: this->DrawDivSmall(dc, staff); break;
	}
	
	if ( !m_r->m_eraseElement )
		m_r->m_currentColour = AxBLACK;
	
    dc->EndGraphic();
	
	return;
}

void MusNeumeSymbol::DrawClef( AxDC *dc, int i, MusStaff *staff)
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() ) {
		return;
	}
	
	int x = this->xrel;
	int y = staff->yrel - m_r->_portee[staff->pTaille] - this->dec_y + m_r->_espace[staff->pTaille]; //with a fudge factor?
	dimin = this->dimin;
	wxString shape = nF_CLEF;
	
	switch (this->value)
	{
		case nC1: shape = nC_CLEF;
		case nF1: y -= m_r->_interl[staff->pTaille]*3; break;
		case nC2: shape = nC_CLEF;
		case nF2: y -= m_r->_interl[staff->pTaille]*2; break;
		case nC3: shape = nC_CLEF; y -= m_r->_interl[staff->pTaille] - 1; break; 
		case nF3: y -= m_r->_interl[staff->pTaille] - 6; break;
		case nC4: shape = nC_CLEF; y += 4; break;
		case nF4: y += 8; break;
		default: break;
	}
	
	m_r->festa_string(dc, x, y, shape, staff, dimin);
	
	char dum = 0;
	oct = -staff->getOctCl (this, &dum);
	
	// on met a jour struct poscle 
	staff->updat_pscle (i,this);
}

void MusNeumeSymbol::DrawComma(AxDC *dc, MusStaff *staff)
{
	int x = this->xrel;
	int y = staff->yrel + this->dec_y;
	m_r->festa_string(dc, x, y, nCOMMA, staff, this->dimin);
}

void MusNeumeSymbol::DrawFlat(AxDC *dc, MusStaff *staff)
{
	int x = this->xrel;
	int y = staff->yrel + this->dec_y;
	m_r->festa_string(dc, x, y, nB_FLAT, staff, this->dimin);
}

void MusNeumeSymbol::DrawNatural(AxDC *dc, MusStaff *staff)
{
	int x = this->xrel;
	int y = staff->yrel + this->dec_y - m_r->_espace[staff->pTaille]/2;
	m_r->festa_string(dc, x, y, nNATURAL, staff, this->dimin);
}

void MusNeumeSymbol::DrawDivMinor(AxDC *dc, MusStaff *staff)
{
	int x = this->xrel;
	int y = staff->yrel + this->dec_y + m_r->_espace[staff->pTaille] - 3;
	m_r->festa_string(dc, x, y, nDIV_MINOR, staff, this->dimin);
}

void MusNeumeSymbol::DrawDivMajor(AxDC *dc, MusStaff *staff)
{
	int x = this->xrel;
	int y = staff->yrel + this->dec_y + 6;
	m_r->festa_string(dc, x, y, nDIV_MAJOR, staff, this->dimin);
}

void MusNeumeSymbol::DrawDivFinal(AxDC *dc, MusStaff *staff)
{
	int x = this->xrel;
	int y = staff->yrel + this->dec_y + m_r->_espace[staff->pTaille] - 2;
	m_r->festa_string(dc, x, y, nDIV_FINAL, staff, this->dimin);
}

void MusNeumeSymbol::DrawDivSmall(AxDC *dc, MusStaff *staff)
{
	int x = this->xrel;
	int y = staff->yrel + this->dec_y;
	m_r->festa_string(dc, x, y, nDIV_SMALL, staff, this->dimin);
}

void MusNeumeSymbol::SetValue(int value, MusStaff *staff, int vflag)
{	
	if ( this->TYPE != NEUME_SYMB )
		return;
	
	switch (value)
	{
			
		case ('1'):
			this->symbolType = NEUME_SYMB_CLEF_C;
			if (m_r)
			{
				m_r->OnBeginEditionClef();
			}
			this->value = nC2;
			if (m_r)
			{
				m_r->OnEndEditionClef();
				m_r->DoRefresh();
			}
			break;
		case ('2'):
			this->symbolType = NEUME_SYMB_CLEF_C;
			if (m_r)
			{
				m_r->OnBeginEditionClef();
			}
			this->value = nC3;
			if (m_r)
			{
				m_r->OnEndEditionClef();
				m_r->DoRefresh();
			}
			break;
		case ('3'):
			this->symbolType = NEUME_SYMB_CLEF_C;
			if (m_r)
			{
				m_r->OnBeginEditionClef();
			}
			this->value = nC4;
			if (m_r)
			{
				m_r->OnEndEditionClef();
				m_r->DoRefresh();
			}
			break;
		case ('4'):
			this->symbolType = NEUME_SYMB_CLEF_F;
			if (m_r)
			{
				m_r->OnBeginEditionClef();
			}
			this->value = nF3;
			if (m_r)
			{
				m_r->OnEndEditionClef();
				m_r->DoRefresh();
			}
			break;
		case ('5'):
			this->symbolType = NEUME_SYMB_CLEF_F;
			if (m_r)
			{
				m_r->OnBeginEditionClef();
			}
			this->value = nF4;
			if (m_r)
			{
				m_r->OnEndEditionClef();
				m_r->DoRefresh();
			}
			break;
		case ('6'): this->symbolType = NEUME_SYMB_COMMA; break;
		case ('F'): this->symbolType = NEUME_SYMB_FLAT; break;
		case ('N'): this->symbolType = NEUME_SYMB_NATURAL; break;
		case ('7'): this->symbolType = NEUME_SYMB_DIVISION_FINAL; break;
		case ('8'): this->symbolType = NEUME_SYMB_DIVISION_MAJOR; break;
		case ('9'): this->symbolType = NEUME_SYMB_DIVISION_MINOR; break;
		case ('0'): this->symbolType = NEUME_SYMB_DIVISION_SMALL; break;
	}
	//need to write back to MEI for other symbols... doubt this will be used at all, but still...
	
	/*if (m_meiref)
	{
		switch (this->value)
		{
			case (nC1): (m_meiref->getAttribute("shape"))->setValue("c"); (m_meiref->getAttribute("line"))->setValue("1"); break;
			case (nC2): (m_meiref->getAttribute("shape"))->setValue("c"); (m_meiref->getAttribute("line"))->setValue("2"); break;
			case (nC3): (m_meiref->getAttribute("shape"))->setValue("c"); (m_meiref->getAttribute("line"))->setValue("3"); break;
			case (nC4): (m_meiref->getAttribute("shape"))->setValue("c"); (m_meiref->getAttribute("line"))->setValue("4"); break;
			case (nF1): (m_meiref->getAttribute("shape"))->setValue("f"); (m_meiref->getAttribute("line"))->setValue("1"); break;
			case (nF2): (m_meiref->getAttribute("shape"))->setValue("f"); (m_meiref->getAttribute("line"))->setValue("2"); break;
			case (nF3): (m_meiref->getAttribute("shape"))->setValue("f"); (m_meiref->getAttribute("line"))->setValue("3"); break;
			case (nF4): (m_meiref->getAttribute("shape"))->setValue("f"); (m_meiref->getAttribute("line"))->setValue("4"); break;
		}
	}*/
	
	if ( m_r )
	{
		m_r->DoRefresh();
		m_r->OnEndEdition();
	}
	
	if (m_meiref)
	{
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
	}
}

int MusNeumeSymbol::getValue()
{
	return value;
}

void MusNeumeSymbol::updateMeiRefClef(string shape, string line) {
    if (m_meiref->getName() == "clef") {
        MeiAttribute *shapeattr = m_meiref->getAttribute("shape");
        if (shapeattr == NULL) {
            m_meiref->addAttribute(MeiAttribute("shape", shape));
        } else {
            shapeattr->setValue(shape);
        }
        MeiAttribute *lineattr = m_meiref->getAttribute("line");
        if (lineattr == NULL) {
            m_meiref->addAttribute(MeiAttribute("line", line));
        } else {
            lineattr->setValue(line);
        }
    }
}

void MusNeumeSymbol::SetPitch(int pitch, int oct) //this is incomplete, there is no other default symbol at the moment.
{
	if ( this->TYPE != NEUME_SYMB )
		return;
	
	if ((this->getType() == NEUME_SYMB_FLAT) || (this->getType() == NEUME_SYMB_NATURAL))
	{
		if ((this->pitch == pitch) && (this->oct == oct ))
			return;
		
		this->oct = oct;
		this->pitch = pitch;
		
		if (m_r)
			m_r->DoRefresh();
	}
}

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
