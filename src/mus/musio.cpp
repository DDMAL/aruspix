/////////////////////////////////////////////////////////////////////////////
// Name:        musio.cpp
// Author:      Laurent Pugin
// Created:     2012
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "musio.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/filename.h"

#include "musio.h"


//----------------------------------------------------------------------------
// MusFileOutputStream
//----------------------------------------------------------------------------

MusFileOutputStream::MusFileOutputStream( MusDoc *doc, wxString filename ) :
wxFileOutputStream( filename )
{
	wxASSERT_MSG( doc, "Document cannot be NULL" );
	m_doc = doc;
}

MusFileOutputStream::MusFileOutputStream( MusDoc *doc, int fd ) :
wxFileOutputStream( fd )
{
	wxASSERT_MSG( doc, "Document cannot be NULL" );
	m_doc = doc;
}

MusFileOutputStream::~MusFileOutputStream()
{
}



//----------------------------------------------------------------------------
// MusFileInputStream
//----------------------------------------------------------------------------

MusFileInputStream::MusFileInputStream( MusDoc *doc, wxString filename  ) :
wxFileInputStream( filename )
{
	wxASSERT_MSG( doc, "Document cannot be NULL" );
	m_doc = doc;
}

MusFileInputStream::MusFileInputStream( MusDoc *doc, int fd ) :
wxFileInputStream( fd )
{
	wxASSERT_MSG( doc, "Document cannot be NULL" );
	m_doc = doc;
}

MusFileInputStream::~MusFileInputStream()
{
}

