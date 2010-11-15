/////////////////////////////////////////////////////////////////////////////
// Name:        musiobin.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
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

#include "musfile.h"


enum
{
	MUS_BIN_ARUSPIX,
	MUS_BIN_ARUSPIX_CMP
};


//----------------------------------------------------------------------------
// MusBinOutput
//----------------------------------------------------------------------------

class MusBinOutput: public MusFileOutputStream
{
public:
    // constructors and destructors
    MusBinOutput( MusFile *file, wxString filename, int flag = MUS_BIN_ARUSPIX );
    virtual ~MusBinOutput();
    
    virtual bool ExportFile( );
	virtual bool WriteFileHeader( const MusFileHeader *header );
	virtual bool WriteSeparator( );
	virtual bool WritePage( const MusPage *page );
	virtual bool WriteStaff( const MusStaff *staff );
	virtual bool WriteNote( const MusNote *note );
	virtual bool WriteSymbol( const MusSymbol *symbol );
    virtual bool WriteNeume( const MusNeume *neume );
    virtual bool WriteLyric( const MusElement *element );
	virtual bool WriteElementAttr( const MusElement *element );
	virtual bool WritePagination( const MusPagination *pagination );
	virtual bool WriteHeaderFooter( const MusHeaderFooter *headerfooter);
    
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

class MusBinInput: public MusFileInputStream
{
public:
    // constructors and destructors
    MusBinInput( MusFile *file, wxString filename, int flag = MUS_BIN_ARUSPIX );
    virtual ~MusBinInput();
    
    virtual bool ImportFile( );
	virtual bool ReadFileHeader( MusFileHeader *header );
	virtual bool ReadSeparator( );
	virtual bool ReadPage( MusPage *page );
	virtual bool ReadStaff( MusStaff *staff );
	virtual bool ReadNote( MusNote *note );
	virtual bool ReadSymbol( MusSymbol *symbol );
    virtual bool ReadNeume( MusNeume *neume );
    virtual bool ReadLyric( MusElement *element );
	virtual bool ReadElementAttr( MusElement *element );
	virtual bool ReadPagination( MusPagination *pagination );
	virtual bool ReadHeaderFooter( MusHeaderFooter *headerfooter);
    
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
