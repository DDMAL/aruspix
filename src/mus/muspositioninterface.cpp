/////////////////////////////////////////////////////////////////////////////
// Name:        muspositioninterface.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "muspositioninterface.h"

//----------------------------------------------------------------------------

#include "musdef.h"

//----------------------------------------------------------------------------
// MusPositionInterface
//----------------------------------------------------------------------------

MusPositionInterface::MusPositionInterface()
{
    m_oct = 5;
    m_pname = PITCH_C;
}


MusPositionInterface::~MusPositionInterface()
{
}

void MusPositionInterface::SetPosition(int pname, int oct) 
{
    m_oct = oct;
    m_pname = pname;
}

bool MusPositionInterface::GetPosition(int *pname, int *oct) 
{
    *oct = m_oct;
    *pname = m_pname;
    return true;
}

bool MusPositionInterface::HasIdenticalPositionInterface( MusPositionInterface *otherPositionInterface )
{
    if ( !otherPositionInterface ) {
        return false;
    }
    if ( this->m_oct != otherPositionInterface->m_oct ) {
        return false;
    }
    if ( this->m_pname != otherPositionInterface->m_pname ) {
        return false;
    }
    return true;
}
