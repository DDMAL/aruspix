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
#include "musstaff.h"


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
}


bool MusPage::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    return !output->WritePage( this );
}


void MusPage::AddSystem( MusSystem *system )
{
	system->SetParent( this );
	m_children.Add( system );
}

int MusPage::GetPageNo() const
{
    wxASSERT_MSG( m_parent, "Doc cannot be NULL");
    
    return m_parent->m_children.Index( *this );
}

int MusPage::GetStaffPosOnPage( MusStaff *staff )
{
    /*
    int position = -1;
    bool success = false;
    wxArrayPtrVoid params;
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
	if ( m_children.IsEmpty() )
		return NULL;
	return (MusSystem*)&m_children[0];
}

MusSystem *MusPage::GetLast( )
{
	if ( m_children.IsEmpty() )
		return NULL;
	int i = GetSystemCount() - 1;
	return (MusSystem*)&m_children[i];
}

MusSystem *MusPage::GetNext( MusSystem *system )
{
    if ( !system || m_children.IsEmpty())
        return NULL;
        
	int i = m_children.Index( *system );

	if ((i == wxNOT_FOUND ) || ( i >= GetSystemCount() - 1 )) 
		return NULL;
	
	return (MusSystem*)&m_children[i + 1];
}

MusSystem *MusPage::GetPrevious( MusSystem *system  )
{
    if ( !system || m_children.IsEmpty())
        return NULL;
        
	int i = m_children.Index( *system );

	if ((i == wxNOT_FOUND ) || ( i <= 0 )) 
        return NULL;
	
    return (MusSystem*)&m_children[i - 1];
}


MusSystem *MusPage::GetAtPos( int y )
{

	y -= ( SYSTEM_OFFSET / 2 );
    MusSystem *system = this->GetFirst();
	if ( !system )
		return NULL;
	
	while ( this->GetNext(system) )
	{
		if ( (int)system->m_y_abs < y )
		{
			// one too much
			if ( this->GetPrevious( system ) ){
				system = this->GetPrevious( system );
				//if ( dif < abs( system->m_y_abs - y ) )
				//	system = this->GetNext( system );
			}
				
			return system;
		}
		system = this->GetNext( system );
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
