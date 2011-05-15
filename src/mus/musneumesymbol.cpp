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
}

MusNeumeSymbol::MusNeumeSymbol(const MusNeumeSymbol &symbol) :
    MusElement(symbol)
{
    symbolType = symbol.symbolType;
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
					code = nC1;
				} else if (line->getValue() == "2") {
					code = nC2;
				} else if (line->getValue() == "3") {
					code = nC3;
				} else if (line->getValue() == "4") {
					code = nC4;
				} else {
                    throw "unknown line for a C clef";
                }
			} else if (shape->getValue() == "F") {
                symbolType = NEUME_SYMB_CLEF_F;
				if (line->getValue() == "1") {
					code = nF1;
                } else if (line->getValue() == "2") {
					code = nF2;
                } else if (line->getValue() == "3") {
					code = nF3;
                } else if (line->getValue() == "4") {
					code = nF4;
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
	char valeurcod = 0;
	
	int oct = this->oct - 4;
	
	switch (this->symbolType)
	{
		case NEUME_SYMB_CLEF_C:
		case NEUME_SYMB_CLEF_F:
		calcoffs (&x,(int)this->code);
		this->dec_y = x;
		this->DrawClef( dc, staff); 
		break;
	}
	
	if ( !m_r->m_eraseElement )
		m_r->m_currentColour = AxBLACK;
	
    dc->EndGraphic();
	
	return;
}

void MusNeumeSymbol::DrawClef( AxDC *dc, MusStaff *staff)
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() ) {
		return;
	}
	
	int x = this->xrel;
	int y = staff->yrel - m_r->_portee[staff->pTaille] - this->dec_y + m_r->_espace[staff->pTaille]; //with a fudge factor?
	dimin = this->dimin;
	wxString shape = nF_CLEF;
	
	switch (this->code)
	{
		case nC1: shape = nC_CLEF;
		case nF1: y -= m_r->_interl[staff->pTaille]*3; break;
		case nC2: shape = nC_CLEF;
		case nF2: y -= m_r->_interl[staff->pTaille]*2; break;
		case nC3: shape = nC_CLEF;
		case nF3: y -= m_r->_interl[staff->pTaille]; break;
		case nC4: shape = nC_CLEF;
		case nF4: break;
		default: break;
	}
	
	m_r->festa_string(dc, x, y, shape, staff, dimin);
}

void MusNeumeSymbol::SetValue(int value, MusStaff *staff, int vflag)
{	
	if ( this->TYPE != NEUME_SYMB )
		return;
	
	if ( this->symbolType == NEUME_SYMB_CLEF_C || this->symbolType == NEUME_SYMB_CLEF_F )
	{
		if (m_r)
		{
			m_r->OnBeginEditionClef();
		}
		
		switch (value)
		{
			case ('1'): this->code = nC2; break;
			case ('2'): this->code = nC3; break;
			case ('3'): this->code = nC4; break;
			case ('4'): this->code = nF3; break;
			case ('5'): this->code = nF4; break;
		}
		
		if (m_r)
		{
			m_r->OnEndEditionClef();
			m_r->DoRefresh();
		}
		
		if (m_meiref)
		{
			switch (this->code)
			{
				case (nC1): updateMeiRefClef("C", "1"); break;
				case (nC2): (m_meiref->getAttribute("shape"))->setValue("C"); (m_meiref->getAttribute("line"))->setValue("2"); break;
				case (nC3): (m_meiref->getAttribute("shape"))->setValue("C"); (m_meiref->getAttribute("line"))->setValue("3"); break;
				case (nC4): (m_meiref->getAttribute("shape"))->setValue("C"); (m_meiref->getAttribute("line"))->setValue("4"); break;
				case (nF1): (m_meiref->getAttribute("shape"))->setValue("F"); (m_meiref->getAttribute("line"))->setValue("1"); break;
				case (nF2): (m_meiref->getAttribute("shape"))->setValue("F"); (m_meiref->getAttribute("line"))->setValue("2"); break;
				case (nF3): (m_meiref->getAttribute("shape"))->setValue("F"); (m_meiref->getAttribute("line"))->setValue("3"); break;
				case (nF4): (m_meiref->getAttribute("shape"))->setValue("F"); (m_meiref->getAttribute("line"))->setValue("4"); break;
			}
		}
	}
	
	if ( m_r )
	{
		m_r->DoRefresh();
		m_r->OnEndEdition();
	}
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

void MusNeumeSymbol::ResetToNeumeSymbol() //this is incomplete, there is no other default symbol at the moment.
{
	MusNeumeSymbol reset;
	reset.symbolType = NEUME_SYMB_CLEF_C;
	reset.code = nC2;
	*this = reset;
}

void MusNeumeSymbol::ResetToClef()
{
	MusNeumeSymbol reset;
	reset.symbolType = NEUME_SYMB_CLEF_C;
	reset.code = nC2;
	*this = reset;
}

MusNeumeSymbolType MusNeumeSymbol::GetSymbolType() {
    return symbolType;
}

MeiElement *MusNeumeSymbol::getMeiRef() {
    return m_meiref;
}
