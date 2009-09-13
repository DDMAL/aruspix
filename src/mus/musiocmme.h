/////////////////////////////////////////////////////////////////////////////
// Name:        musiocmme.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IOCMME_H__
#define __MUS_IOCMME_H__

#ifdef __GNUG__
    #pragma interface "musiocmme.cpp"
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

// WDR: class declarations


//----------------------------------------------------------------------------
// MusCmmeOutput
//----------------------------------------------------------------------------

class MusCmmeOutput: public MusFileOutputStream
{
public:
    // constructors and destructors
    MusCmmeOutput( MusFile *file, wxString filename );
    virtual ~MusCmmeOutput();
    
    // WDR: method declarations for MusCmmeOutput
	virtual bool ExportFile( );
	virtual bool WriteFileHeader( const MusFileHeader *header );
	virtual bool WriteStaff( const MusStaff *staff );
	virtual bool WriteNote( const MusNote *note );
	virtual bool WriteSymbole( const MusSymbol *symbol );
    virtual bool WriteLyric( const MusElement *element );
	virtual bool WriteElementAttr( const MusElement *element );
    
private:
    // WDR: member variable declarations for MusCmmeOutput
	wxString m_filename;
    TiXmlElement *m_xml_root;
    TiXmlElement *m_xml_current;


private:
	// WDR: handler declarations for MusCmmeOutput
};


//----------------------------------------------------------------------------
// MusCmmeInput
//----------------------------------------------------------------------------

class MusCmmeInput: public MusFileInputStream
{
public:
    // constructors and destructors
    MusCmmeInput( MusFile *file, wxString filename );
    virtual ~MusCmmeInput();
    
    // WDR: method declarations for MusCmmeInput
	virtual bool ImportFile( );
	virtual bool ReadFileHeader( MusFileHeader *header );
	virtual bool ReadSeparator( );
	virtual bool ReadPage( MusPage *page );
	virtual bool ReadStaff( MusStaff *staff );
	virtual bool ReadNote( MusNote *note );
	virtual bool ReadSymbole( MusSymbol *symbol );
    virtual bool ReadLyric( MusElement *element );
	virtual bool ReadElementAttr( MusElement *element );
	virtual bool ReadPagination( MusPagination *pagination );
	virtual bool ReadHeaderFooter( MusHeaderFooter *headerfooter);
    
private:
    // WDR: member variable declarations for MusCmmeInput
    int m_vmaj, m_vmin, m_vrev;

private:
	// WDR: handler declarations for MusCmmeInput
};


#endif
