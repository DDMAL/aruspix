/////////////////////////////////////////////////////////////////////////////
// Name:        mussystem.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "system.h"

//----------------------------------------------------------------------------

#include <assert.h>

//----------------------------------------------------------------------------

#include "vrv.h"
#include "io.h"
#include "measure.h"
#include "page.h"

namespace vrv {

//----------------------------------------------------------------------------
// System
//----------------------------------------------------------------------------

System::System() :
	DocObject("system-")
{
	Clear( );
}

System::System( const System& system )
{
    int i;

	m_systemLeftMar = system.m_systemLeftMar;
	m_systemRightMar = system.m_systemRightMar;
	m_xAbs = system.m_xAbs;
	m_xRel = system.m_xRel;
	m_xDrawing = system.m_xDrawing;
	m_yAbs = system.m_yAbs;
	m_yRel = system.m_yRel;
	m_yDrawing = system.m_yDrawing;
    
	for (i = 0; i < this->GetMeasureCount(); i++)
	{
        Measure *nmeasure = new Measure( *(Measure*)system.m_children[i] );
        this->AddMeasure( nmeasure );
	}
}

System::~System()
{
}

void System::Clear( )
{
	ClearChildren();
	m_systemLeftMar = 50;
	m_systemRightMar = 50;
	m_xAbs = AX_UNSET;
    m_xRel = 0;
	m_xDrawing = 0;
	m_yAbs = AX_UNSET;
    m_yRel = 0;
	m_yDrawing = 0;
    m_totalDrawingWidth = 0;
}


int System::Save( ArrayPtrVoid params )
{
    // param 0: output stream
    FileOutputStream *output = (FileOutputStream*)params[0];       
    if (!output->WriteSystem( this )) {
        return FUNCTOR_STOP;
    }
    return FUNCTOR_CONTINUE;

}

void System::AddMeasure( Measure *measure )
{
	measure->SetParent( this );
	m_children.push_back( measure );
    Modify();
}

int System::GetVerticalSpacing()
{
    return 0; // arbitrary generic value
}

Measure *System::GetFirst( )
{
	if ( m_children.empty() )
		return NULL;
	return (Measure*)m_children[0];
}

Measure *System::GetLast( )
{
	if ( m_children.empty() )
		return NULL;
	int i = (int)m_children.size() - 1;
	return (Measure*)m_children[i];
}

Measure *System::GetNext( Measure *measure )
{
    if ( !measure || m_children.empty())
        return NULL;
        
	int i = GetChildIndex( measure );
    
	if ((i == -1 ) || ( i >= GetMeasureCount() - 1 ))
		return NULL;
	
	return (Measure*)m_children[i + 1];
	
}

Measure *System::GetPrevious( Measure *measure  )
{
    if ( !measure || m_children.empty() )
        return NULL;
        
	int i = GetChildIndex( measure );

	if ((i == -1 ) || ( i <= 0 ))
        return NULL;
	
    return (Measure*)m_children[i - 1];
}


Measure *System::GetAtPos( int x )
{
	//y += ( STAFF_OFFSET / 2 );
	Measure *measure = this->GetFirst();
	if ( !measure )
		return NULL;
	
    
    Measure *next = NULL;
	while ( (next = this->GetNext(measure) ) )
	{
		if ( (int)measure->m_xDrawing < x )
		{
			return measure;
		}
		measure = next;
	}

	return measure;
}

void System::SetValues( int type )
{
    /*
    int i;
    std::string values;
    for (i = 0; i < GetStaffCount(); i++) 
	{
        switch ( type ) {
            case PAGE_VALUES_VOICES: values += Vrv::StringFormat("%d;", (m_children[i])->voix ); break;
            case PAGE_VALUES_INDENT: values += Vrv::StringFormat("%d;", (m_children[i])->indent ); break;
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
    Vrv::LogDebug("TODO");
    return;
}

//----------------------------------------------------------------------------
// System functor methods
//----------------------------------------------------------------------------

int System::TrimSystem( ArrayPtrVoid params )
{
    if ( !m_parent || !dynamic_cast<Page*>(m_parent) ) {
        return FUNCTOR_CONTINUE;
    }
    Page *page = dynamic_cast<Page*>(m_parent);
    
    int system_length = (this->m_contentBB_x2 - this->m_contentBB_x1) + page->m_pageRightMar;
    if ( page->m_pageWidth < system_length ) {
        page->m_pageWidth = system_length;
    }
    return FUNCTOR_SIBLINGS;
}

int System::Align( ArrayPtrVoid params )
{
    // param 0: the measureAligner (unused)
    // param 1: the time (unused)
    // param 2: the systemAligner
    // param 3: the staffNb (unused)
    SystemAligner **systemAligner = (SystemAligner**)params[2];
    
    // When calculating the alignment, the position has to be 0
    m_xRel = 0;
    m_systemAligner.Reset();
    (*systemAligner) = &m_systemAligner;
    
    return FUNCTOR_CONTINUE;
}


int System::SetAligmentYPos( ArrayPtrVoid params )
{
    // param 0: the previous staff height
    // param 1: the staff margin (unused)
    // param 2: the staff interline sizes (int[2]) (unused)
    // param 2: the functor to be redirected to SystemAligner
    int *previousStaffHeight = (int*)params[0];
    MusFunctor *setAligmnentPosY = (MusFunctor*)params[3];
    
    (*previousStaffHeight) = 0;
    
    m_systemAligner.Process( setAligmnentPosY, params);
    
    return FUNCTOR_SIBLINGS;
}


int System::IntegrateBoundingBoxYShift( ArrayPtrVoid params )
{
    // param 0: the cumulated shift
    // param 1: the functor to be redirected to SystemAligner
    int *shift = (int*)params[0];
    MusFunctor *integrateBoundingBoxYShift = (MusFunctor*)params[1];
    
    m_xRel = this->m_systemLeftMar;
    (*shift) = 0;
    m_systemAligner.Process( integrateBoundingBoxYShift, params);
    
    return FUNCTOR_SIBLINGS;
}

int System::AlignMeasures( ArrayPtrVoid params )
{
    // param 0: the cumulated shift
    int *shift = (int*)params[0];
    
    (*shift) = 0;
    
    return FUNCTOR_CONTINUE;
}

int System::AlignMeasuresEnd( ArrayPtrVoid params )
{
    // param 0: the cumulated shift
    int *shift = (int*)params[0];
    
    m_totalDrawingWidth = (*shift);
    
    return FUNCTOR_CONTINUE;
}

int System::AlignSystems( ArrayPtrVoid params )
{
    // param 0: the cumulated shift
    // param 1: the system margin
    int *shift = (int*)params[0];
    int *systemMargin = (int*)params[1];
    
    this->m_yRel = (*shift);
    
    assert( m_systemAligner.GetBottomAlignment() );
    
    (*shift) += m_systemAligner.GetBottomAlignment()->GetYRel() - (*systemMargin);
    
    return FUNCTOR_SIBLINGS;
}


int System::JustifyX( ArrayPtrVoid params )
{
    // param 0: the justification ratio (unused)
    // param 1: the system full width (without system margins)
    // param 2: the functor to be redirected to the MeasureAligner (unused)
    double *ratio = (double*)params[0];
    int *systemFullWidth = (int*)params[1];
    
    (*ratio) = (double)((*systemFullWidth) - this->m_systemLeftMar - this->m_systemRightMar) / (double)m_totalDrawingWidth;
    
    if ((*ratio) < 0.8 ) {
        // Arbitrary value for avoiding over-compressed justification
        Vrv::LogWarning("Justification stop because of a ratio smaller the 0.8");
        return FUNCTOR_SIBLINGS;
    }
    
    return FUNCTOR_CONTINUE;
}

} // namespace vrv
