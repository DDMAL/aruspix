/////////////////////////////////////////////////////////////////////////////
// Name:        musiomei.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IOMEI_H__
#define __MUS_IOMEI_H__

#ifdef __GNUG__
    #pragma interface "musiomei.cpp"
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
// MusMeiOutput
//----------------------------------------------------------------------------

class MusMeiOutput: public MusFileOutputStream
{
public:
    // constructors and destructors
    MusMeiOutput( MusFile *file, wxString filename );
    virtual ~MusMeiOutput();
    
    // WDR: method declarations for MusMeiOutput
	virtual bool ExportFile( );
	virtual bool WriteFileHeader( const MusFileHeader *header );
    virtual bool WritePage( const MusPage *page );
	virtual bool WriteStaff( const MusStaff *staff );
	virtual bool WriteNote( const MusNote *note );
	virtual bool WriteSymbole( const MusSymbol *symbol );
    virtual bool WriteLyric( const MusElement *element );
	virtual bool WriteElementAttr( const MusElement *element );
    
private:
    void WriteParts( TiXmlElement *parts );
    void WriteScore( TiXmlElement *score );
    
private:
    // WDR: member variable declarations for MusMeiOutput
	wxString m_filename;
    TiXmlElement *m_xml_root;
    TiXmlElement *m_xml_current;
    TiXmlElement **m_xml_staves;


private:
	// WDR: handler declarations for MusMeiOutput
};


//----------------------------------------------------------------------------
// MusMeiInput
//----------------------------------------------------------------------------

class MusMeiInput: public MusFileInputStream
{
public:
    // constructors and destructors
    MusMeiInput( MusFile *file, wxString filename );
    virtual ~MusMeiInput();
    
    // WDR: method declarations for MusMeiInput
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
    TiXmlNode *GetFirstChild( TiXmlNode *node, wxString element );
    void ReadParts( TiXmlElement *parts );
    void ReadScore( TiXmlElement *score );
    
private:
    // WDR: member variable declarations for MusMeiInput

private:
	// WDR: handler declarations for MusMeiInput
	wxString m_filename;
    TiXmlElement *m_xml_root;
    TiXmlElement *m_xml_current;
};


#endif
