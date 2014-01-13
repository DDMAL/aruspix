/////////////////////////////////////////////////////////////////////////////
// Name:        mustie.cpp
// Author:      Rodolfo Zitellini
// Created:     26/06/2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "mustie.h"

//----------------------------------------------------------------------------

#include <assert.h>

//----------------------------------------------------------------------------
// Tie
//----------------------------------------------------------------------------

Tie::Tie():
MusLayerElement("tie-")
{
    m_first = NULL;
    m_second = NULL;
}


Tie::~Tie()
{
    if (m_first && m_first->GetTieAttrTerminal() == this) {
        m_first->ResetTieAttrInitial();
    }
    if (m_second && m_second->GetTieAttrTerminal() == this) {
        m_second->ResetTieAttrTerminal();
    }
}

void Tie::SetFirstNote( MusNote *note )
{
    assert( !m_first );
    m_first = note;
}

void Tie::SetSecondNote( MusNote *note )
{
    assert( !m_second );
    m_second = note;
}