/////////////////////////////////////////////////////////////////////////////
// Name:        musdiv.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musdiv.h"
#include "mussection.h"
#include "musio.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusDivs );
WX_DEFINE_OBJARRAY( ArrayOfMusParts );


//----------------------------------------------------------------------------
// MusDiv
//----------------------------------------------------------------------------

MusDiv::MusDiv():
	MusLogicalObject()
{
    m_score = NULL;
    m_partSet = NULL;
    m_doc = NULL;
}

MusDiv::~MusDiv()
{
}

void MusDiv::AddScore( MusScore *score )
{
	wxASSERT_MSG( !m_partSet, "MusPartSet has to be NULL");
	
	score->SetDiv( this );	
    m_score = score;
}

void MusDiv::AddPartSet( MusPartSet *partSet )
{
	wxASSERT_MSG( !m_score, "MusScore has to be NULL");
	
	partSet->SetDiv( this );	
    m_partSet = partSet;
}

void MusDiv::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    output->WriteDiv( this );
    
    // save score
    MusScoreFunctor score( &MusScore::Save );
    this->Process( &score, params );
    // save parts
    MusPartSetFunctor parts( &MusPartSet::Save );
    this->Process( &parts, params );
}

void MusDiv::Load( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileInputStream *input = (MusFileInputStream*)params[0];       
    
    // load score
    MusScore *score;
    if( (score = input->ReadScore()) ) {
        score->Load( params );
        this->AddScore( score );
    }
    // load parts
    MusPartSet *partSet;
    if( (partSet = input->ReadPartSet()) ) {
        partSet->Load( params );
        this->AddPartSet( partSet );
    }
}

// functors for MusDiv

void MusDiv::Process(MusFunctor *functor, wxArrayPtrVoid params )
{
    if (functor->m_success) {
        return;
    }
    
    // measure music
    MusScoreFunctor *scoreFunctor = dynamic_cast<MusScoreFunctor*>(functor);
    MusPartSetFunctor *partSetFunctor = dynamic_cast<MusPartSetFunctor*>(functor);
    if (m_score) {
        functor->Call( m_score, params );
        if (scoreFunctor) { // is is a MusMeasureFunctor, call it
            scoreFunctor->Call( m_score, params );
        }
        else { // process it further
            m_score->Process( functor, params );
        }
	}
    if (m_partSet) {
        functor->Call( m_partSet, params );
        if (partSetFunctor) { // is is a MusMeasureFunctor, call it
            partSetFunctor->Call( m_partSet, params );
        }
        else { // process it further
            m_partSet->Process( functor, params );
        }        
    }
}

//----------------------------------------------------------------------------
// MusScore
//----------------------------------------------------------------------------

MusScore::MusScore():
	MusLogicalObject()
{
    m_div = NULL;
}


MusScore::~MusScore()
{
}

void MusScore::AddSection( MusSection *section )
{
	section->SetScore( this );
    m_sections.Add( section );
}

void MusScore::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    output->WriteScore( this );
    
    // save sections
    MusSectionFunctor section( &MusSection::Save );
    this->Process( &section, params );
}

void MusScore::Load( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileInputStream *input = (MusFileInputStream*)params[0];       
    
    // load score
    MusSection *section;
    while ( (section = input->ReadSection()) ) {
        section->Load( params );
        this->AddSection( section );
    }
}

// functors for MusScore

void MusScore::Process(MusFunctor *functor, wxArrayPtrVoid params )
{
    if (functor->m_success) {
        return;
    }
    
    MusSectionFunctor *sectionFunctor = dynamic_cast<MusSectionFunctor*>(functor);
    MusSection *section;
    int i;
    for (i = 0; i < (int)m_sections.GetCount(); i++) 
	{
        section = &m_sections[i];
        functor->Call( section, params );
        if (sectionFunctor) { // is is a MusSectionFunctor, call it
            sectionFunctor->Call( section, params );
        }
        else { // process it further
            section->Process( functor, params );
        }
	}
}

//----------------------------------------------------------------------------
// MusPartSet
//----------------------------------------------------------------------------

MusPartSet::MusPartSet():
	MusLogicalObject()
{
    m_div = NULL;
}


MusPartSet::~MusPartSet()
{
}

void MusPartSet::AddPart( MusPart *part )
{
	part->SetPartSet( this );
    m_parts.Add( part );
}

void MusPartSet::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    output->WritePartSet( this );
    
    // save sections
    MusPartFunctor part( &MusPart::Save );
    this->Process( &part, params );
}

void MusPartSet::Load( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileInputStream *input = (MusFileInputStream*)params[0];       
    
    // load score
    MusPart *part;
    while ( (part = input->ReadPart()) ) {
        part->Load( params );
        this->AddPart( part );
    }
}

// functors for MusPartSet

void MusPartSet::Process(MusFunctor *functor, wxArrayPtrVoid params )
{
    if (functor->m_success) {
        return;
    }
    
    MusPartFunctor *partFunctor = dynamic_cast<MusPartFunctor*>(functor);
    MusPart *part;
    int i;
    for (i = 0; i < (int)m_parts.GetCount(); i++) 
	{
        part = &m_parts[i];
        functor->Call( part, params );
        if (partFunctor) { // is is a MusPartFunctor, call it
            partFunctor->Call( part, params );
        }
        else { // process it further
            part->Process( functor, params );
        }
	}
}

//----------------------------------------------------------------------------
// MusPart
//----------------------------------------------------------------------------

MusPart::MusPart():
	MusLogicalObject()
{
    m_partSet = NULL;
}


MusPart::~MusPart()
{
}


void MusPart::AddSection( MusSection *section )
{
	section->SetPart( this );
    m_sections.Add( section );
}

void MusPart::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    output->WritePart( this );
    
    // save sections
    MusSectionFunctor section( &MusSection::Save );
    this->Process( &section, params );
}

void MusPart::Load( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileInputStream *input = (MusFileInputStream*)params[0];       
    
    MusSection *section;
    // load score
    while ( (section = input->ReadSection()) ) {
        section->Load( params );
        this->AddSection( section );
    }
}

// functors for MusPart

void MusPart::Process(MusFunctor *functor, wxArrayPtrVoid params )
{
    if (functor->m_success) {
        return;
    }
    
    MusSectionFunctor *sectionFunctor = dynamic_cast<MusSectionFunctor*>(functor);
    MusSection *section;
	int i;
    for (i = 0; i < (int)m_sections.GetCount(); i++) 
	{
        section = &m_sections[i];
        functor->Call( section, params );
        if (sectionFunctor) { // is is a MusSectionFunctor, call it
            sectionFunctor->Call( section, params );
        }
        else { // process it further
            section->Process( functor, params );
        }
	}
}



