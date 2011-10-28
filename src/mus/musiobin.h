/////////////////////////////////////////////////////////////////////////////
// Name:        musiobin.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IOBIN_H__
#define __MUS_IOBIN_H__

#ifdef __GNUG__
    #pragma interface "musiobin.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"

#include "musdoc.h"


enum
{
	MUS_BIN_ARUSPIX,
	MUS_BIN_ARUSPIX_CMP
};


//----------------------------------------------------------------------------
// MusBinOutput
//----------------------------------------------------------------------------

/**
 * This class is a file output stream for binary object serialization.
 * Broken in Aruspix 2.0 (musiobin.cpp has been removed from the xcode project).
*/
class MusBinOutput: public MusFileOutputStream
{
public:
    // constructors and destructors
    MusBinOutput( MusDoc *file, wxString filename, int flag = MUS_BIN_ARUSPIX ) {};
    virtual ~MusBinOutput() {};
    
    bool ExportFile( ) { return false; };
    /*
	bool WriteFileHeader( const MusFileHeader *header ) {};
	bool WriteSeparator( ) {};
	bool WritePage( const MusPage *page ) {};
	bool WriteStaff( const MusLaidOutStaff *staff ) {};
	bool WriteNote( const MusNote1 *note ) {};
	bool WriteSymbol( const MusSymbol1 *symbol ) {};
    bool WriteNeume( const MusNeume *neume ) {};
    bool WriteLyric( const MusElement *element ) {};
	bool WriteElementAttr( const MusElement *element ) {};
	bool WritePagination( const MusPagination *pagination ) {};
	bool WriteHeaderFooter( const MusWWGData *headerfooter) {};
    */
    
private:
    wxUint16 uint16;
	wxInt16 int16;
	wxUint32 uint32;
	wxInt32 int32;
	wxString m_filename;
	int m_flag;

private:
	};


//----------------------------------------------------------------------------
// MusBinInput
//----------------------------------------------------------------------------

/**
 * This class is a file input stream for binary object serialization.
 * Broken in Aruspix 2.0 (musiobin.cpp has been removed from the xcode project).
*/
class MusBinInput: public MusFileInputStream
{
public:
    // constructors and destructors
    MusBinInput( MusDoc *file, wxString filename, int flag = MUS_BIN_ARUSPIX ) {};
    virtual ~MusBinInput() {};
    
    bool ImportFile( ) { return false; };
    /*
	bool ReadFileHeader( MusFileHeader *header ) {};
	bool ReadSeparator( ) {};
	bool ReadPage( MusPage *page ) {};
	bool ReadStaff( MusLaidOutStaff *staff ) {};
	bool ReadNote( MusNote1 *note ) {};
	bool ReadSymbol( MusSymbol1 *symbol ) {};
    bool ReadNeume( MusNeume *neume ) {};
    bool ReadLyric( MusElement *element ) {};
	bool ReadElementAttr( MusElement *element ) {};
	bool ReadPagination( MusPagination *pagination ) {};
	bool ReadHeaderFooter( MusWWGData *headerfooter) {};
    */
    
private:
    wxUint16 uint16;
	wxInt16 int16;
	wxUint32 uint32;
	wxInt32 int32;
	int m_flag;
    int m_vmaj, m_vmin, m_vrev;

private:
	};


#endif
