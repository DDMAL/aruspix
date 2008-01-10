/////////////////////////////////////////////////////////////////////////////
// Name:        iowwg.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IOWWG_H__
#define __MUS_IOWWG_H__

#ifdef AX_WG

#ifdef __GNUG__
    #pragma interface "iowwg.cpp"
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
// WwgOutput
//----------------------------------------------------------------------------

class WwgOutput: public WgFileOutputStream
{
public:
    // constructors and destructors
    WwgOutput( WgFile *file, wxString filename, int flag = WWG_ARUSPIX );
    virtual ~WwgOutput();
    
    // WDR: method declarations for WwgOutput
	virtual bool ExportFile( );
	virtual bool WriteFileHeader( const WgFileHeader *header );
	virtual bool WriteParametersMidi( const WgParametersMidi *midi );
	virtual bool WriteParameters2( const WgParameters2 *param2 );
	virtual bool WriteFonts( const ArrayOfWgFonts *fonts );
	virtual bool WriteSeparator( );
	virtual bool WritePage( const WgPage *page );
	virtual bool WriteStaff( const WgStaff *staff );
	virtual bool WriteNote( const WgNote *note );
	virtual bool WriteSymbole( const WgSymbole *symbole );
	virtual bool WriteElementAttr( const WgElement *element );
	virtual bool WriteDebord( const WgElement *element );
	virtual bool WritePagination( const WgPagination *pagination );
	virtual bool WriteHeaderFooter( const WgHeaderFooter *headerfooter);
    
private:
    // WDR: member variable declarations for WwgOutput
	wxUint16 uint16;
	wxInt16 int16;
	wxUint32 uint32;
	wxInt32 int32;
	wxString m_filename;
	int m_flag;

private:
	// WDR: handler declarations for WwgOutput
};


//----------------------------------------------------------------------------
// WwgInput
//----------------------------------------------------------------------------

class WwgInput: public WgFileInputStream
{
public:
    // constructors and destructors
    WwgInput( WgFile *file, wxString filename, int flag = WWG_ARUSPIX );
    virtual ~WwgInput();
    
    // WDR: method declarations for WwgInput
	virtual bool ImportFile( );
	virtual bool ReadFileHeader( WgFileHeader *header );
	virtual bool ReadParametersMidi( WgParametersMidi *midi );
	virtual bool ReadParameters2( WgParameters2 *param2 );
	virtual bool ReadFonts( ArrayOfWgFonts *fonts );
	virtual bool ReadSeparator( );
	virtual bool ReadPage( WgPage *page );
	virtual bool ReadStaff( WgStaff *staff );
	virtual bool ReadNote( WgNote *note );
	virtual bool ReadSymbole( WgSymbole *symbole );
	virtual bool ReadElementAttr( WgElement *element );
	virtual bool ReadDebord( WgElement *element );
	virtual bool ReadPagination( WgPagination *pagination );
	virtual bool ReadHeaderFooter( WgHeaderFooter *headerfooter);
    
private:
    // WDR: member variable declarations for WwgInput
	wxUint16 uint16;
	wxInt16 int16;
	wxUint32 uint32;
	wxInt32 int32;
	int m_flag;

private:
	// WDR: handler declarations for WwgInput
};

#endif //AX_WG

#endif
