/////////////////////////////////////////////////////////////////////////////
// Name:        musbarline.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "musbarline.h"

//----------------------------------------------------------------------------
// MusBarline
//----------------------------------------------------------------------------

MusBarline::MusBarline():
	MusLayerElement("bline-")
{
    m_barlineType = BARLINE_SINGLE;
    m_partialBarline = false; // this was used unitialized
    m_onStaffOnly = false; // is this good?
}


MusBarline::~MusBarline()
{
}

bool MusBarline::operator==( MusObject& other )
{
    MusBarline *otherBarline = dynamic_cast<MusBarline*>( &other );
    if ( !otherBarline ) {
        return false;
    }
    return true;
}
