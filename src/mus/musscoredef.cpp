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
    this->ResetUuid();
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

MusStaffDef *MusScoreDef::GetStaffDef( int n )
{
    MusStaffDef *staffDef = NULL;
    ArrayPtrVoid params;
	params.push_back( &n );
    params.push_back( &staffDef );
    MusFunctor findStaffDef( &MusObject::FindStaffDefByNumber );
    this->Process( &findStaffDef, params );
    
    // the staffDef should never be NULL
    assert(staffDef);
    
    return staffDef;
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

//----------------------------------------------------------------------------
// MusStaffDef functor methods
//----------------------------------------------------------------------------

int MusStaffDef::FindStaffDefByNumber( ArrayPtrVoid params )
{
    // param 0: the n we are looking for
    // param 1: the pointer to pointer to the MusStaffDef
    int *n = (int*)params[0];
    MusStaffDef **staffDef = (MusStaffDef**)params[1];
    
    if ( (*staffDef) ) {
        // this should not happen, but just in case
        return FUNCTOR_STOP;
    }
    
    if ( this->GetStaffNo() == (*n) ) {
        (*staffDef) = this;
        //Mus::LogDebug("Found it!");
        return FUNCTOR_STOP;
    }
    //Mus::LogDebug("Still looking for it...");
    return FUNCTOR_CONTINUE;
}

