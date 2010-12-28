/////////////////////////////////////////////////////////////////////////////
// Name:        musobject.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "musobject.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "musobject.h"
#include "musrc.h"
#include "musfile.h"
#include "mus.h"



//----------------------------------------------------------------------------
// MusObject
//----------------------------------------------------------------------------

MusObject::MusObject() :
	wxObject()
{
	m_r = NULL;
	m_f = NULL;
	m_fh = NULL;
	m_p = NULL;
	m_ok = false;
}

MusObject::~MusObject()
{
}

bool MusObject::Init( MusRC *renderer )
{
	if ( m_ok )
		return true;

	if ( !renderer  || !renderer->m_f || !renderer->m_fh )
		return false;

	m_r = renderer;
	m_f = renderer->m_f;
	m_fh = renderer->m_fh;
	m_p = &m_fh->param;

	m_ok = true;
	return true;
}




