/////////////////////////////////////////////////////////////////////////////
// Name:        musmeasure.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musio.h"
#include "musmeasure.h"
#include "musstaff.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusMeasures );


//----------------------------------------------------------------------------
// MusMeasure
//----------------------------------------------------------------------------

MusMeasure::MusMeasure():
    MusLogicalObject()
{
    m_section = NULL;
}

MusMeasure::~MusMeasure()
{
}

bool MusMeasure::Check()
{
    wxASSERT( m_section );
    return ( m_section && MusLogicalObject::Check()  );
}



void MusMeasure::AddStaff( MusStaff *staff )
{
	staff->SetMeasure( this );
	m_staves.Add( staff );
}

MusStaff *MusMeasure::GetStaff( int staffNo )
{
    if ( staffNo >= (int)m_staves.GetCount() ) {
        return NULL;
    }
    return &m_staves[staffNo];
}

void MusMeasure::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    output->WriteMeasure( this );
    
    // save staves
    MusStaffFunctor staff( &MusStaff::Save );
    this->Process( &staff, params );
}

// functors for MusMeasure

void MusMeasure::Process(MusFunctor *functor, wxArrayPtrVoid params )
{
    if (functor->m_success) {
        return;
    }
    
    MusStaffFunctor *staffFunctor = dynamic_cast<MusStaffFunctor*>(functor);
    MusStaff *staff;
    int i;
    for (i = 0; i < (int)m_staves.GetCount(); i++) 
	{
        staff = &m_staves[i];
        functor->Call( staff, params );
        if (staffFunctor) { // is is a MusStaffFunctor, call it
            staffFunctor->Call( staff, params );
        }
        else { // process it further
            staff->Process( functor, params );
        }
	}
}
