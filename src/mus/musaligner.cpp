/////////////////////////////////////////////////////////////////////////////
// Name:        musaligner.cpp
// Author:      Laurent Pugin
// Created:     2013
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "musaligner.h"

//----------------------------------------------------------------------------

#include <assert.h>
#include <math.h>

//----------------------------------------------------------------------------

#include "mus.h"
#include "muslayerelement.h"

//----------------------------------------------------------------------------
// MusSystemAligner
//----------------------------------------------------------------------------

MusSystemAligner::MusSystemAligner():
    MusObject()
{
}

MusSystemAligner::~MusSystemAligner()
{
    
}

MusStaffAlignment* MusSystemAligner::GetStaffAlignment( int idx)
{
    if (idx < GetStaffAlignmentCount()) {
        //Mus::LogDebug("Returning staff alignment %d", idx);
        return (MusStaffAlignment*)m_children[idx];
    }
    // check that we are searching for the next one (not gap)
    assert( idx == GetStaffAlignmentCount() );
    //Mus::LogDebug("Creating staff alignment");
    
    // This is the first time we are looking for it (e.g., first staff)
    // We create the MusStaffAlignment
    MusStaffAlignment *alignment = new MusStaffAlignment();
    alignment->SetParent( this );
    m_children.push_back( alignment );
    return alignment;
}


//----------------------------------------------------------------------------
// MusStaffAlignment
//----------------------------------------------------------------------------

MusStaffAlignment::MusStaffAlignment():
    MusObject()
{
    m_y_rel = 0;
}

MusStaffAlignment::~MusStaffAlignment()
{
    
}

//----------------------------------------------------------------------------
// MusMeasureAligner
//----------------------------------------------------------------------------

MusMeasureAligner::MusMeasureAligner():
    MusObject()
{
    m_max = 0.0;
}

MusMeasureAligner::~MusMeasureAligner()
{
    
}

void MusMeasureAligner::AddAlignment( MusAlignment *alignment, int idx )
{
	alignment->SetParent( this );
    if ( idx == -1 ) {
        m_children.push_back( alignment );
    }
    else {
        InsertChild( alignment, idx );
    }
}

MusAlignment* MusMeasureAligner::GetAlignmentAtTime( double time, MusAlignmentType type )
{
    int i;
    int idx = -1; // the index if we reach the end.
    MusAlignment *alignment = NULL;
    // First try to see if we already have something at the time position
    for (i = 0; i < GetAlignmentCount(); i++)
    {
        alignment = (MusAlignment*)m_children[i];
        if ( (alignment->GetTime() == time) && (alignment->GetType() == type) ) {
            return alignment;
        }
        // nothing found, do not go any further but keep the index
        if (alignment->GetTime() > time) {
            idx = i;
            break;
        }
    }
    // nothing found
    MusAlignment *newAlignement = new MusAlignment( time, type );
    AddAlignment( newAlignement, idx );
    return newAlignement;
}

void MusMeasureAligner::SetMaxTime( double time )
{
    if ( m_max < time ) {
        m_max = time;
    }
}

//----------------------------------------------------------------------------
// MusAlignement
//----------------------------------------------------------------------------

MusAlignment::MusAlignment( ):
    MusObject()
{
    m_x_rel = 0;
    m_x_shift = 0;
    m_time = 0.0;
    m_type = ALIGNMENT_DEFAULT;
}

MusAlignment::MusAlignment( double time, MusAlignmentType type ):
    MusObject()
{
    m_x_shift = 0;
    m_time = time;
    m_type = type;
}

MusAlignment::~MusAlignment()
{
    
}

void MusAlignment::SetXRel( int x_rel )
{
    m_x_rel = x_rel;
}

void MusAlignment::SetXShift( int x_shift )
{
    if ( x_shift > m_x_shift )
    {
        m_x_shift = x_shift;
    }
}

//----------------------------------------------------------------------------
// Functors
//----------------------------------------------------------------------------

int MusMeasureAligner::IntegrateBoundingBoxShift( ArrayPtrVoid params )
{
    // param 0: the cumulated shift
    // param 1: the functor to be redirected to MusAligner
    int *shift = (int*)params[0];
    
    // We start a new MusMeasureAligner
    // Reset the cumulated shift to 0;
    (*shift) = 0;
    
    return FUNCTOR_CONTINUE;
}

int MusAlignment::IntegrateBoundingBoxShift( ArrayPtrVoid params )
{
    // param 0: the cumulated shift
    // param 1: the functor to be redirected to MusAligner
    int *shift = (int*)params[0];
    
    // integrates the m_x_shift into the m_x_rel
    m_x_rel += m_x_shift + (*shift);
    // cumulate the shift value
    (*shift) += m_x_shift;

    return FUNCTOR_CONTINUE;
}

int MusMeasureAligner::SetAligmentXPos( ArrayPtrVoid params )
{
    // param 0: the previous time position
    // param 1: the previous x rel position
    // param 2: the functor to be redirected to MusAligner (unused)
    double *previousTime = (double*)params[0];
    int *previousXRel = (int*)params[1];
    
    // We start a new MusMeasureAligner
    // Reset the previous time position and x_rel to 0;
    (*previousTime) = 0.0;
    (*previousXRel) = 0;
    
    return FUNCTOR_CONTINUE;
}

int MusAlignment::SetAligmentXPos( ArrayPtrVoid params )
{
    // param 0: the previous time position
    // param 1: the previous x rel position
    // param 2: the functor to be redirected to MusAligner (unused)
    double *previousTime = (double*)params[0];
    int *previousXRel = (int*)params[1];
    
    int intervalXRel = 0;
    double intervalTime = (m_time - (*previousTime));
    if ( intervalTime > 0.0 ) {
        intervalXRel = pow( intervalTime, 0.60 ) * 2.5; // 2.5 is an abritrary value
    }
    
    m_x_rel = (*previousXRel) + (intervalXRel);
    (*previousTime) = m_time;
    (*previousXRel) = m_x_rel;
    
    return FUNCTOR_CONTINUE;
}