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
	defin = 20;
}


void MusPage::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    output->WritePage( this );
    
    // save system
    MusSystemFunctor system( &MusSystem::Save );
    this->Process( &system, params );
}

void MusPage::Load( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileInputStream *input = (MusFileInputStream*)params[0];       
    
    // load systems
    MusSystem *system;
    while ( (system = input->ReadSystem()) ) {
        system->Load( params );
        this->AddSystem( system );
    }
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

	y += ( SYSTEM_OFFSET / 2 );
    MusSystem *system = this->GetFirst();
	if ( !system )
		return NULL;
	
	int dif =  abs( system->m_y_abs - y );
	while ( this->GetNext(system) )
	{
		if ( (int)system->m_y_abs < y )
		{
			// one too much
			if ( this->GetPrevious( system ) ){
				system = this->GetPrevious( system );
				if ( dif < abs( system->m_y_abs - y ) )
					system = this->GetNext( system );
			}
				
			return system;
		}
		system = this->GetNext( system );
		dif = abs( system->m_y_abs - y );
	}

	if ( ( (int)system->m_y_abs < y )  && this->GetPrevious( system ) )
		system = this->GetPrevious( system );

	return system;
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

void MusPage::Process(MusFunctor *functor, wxArrayPtrVoid params )
{
    if (functor->m_success) {
        return;
    }
    
    MusSystemFunctor *sysFunctor = dynamic_cast<MusSystemFunctor*>(functor);
    MusSystem *system;
	int i;
    for (i = 0; i < GetSystemCount(); i++) 
	{
        
        system = &m_systems[i];
        functor->Call( system, params );
        if (sysFunctor) { // is is a MusSystemFunctor, call it and stop
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
        yy = system->UpdateStaffPositions( yy );
    }
}

