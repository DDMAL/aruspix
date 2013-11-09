/////////////////////////////////////////////////////////////////////////////
// Name:        musmeasure.h
// Author:      Laurent Pugin
// Created:     2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "musmeasure.h"

//----------------------------------------------------------------------------

#include <assert.h>

//----------------------------------------------------------------------------

#include "mus.h"
#include "musaligner.h"
#include "musdef.h"
#include "musio.h"
#include "musstaff.h"

//----------------------------------------------------------------------------
// MusMeasure
//----------------------------------------------------------------------------

MusMeasure::MusMeasure( bool measureMusic, int logMeasureNb ):
MusDocObject()
{
	Clear( );
    m_measuredMusic = measureMusic;
    m_logMeasureNb = logMeasureNb;
    if ( !m_measuredMusic ) {
        m_x_abs = 0;
    }
}

MusMeasure::~MusMeasure()
{
    
}

MusMeasure::MusMeasure( const MusMeasure& measure )
{
    m_logMeasureNb = measure.m_logMeasureNb;
	m_x_abs = measure.m_x_abs;
    m_x_rel = measure.m_x_rel;
	m_x_drawing = measure.m_x_drawing;
    
    int i;
	for (i = 0; i < measure.GetStaffCount(); i++)
	{
        MusStaff *nstaff = new MusStaff( *(MusStaff*)measure.m_children[i] );
        this->AddStaff( nstaff );
	}
}

void MusMeasure::Clear()
{
	ClearChildren();
    m_parent = NULL;
    m_logMeasureNb = -1;
    m_measuredMusic = true;
    m_x_abs = AX_UNSET;
    m_x_rel = 0;
    m_x_drawing = 0;
}

int MusMeasure::Save( ArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];
    if (!output->WriteMeasure( this )) {
        return FUNCTOR_STOP;
    }
    return FUNCTOR_CONTINUE;

}

void MusMeasure::AddStaff( MusStaff *staff )
{
	staff->SetParent( this );
	m_children.push_back( staff );
}

int MusMeasure::GetMeasureNo() const
{
    assert( m_parent ); // System cannot be NULL
    
    return m_parent->GetChildIndex( this );
}

MusStaff *MusMeasure::GetFirst( )
{
	if ( m_children.empty() )
		return NULL;
	return (MusStaff*)m_children[0];
}

MusStaff *MusMeasure::GetLast( )
{
	if ( m_children.empty() )
		return NULL;
	int i = GetStaffCount() - 1;
	return (MusStaff*)m_children[i];
}

MusStaff *MusMeasure::GetNext( MusStaff *staff )
{
    if ( !staff || m_children.empty())
        return NULL;
    
	int i = 0; GetChildIndex( staff );
    
	if ((i == -1 ) || ( i >= GetStaffCount() - 1 ))
		return NULL;
    
	return (MusStaff*)m_children[i + 1];
}

MusStaff *MusMeasure::GetPrevious( MusStaff *staff )
{
    if ( !staff || m_children.empty())
        return NULL;
    
	int i = GetChildIndex( staff );
    
	if ((i == -1 ) || ( i <= 0 ))
        return NULL;
	
    return (MusStaff*)m_children[i - 1];
}


MusStaff *MusMeasure::GetStaff( int StaffNo )
{
    if ( StaffNo > (int)m_children.size() - 1 )
        return NULL;
	
	return (MusStaff*)m_children[StaffNo];
}

int MusMeasure::GetXRel()
{
    if (m_alignment) {
        return m_alignment->GetXRel();
    }
    return 0;
}

//----------------------------------------------------------------------------
// MusMeasure functor methods
//----------------------------------------------------------------------------

int MusMeasure::Align( ArrayPtrVoid params )
{
    // param 0: the aligner
    // param 1: the measureAligner
    // param 2: the measureNb
    // param 3: the time
    MusAligner **aligner = (MusAligner**)params[0];
    MusMeasureAligner **measureAligner = (MusMeasureAligner**)params[1];
	int *measureNb = (int*)params[2];
    
    // this gets (or creates) the measureAligner for the measure
    (*measureAligner) = (*aligner)->GetMeasureAligner(*measureNb);
    
    assert( *measureAligner );
    
    // Set the pointer of the m_alignment
    m_alignment = (*measureAligner)->GetMeasureAlignment();
    
    // for next measure
    (*measureNb)++;
    
    return FUNCTOR_CONTINUE;
}

