/////////////////////////////////////////////////////////////////////////////
// Name:        musrest.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "musrest.h"

//----------------------------------------------------------------------------
// Rest
//----------------------------------------------------------------------------

Rest::Rest():
	MusLayerElement("rest-"), MusDurationInterface(), PositionInterface()
{
    // by default set automatic height
    m_pname = REST_AUTO;
}

Rest::~Rest()
{
}

bool Rest::operator==( MusObject& other )
{
    Rest *otherRest = dynamic_cast<Rest*>( &other );
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
