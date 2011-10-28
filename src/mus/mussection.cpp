/////////////////////////////////////////////////////////////////////////////
// Name:        mussection.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "mussection.h"
#include "musmeasure.h"
#include "musstaff.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusSections );
WX_DEFINE_OBJARRAY( ArrayOfMusSectionElements );


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


void MusSection::AddSectionElement( MusSectionInterface *sectionElement )
{
	sectionElement->SetSection( this );
	m_sectionElements.Add( sectionElement );
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

//----------------------------------------------------------------------------
// MusSectionInterface
//----------------------------------------------------------------------------

MusSectionInterface::MusSectionInterface()
{
    m_section = NULL;
}


MusSectionInterface::~MusSectionInterface()
{
}
