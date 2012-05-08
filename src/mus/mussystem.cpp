/////////////////////////////////////////////////////////////////////////////
// Name:        mussystem.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "mussystem.h"
#include "muslaidoutstaff.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusSystems );

//----------------------------------------------------------------------------
// MusSystem
//----------------------------------------------------------------------------

MusSystem::MusSystem() :
	MusLayoutObject()
{
	Clear( );
}

MusSystem::MusSystem( const MusSystem& system )
{
    int i;

	indent = system.indent;
	indentDroite = system.indentDroite;
	lrg_lign = system.lrg_lign;
	m_yrel = system.m_xrel;
	m_xrel = system.m_yrel;

	for (i = 0; i < (int)system.m_staves.GetCount(); i++)
	{
        MusLaidOutStaff *nstaff = new MusLaidOutStaff( *&system.m_staves[i] );
        nstaff->SetSystem( this );
        this->AddStaff( nstaff );
	}
}

MusSystem::~MusSystem()
{
}

void MusSystem::Clear( )
{
	m_staves.Clear( );
    m_page = NULL;
	indent = 0;
	indentDroite = 0;
	lrg_lign = 190;
	m_yrel = 0;
	m_xrel = 0;
}



void MusSystem::AddStaff( MusLaidOutStaff *staff )
{
	staff->SetSystem( this );
	m_staves.Add( staff );
}

void MusSystem::CheckIntegrity()
{
	wxASSERT_MSG( m_page, "MusPage parent cannot be NULL");
	
	MusLaidOutStaff *staff;
	int i;
    for (i = 0; i < this->GetStaffCount(); i++) 
	{
		staff = &m_staves[i];
        staff->CheckIntegrity();
	}
}

int MusSystem::GetSystemNo() const
{
    wxASSERT_MSG( m_page, "Page cannot be NULL");
    
    return m_page->m_systems.Index( *this );
}

/*
void MusSystem::ClearStaves( MusDC *dc, MusLaidOutStaff *start )
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;

	int j;
	for(j = 0; j < nbrePortees; j++)
	{
		if (start && (start != &this->m_staves[j]))
			continue;
		else
			start = NULL;
		(&this->m_staves[j])->ClearElements( dc );
	}
}
*/

MusLaidOutStaff *MusSystem::GetFirst( )
{
	if ( m_staves.IsEmpty() )
		return NULL;
	return &m_staves[0];
}

MusLaidOutStaff *MusSystem::GetLast( )
{
	if ( m_staves.IsEmpty() )
		return NULL;
	int i = (int)m_staves.GetCount() - 1;
	return &m_staves[i];
}

MusLaidOutStaff *MusSystem::GetNext( MusLaidOutStaff *staff )
{
    if ( !staff || m_staves.IsEmpty())
        return NULL;
        
	int i = m_staves.Index( *staff );

	if ((i == wxNOT_FOUND ) || ( i >= GetStaffCount() - 1 )) 
		return NULL;
	
	return &m_staves[i + 1];
	
}

MusLaidOutStaff *MusSystem::GetPrevious( MusLaidOutStaff *staff  )
{
    if ( !staff || m_staves.IsEmpty())
        return NULL;
        
	int i = m_staves.Index( *staff );

	if ((i == wxNOT_FOUND ) || ( i <= 0 ))
        return NULL;
	
    return &m_staves[i - 1];
}


MusLaidOutStaff *MusSystem::GetAtPos( int y )
{
	y += ( STAFF_OFFSET / 2 );
	MusLaidOutStaff *staff = this->GetFirst();
	if ( !staff )
		return NULL;
	
	int dif =  abs( staff->yrel - y );
	while ( this->GetNext(staff) )
	{
		if ( (int)staff->yrel < y )
		{
			// one too much
			if ( this->GetPrevious( staff ) ){
				staff = this->GetPrevious( staff );
				if ( dif < abs( staff->yrel - y ) )
					staff = this->GetNext( staff );
			}
				
			return staff;
		}
		staff = this->GetNext( staff );
		dif = abs( staff->yrel - y );
	}

	if ( ( (int)staff->yrel < y )  && this->GetPrevious( staff ) )
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
            case PAGE_VALUES_VOICES: values += wxString::Format("%d;", (&m_staves[i])->voix ); break;
            case PAGE_VALUES_INDENT: values += wxString::Format("%d;", (&m_staves[i])->indent ); break;
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
            case PAGE_VALUES_VOICES: (&m_staves[i])->voix = atoi( values_arr[i].c_str() ); break;
            case PAGE_VALUES_INDENT: (&m_staves[i])->indent = atoi( values_arr[i].c_str() ); break;
        }	
	}
    */
    wxLogDebug("TODO");
    return;
}

// functors for MusSystem

void MusSystem::Process(MusLayoutFunctor *functor, wxArrayPtrVoid params )
{
    MusLaidOutStaffFunctor *staffFunctor = dynamic_cast<MusLaidOutStaffFunctor*>(functor);
    MusLaidOutStaff *staff;
	int i;
    for (i = 0; i < (int)m_staves.GetCount(); i++) 
	{
        staff = &m_staves[i];
        if (staffFunctor) { // is is a MusLaidOutStaffFunctor, call it
            staffFunctor->Call( staff, params );
        }
        else { // process it further
            staff->Process( functor, params );
        }
	}
}

/*
void MusSystem::CountVoices( wxArrayPtrVoid params )
{
    // param 0; int (min number of voice number)
    // param 1; int (max number of voice number)
    
    int *min_voice = (int*)params[0];
    int *max_voice = (int*)params[1];
    
	int i;
    MusLaidOutStaff *staff;

    for (i = 0; i < GetStaffCount(); i++) 
	{
		staff = &m_staves[i];
        if (staff->voix > (*max_voice)) {
           (*max_voice) = staff->voix;
        }
        if (staff->voix < (*min_voice)) {
           (*min_voice) = staff->voix;
        }
	}

}
*/
