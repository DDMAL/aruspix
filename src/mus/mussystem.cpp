/////////////////////////////////////////////////////////////////////////////
// Name:        mussystem.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "mussystem.h"

//----------------------------------------------------------------------------

#include <assert.h>

//----------------------------------------------------------------------------

#include "mus.h"
#include "musio.h"
#include "musmeasure.h"
#include "muspage.h"

//----------------------------------------------------------------------------
// MusSystem
//----------------------------------------------------------------------------

MusSystem::MusSystem() :
	MusDocObject()
{
	Clear( );
}

MusSystem::MusSystem( const MusSystem& system )
{
    int i;

	m_systemLeftMar = system.m_systemLeftMar;
	m_systemRightMar = system.m_systemRightMar;
	m_x_abs = system.m_x_abs;
	m_y_abs = system.m_y_abs;
	m_y_rel = system.m_y_rel;
	m_y_drawing = system.m_y_drawing;
    
	for (i = 0; i < this->GetMeasureCount(); i++)
	{
        MusMeasure *nmeasure = new MusMeasure( *(MusMeasure*)system.m_children[i] );
        this->AddMeasure( nmeasure );
	}
}

MusSystem::~MusSystem()
{
}

void MusSystem::Clear( )
{
	ClearChildren();
	m_systemLeftMar = 0;
	m_systemRightMar = 0;
	m_x_abs = AX_UNSET;
	m_y_abs = AX_UNSET;
    m_y_rel = 0;
	m_y_drawing = 0;
}


int MusSystem::Save( ArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    if (!output->WriteSystem( this )) {
        return FUNCTOR_STOP;
    }
    return FUNCTOR_CONTINUE;

}

void MusSystem::AddMeasure( MusMeasure *measure )
{
	measure->SetParent( this );
	m_children.push_back( measure );
    Modify();
}

int MusSystem::GetSystemNo() const
{
    assert( m_parent ); // Page cannot be NULL
    
    return m_parent->GetChildIndex( this );
}

int MusSystem::GetVerticalSpacing()
{
    return 100; // arbitrary generic value
}

/*
void MusSystem::SetDoc( ArrayPtrVoid params )
{
    Mus::LogDebug("PROUT");    
}
*/

/*
void MusSystem::ClearStaves( MusDC *dc, MusStaff *start )
{
	assert( dc ); // DC cannot be NULL
	if ( !Check() )
		return;

	int j;
	for(j = 0; j < nbrePortees; j++)
	{
		if (start && (start != this->m_children[j]))
			continue;
		else
			start = NULL;
		(this->m_children[j])->ClearElements( dc );
	}
}
*/

MusMeasure *MusSystem::GetFirst( )
{
	if ( m_children.empty() )
		return NULL;
	return (MusMeasure*)m_children[0];
}

MusMeasure *MusSystem::GetLast( )
{
	if ( m_children.empty() )
		return NULL;
	int i = (int)m_children.size() - 1;
	return (MusMeasure*)m_children[i];
}

MusMeasure *MusSystem::GetNext( MusMeasure *measure )
{
    if ( !measure || m_children.empty())
        return NULL;
        
	int i = GetChildIndex( measure );
    
	if ((i == -1 ) || ( i >= GetMeasureCount() - 1 ))
		return NULL;
	
	return (MusMeasure*)m_children[i + 1];
	
}

MusMeasure *MusSystem::GetMeasure( int MeasureNo )
{
    if ( MeasureNo > (int)m_children.size() - 1 )
        return NULL;
	
	return (MusMeasure*)m_children[MeasureNo];
}


MusMeasure *MusSystem::GetPrevious( MusMeasure *measure  )
{
    if ( !measure || m_children.empty() )
        return NULL;
        
	int i = GetChildIndex( measure );

	if ((i == -1 ) || ( i <= 0 ))
        return NULL;
	
    return (MusMeasure*)m_children[i - 1];
}


MusMeasure *MusSystem::GetAtPos( int x )
{
	//y += ( STAFF_OFFSET / 2 );
	MusMeasure *measure = this->GetFirst();
	if ( !measure )
		return NULL;
	
    
    MusMeasure *next = NULL;
	while ( (next = this->GetNext(measure) ) )
	{
		if ( (int)measure->m_x_drawing < x )
		{
			return measure;
		}
		measure = next;
	}

	return measure;
}

void MusSystem::SetValues( int type )
{
    /*
    int i;
    std::string values;
    for (i = 0; i < GetStaffCount(); i++) 
	{
        switch ( type ) {
            case PAGE_VALUES_VOICES: values += Mus::StringFormat("%d;", (m_children[i])->voix ); break;
            case PAGE_VALUES_INDENT: values += Mus::StringFormat("%d;", (m_children[i])->indent ); break;
        }
	}
    values = wxGetTextFromUser( "Enter values for the pages", "", values );
    if (values.Length() == 0 ) {
        return;
    }
    wxArrayString values_arr = wxStringTokenize(values, ";");
    for (i = 0; (i < GetStaffCount()) && (i < (int)values_arr.GetCount()) ; i++) 
	{
        switch ( type ) {
            case PAGE_VALUES_VOICES: (m_children[i])->voix = atoi( values_arr[i].c_str() ); break;
            case PAGE_VALUES_INDENT: (m_children[i])->indent = atoi( values_arr[i].c_str() ); break;
        }	
	}
    */
    Mus::LogDebug("TODO");
    return;
}

//----------------------------------------------------------------------------
// MusSystem functor methods
//----------------------------------------------------------------------------

int MusSystem::TrimSystem( ArrayPtrVoid params )
{
    if ( !m_parent || !dynamic_cast<MusPage*>(m_parent) ) {
        return FUNCTOR_CONTINUE;
    }
    MusPage *page = dynamic_cast<MusPage*>(m_parent);
    
    int system_length = (this->m_contentBB_x2 - this->m_contentBB_x1) + page->m_pageRightMar;
    if ( page->m_pageWidth < system_length ) {
        page->m_pageWidth = system_length;
    }
    return FUNCTOR_SIBLINGS;
}

int MusSystem::Align( ArrayPtrVoid params )
{
    // param 0: the aligner
    // param 1: the measureAligner
    // param 2: the measureNb
    // param 3: the time
    MusAligner **aligner = (MusAligner**)params[0];
    
    m_aligner.ClearChildren();
    (*aligner) = &m_aligner;
    
    return FUNCTOR_CONTINUE;
}


int MusSystem::IntegrateBoundingBoxShift( ArrayPtrVoid params )
{
    // param 0: the cumulated shift
    // param 1: the functor to be redirected to MusAligner
    MusFunctor *integrateBoundingBoxShift = (MusFunctor*)params[1];
    
    m_aligner.Process( integrateBoundingBoxShift, params);
    
    return FUNCTOR_SIBLINGS;
}

int MusSystem::SetAligmentXPos( ArrayPtrVoid params )
{
    // param 0: the previous time position (unused)
    // param 1: the previous x rel position (unused)
    // param 2: the functor to be redirected to MusAligner
    MusFunctor *setAligmnentPosX = (MusFunctor*)params[2];

    m_aligner.Process( setAligmnentPosX, params);
    
    return FUNCTOR_SIBLINGS;
}


