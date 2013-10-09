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

#include "musdef.h"
#include "musdoc.h"
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


bool MusPage::Save( ArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    return !output->WritePage( this );
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
