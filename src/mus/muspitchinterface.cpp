/////////////////////////////////////////////////////////////////////////////
// Name:        muspitchinterface.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

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
