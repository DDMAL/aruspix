/////////////////////////////////////////////////////////////////////////////
// Name:        musaligner.cpp
// Author:      Laurent Pugin
// Created:     2013
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "aligner.h"

//----------------------------------------------------------------------------

#include <assert.h>
#include <math.h>

//----------------------------------------------------------------------------

#include "vrv.h"
#include "layerelement.h"

namespace vrv {

//----------------------------------------------------------------------------
// SystemAligner
//----------------------------------------------------------------------------

SystemAligner::SystemAligner():
    Object()
{
    m_bottomAlignment = NULL;
}

SystemAligner::~SystemAligner()
{
    
}


void SystemAligner::Reset()
{
    this->ClearChildren();
    m_bottomAlignment = NULL;
    m_bottomAlignment = GetStaffAlignment( 0 );
}

StaffAlignment* SystemAligner::GetStaffAlignment( int idx )
{
    // The last one is always the bottomAlignment (unless if not created)
    if ( m_bottomAlignment ) {
        // remove it temporarily
        this->m_children.pop_back();
    }
    
    if (idx < GetStaffAlignmentCount()) {
        this->m_children.push_back( m_bottomAlignment );
        return (StaffAlignment*)m_children[idx];
    }
    // check that we are searching for the next one (not gap)
    assert( idx == GetStaffAlignmentCount() );
    //Vrv::LogDebug("Creating staff alignment");
    
    // This is the first time we are looking for it (e.g., first staff)
    // We create the StaffAlignment
    StaffAlignment *alignment = new StaffAlignment();
    alignment->SetParent( this );
    m_children.push_back( alignment );
    
    if ( m_bottomAlignment ) {
        this->m_children.push_back( m_bottomAlignment );
    }
    
    return alignment;
}


//----------------------------------------------------------------------------
// StaffAlignment
//----------------------------------------------------------------------------

StaffAlignment::StaffAlignment():
    Object()
{
    m_yRel = 0;
    m_yShift = 0;
}

StaffAlignment::~StaffAlignment()
{
    
}

void StaffAlignment::SetYShift( int yShift )
{
    if ( yShift < m_yShift )
    {
        m_yShift = yShift;
    }
}

//----------------------------------------------------------------------------
// MeasureAligner
//----------------------------------------------------------------------------

MeasureAligner::MeasureAligner():
    Object()
{
    m_leftAlignment = NULL;
    m_rightAlignment = NULL;
    //m_totalWidth = 0;
}

MeasureAligner::~MeasureAligner()
{
}

void MeasureAligner::Reset()
{
    this->ClearChildren();
    m_leftAlignment = new Alignment( 0.0, ALIGNMENT_MEASURE_START );
    AddAlignment( m_leftAlignment );
    m_rightAlignment = new Alignment( 0.0, ALIGNMENT_MEASURE_END );
    AddAlignment( m_rightAlignment );
}

void MeasureAligner::AddAlignment( Alignment *alignment, int idx )
{
	alignment->SetParent( this );
    if ( idx == -1 ) {
        m_children.push_back( alignment );
    }
    else {
        InsertChild( alignment, idx );
    }
}

Alignment* MeasureAligner::GetAlignmentAtTime( double time, MusAlignmentType type )
{
    int i;
    int idx = -1; // the index if we reach the end.
    Alignment *alignment = NULL;
    // First try to see if we already have something at the time position
    for (i = 0; i < GetAlignmentCount(); i++)
    {
        alignment = (Alignment*)m_children[i];
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
    Alignment *newAlignement = new Alignment( time, type );
    AddAlignment( newAlignement, idx );
    return newAlignement;
}

void MeasureAligner::SetMaxTime( double time )
{
    if ( m_rightAlignment->GetTime() < time ) {
        m_rightAlignment->SetTime( time );
    }
}

//----------------------------------------------------------------------------
// MusAlignement
//----------------------------------------------------------------------------

Alignment::Alignment( ):
    Object()
{
    m_xRel = 0;
    m_xShift = 0;
    m_maxWidth = 0;
    m_time = 0.0;
    m_type = ALIGNMENT_DEFAULT;
}

Alignment::Alignment( double time, MusAlignmentType type ):
    Object()
{
    m_xRel = 0;
    m_xShift = 0;
    m_maxWidth = 0;
    m_time = time;
    m_type = type;
}

Alignment::~Alignment()
{
    
}

void Alignment::SetXRel( int x_rel )
{
    m_xRel = x_rel;
}

void Alignment::SetXShift( int xShift )
{
    if ( xShift > m_xShift )
    {
        m_xShift = xShift;
    }
}

void Alignment::SetMaxWidth( int max_width )
{
    if ( max_width > m_maxWidth )
    {
        m_maxWidth = max_width;
    }
}

//----------------------------------------------------------------------------
// Functors
//----------------------------------------------------------------------------

int StaffAlignment::SetAligmentYPos( ArrayPtrVoid params )
{
    // param 0: the previous staff height
    // param 1: the staff margin
    // param 2: the staff interline sizes (int[2])
    // param 2: the functor to be redirected to SystemAligner (unused)
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

int StaffAlignment::IntegrateBoundingBoxYShift( ArrayPtrVoid params )
{
    // param 0: the cumulated shift
    // param 1: the functor to be redirected to the SystemAligner (unused)
    int *shift = (int*)params[0];
    
    // integrates the m_xShift into the m_xRel
    m_yRel += m_yShift + (*shift);
    // cumulate the shift value
    (*shift) += m_yShift;
    m_yShift = 0;
    
    return FUNCTOR_CONTINUE;
}

int MeasureAligner::IntegrateBoundingBoxXShift( ArrayPtrVoid params )
{
    // param 0: the cumulated shift
    // param 1: the cumulated width
    // param 2: the functor to be redirected to the MeasureAligner (unused)
    int *shift = (int*)params[0];
    
    // We start a new MeasureAligner
    // Reset the cumulated shift to 0;
    (*shift) = 0;
    
    return FUNCTOR_CONTINUE;
}

int Alignment::IntegrateBoundingBoxXShift( ArrayPtrVoid params )
{
    // param 0: the cumulated shift
    // param 1: the functor to be redirected to the MeasureAligner (unused)
    int *shift = (int*)params[0];
    
    // integrates the m_xShift into the m_xRel
    m_xRel += m_xShift + (*shift);
    // cumulate the shift value and the width
    (*shift) += m_xShift;

    // reset member to 0
    m_xShift = 0;
    
    return FUNCTOR_CONTINUE;
}

int MeasureAligner::SetAligmentXPos( ArrayPtrVoid params )
{
    // param 0: the previous time position
    // param 1: the previous x rel position
    // param 2: the functor to be redirected to the MeasureAligner (unused)
    double *previousTime = (double*)params[0];
    int *previousXRel = (int*)params[1];
    
    // We start a new MeasureAligner
    // Reset the previous time position and x_rel to 0;
    (*previousTime) = 0.0;
    (*previousXRel) = 0;
    
    return FUNCTOR_CONTINUE;
}

int Alignment::SetAligmentXPos( ArrayPtrVoid params )
{
    // param 0: the previous time position
    // param 1: the previous x rel position
    // param 2: the functor to be redirected to the MeasureAligner (unused)
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


int Alignment::JustifyX( ArrayPtrVoid params )
{
    // param 0: the justification ratio
    // param 1: the system full width (without system margins) (unused)
    // param 2: the functor to be redirected to the MeasureAligner (unused)
    double *ratio = (double*)params[0];
    
    this->m_xRel = ceil((*ratio) * (double)this->m_xRel);

    return FUNCTOR_CONTINUE;
}
    
} // namespace vrv
