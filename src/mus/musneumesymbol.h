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

#include "muselement.h"

enum MusNeumeSymbolType {
    NEUME_SYMB_COMMA,
    NEUME_SYMB_FLAT,
    NEUME_SYMB_NATURAL,
    NEUME_SYMB_CLEF_C,
    NEUME_SYMB_CLEF_F,
    NEUME_SYMB_CUSTOS,
    NEUME_SYMB_DIVISION_FINAL,
    NEUME_SYMB_DIVISION_MAJOR,
    NEUME_SYMB_DIVISION_MINOR,
    NEUME_SYMB_DIVISION_SMALL
};

class MusNeumeSymbol : public MusElement {
public:
	MusNeumeSymbol();
    MusNeumeSymbol(MusNeumeSymbolType type);
	MusNeumeSymbol(const MusNeumeSymbol &symbol);
	virtual ~MusNeumeSymbol() {}
	
    void SetSymbolType(MusNeumeSymbolType type);
    
private:
    MusNeumeSymbolType symbolType;
    

};

#endif // __MUS_NEUME_SYMBOL_H__