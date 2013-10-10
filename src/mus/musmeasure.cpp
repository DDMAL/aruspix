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

#include "musdef.h"
#include "musio.h"
#include "muslayer.h"

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
	for (i = 0; i < measure.GetLayerCount(); i++)
	{
        MusLayer *nlayer = new MusLayer( *(MusLayer*)measure.m_children[i] );
        this->AddLayer( nlayer );
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

bool MusMeasure::Save( ArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];
    return !output->WriteMeasure( this );
}

void MusMeasure::AddLayer( MusLayer *layer )
{
	layer->SetParent( this );
	m_children.push_back( layer );
}

int MusMeasure::GetMeasureNo() const
{
    assert( m_parent ); // System cannot be NULL
    
    return m_parent->GetChildIndex( this );
}

MusLayer *MusMeasure::GetFirst( )
{
	if ( m_children.empty() )
		return NULL;
	return (MusLayer*)m_children[0];
}

MusLayer *MusMeasure::GetLast( )
{
	if ( m_children.empty() )
		return NULL;
	int i = GetLayerCount() - 1;
	return (MusLayer*)m_children[i];
}

MusLayer *MusMeasure::GetNext( MusLayer *layer )
{
    if ( !layer || m_children.empty())
        return NULL;
    
	int i = 0; GetChildIndex( layer );
    
	if ((i == -1 ) || ( i >= GetLayerCount() - 1 ))
		return NULL;
    
	return (MusLayer*)m_children[i + 1];
}

MusLayer *MusMeasure::GetPrevious( MusLayer *layer )
{
    if ( !layer || m_children.empty())
        return NULL;
    
	int i = GetChildIndex( layer );
    
	if ((i == -1 ) || ( i <= 0 ))
        return NULL;
	
    return (MusLayer*)m_children[i - 1];
}


MusLayer *MusMeasure::GetLayer( int LayerNo )
{
    if ( LayerNo > (int)m_children.size() - 1 )
        return NULL;
	
	return (MusLayer*)m_children[LayerNo];
}

