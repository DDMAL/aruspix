/*
 *  musneumesymbol.h
 *  aruspix
 *
 *  Created by Alastair Porter on 11-03-29.
 *  Copyright 2011 com.aruspix.www. All rights reserved.
 *
 */

#ifndef __MUS_NEUME_SYMBOL_H__
#define __MUS_NEUME_SYMBOL_H__


#include "wx/wx.h"

#include "muslayer.h"
#include "muspositioninterface.h"

#include <string>
using std::string;

#include <mei/meielement.h>
#include <mei/exceptions.h>

using mei::MeiElement;
using mei::MeiAttribute;

enum MusNeumeSymbolType {
    NEUME_SYMB_COMMA,
    NEUME_SYMB_FLAT,
    NEUME_SYMB_NATURAL,
    NEUME_SYMB_CLEF_C,
    NEUME_SYMB_CLEF_F,
    NEUME_SYMB_DIVISION_FINAL,
    NEUME_SYMB_DIVISION_MAJOR,
    NEUME_SYMB_DIVISION_MINOR,
    NEUME_SYMB_DIVISION_SMALL
};


//----------------------------------------------------------------------------
// MusNeumeSymbol
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <?> element. 
 */
class MusNeumeSymbol : public MusLayerElement, public MusPositionInterface {
public:
	MusNeumeSymbol();
	MusNeumeSymbol(MeiElement &meielement);
	MusNeumeSymbol(const MusNeumeSymbol &symbol);
	virtual ~MusNeumeSymbol() {}
    
    virtual wxString MusClassName( ) { return "MusNeumeSymbol"; };
	
	void calcoffs (int *offst, int value);
    MusNeumeSymbolType GetSymbolType();
	void SetValue(int value, MusLaidOutStaff *staff, int flag);
	int getValue();
	void SetPitch(int pitch, int oct);
	
	void ResetToNeumeSymbol();
	void ResetToClef();
	MusNeumeSymbolType getType();
	
    MeiElement *getMeiRef();
	void deleteMeiRef();
    
    // ax2 - member previously in MusElement (duplicated in MusNeume
    std::string PitchToStr(int pitch);
    int StrToPitch(std::string pitch);


private:
    MusNeumeSymbolType symbolType;
	MeiElement *m_meiref;
    void updateMeiRefClef(string shape, string line);
    void updateMeiRefDiv(string form);
    void updateMeiRefAccid(string accid, int pitch, int octave);
	int value; //this is to denote the line a clef lies on
};

#endif // __MUS_NEUME_SYMBOL_H__
