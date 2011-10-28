/////////////////////////////////////////////////////////////////////////////
// Name:        musioxml.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IOMEI_H__
#define __MUS_IOMEI_H__

#ifdef __GNUG__
    #pragma interface "musioxml.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"

// TINYXML
#if defined (__WXMSW__)
    #include "tinyxml.h"
#else
    #include "tinyxml/tinyxml.h"
#endif

#include "musdoc.h"



/*
    Scaffold for MusicXML ouput and input
    Not implemeneted! Was copied from CMME output
*/

//----------------------------------------------------------------------------
// MusXmlOutput
//----------------------------------------------------------------------------

class MusXmlOutput: public MusFileOutputStream
{
public:
    // constructors and destructors
    MusXmlOutput( MusDoc *file, wxString filename );
    virtual ~MusXmlOutput();
    
    virtual bool ExportFile( );
    
private:
    wxString m_filename;
    TiXmlElement *m_xml_root;
    TiXmlElement *m_xml_current;


private:
	};


//----------------------------------------------------------------------------
// MusXmlInput
//----------------------------------------------------------------------------

class MusXmlInput: public MusFileInputStream
{
public:
    // constructors and destructors
    MusXmlInput( MusDoc *file, wxString filename );
    virtual ~MusXmlInput();
    
    virtual bool ImportFile( );
    
private:
        int m_vmaj, m_vmin, m_vrev;

private:
	};


#endif
