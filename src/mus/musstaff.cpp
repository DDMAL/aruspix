/////////////////////////////////////////////////////////////////////////////
// Name:        musstaff.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "musstaff.h"

//----------------------------------------------------------------------------

#include <assert.h>

//----------------------------------------------------------------------------

#include "musdef.h"
#include "musio.h"
#include "musmeasure.h"

//----------------------------------------------------------------------------
// MusStaff
//----------------------------------------------------------------------------

MusStaff::MusStaff( int logStaffNb ):
	MusDocObject()
{
	Clear( );
    //wxASSERT ( logStaffNb > 0 );
    m_logStaffNb = logStaffNb;
}

MusStaff::MusStaff( const MusStaff& staff )
{
    m_parent = NULL;
	totGrp = staff.totGrp;
	//noLigne = staff.noLigne;
	armTyp = staff.armTyp;
	armNbr = staff.armNbr;
	notAnc = staff.notAnc;
	grise = staff.grise;
	invisible = staff.invisible;
	vertBarre = staff.vertBarre;
	brace = staff.brace;
	staffSize = staff.staffSize;
	portNbLine = staff.portNbLine;
	accol = staff.accol;
	accessoire = staff.accessoire;
	m_y_abs = staff.m_y_abs;
    m_y_rel = staff.m_y_rel;
	m_y_drawing = staff.m_y_drawing;

    int i;
	for (i = 0; i < staff.GetMeasureCount(); i++)
	{
        MusMeasure *nmeasure = new MusMeasure( *(MusMeasure*)staff.m_children[i] );
        this->AddMeasure( nmeasure );
	}
}

MusStaff::~MusStaff()
{
    
}

void MusStaff::Clear()
{
	ClearChildren();
    m_parent = NULL;
	noGrp = 0;
	totGrp = 0;
	//noLigne = 0; // ax2
	armTyp = 0;
	armNbr = 0;
	notAnc = false; // LP we want modern notation :))
	grise = false;
	invisible = false;
	vertBarre = 0;
	brace = 0;
	staffSize = 0; 
	portNbLine = 5;
	accol = 0;
	accessoire = 0;
	m_y_abs = AX_UNSET;
    m_y_rel = 0;
	m_y_drawing = 0;
    
    //
    //beamListPremier = NULL;
}

bool MusStaff::Save( ArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];         
    return !output->WriteStaff( this );
}

void MusStaff::AddMeasure( MusMeasure *measure )
{
	measure->SetParent( this );
	m_children.push_back( measure );
}

void MusStaff::CopyAttributes( MusStaff *nstaff )
{
	if ( !nstaff )
		return;

	nstaff->Clear();
	nstaff->noGrp = noGrp;
	nstaff->totGrp = totGrp;
	//nstaff->noLigne = noLigne;
	nstaff->armTyp = armTyp;
	nstaff->armNbr = armNbr;
	nstaff->notAnc = notAnc;
	nstaff->grise = grise;
	nstaff->invisible = invisible;
	nstaff->vertBarre = vertBarre;
	nstaff->brace = brace;
	nstaff->staffSize = staffSize;
	nstaff->portNbLine = portNbLine;
	nstaff->accol = accol;
	nstaff->accessoire = accessoire;
	nstaff->m_y_abs = m_y_abs;
    nstaff->m_y_rel = m_y_rel;
	nstaff->m_y_drawing = m_y_drawing;
}

int MusStaff::GetStaffNo() const
{
    assert( m_parent ); // System cannot be NULL
    
    return m_parent->GetChildIndex( this );
}

MusMeasure *MusStaff::GetFirst( )
{
	if ( m_children.empty() )
		return NULL;
	return (MusMeasure*)m_children[0];
}

MusMeasure *MusStaff::GetLast( )
{
	if ( m_children.empty() )
		return NULL;
	int i = GetMeasureCount() - 1;
	return (MusMeasure*)m_children[i];
}

MusMeasure *MusStaff::GetNext( MusMeasure *measure )
{	
    if ( !measure || m_children.empty())
        return NULL;
        
	int i = GetChildIndex( measure );

	if ((i == -1 ) || ( i >= GetMeasureCount() - 1 ))
		return NULL;

	return (MusMeasure*)m_children[i + 1];
}

MusMeasure *MusStaff::GetPrevious( MusMeasure *measure )
{
    if ( !measure || m_children.empty())
        return NULL;
        
	int i = GetChildIndex( measure );

	if ((i == -1 ) || ( i <= 0 ))
        return NULL;
	
    return (MusMeasure*)m_children[i - 1];
}


MusMeasure *MusStaff::GetMeasure( int MeasureNo )
{
    if ( MeasureNo > (int)m_children.size() - 1 )
        return NULL;
	
	return (MusMeasure*)m_children[MeasureNo];
}


int MusStaff::GetVerticalSpacing()
{
    return 30; // arbitrary generic value
}

bool MusStaff::GetPosOnPage( ArrayPtrVoid params )
{
    // param 0: the MusStaff we are looking for
    // param 1: the position on the page (int)
    // param 2; the success flag (bool)
    MusStaff *staff = (MusStaff*)params[0];
	int *position = (int*)params[1];
    bool *success = (bool*)params[2];
    
    if ( (*success) ) {
        return true;
    } 
    (*position)++;
    if ( this == staff ) {
        (*success) = true;
        return true;
    }
    // to be verified
    return false;
}

