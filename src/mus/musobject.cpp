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
#include "muswindow.h"
#include "musfile.h"
#include "mus.h"


// WDR: class implementations

//----------------------------------------------------------------------------
// MusObject
//----------------------------------------------------------------------------

MusObject::MusObject() :
	wxObject()
{
	m_w = NULL;
	m_f = NULL;
	m_fh = NULL;
	m_p = NULL;
	m_ok = false;
}

MusObject::~MusObject()
{
}

bool MusObject::Init( MusWindow *win )
{
	if ( m_ok )
		return true;

	if ( !win  || !win->m_f || !win->m_fh )
		return false;

	m_w = win;
	m_f = win->m_f;
	m_fh = win->m_fh;
	m_p = &m_fh->param;

	m_ok = true;
	return true;
}


// WDR: handler implementations for MusObject


