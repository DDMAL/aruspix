/////////////////////////////////////////////////////////////////////////////
// Name:        musneume.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "musneume.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "musneume.h"
#include "muswindow.h"
#include "musstaff.h"
#include "muspage.h"
#include "musdef.h"

#include <math.h>

// sorting function
int SortElements(MusNeume **first, MusNeume **second)
{
	if ( (*first)->xrel < (*second)->xrel )
		return -1;
	else if ( (*first)->xrel > (*second)->xrel )
		return 1;
	else 
		return 0;
}

// WDR: class implementations

//----------------------------------------------------------------------------
// MusNeume
//----------------------------------------------------------------------------

MusNeume::MusNeume():
	MusElement()
{
	TYPE = NEUME;
}

MusNeume::MusNeume( char _sil, unsigned char _val, unsigned char _code )
	: MusElement()
{
	TYPE = NEUME;
}

MusNeume::MusNeume( const MusNeume& neume )
	: MusElement( neume )
{
	TYPE = neume.TYPE;
    // copy each member
    // ...
} 

MusNeume& MusNeume::operator=( const MusNeume& neume )
{
	if ( this != &neume ) // not self assignement
	{
		// For base class MusElement copy assignement
		(MusElement&)*this = neume;
		TYPE = neume.TYPE;
        // copy each member
        // ...
	}
	return *this;
}

MusNeume::~MusNeume()
{	
}



void MusNeume::SetPitch( int code, int oct, MusStaff *staff )
{
	if ( this->TYPE != NEUME )
		return;

    // Change the neume pitch
    // ...

	if (m_w)
		m_w->Refresh();
}



void MusNeume::SetValue( int value, MusStaff *staff, int vflag )
{	
	if ( this->TYPE != NEUME )
		return;

    // Change the neume value
    // ...
	
	if (m_w)
		m_w->Refresh();
}




void MusNeume::Draw( wxDC *dc, MusStaff *staff)
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	wxASSERT_MSG( m_w, "MusWindow cannot be NULL ");
	if ( !Check() )
		return;	

    // Draw the neume
    // ...
	
	return;
}

// WDR: handler implementations for MusNeume



