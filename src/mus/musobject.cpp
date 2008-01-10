/////////////////////////////////////////////////////////////////////////////
// Name:        wgobject.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_WG

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
// WgObject
//----------------------------------------------------------------------------

WgObject::WgObject() :
	wxObject()
{
	m_w = NULL;
	m_f = NULL;
	m_fh = NULL;
	m_p = NULL;
	m_p2 = NULL;
	m_ok = false;
}

WgObject::~WgObject()
{
}

bool WgObject::Init( WgWindow *win )
{
	if ( m_ok )
		return true;

	if ( !win  || !win->m_f || !win->m_fh ||!win->m_p2 )
		return false;

	m_w = win;
	m_f = win->m_f;
	m_fh = win->m_fh;
	m_p = &m_fh->param;
	m_p2 = win->m_p2;

	m_ok = true;
	return true;
}


// WDR: handler implementations for WgObject

#endif // AX_WG
