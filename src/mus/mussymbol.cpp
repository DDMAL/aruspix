/////////////////////////////////////////////////////////////////////////////
// Name:        mussymbol.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "mussymbol.h"


//----------------------------------------------------------------------------
// MusSymbol
//----------------------------------------------------------------------------

MusSymbol::MusSymbol():
	MusLayerElement(), MusPositionInterface()
{
    m_type = SYMBOL_UNDEFINED;
    Init();

}

MusSymbol::MusSymbol( SymbolType type ):
	MusLayerElement(), MusPositionInterface()
{
    m_type = type;
    Init();
}

MusSymbol::~MusSymbol()
{
}


void MusSymbol::Init()
{
    // DOT
    m_dot = 0;
    // ACCID
    m_accid = 0;
}
