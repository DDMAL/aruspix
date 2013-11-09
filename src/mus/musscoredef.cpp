/////////////////////////////////////////////////////////////////////////////
// Name:        musscoredef.cpp
// Author:      Laurent Pugin
// Created:     2013/11/08
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "musscoredef.h"

//----------------------------------------------------------------------------

#include <assert.h>

//----------------------------------------------------------------------------

#include "mus.h"
#include "musio.h"

//----------------------------------------------------------------------------
// MusScoreDef
//----------------------------------------------------------------------------

MusScoreDef::MusScoreDef() :
	MusObject()
{
}

MusScoreDef::~MusScoreDef()
{
}

void MusScoreDef::AddStaffGrp( MusStaffGrp *staffGrp )
{
	staffGrp->SetParent( this );
	m_children.push_back( staffGrp );
}


//----------------------------------------------------------------------------
// MusStaffGrp
//----------------------------------------------------------------------------

MusStaffGrp::MusStaffGrp() :
    MusObject()
{
}

MusStaffGrp::~MusStaffGrp()
{
}

void MusStaffGrp::AddStaffDef( MusStaffDef *staffDef )
{
	staffDef->SetParent( this );
	m_children.push_back( staffDef );
}


int MusStaffGrp::Save( ArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];
    if (!output->WriteStaffGrp( this )) {
        return FUNCTOR_STOP;
    }
    return FUNCTOR_CONTINUE;
    
}


//----------------------------------------------------------------------------
// MusStaffDef
//----------------------------------------------------------------------------

MusStaffDef::MusStaffDef() :
    MusObject()
{
}

MusStaffDef::~MusStaffDef()
{
}

int MusStaffDef::Save( ArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];
    if (!output->WriteStaffDef( this )) {
        return FUNCTOR_STOP;
    }
    return FUNCTOR_CONTINUE;
    
}
