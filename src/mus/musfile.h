/////////////////////////////////////////////////////////////////////////////
// Name:        musfile.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_FILE_H__
#define __MUS_FILE_H__

#ifdef __GNUG__
    #pragma interface "musfile.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"
//#include "wx/dynarray.h"

#include "musfile.h"
#include "muspage.h"
#include "musstaff.h"
#include "muselement.h"
#include "musnote.h"
#include "mussymbol.h"
#include "mus.h"

WX_DECLARE_OBJARRAY( MusPage, ArrayOfWgPages);

WX_DECLARE_OBJARRAY( MusFont, ArrayOfWgFonts);



// WDR: class declarations


//----------------------------------------------------------------------------
// MusFile
//----------------------------------------------------------------------------

class MusFile
{
	//friend class MusFileOutputStream;
	friend class MusFileInputStream;

public:
    // constructors and destructors
    MusFile();
    virtual ~MusFile();
    
    // WDR: method declarations for MusFile
	void CheckIntegrity();
    
public:
    // WDR: member variable declarations for MusFile
    /** nom complet du fichier */
    wxString m_fname;
    /** FileHeader du fichier - contient Parametres */
    MusFileHeader m_fheader;
    /** ParametresMidi du fichier */
    MusParametersMidi m_midi;
    /** Parametres2 du fichier */
    MusParameters2 m_param2;
    /** PolicesTable du fichier */
    ArrayOfWgFonts m_fonts;
    /** FileData du fichier - contient les pages */
    ArrayOfWgPages m_pages;
	/** Masque fixe **/
	MusPage m_masqueFixe;
	/** Masque fixe **/
	MusPage m_masqueVariable;
	/** Pagination **/
	MusPagination m_pagination;
	/** Header **/
	MusHeaderFooter m_header;
	/** Footer **/
	MusHeaderFooter m_footer;
	/** Separateur **/
	static char* sep;

private:
	// WDR: handler declarations for MusFile

};


//----------------------------------------------------------------------------
// MusFileOutputStream
//----------------------------------------------------------------------------

class MusFileOutputStream: public wxFileOutputStream
{
public:
    // constructors and destructors
    MusFileOutputStream( MusFile *file, wxString filename );
	MusFileOutputStream( MusFile *file, int fd );
	//MusFileOutputStream( MusFile *file, wxFile *wxfile );
    virtual ~MusFileOutputStream();
    
    // WDR: method declarations for MusFileOutputStream
	virtual bool ExportFile( ) { return true; }
	virtual bool WriteFileHeader( const MusFileHeader *header ) { return true; }
	virtual bool WriteParametersMidi( const MusParametersMidi *midi ) { return true; }
	virtual bool WriteParameters2( const MusParameters2 *param2 ) { return true; }
	virtual bool WriteFonts( const ArrayOfWgFonts *fonts ) { return true; }
	virtual bool WriteSeparator( ) { return true; }
	virtual bool WritePage( const MusPage *page ) { return true; }
	virtual bool WriteStaff( const MusStaff *staff ) { return true; }
	virtual bool WriteNote( const MusNote *note ) { return true; }
	virtual bool WriteSymbole( const MusSymbol *symbole ) { return true; }
	virtual bool WriteElementAttr( const MusElement *element ) { return true; }
	virtual bool WriteDebord( const MusElement *element ) { return true; }
	virtual bool WritePagination( const MusPagination *pagination ) { return true; }
	virtual bool WriteHeaderFooter( const MusHeaderFooter *headerfooter) { return true; }
    
public:
    // WDR: member variable declarations for MusFileOutputStream

protected:
	// WDR: handler declarations for MusFileOutputStream
	MusFile *m_file;

};

//----------------------------------------------------------------------------
// MusFileInputStream
//----------------------------------------------------------------------------

class MusFileInputStream: public wxFileInputStream
{
public:
    // constructors and destructors
    MusFileInputStream( MusFile *file, wxString filename );
    virtual ~MusFileInputStream();
    
    // WDR: method declarations for MusFileInputStream
	// read
	virtual bool ImportFile( ) { return true; }
	virtual bool ReadFileHeader( MusFileHeader *header ) { return true; }
	virtual bool ReadParametersMidi( MusParametersMidi *midi ) { return true; }
	virtual bool ReadParameters2( MusParameters2 *param2 ) { return true; }
	virtual bool ReadFonts( ArrayOfWgFonts *fonts) { return true; }
	virtual bool ReadSeparator( ) { return true; }
	virtual bool ReadPage( MusPage *page ) { return true; }
	virtual bool ReadStaff( MusStaff *staff ) { return true; }
	virtual bool ReadNote( MusNote *note ) { return true; }
	virtual bool ReadSymbole( MusSymbol *symbole ) { return true; }
	virtual bool ReadElementAttr( MusElement *element ) { return true; }
	virtual bool ReadDebord( MusElement *element ) { return true; }
	virtual bool ReadPagination( MusPagination *pagination ) { return true; }
	virtual bool ReadHeaderFooter( MusHeaderFooter *headerfooter) { return true; }
    
public:
    // WDR: member variable declarations for MusFileInputStream

protected:
	// WDR: handler declarations for MusFileInputStream
	MusFile *m_file;

};


#endif
