/////////////////////////////////////////////////////////////////////////////
// Name:        muspositioninterface.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "muspositioninterface.h"


//----------------------------------------------------------------------------
// MusPositionInterface
//----------------------------------------------------------------------------

MusPositionInterface::MusPositionInterface()
{
    m_oct = 0;
    m_pname = 0;
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
