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
    Object()
{
    m_bottomAlignment = NULL;
}

MusSystemAligner::~MusSystemAligner()
{
    
}


void MusSystemAligner::Reset()
{
    this->ClearChildren();
    m_bottomAlignment = NULL;
    m_bottomAlignment = GetStaffAlignment( 0 );
}

MusStaffAlignment* MusSystemAligner::GetStaffAlignment( int idx )
{
    // The last one is always the bottomAlignment (unless if not created)
    if ( m_bottomAlignment ) {
        // remove it temporarily
        this->m_children.pop_back();
    }
    
    if (idx < GetStaffAlignmentCount()) {
        this->m_children.push_back( m_bottomAlignment );
        return (MusStaffAlignment*)m_children[idx];
    }
    // check that we are searching for the next one (not gap)
    assert( idx == GetStaffAlignmentCount() );
    //Vrv::LogDebug("Creating staff alignment");
    
    // This is the first time we are looking for it (e.g., first staff)
    // We create the MusStaffAlignment
    MusStaffAlignment *alignment = new MusStaffAlignment();
    alignment->SetParent( this );
    m_children.push_back( alignment );
    
    if ( m_bottomAlignment ) {
        this->m_children.push_back( m_bottomAlignment );
    }
    
    return alignment;
}


//----------------------------------------------------------------------------
// MusStaffAlignment
//----------------------------------------------------------------------------

MusStaffAlignment::MusStaffAlignment():
    Object()
{
    m_yRel = 0;
    m_yShift = 0;
}

MusStaffAlignment::~MusStaffAlignment()
{
    
}

void MusStaffAlignment::SetYShift( int yShift )
{
    if ( yShift < m_yShift )
    {
        m_yShift = yShift;
    }
}

//----------------------------------------------------------------------------
// MusMeasureAligner
//----------------------------------------------------------------------------

MusMeasureAligner::MusMeasureAligner():
    Object()
{
    m_leftAlignment = NULL;
    m_rightAlignment = NULL;
    //m_totalWidth = 0;
}

MusMeasureAligner::~MusMeasureAligner()
{
}

void MusMeasureAligner::Reset()
{
    this->ClearChildren();
    m_leftAlignment = new MusAlignment( 0.0, ALIGNMENT_MEASURE_START );
    AddAlignment( m_leftAlignment );
    m_rightAlignment = new MusAlignment( 0.0, ALIGNMENT_MEASURE_END );
    AddAlignment( m_rightAlignment );
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
        double alignment_time = alignment->GetTime();
        if ( Vrv::AreEqual( alignment_time, time ) && (alignment->GetType() == type) ) {
            return alignment;
        }
        // nothing found, do not go any further but keep the index
        if (alignment->GetTime() > time) {
            idx = i;
            break;
        }
    }
    // nothing found
    if ( idx == -1 ) {
        // this is tricky! Because we want m_rightAlignment to always stay at the end,
        // we always to insert _before_ the last one - m_rightAlignment is added in Reset()
        idx = GetAlignmentCount() - 1;
    }
    MusAlignment *newAlignement = new MusAlignment( time, type );
    AddAlignment( newAlignement, idx );
    return newAlignement;
}

void MusMeasureAligner::SetMaxTime( double time )
{
    if ( m_rightAlignment->GetTime() < time ) {
        m_rightAlignment->SetTime( time );
    }
}

//----------------------------------------------------------------------------
// MusAlignement
//----------------------------------------------------------------------------

MusAlignment::MusAlignment( ):
    Object()
{
    m_xRel = 0;
    m_xShift = 0;
    m_maxWidth = 0;
    m_time = 0.0;
    m_type = ALIGNMENT_DEFAULT;
}

MusAlignment::MusAlignment( double time, MusAlignmentType type ):
    Object()
{
    m_xRel = 0;
    m_xShift = 0;
    m_maxWidth = 0;
    m_time = time;
    m_type = type;
}

MusAlignment::~MusAlignment()
{
    
}

