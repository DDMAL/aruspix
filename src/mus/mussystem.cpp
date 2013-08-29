/////////////////////////////////////////////////////////////////////////////
// Name:        mussystem.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musio.h"
#include "mussystem.h"
#include "musstaff.h"


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

	for (i = 0; i < this->GetStaffCount(); i++)
	{
        MusStaff *nstaff = new MusStaff( *(MusStaff*)&system.m_children[i] );
        this->AddStaff( nstaff );
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
	m_y_abs = 0;
	m_x_abs = 0;
}


bool MusSystem::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    return !output->WriteSystem( this );
}

void MusSystem::AddStaff( MusStaff *staff )
{
	staff->SetParent( this );
	m_children.Add( staff );
}

int MusSystem::GetSystemNo() const
{
    wxASSERT_MSG( m_parent, "Page cannot be NULL");
    
    return m_parent->m_children.Index( *this );
}

/*
void MusSystem::SetDoc( wxArrayPtrVoid params )
{
    wxLogDebug("PROUT");    
}
*/

/*
void MusSystem::ClearStaves( MusDC *dc, MusStaff *start )
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;

	int j;
	for(j = 0; j < nbrePortees; j++)
	{
		if (start && (start != &this->m_children[j]))
			continue;
		else
			start = NULL;
		(&this->m_children[j])->ClearElements( dc );
	}
}
*/

MusStaff *MusSystem::GetFirst( )
{
	if ( m_children.IsEmpty() )
		return NULL;
	return (MusStaff*)&m_children[0];
}

MusStaff *MusSystem::GetLast( )
{
	if ( m_children.IsEmpty() )
		return NULL;
	int i = (int)m_children.GetCount() - 1;
	return (MusStaff*)&m_children[i];
}

MusStaff *MusSystem::GetNext( MusStaff *staff )
{
    if ( !staff || m_children.IsEmpty())
        return NULL;
        
	int i = m_children.Index( *staff );

	if ((i == wxNOT_FOUND ) || ( i >= GetStaffCount() - 1 )) 
		return NULL;
	
	return (MusStaff*)&m_children[i + 1];
	
}

MusStaff *MusSystem::GetStaff( int StaffNo )
{
    if ( StaffNo > (int)m_children.GetCount() - 1 )
        return NULL;
	
	return (MusStaff*)&m_children[StaffNo];
}


MusStaff *MusSystem::GetPrevious( MusStaff *staff  )
{
    if ( !staff || m_children.IsEmpty())
        return NULL;
        
	int i = m_children.Index( *staff );

	if ((i == wxNOT_FOUND ) || ( i <= 0 ))
        return NULL;
	
    return (MusStaff*)&m_children[i - 1];
}


MusStaff *MusSystem::GetAtPos( int y )
{
	//y += ( STAFF_OFFSET / 2 );
	MusStaff *staff = this->GetFirst();
	if ( !staff )
		return NULL;
	
	//int dif =  abs( staff->m_y_drawing - y );
	while ( this->GetNext(staff) )
	{
		if ( (int)staff->m_y_drawing < y )
		{
			// one too much
			if ( this->GetPrevious( staff ) ){
				staff = this->GetPrevious( staff );
				//if ( dif < abs( staff->m_y_drawing - y ) )
				//	staff = this->GetNext( staff );
			}
				
			return staff;
		}
		staff = this->GetNext( staff );
		//dif = abs( staff->m_y_drawing - y );
	}

	if ( ( (int)staff->m_y_drawing < y )  && this->GetPrevious( staff ) )
		staff = this->GetPrevious( staff );

	return staff;
}

void MusSystem::SetValues( int type )
{
    /*
    int i;
    wxString values;
    for (i = 0; i < GetStaffCount(); i++) 
	{
        switch ( type ) {
            case PAGE_VALUES_VOICES: values += wxString::Format("%d;", (&m_children[i])->voix ); break;
            case PAGE_VALUES_INDENT: values += wxString::Format("%d;", (&m_children[i])->indent ); break;
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
            case PAGE_VALUES_VOICES: (&m_children[i])->voix = atoi( values_arr[i].c_str() ); break;
            case PAGE_VALUES_INDENT: (&m_children[i])->indent = atoi( values_arr[i].c_str() ); break;
        }	
	}
    */
    wxLogDebug("TODO");
    return;
}

