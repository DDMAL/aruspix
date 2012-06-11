/////////////////////////////////////////////////////////////////////////////
// Name:        musnote.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musnote.h"


//----------------------------------------------------------------------------
// MusNote
//----------------------------------------------------------------------------

MusNote::MusNote():
	MusLayerElement(), MusDurationInterface(), MusPitchInterface()
{
    m_artic = 0;
    m_chord = 0;
    m_colored = false;
    m_headshape = 0;
    m_lig = 0;
    m_ligObliqua = false;
    memset(m_slur, 0, sizeof(unsigned char) * NOTE_MAX_SLURS);
    m_stemDir = 0;
    m_stemLen = 0;
}


MusNote::~MusNote()
{
}
