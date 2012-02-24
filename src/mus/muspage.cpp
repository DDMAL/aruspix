/////////////////////////////////////////////////////////////////////////////
// Name:        muspage.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

//#include "wx/tokenzr.h"

#include "muspage.h"
#include "musdoc.h"
#include "mussystem.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusPages );

//----------------------------------------------------------------------------
// MusPage
//----------------------------------------------------------------------------

MusPage::MusPage() :
	MusLayoutObject()
{
	Clear( );
}

MusPage::~MusPage()
{
}

void MusPage::Clear( )
{
	m_systems.Clear( );
	noMasqueFixe = false;
	noMasqueVar = false;
	reserve = 0;
	defin = 20;
	npage = 0;
}



void MusPage::AddSystem( MusSystem *system )
{
	system->SetPage( this );
	m_systems.Add( system );
}

int MusPage::GetPageNo() const
{
    wxASSERT_MSG( m_layout, "Layout cannot be NULL");
    
    return m_layout->m_pages.Index( *this );
}


MusSystem *MusPage::GetFirst( )
{
	if ( m_systems.IsEmpty() )
		return NULL;
	return &m_systems[0];
}

MusSystem *MusPage::GetLast( )
{
	if ( m_systems.IsEmpty() )
		return NULL;
	int i = GetSystemCount() - 1;
	return &m_systems[i];
}

MusSystem *MusPage::GetNext( MusSystem *system )
{
    if ( !system || m_systems.IsEmpty())
        return NULL;
        
	int i = m_systems.Index( *system );

	if ((i == wxNOT_FOUND ) || ( i >= GetSystemCount() - 1 )) 
		return NULL;
	
	return &m_systems[i + 1];
}

MusSystem *MusPage::GetPrevious( MusSystem *system  )
{
    if ( !system || m_systems.IsEmpty())
        return NULL;
        
	int i = m_systems.Index( *system );

	if ((i == wxNOT_FOUND ) || ( i <= 0 )) 
        return NULL;
	
    return &m_systems[i - 1];
}


MusSystem *MusPage::GetAtPos( int y )
{
/*
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
*/
    wxLogDebug("TODO");
    return NULL;
}

void MusPage::SetValues( int type )
{
/* 
    int i;
    wxString values;
    for (i = 0; i < nbrePortees; i++) 
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
    for (i = 0; (i < nbrePortees) && (i < (int)values_arr.GetCount()) ; i++) 
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

// functors for MusPage

void MusPage::Process(MusLayoutFunctor *functor, wxArrayPtrVoid params )
{
    MusSystemFunctor *sysFunctor = dynamic_cast<MusSystemFunctor*>(functor);
    MusSystem *system;
	int i;
    for (i = 0; i < GetSystemCount(); i++) 
	{
        system = &m_systems[i];
        if (sysFunctor) { // is is a MusSystemFunctor, call it
            sysFunctor->Call( system, params );
        }
        else { // process it further
            system->Process( functor, params );
        }
	}
}

void MusPage::ProcessStaves( wxArrayPtrVoid params )
{
    // param 0: MusSystemFunctor
    // param 1; wxArrayPtrVoid
    
    MusSystemFunctor *systemFunctor = (MusSystemFunctor*)params[0];
    wxArrayPtrVoid *systemParams = (wxArrayPtrVoid*)params[1];
    
	int i;
    MusSystem *system;

    for (i = 0; i < this->GetSystemCount(); i++) 
	{
		system = &m_systems[i];
		systemFunctor->Call( system, *systemParams );	
	}
}


void MusPage::ProcessVoices( wxArrayPtrVoid params )
{
    /*
    // param 0: MusStaffFunctor
    // param 1; wxArrayPtrVoid 
    // param 2; int (voice number)
    
    MusStaffFunctor *staffFunctor = (MusStaffFunctor*)params[0];
    wxArrayPtrVoid *staffParams = (wxArrayPtrVoid*)params[1];
    int *voice = (int*)params[2];
    
	int i;
    MusLaidOutStaff *staff;

    for (i = 0; i < nbrePortees; i++) 
	{
		staff = &m_staves[i];
        if (staff->voix == (*voice)) {
            staffFunctor->Call( staff, *staffParams );
        }
	}
    */
    wxLogDebug("TODO");
    return;
}

void MusPage::CountVoices( wxArrayPtrVoid params )
{
    /*
    // param 0; int (min number of voice number)
    // param 1; int (max number of voice number)
    
    int *min_voice = (int*)params[0];
    int *max_voice = (int*)params[1];
    
	int i;
    MusLaidOutStaff *staff;

    for (i = 0; i < nbrePortees; i++) 
	{
		staff = &m_staves[i];
        if (staff->voix > (*max_voice)) {
           (*max_voice) = staff->voix;
        }
        if (staff->voix < (*min_voice)) {
           (*min_voice) = staff->voix;
        }
	}
    */
    wxLogDebug("TODO");
    return;
}


void MusPage::UpdateSystemPositions( ) 
{
	if ( !this->Check() )
		return;

	int i;
	MusSystem *system = NULL;
    
    int yy =  m_layout->m_pageHeight;
    for (i = 0; i < this->GetSystemCount(); i++) 
	{
		system = &this->m_systems[i];
        //system->Init( m_r );
        system->m_yrel = yy;
        yy = system->UpdateStaffPositions( );
    }
}

