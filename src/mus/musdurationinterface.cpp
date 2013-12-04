/////////////////////////////////////////////////////////////////////////////
// Name:        musdurationinterface.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "musdurationinterface.h"

//----------------------------------------------------------------------------

#include <math.h>

//----------------------------------------------------------------------------

#include "mus.h"
#include "musdef.h"
#include "musbeam.h"

/**
 * Define the maximum levels between a beam and its notes
 */
#define MAX_BEAM_DEPTH 5 


//----------------------------------------------------------------------------
// MusDurationInterface
//----------------------------------------------------------------------------

MusDurationInterface::MusDurationInterface()
{
    m_breakSec = 0;
    m_dots = 0;
    m_dur = 0;
    m_num = 1;
    m_numBase = 1;
    memset(m_tuplet, 0, DURATION_MAX_TUPLETS * sizeof(unsigned char));
    m_fermata = false;
}


MusDurationInterface::~MusDurationInterface()
{
}

void MusDurationInterface::SetDuration( int value )
{
    this->m_dur = value;
}


double MusDurationInterface::GetAlignementDuration( int num, int numbase )
{
    double duration = DUR_MAX / pow (2.0, (double)(m_dur - 2.0)) * numbase / num;
    if ( m_dots > 0 ) {
        duration = 2 * duration - (duration / pow(2, m_dots));
    }
    //Mus::LogDebug("Duration %d; Dot %d; Alignement %f", m_dur, m_dots, duration );
    return duration;
}

bool MusDurationInterface::IsInBeam( MusObject *noteOrRest )
{
    MusBeam *beam = dynamic_cast<MusBeam*>( noteOrRest->GetFirstParent( &typeid(MusBeam), MAX_BEAM_DEPTH ) );
    if ( !beam ) {
        return false;
    }
    return true;
}

bool MusDurationInterface::IsFirstInBeam( MusObject *noteOrRest )
{
    MusBeam *beam = dynamic_cast<MusBeam*>( noteOrRest->GetFirstParent( &typeid(MusBeam), MAX_BEAM_DEPTH ) );
    if ( !beam ) {
        return false;
    }
    ListOfMusObjects *notesOrRests = beam->GetList( beam );
    ListOfMusObjects::iterator iter = notesOrRests->begin();
    if ( *iter == noteOrRest ) {
        return true;
    }
    return false;    
}

bool MusDurationInterface::IsLastInBeam( MusObject *noteOrRest )
{
    MusBeam *beam = dynamic_cast<MusBeam*>( noteOrRest->GetFirstParent( &typeid(MusBeam), MAX_BEAM_DEPTH ) );
    if ( !beam ) {
        return false;
    }
    ListOfMusObjects *notesOrRests = beam->GetList( beam );
    ListOfMusObjects::reverse_iterator iter = notesOrRests->rbegin();
    if ( *iter == noteOrRest ) {
        return true;
    }
    return false;    
    
}


bool MusDurationInterface::HasIdenticalDurationInterface( MusDurationInterface *otherDurationInterface )
{
    if ( !otherDurationInterface ) {
        return false;
    }
    // beam requires value by value comparison
    //if ( this->m_beam != otherDurationInterface->m_beam ) {
    //    return false;
    //}
    if ( this->m_breakSec != otherDurationInterface->m_breakSec ) {
        return false;
    }
    if ( this->m_dots != otherDurationInterface->m_dots ) {
        return false;
    }
    if ( this->m_dur != otherDurationInterface->m_dur ) {
        return false;
    }
    if ( this->m_num != otherDurationInterface->m_num ) {
        return false;
    }
    if ( this->m_numBase != otherDurationInterface->m_numBase ) {
        return false;
    }
    // tuplet requires value by value comparison
    //if ( this->m_tuplet != otherDurationInterface->m_tuplet ) {
    //    return false;
    //}
    if ( this->m_fermata != otherDurationInterface->m_fermata ) {
        return false;
    }
    return true;
}
