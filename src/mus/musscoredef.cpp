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
#include "musclef.h"
#include "musio.h"
#include "muskeysig.h"
#include "musmensur.h"

/**
 * Define the maximum levels of staffGrp within a scoreDef
 */
#define MAX_STAFFGRP_DEPTH 5

//----------------------------------------------------------------------------
// MusScoreOrStaffDefAttrInterface
//----------------------------------------------------------------------------

MusScoreOrStaffDefAttrInterface::MusScoreOrStaffDefAttrInterface()
{
    m_clef = NULL;
    m_keySig = NULL;
    m_mensur = NULL;
}

MusScoreOrStaffDefAttrInterface::~MusScoreOrStaffDefAttrInterface()
{
    if (m_clef) {
        delete m_clef;
    }
    if (m_keySig) {
        delete m_keySig;
    }
    if (m_mensur) {
        delete m_mensur;
    }
}

void MusScoreOrStaffDefAttrInterface::ReplaceClef( MusClef *newClef )
{
    if ( newClef ) {
        if (m_clef) {
            delete m_clef;
        }
        m_clef = new MusClef( *newClef );
    }
}

void MusScoreOrStaffDefAttrInterface::ReplaceKeySig( MusKeySig *newKeySig )
{
    if ( newKeySig ) {
        if (m_keySig) {
            delete m_keySig;
        }
        m_keySig = new MusKeySig( *newKeySig );
    }
}

void MusScoreOrStaffDefAttrInterface::ReplaceMensur( MusMensur *newMensur )
{
    if ( newMensur ) {
        if (m_mensur) {
            delete m_mensur;
        }
        m_mensur = new MusMensur( *newMensur );
    }
}

//----------------------------------------------------------------------------
// MusScoreDef
//----------------------------------------------------------------------------

MusScoreDef::MusScoreDef() :
	MusObject(), MusScoreOrStaffDefAttrInterface()
{
    m_clef = NULL;
    m_keySig = NULL;
    m_mensur = NULL;
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

void MusScoreDef::Replace( MusScoreDef *newScoreDef )
{
    ReplaceClef( newScoreDef->m_clef );
    ReplaceKeySig( newScoreDef->m_keySig );
    ReplaceMensur( newScoreDef->m_mensur );
    
    ArrayPtrVoid params;
	params.push_back( this );
    MusFunctor replaceStaffDefsInScoreDef( &MusObject::ReplaceStaffDefsInScoreDef );
    newScoreDef->Process( &replaceStaffDefsInScoreDef, params );
}

void MusScoreDef::Replace( MusStaffDef *newStaffDef )
{
    // first find the staffDef with the same @n
    MusStaffDef *staffDef = this->GetStaffDef( newStaffDef->GetStaffNo() );
    
    // if found, replace attributes
    if (staffDef) {
        staffDef->ReplaceClef( newStaffDef->m_clef );
        staffDef->ReplaceKeySig( newStaffDef->m_keySig );
        staffDef->ReplaceMensur( newStaffDef->m_mensur );
    }
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
    MusObject(), MusScoreOrStaffDefAttrInterface()
{
}

MusStaffDef::~MusStaffDef()
{
}

void MusStaffDef::SetScoreDefPtr()
{
    m_parentScoreDef = dynamic_cast<MusScoreDef*>( this->GetFirstParent( &typeid(MusScoreDef), MAX_STAFFGRP_DEPTH ) );
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

int MusStaffDef::ReplaceStaffDefsInScoreDef( ArrayPtrVoid params )
{
    // param 0: the scoreDef
    MusScoreDef *scoreDef = (MusScoreDef*)params[0];
    
    scoreDef->Replace( this );
    
    return FUNCTOR_CONTINUE;
}

