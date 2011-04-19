/*
 *  musneumesymbol.cpp
 *  aruspix
 *
 *  Created by Alastair Porter on 11-03-29.
 *  Copyright 2011 com.aruspix.www. All rights reserved.
 *
 */

#include "musneumesymbol.h"

MusNeumeSymbol::MusNeumeSymbol() :
    MusElement()
{
}

MusNeumeSymbol::MusNeumeSymbol(MusNeumeSymbolType type) :
    MusElement()
{
	symbolType = type;
}

MusNeumeSymbol::MusNeumeSymbol(const MusNeumeSymbol &symbol) :
    MusElement(symbol)
{
    symbolType = symbol.symbolType;
}

void MusNeumeSymbol::SetSymbolType(MusNeumeSymbolType type) {
    symbolType = type;
}