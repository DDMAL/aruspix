/////////////////////////////////////////////////////////////////////////////
// Name:        musioxml.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
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

#include "musfile.h"



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
    MusXmlOutput( MusFile *file, wxString filename );
    virtual ~MusXmlOutput();
    
    virtual bool ExportFile( );
	virtual bool WriteFileHeader( const MusFileHeader *header );
	virtual bool WriteStaff( const MusStaff *staff );
	virtual bool WriteNote( const MusNote *note );
	virtual bool WriteSymbol( const MusSymbol *symbol );
    virtual bool WriteLyric( const MusElement *element );
	virtual bool WriteElementAttr( const MusElement *element );
    
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
    MusXmlInput( MusFile *file, wxString filename );
    virtual ~MusXmlInput();
    
    virtual bool ImportFile( );
	virtual bool ReadFileHeader( MusFileHeader *header );
	virtual bool ReadSeparator( );
	virtual bool ReadPage( MusPage *page );
	virtual bool ReadStaff( MusStaff *staff );
	virtual bool ReadNote( MusNote *note );
	virtual bool ReadSymbol( MusSymbol *symbol );
    virtual bool ReadLyric( MusElement *element );
	virtual bool ReadElementAttr( MusElement *element );
	virtual bool ReadPagination( MusPagination *pagination );
	virtual bool ReadHeaderFooter( MusHeaderFooter *headerfooter);
    
private:
        int m_vmaj, m_vmin, m_vrev;

private:
	};


#endif