void MusAlignment::SetXRel( int x_rel )
{
    m_xRel = x_rel;
}

void MusAlignment::SetXShift( int xShift )
{
    if ( xShift > m_xShift )
    {
        m_xShift = xShift;
    }
}

void MusAlignment::SetMaxWidth( int max_width )
{
    if ( max_width > m_maxWidth )
    {
        m_maxWidth = max_width;
    }
}

//----------------------------------------------------------------------------
// Functors
//----------------------------------------------------------------------------

int MusStaffAlignment::SetAligmentYPos( ArrayPtrVoid params )
{
    // param 0: the previous staff height
    // param 1: the staff margin
    // param 2: the staff interline sizes (int[2])
    // param 2: the functor to be redirected to MusSystemAligner (unused)
    int *previousStaffHeight = (int*)params[0];
    int *staffMargin = (int*)params[1];
    int **interlineSizes = (int**)params[2];

    int min_shift = (*staffMargin) + (*previousStaffHeight);
    
    if ( m_yShift > -min_shift) {
        m_yShift = -min_shift;
    }
    
    // for now always for interlines, eventually should be taken from the staffDef, so should the staff size
    (*previousStaffHeight) = 4 * (*interlineSizes)[0];
    
    return FUNCTOR_CONTINUE;
}

int MusStaffAlignment::IntegrateBoundingBoxYShift( ArrayPtrVoid params )
{
    // param 0: the cumulated shift
    // param 1: the functor to be redirected to the MusSystemAligner (unused)
    int *shift = (int*)params[0];
    
    // integrates the m_xShift into the m_xRel
    m_yRel += m_yShift + (*shift);
    // cumulate the shift value
    (*shift) += m_yShift;
    m_yShift = 0;
    
    return FUNCTOR_CONTINUE;
}

int MusMeasureAligner::IntegrateBoundingBoxXShift( ArrayPtrVoid params )
{
    // param 0: the cumulated shift
    // param 1: the cumulated width
    // param 2: the functor to be redirected to the MusMeasureAligner (unused)
    int *shift = (int*)params[0];
    
    // We start a new MusMeasureAligner
    // Reset the cumulated shift to 0;
    (*shift) = 0;
    
    return FUNCTOR_CONTINUE;
}

int MusAlignment::IntegrateBoundingBoxXShift( ArrayPtrVoid params )
{
    // param 0: the cumulated shift
    // param 1: the functor to be redirected to the MusMeasureAligner (unused)
    int *shift = (int*)params[0];
    
    // integrates the m_xShift into the m_xRel
    m_xRel += m_xShift + (*shift);
    // cumulate the shift value and the width
    (*shift) += m_xShift;

    // reset member to 0
    m_xShift = 0;
    
    return FUNCTOR_CONTINUE;
}

int MusMeasureAligner::SetAligmentXPos( ArrayPtrVoid params )
{
    // param 0: the previous time position
    // param 1: the previous x rel position
    // param 2: the functor to be redirected to the MusMeasureAligner (unused)
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
    // param 2: the functor to be redirected to the MusMeasureAligner (unused)
    double *previousTime = (double*)params[0];
    int *previousXRel = (int*)params[1];
    
    int intervalXRel = 0;
    double intervalTime = (m_time - (*previousTime));
    if ( intervalTime > 0.0 ) {
        intervalXRel = pow( intervalTime, 0.60 ) * 2.5; // 2.5 is an abritrary value
    }
    
    m_xRel = (*previousXRel) + (intervalXRel);
    (*previousTime) = m_time;
    (*previousXRel) = m_xRel;
    
    return FUNCTOR_CONTINUE;
}


int MusAlignment::JustifyX( ArrayPtrVoid params )
{
    // param 0: the justification ratio
    // param 1: the system full width (without system margins) (unused)
    // param 2: the functor to be redirected to the MusMeasureAligner (unused)
    double *ratio = (double*)params[0];
    
    this->m_xRel = ceil((*ratio) * (double)this->m_xRel);

    return FUNCTOR_CONTINUE;
}

