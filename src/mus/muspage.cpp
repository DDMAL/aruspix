/////////////////////////////////////////////////////////////////////////////
// Name:        muspage.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "muspage.h"

//----------------------------------------------------------------------------

#include <assert.h>

//----------------------------------------------------------------------------

#include "musaligner.h"
#include "musbboxdc.h"
#include "musdef.h"
#include "musdoc.h"
#include "musrc.h"
#include "musstaff.h"
#include "mussystem.h"

//----------------------------------------------------------------------------
// MusPage
//----------------------------------------------------------------------------

MusPage::MusPage() :
	MusDocObject()
{
	Clear( );
}

MusPage::~MusPage()
{
}

void MusPage::Clear( )
{
	ClearChildren( );
	defin = 20;
    // by default we have no values and use the document ones
    m_pageHeight = -1;
    m_pageWidth = -1;
    m_pageLeftMar = 0;
    m_pageRightMar = 0;
    m_pageTopMar = 0;
}


int MusPage::Save( ArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    if (!output->WritePage( this )) {
        return FUNCTOR_STOP;
    }
    return FUNCTOR_CONTINUE;
}


void MusPage::AddSystem( MusSystem *system )
{
	system->SetParent( this );
	m_children.push_back( system );
    Modify();
}

int MusPage::GetPageNo() const
{
    assert( m_parent ); // Doc cannot be NULL
    
    return m_parent->GetChildIndex( this );
}

int MusPage::GetStaffPosOnPage( MusStaff *staff )
{
    /*
    int position = -1;
    bool success = false;
    ArrayPtrVoid params;
    params.Add( staff );
    params.Add( &position );
    params.Add( &success );
    MusStaffFunctor getStaffPosOnPage( &MusStaff::GetPosOnPage );
    Process( &getStaffPosOnPage, params );    
    return position;
    */ // ax2.3
    return 0;
}


MusSystem *MusPage::GetFirst( )
{
	if ( m_children.empty() )
		return NULL;
	return (MusSystem*)m_children[0];
}

MusSystem *MusPage::GetLast( )
{
	if ( m_children.empty() )
		return NULL;
	int i = GetSystemCount() - 1;
	return (MusSystem*)m_children[i];
}

MusSystem *MusPage::GetNext( MusSystem *system )
{
    if ( !system || m_children.empty())
        return NULL;
        
	int i = this->GetChildIndex( system );

	if ((i == -1 ) || ( i >= GetSystemCount() - 1 ))
		return NULL;
	
	return (MusSystem*)m_children[i + 1];
}

MusSystem *MusPage::GetPrevious( MusSystem *system  )
{
    if ( !system || m_children.empty())
        return NULL;
        
	int i = GetChildIndex( system );

	if ((i == -1 ) || ( i <= 0 ))
        return NULL;
	
    return (MusSystem*)m_children[i - 1];
}


MusSystem *MusPage::GetAtPos( int y )
{

	y -= ( SYSTEM_OFFSET / 2 );
    MusSystem *system = this->GetFirst();
	if ( !system )
		return NULL;
	
    MusSystem *next = NULL;
	while ( (next = this->GetNext(system)) )
	{
		if ( (int)next->m_y_abs < y )
		{
			return system;
		}
		system = next;
	}

	return system;
}

void MusPage::Layout( bool trim )
{
    if (!dynamic_cast<MusDoc*>(m_parent)) {
        assert( false );
        return;
    }
    MusDoc *doc = dynamic_cast<MusDoc*>(m_parent);
       
    ArrayPtrVoid params;
    
    // align the content of the page
    MusAligner *alignerPtr = NULL;
    MusMeasureAligner *measureAlignerPtr = NULL;
    int measureNb = 0;
    double time = 0.0;
    params.push_back( &alignerPtr );
    params.push_back( &measureAlignerPtr );
    params.push_back( &measureNb );
    params.push_back( &time );
    MusFunctor align( &MusObject::Align );
    this->Process( &align, params );
    
    // set the x position of each MusAlignment
    params.clear();
    double previousTime = 0.0;
    int previousXRel = 0;
    params.push_back( &previousTime );
    params.push_back( &previousXRel );
    MusFunctor setAlignment( &MusObject::SetAligmentXPos );
    // special case: because we redirect the functor, pass is a parameter to itself (!)
    params.push_back( &setAlignment );
    this->Process( &setAlignment, params );
    
    // render it for filling the bounding boxing
    MusRC rc;
    MusBBoxDC bb_dc( &rc, 0, 0 );
    rc.SetDoc(doc);
    rc.DrawPage(  &bb_dc, this, false );
    
    // set the X shift of the MusAlignment using the bounding boxes
    params.clear();
    int measure_shift = 0;
    int element_shift = 0;
    int previous_width = 0;
    params.push_back( &measure_shift );
    params.push_back( &element_shift );
    params.push_back( &previous_width );
    MusFunctor setBoundingBoxShift( &MusLayerElement::SetBoundingBoxShift );
    this->Process( &setBoundingBoxShift, params );
    
    // integrate the bounding box shift
    params.clear();
    int shift = 0;
    params.push_back( &shift );
    MusFunctor integrateBoundingBoxShift( &MusObject::IntegrateBoundingBoxShift );
    // special case: because we redirect the functor, pass is a parameter to itself (!)
    params.push_back( &integrateBoundingBoxShift );
    this->Process( &integrateBoundingBoxShift, params );
    
    params.clear();
    int system_shift = doc->m_pageHeight - this->m_pageTopMar;
    int staff_shift = 0;
    params.push_back( &system_shift );
    params.push_back( &staff_shift );
    MusFunctor updateYPosition( &MusObject::LayOutSystemAndStaffYPos );
    this->Process( &updateYPosition, params );
    
    if ( trim ) {    
        // Trim the page to the needed position
        // LP I am not sure about this. m_pageHeight / Width should not be modified
        this->m_pageWidth = 0; // first resest the page to 0
        this->m_pageHeight = doc->m_pageHeight;
        params.clear();
        
        MusFunctor trimSystem(&MusObject::TrimSystem);
        this->Process( &trimSystem, params );
    }
    
    rc.DrawPage(  &bb_dc, this , false );
}

void MusPage::SetValues( int type )
{
/* 
    int i;
    std::string values;
    for (i = 0; i < nbrePortees; i++) 
	{
        switch ( type ) {
            case PAGE_VALUES_VOICES: values += Mus::StringFormat("%d;", (&m_staves[i])->voix ); break;
            case PAGE_VALUES_INDENT: values += Mus::StringFormat("%d;", (&m_staves[i])->indent ); break;
        }
	}
    values = wxGetTextFromUser( "Enter values for the pages", "", values );
    if (values.Length() == 0 ) {
        return;
    }
    wxArrayString values_arr = wxStringTokenize(values, ";");
    for (i = 0; (i < nbrePortees) && (i < (int)values_arr.GetCount()) ; i++) 
	{
        switch ( type ) {
            case PAGE_VALUES_VOICES: (&m_staves[i])->voix = atoi( values_arr[i].c_str() ); break;
            case PAGE_VALUES_INDENT: (&m_staves[i])->indent = atoi( values_arr[i].c_str() ); break;
        }	
	}
*/
    Mus::LogDebug("TODO");
    return;
}
