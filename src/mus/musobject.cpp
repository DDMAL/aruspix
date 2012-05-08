/////////////////////////////////////////////////////////////////////////////
// Name:        musobject.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musobject.h"
#include "musrc.h"
#include "musdoc.h"

//----------------------------------------------------------------------------
// MusObject
//----------------------------------------------------------------------------

MusObject::MusObject() :
	wxObject()
{
    m_id = 0;
}

MusObject::~MusObject()
{
}


//----------------------------------------------------------------------------
// MusLogicalObject
//----------------------------------------------------------------------------

MusLogicalObject::MusLogicalObject() :
	MusObject()
{
}

MusLogicalObject::~MusLogicalObject()
{
}


//----------------------------------------------------------------------------
// MusLayoutObject
//----------------------------------------------------------------------------

MusLayoutObject::MusLayoutObject() :
	MusObject()
{
	m_doc = NULL;
	m_ok = false;
}

MusLayoutObject::~MusLayoutObject()
{
}

bool MusLayoutObject::Init( MusRC *renderer )
{
	if ( m_ok )
		return true;

	if ( !renderer  || !renderer->m_doc )
		return false;

	m_doc = renderer->m_doc;

	m_ok = true;
	return true;
}




