/////////////////////////////////////////////////////////////////////////////
// Name:        musstaff.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musio.h"
#include "musstaff.h"
#include "muslayer.h"

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
	m_y_drawing = staff.m_y_drawing;

    int i;
	for (i = 0; i < staff.GetLayerCount(); i++)
	{
        MusLayer *nlayer = new MusLayer( *(MusLayer*)&staff.m_children[i] );
        this->AddLayer( nlayer );
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
	m_y_abs = 0;
	m_y_drawing = 0;
    
    //
    //beamListPremier = NULL;
}

bool MusStaff::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];         
    return !output->WriteStaff( this );
}

void MusStaff::AddLayer( MusLayer *layer )
{
	layer->SetParent( this );
	m_children.Add( layer );
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
	nstaff->m_y_drawing = m_y_drawing;
}

int MusStaff::GetStaffNo() const
{
    wxASSERT_MSG( m_parent, "System cannot be NULL");
    
    return m_parent->m_children.Index( *this );
}

MusLayer *MusStaff::GetFirst( )
{
	if ( m_children.IsEmpty() )
		return NULL;
	return (MusLayer*)&m_children[0];
}

MusLayer *MusStaff::GetLast( )
{
	if ( m_children.IsEmpty() )
		return NULL;
	int i = GetLayerCount() - 1;
	return (MusLayer*)&m_children[i];
}

MusLayer *MusStaff::GetNext( MusLayer *layer )
{	
    if ( !layer || m_children.IsEmpty())
        return NULL;
        
	int i = m_children.Index( *layer );

	if ((i == wxNOT_FOUND ) || ( i >= GetLayerCount() - 1 )) 
		return NULL;

	return (MusLayer*)&m_children[i + 1];
}

MusLayer *MusStaff::GetPrevious( MusLayer *layer )
{
    if ( !layer || m_children.IsEmpty())
        return NULL;
        
	int i = m_children.Index( *layer );

	if ((i == wxNOT_FOUND ) || ( i <= 0 )) 
        return NULL;
	
    return (MusLayer*)&m_children[i - 1];
}


MusLayer *MusStaff::GetLayer( int LayerNo )
{
    if ( LayerNo > (int)m_children.GetCount() - 1 )
        return NULL;
	
	return (MusLayer*)&m_children[LayerNo];
}

bool MusStaff::GetPosOnPage( wxArrayPtrVoid params )
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

bool MusStaff::UpdateYPosition( wxArrayPtrVoid params )
{
    // param 0: the MusLayerElement we point to
	int *current_y_shift = (int*)params[0];  
    
    int negative_offset = this->m_y_abs - this->m_contentBB_y2;
    this->m_y_abs = (*current_y_shift) + negative_offset;
    (*current_y_shift) -= (this->m_contentBB_y2 - this->m_contentBB_y1);
    return true;
}

