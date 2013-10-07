/////////////////////////////////////////////////////////////////////////////
// Name:        muspitchinterface.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "muspitchinterface.h"

//----------------------------------------------------------------------------
// MusPitchInterface
//----------------------------------------------------------------------------

MusPitchInterface::MusPitchInterface()
{
    m_accid = 0;
    m_oct = 0;
    m_pname = 0;
}


MusPitchInterface::~MusPitchInterface()
{
}

void MusPitchInterface::SetPitch(int pname, int oct) 
{
    m_oct = oct;
    m_pname = pname;
}

bool MusPitchInterface::GetPitch(int *pname, int *oct) 
{
    *oct = m_oct;
    *pname = m_pname;
    return true;
}

bool MusPitchInterface::HasIdenticalPitchInterface( MusPitchInterface *otherPitchInterface )
{
    if ( !otherPitchInterface ) {
        return false;
    }
    if ( this->m_accid != otherPitchInterface->m_accid ) {
        return false;
    }
    if ( this->m_oct != otherPitchInterface->m_oct ) {
        return false;
    }
    if ( this->m_pname != otherPitchInterface->m_pname ) {
        return false;
    }
    return true;
}
