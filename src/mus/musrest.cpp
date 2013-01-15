/////////////////////////////////////////////////////////////////////////////
// Name:        musrest.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musrest.h"


//----------------------------------------------------------------------------
// MusRest
//----------------------------------------------------------------------------

MusRest::MusRest():
	MusLayerElement(), MusDurationInterface(), MusPositionInterface()
{
    // by default set automatic height
    m_pname = REST_AUTO;
}


MusRest::~MusRest()
{
}

bool MusRest::operator==( MusLayerElement& other)
{
    MusRest *otherRest = dynamic_cast<MusRest*>( &other );
    if ( !otherRest ) {
        return false;
    }
    if ( !this->HasIdenticalPositionInterface( otherRest ) ) {
        return false;
    }
    if ( !this->HasIdenticalDurationInterface( otherRest ) ) {
        return false;
    }
    return true;
}
