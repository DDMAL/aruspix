/////////////////////////////////////////////////////////////////////////////
// Name:        mussection.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musio.h"
#include "mussection.h"
#include "musmeasure.h"
#include "musstaff.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusSections );

//----------------------------------------------------------------------------
// MusSection
//----------------------------------------------------------------------------

MusSection::MusSection():
	MusLogicalObject()
{
    m_score = NULL;
    m_part = NULL;
}


MusSection::~MusSection()
{
}


bool MusSection::Check()
{
    wxASSERT( ( m_score || m_part ) );
    return ( ( m_score || m_part )  && MusLogicalObject::Check() );
}



void MusSection::AddMeasure( MusMeasure *measure )
{
    wxASSERT_MSG( m_staves.IsEmpty(), "A section cannot contain measures and staves at the same time" );    
	measure->SetSection( this );
	m_measures.Add( measure );
}


void MusSection::AddStaff( MusStaff *staff )
{
    wxASSERT_MSG( m_measures.IsEmpty(), "A section cannot contain measures and staves at the same time" );    
	staff->SetSection( this );
	m_staves.Add( staff );
}

MusStaff *MusSection::GetStaff( int staffNo )
{
    if ( staffNo >= (int)m_staves.GetCount() ) {
        return NULL;
    }
    return &m_staves[staffNo];
}

void MusSection::SetScore( MusScore *score )
{
    wxASSERT_MSG( !m_part, "A section cannot be child of a score and of a part at the same time" );
    m_score = score;
}


void MusSection::SetPart( MusPart *part )
{
    wxASSERT_MSG( !m_score, "A section cannot be child of a score and of a part at the same time" );
    m_part = part;
}

void MusSection::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    output->WriteSection( this );
    
    // save measures ( measured music )
    MusMeasureFunctor measure( &MusMeasure::Save );
    this->Process( &measure, params );
    // save staves ( unmeasured music )
    MusStaffFunctor staff( &MusStaff::Save );
    this->Process( &staff, params );
}

void MusSection::Load( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileInputStream *input = (MusFileInputStream*)params[0];       
    
    // load measures ( measured music )
    MusMeasure *measure;
    while ( (measure = input->ReadMeasure()) ) {
        measure->Load( params );
        this->AddMeasure( measure );
    }
    // load staves ( unmeasured music )
    MusStaff *staff;
    while ( (staff = input->ReadStaff()) ) {
        staff->Load( params );
        this->AddStaff( staff );
    }
}

// functors for MusSection

void MusSection::Process(MusFunctor *functor, wxArrayPtrVoid params )
{
    if (functor->m_success) {
        return;
    }
    
    // measure music
    MusMeasureFunctor *measureFunctor = dynamic_cast<MusMeasureFunctor*>(functor);
    MusMeasure *measure;
	int i;
    for (i = 0; i < (int)m_measures.GetCount(); i++) 
	{
        measure = &m_measures[i];
        functor->Call( measure, params );
        if (measureFunctor) { // is is a MusMeasureFunctor, call it
            measureFunctor->Call( measure, params );
        }
        else { // process it further
            measure->Process( functor, params );
        }
	}
    // unmeasured music
    MusStaffFunctor *staffFunctor = dynamic_cast<MusStaffFunctor*>(functor);
    MusStaff *staff;
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
