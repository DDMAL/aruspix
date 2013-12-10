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
	MusDocObject("page-")
{
	Clear( );
}

MusPage::~MusPage()
{
}

void MusPage::Clear( )
{
	ClearChildren( );
    m_drawing_scoreDef.Clear();
	defin = 18;
    // by default we have no values and use the document ones
    m_pageHeight = -1;
    m_pageWidth = -1;
    m_pageLeftMar = 0;
    m_pageRightMar = 0;
    m_pageTopMar = 0;
    this->ResetUuid();
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
    
    // Align the content of the page using measure and system aligners
    // After this:
    // - each MusLayerElement object will have its MusAlignment pointer initialized
    // - each MusStaff object will then have its MusStaffAlignment pointer initialized
    MusMeasureAligner *measureAlignerPtr = NULL;
    double time = 0.0;
    MusSystemAligner *systemAlignerPtr = NULL;
    int staffNb = 0;
    params.push_back( &measureAlignerPtr );
    params.push_back( &time );
    params.push_back( &systemAlignerPtr );
    params.push_back( &staffNb );
    MusFunctor align( &MusObject::Align );
    this->Process( &align, params );
    
    // Set the X position of each MusAlignment
    // Does a duration-based non linear spacing looking at the duration space between two MusAlignment objects
    params.clear();
    double previousTime = 0.0;
    int previousXRel = 0;
    params.push_back( &previousTime );
    params.push_back( &previousXRel );
    MusFunctor setAlignmentX( &MusObject::SetAligmentXPos );
    // Special case: because we redirect the functor, pass is a parameter to itself (!)
    params.push_back( &setAlignmentX );
    this->Process( &setAlignmentX, params );
    
    // Render it for filling the bounding boxing
    MusRC rc;
    MusBBoxDC bb_dc( &rc, 0, 0 );
    rc.SetDoc(doc);
    rc.DrawPage(  &bb_dc, this, false );
    
    // Adjust the X shift of the MusAlignment looking at the bounding boxes
    // Look at each MusLayerElement and changes the m_x_shift if the bouding box is overlapping
    params.clear();
    int previous_width = 0;
    params.push_back( &previous_width );
    MusFunctor setBoundingBoxXShift( &MusObject::SetBoundingBoxXShift );
    this->Process( &setBoundingBoxXShift, params );
    
    // Integrate the X bounding box shift of the elements
    // Once the m_x_shift have been calculated, move all positions accordingly
    params.clear();
    int shift = 0;
    int width = 0;
    params.push_back( &shift );
    params.push_back( &width );
    MusFunctor integrateBoundingBoxXShift( &MusObject::IntegrateBoundingBoxXShift );
    // special case: because we redirect the functor, pass is a parameter to itself (!)
    params.push_back( &integrateBoundingBoxXShift );
    this->Process( &integrateBoundingBoxXShift, params );
    
    // Adjust the Y shift of the MusStaffAlignment looking at the bounding boxes
    // Look at each MusStaff and changes the m_y_shift if the bounding box is overlapping 
    params.clear();
    int previous_height = 0;
    params.push_back( &previous_height );
    MusFunctor setBoundingBoxYShift( &MusObject::SetBoundingBoxYShift );
    this->Process( &setBoundingBoxYShift, params );
    
    // Set the Y position of each MusStaffAlignment
    // Adjusts the Y shift for making sure there is a minimal space (staffMargin) between each staff
    params.clear();
    int previousStaffHeight = 0; // 0 for the first staff, reset for each system (see MusSystem::SetAlignmentYPos)
    int staffMargin = doc->m_staffSize[0]; // the minimal space we want to have between each staff
    int* interlineSizes = doc->m_interl; // the interline sizes to be used for calculating the (previous) staff height
    params.push_back( &previousStaffHeight );
    params.push_back( &staffMargin );
    params.push_back( &interlineSizes );
    MusFunctor setAlignmentY( &MusObject::SetAligmentYPos );
    // special case: because we redirect the functor, pass is a parameter to itself (!)
    params.push_back( &setAlignmentY );
    this->Process( &setAlignmentY, params );
    
    // Integrate the Y shift of the staves
    // Once the m_y_shift have been calculated, move all positions accordingly
    params.clear();
    shift = 0;
    params.push_back( &shift );
    MusFunctor integrateBoundingBoxYShift( &MusObject::IntegrateBoundingBoxYShift );
    // special case: because we redirect the functor, pass is a parameter to itself (!)
    params.push_back( &integrateBoundingBoxYShift );
    this->Process( &integrateBoundingBoxYShift, params );
    
    // Adjust measure X position
    params.clear();
    shift = 0;
    params.push_back( &shift );
    MusFunctor alignMeasures( &MusObject::AlignMeasures );
    this->Process( &alignMeasures, params );
    
    // Adjust system Y position
    params.clear();
    shift = doc->m_pageHeight - doc->m_pageTopMar;
    int systemMargin = doc->m_staffSize[0];
    params.push_back( &shift );
    params.push_back( &systemMargin );
    MusFunctor alignSystems( &MusObject::AlignSystems );
    this->Process( &alignSystems, params );
    
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
