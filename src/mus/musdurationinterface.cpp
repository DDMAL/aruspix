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
    m_fermata = false;
}


MusDurationInterface::~MusDurationInterface()
{
}

void MusDurationInterface::SetDuration( int value )
{
    this->m_dur = value;
}

bool MusDurationInterface::HasIdenticalDurationInterface( MusDurationInterface *otherDurationInterface )
{
    if ( !otherDurationInterface ) {
        return false;
    }
    // beam requires value by value comparison
    //if ( this->m_beam != otherDurationInterface->m_beam ) {
    //    return false;
    //}
    if ( this->m_breakSec != otherDurationInterface->m_breakSec ) {
        return false;
    }
    if ( this->m_dots != otherDurationInterface->m_dots ) {
        return false;
    }
    if ( this->m_dur != otherDurationInterface->m_dur ) {
        return false;
    }
    if ( this->m_num != otherDurationInterface->m_num ) {
        return false;
    }
    if ( this->m_numBase != otherDurationInterface->m_numBase ) {
        return false;
    }
    // tuplet requires value by value comparison
    //if ( this->m_tuplet != otherDurationInterface->m_tuplet ) {
    //    return false;
    //}
    if ( this->m_fermata != otherDurationInterface->m_fermata ) {
        return false;
    }
    return true;
}
