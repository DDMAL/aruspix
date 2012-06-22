/////////////////////////////////////////////////////////////////////////////
// Name:        musdurationinterface.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musdurationinterface.h"


//----------------------------------------------------------------------------
// MusDurationInterface
//----------------------------------------------------------------------------

MusDurationInterface::MusDurationInterface()
{
    memset(m_beam, 0, DURATION_MAX_BEAMS * sizeof(unsigned char));
    m_breakSec = 0;
    m_dots = 0;
    m_dur = 0;
    m_num = 1;
    m_numBase = 1;
    memset(m_tuplet, 0, DURATION_MAX_TUPLETS * sizeof(unsigned char));
}


MusDurationInterface::~MusDurationInterface()
{
}

void MusDurationInterface::SetDuration( int value )
{
    this->m_dur = value;
}
