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
