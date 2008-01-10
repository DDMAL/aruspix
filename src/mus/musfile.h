/////////////////////////////////////////////////////////////////////////////
// Name:        wgfile.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_FILE_H__
#define __MUS_FILE_H__

#ifdef AX_WG

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

WX_DECLARE_OBJARRAY( WgPage, ArrayOfWgPages);

WX_DECLARE_OBJARRAY( WgFont, ArrayOfWgFonts);



// WDR: class declarations


//----------------------------------------------------------------------------
// WgFile
//----------------------------------------------------------------------------

class WgFile
{
	//friend class WgFileOutputStream;
	friend class WgFileInputStream;

public:
    // constructors and destructors
    WgFile();
    virtual ~WgFile();
    
    // WDR: method declarations for WgFile
	void CheckIntegrity();
    
public:
    // WDR: member variable declarations for WgFile
    /** nom complet du fichier */
    wxString m_fname;
    /** FileHeader du fichier - contient Parametres */
    WgFileHeader m_fheader;
    /** ParametresMidi du fichier */
    WgParametersMidi m_midi;
    /** Parametres2 du fichier */
    WgParameters2 m_param2;
    /** PolicesTable du fichier */
    ArrayOfWgFonts m_fonts;
    /** FileData du fichier - contient les pages */
    ArrayOfWgPages m_pages;
	/** Masque fixe **/
	WgPage m_masqueFixe;
	/** Masque fixe **/
	WgPage m_masqueVariable;
	/** Pagination **/
	WgPagination m_pagination;
	/** Header **/
	WgHeaderFooter m_header;
	/** Footer **/
	WgHeaderFooter m_footer;
	/** Separateur **/
	static char* sep;

private:
	// WDR: handler declarations for WgFile

};


//----------------------------------------------------------------------------
// WgFileOutputStream
//----------------------------------------------------------------------------

class WgFileOutputStream: public wxFileOutputStream
{
public:
    // constructors and destructors
    WgFileOutputStream( WgFile *file, wxString filename );
	WgFileOutputStream( WgFile *file, int fd );
	//WgFileOutputStream( WgFile *file, wxFile *wxfile );
    virtual ~WgFileOutputStream();
    
    // WDR: method declarations for WgFileOutputStream
	virtual bool ExportFile( ) { return true; }
	virtual bool WriteFileHeader( const WgFileHeader *header ) { return true; }
	virtual bool WriteParametersMidi( const WgParametersMidi *midi ) { return true; }
	virtual bool WriteParameters2( const WgParameters2 *param2 ) { return true; }
	virtual bool WriteFonts( const ArrayOfWgFonts *fonts ) { return true; }
	virtual bool WriteSeparator( ) { return true; }
	virtual bool WritePage( const WgPage *page ) { return true; }
	virtual bool WriteStaff( const WgStaff *staff ) { return true; }
	virtual bool WriteNote( const WgNote *note ) { return true; }
	virtual bool WriteSymbole( const WgSymbole *symbole ) { return true; }
	virtual bool WriteElementAttr( const WgElement *element ) { return true; }
	virtual bool WriteDebord( const WgElement *element ) { return true; }
	virtual bool WritePagination( const WgPagination *pagination ) { return true; }
	virtual bool WriteHeaderFooter( const WgHeaderFooter *headerfooter) { return true; }
    
public:
    // WDR: member variable declarations for WgFileOutputStream

protected:
	// WDR: handler declarations for WgFileOutputStream
	WgFile *m_file;

};

//----------------------------------------------------------------------------
// WgFileInputStream
//----------------------------------------------------------------------------

class WgFileInputStream: public wxFileInputStream
{
public:
    // constructors and destructors
    WgFileInputStream( WgFile *file, wxString filename );
    virtual ~WgFileInputStream();
    
    // WDR: method declarations for WgFileInputStream
	// read
	virtual bool ImportFile( ) { return true; }
	virtual bool ReadFileHeader( WgFileHeader *header ) { return true; }
	virtual bool ReadParametersMidi( WgParametersMidi *midi ) { return true; }
	virtual bool ReadParameters2( WgParameters2 *param2 ) { return true; }
	virtual bool ReadFonts( ArrayOfWgFonts *fonts) { return true; }
	virtual bool ReadSeparator( ) { return true; }
	virtual bool ReadPage( WgPage *page ) { return true; }
	virtual bool ReadStaff( WgStaff *staff ) { return true; }
	virtual bool ReadNote( WgNote *note ) { return true; }
	virtual bool ReadSymbole( WgSymbole *symbole ) { return true; }
	virtual bool ReadElementAttr( WgElement *element ) { return true; }
	virtual bool ReadDebord( WgElement *element ) { return true; }
	virtual bool ReadPagination( WgPagination *pagination ) { return true; }
	virtual bool ReadHeaderFooter( WgHeaderFooter *headerfooter) { return true; }
    
public:
    // WDR: member variable declarations for WgFileInputStream

protected:
	// WDR: handler declarations for WgFileInputStream
	WgFile *m_file;

};


#endif //AX_WG

#endif
