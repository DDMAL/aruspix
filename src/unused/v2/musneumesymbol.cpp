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
}

MusNeumeSymbol::MusNeumeSymbol(const MusNeumeSymbol &symbol) :
    MusLayerElement(symbol), MusPositionInterface(symbol)
{
    symbolType = symbol.symbolType;
	value = symbol.value;
    m_pname = symbol.m_pname;
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

void MusNeumeSymbol::SetValue(int value, MusStaff *staff, int vflag)
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
}

int MusNeumeSymbol::getValue()
{
	return value;
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

std::string MusNeumeSymbol::PitchToStr(int pitch)
{
    std::string value;
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


