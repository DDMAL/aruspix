/////////////////////////////////////////////////////////////////////////////
// Name:        musiowwg.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IOWWG_H__
#define __MUS_IOWWG_H__

#ifdef __GNUG__
    #pragma interface "musiowwg.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"

#include "musfile.h"

// WDR: class declarations

enum
{
	WWG_ARUSPIX,
	WWG_WOLFGANG,
	WWG_ARUSPIX_CMP
};


//----------------------------------------------------------------------------
// MusWWGOutput
//----------------------------------------------------------------------------

class MusWWGOutput: public MusFileOutputStream
{
public:
    // constructors and destructors
    MusWWGOutput( MusFile *file, wxString filename, int flag = WWG_ARUSPIX );
    virtual ~MusWWGOutput();
    
    // WDR: method declarations for MusWWGOutput
	virtual bool ExportFile( );
	virtual bool WriteFileHeader( const MusFileHeader *header );
	virtual bool WriteParametersMidi( const MusParametersMidi *midi );
	virtual bool WriteParameters2( const MusParameters2 *param2 );
	virtual bool WriteFonts( const ArrayOfWgFonts *fonts );
	virtual bool WriteSeparator( );
	virtual bool WritePage( const MusPage *page );
	virtual bool WriteStaff( const MusStaff *staff );
	virtual bool WriteNote( const MusNote *note );
	virtual bool WriteSymbole( const MusSymbol *symbole );
	virtual bool WriteElementAttr( const MusElement *element );
	virtual bool WriteDebord( const MusElement *element );
	virtual bool WriteLyric( const MusElement *element );
	virtual bool WritePagination( const MusPagination *pagination );
	virtual bool WriteHeaderFooter( const MusHeaderFooter *headerfooter);
    
private:
    // WDR: member variable declarations for MusWWGOutput
	wxUint16 uint16;
	wxInt16 int16;
	wxUint32 uint32;
	wxInt32 int32;
	wxString m_filename;
	int m_flag;

private:
	// WDR: handler declarations for MusWWGOutput
};


//----------------------------------------------------------------------------
// MusWWGInput
//----------------------------------------------------------------------------

class MusWWGInput: public MusFileInputStream
{
public:
    // constructors and destructors
    MusWWGInput( MusFile *file, wxString filename, int flag = WWG_ARUSPIX );
    virtual ~MusWWGInput();
    
    // WDR: method declarations for MusWWGInput
	virtual bool ImportFile( );
	virtual bool ReadFileHeader( MusFileHeader *header );
	virtual bool ReadParametersMidi( MusParametersMidi *midi );
	virtual bool ReadParameters2( MusParameters2 *param2 );
	virtual bool ReadFonts( ArrayOfWgFonts *fonts );
	virtual bool ReadSeparator( );
	virtual bool ReadPage( MusPage *page );
	virtual bool ReadStaff( MusStaff *staff );
	virtual bool ReadNote( MusNote *note );
	virtual bool ReadSymbole( MusSymbol *symbole );
	virtual bool ReadElementAttr( MusElement *element );
	virtual bool ReadDebord( MusElement *element );
	virtual bool ReadLyric( MusElement *element );
	virtual bool ReadPagination( MusPagination *pagination );
	virtual bool ReadHeaderFooter( MusHeaderFooter *headerfooter);
    
private:
    // WDR: member variable declarations for MusWWGInput
	wxUint16 uint16;
	wxInt16 int16;
	wxUint32 uint32;
	wxInt32 int32;
	int m_flag;

private:
	// WDR: handler declarations for MusWWGInput
};


#endif
