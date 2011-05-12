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

#include "muspage.h"
#include "musstaff.h"
#include "muselement.h"
#include "musnote.h"
#include "mussymbol.h"
#include "musneume.h"
#include "mus.h"

WX_DECLARE_OBJARRAY( MusPage, ArrayOfMusPages);




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
    
    void CheckIntegrity();
    // moulinette
    virtual void Process(MusPageFunctor *functor, wxArrayPtrVoid params );
    void GetNumberOfVoices( int *min_voice, int *max_voice );
    MusStaff *GetVoice( int i );

    MeiDocument *GetMeiDocument();
    void SetMeiDocument(MeiDocument *doc);
    
public:
        /** nom complet du fichier */
    wxString m_fname;
    /** FileHeader du fichier - contient Parametres */
    MusFileHeader m_fheader;
    /** FileData du fichier - contient les pages */
    ArrayOfMusPages m_pages;
	/** Pagination **/
	MusPagination m_pagination;
	/** Header **/
	MusHeaderFooter m_header;
	/** Footer **/
	MusHeaderFooter m_footer;
	/** Separateur **/
	static char* sep;

private:
    MeiDocument *m_meidoc;
	
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
    
    virtual bool ExportFile( ) { return true; }
	virtual bool WriteFileHeader( const MusFileHeader *header ) { return true; }
	virtual bool WriteParametersMidi( ) { return true; }
	virtual bool WriteParameters2( const MusParameters *param ) { return true; }
	virtual bool WriteFonts( ) { return true; }
	virtual bool WriteSeparator( ) { return true; }
	virtual bool WritePage( const MusPage *page ) { return true; }
	virtual bool WriteStaff( const MusStaff *staff ) { return true; }
	virtual bool WriteNote( const MusNote *note ) { return true; }
	virtual bool WriteSymbol( const MusSymbol *symbol ) { return true; }
	virtual bool WriteElementAttr( const MusElement *element ) { return true; }
	virtual bool WriteDebord( const MusElement *element ) { return true; }
	virtual bool WriteLyric( const MusElement * element ) { return true; }
	virtual bool WritePagination( const MusPagination *pagination ) { return true; }
	virtual bool WriteHeaderFooter( const MusHeaderFooter *headerfooter) { return true; }
    
public:
    
protected:
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
    MusFileInputStream( MusFile *file, int fr );
    virtual ~MusFileInputStream();
    
    // read
	virtual bool ImportFile( ) { return true; }
	virtual bool ReadFileHeader( MusFileHeader *header ) { return true; }
	virtual bool ReadParametersMidi( ) { return true; }
	virtual bool ReadParameters2( MusParameters *param ) { return true; }
	virtual bool ReadFonts( ) { return true; }
	virtual bool ReadSeparator( ) { return true; }
	virtual bool ReadPage( MusPage *page ) { return true; }
	virtual bool ReadStaff( MusStaff *staff ) { return true; }
	virtual bool ReadNote( MusNote *note ) { return true; }
	virtual bool ReadSymbol( MusSymbol *symbol ) { return true; }
	virtual bool ReadElementAttr( MusElement *element ) { return true; }
	virtual bool ReadDebord( MusElement *element ) { return true; }
	virtual bool ReadLyric( MusElement * element ) { return true; }
	virtual bool ReadPagination( MusPagination *pagination ) { return true; }
	virtual bool ReadHeaderFooter( MusHeaderFooter *headerfooter) { return true; }
    
public:
    
protected:
		MusFile *m_file;

};


#endif
