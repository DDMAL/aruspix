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
	m_x_abs = system.m_x_abs;
	m_y_abs = system.m_y_abs;

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

bool MusSystem::Check()
{
    wxASSERT( m_page );
    return (m_page && MusLayoutObject::Check());
}

void MusSystem::Clear( )
{
	m_staves.Clear( );
    m_page = NULL;
	indent = 0;
	indentDroite = 0;
	lrg_lign = 190;
	m_y_abs = 0;
	m_x_abs = 0;
}


void MusSystem::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    output->WriteSystem( this );
    
    // save staves
    MusLaidOutStaffFunctor staff( &MusLaidOutStaff::Save );
    this->Process( &staff, params );
}

void MusSystem::Load( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileInputStream *input = (MusFileInputStream*)params[0];       
    
    // load staves
    MusLaidOutStaff *staff;
    while ( (staff = input->ReadLaidOutStaff()) ) {
        staff->Load( params );
        this->AddStaff( staff );
    }
}


void MusSystem::AddStaff( MusLaidOutStaff *staff )
{
	staff->SetSystem( this );
	m_staves.Add( staff );
}

int MusSystem::GetSystemNo() const
{
    wxASSERT_MSG( m_page, "Page cannot be NULL");
    
    return m_page->m_systems.Index( *this );
}

/*
void MusSystem::SetDoc( wxArrayPtrVoid params )
{
    wxLogDebug("PROUT");    
}
*/

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

MusLaidOutStaff *MusSystem::GetStaff( int StaffNo )
{
    if ( StaffNo > (int)m_staves.GetCount() - 1 )
        return NULL;
	
	return &m_staves[StaffNo];
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
	
	int dif =  abs( staff->m_y_drawing - y );
	while ( this->GetNext(staff) )
	{
		if ( (int)staff->m_y_drawing < y )
		{
			// one too much
			if ( this->GetPrevious( staff ) ){
				staff = this->GetPrevious( staff );
				if ( dif < abs( staff->m_y_drawing - y ) )
					staff = this->GetNext( staff );
			}
				
			return staff;
		}
		staff = this->GetNext( staff );
		dif = abs( staff->m_y_drawing - y );
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


int MusSystem::UpdateStaffPositions( int last_staff ) 
{
    /*
	if ( !this->Check() )
		return this->m_y_abs;
    
	int i, yy;
	MusLaidOutStaff *staff = NULL;
    
	yy = last_staff;
    for (i = 0; i < this->GetStaffCount(); i++) 
	{
		staff = &this->m_staves[i];
        yy -= staff->ecart * m_layout->m_interl[ staff->staffSize ];
        //staff->clefIndex.compte = 0;
        
		// Calcul du TAB initial, s'il y a lieu 
		//orgx = staff->indent ? staff->indent*10 : 0;
        
		// calcul du point d'ancrage des curseurs au-dessus de la ligne superieure
		staff->m_y_abs = yy; /// + m_layout->m_staffSize[ staff->staffSize ];
        
        // we are handling the first staff - update the position of the system as well
        if ( i == 0 ) { 
            this->m_y_abs = yy;
        }
	}
    return yy;
    */
}

// functors for MusSystem

void MusSystem::Process(MusFunctor *functor, wxArrayPtrVoid params )
{
    if (functor->m_success) {
        return;
    }
    
    MusLaidOutStaffFunctor *staffFunctor = dynamic_cast<MusLaidOutStaffFunctor*>(functor);
    MusLaidOutStaff *staff;
	int i;
    for (i = 0; i < (int)m_staves.GetCount(); i++) 
	{
        staff = &m_staves[i];
        functor->Call( staff, params );
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
