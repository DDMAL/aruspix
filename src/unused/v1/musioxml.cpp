/////////////////////////////////////////////////////////////////////////////
// Name:        musioxml.cpp
// Author:      Laurent Pugin
// Created:     2008
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "musioxml.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/filename.h"

#include "musioxml.h"

#include "muspage.h"
#include "musstaff.h"
//#include "musnote.h"
//#include "mussymbol.h"


#include "app/axapp.h"

//----------------------------------------------------------------------------
// MusXmlOutput
//----------------------------------------------------------------------------

MusXmlOutput::MusXmlOutput( MusDoc *doc, wxString filename ) :
    // This is pretty bad. We open a bad fileoutputstream as we don't use it
	MusFileOutputStream( doc, -1 )
{
	m_filename = filename;
    m_xml_root = NULL;
    m_xml_current = NULL;

	// write xml file
	wxASSERT( !m_xml_root );
	m_xml_root = new TiXmlElement("Piece"); // new root
}

MusXmlOutput::~MusXmlOutput()
{
    if (m_xml_root)
        delete m_xml_root;

}

bool MusXmlOutput::ExportFile( )
{
	return true;
    
}

//----------------------------------------------------------------------------
// MusXmlInput
//----------------------------------------------------------------------------

MusXmlInput::MusXmlInput( MusDoc *doc, wxString filename ) :
	MusFileInputStream( doc, filename )
{
	m_vmaj = m_vmin = m_vrev = 10000; // arbitrary version, we assume we will never reach version 10000...
}

MusXmlInput::~MusXmlInput()
{
}

bool MusXmlInput::ImportFile( )
{
	return true;
}
