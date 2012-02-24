/////////////////////////////////////////////////////////////////////////////
// Name:        musdoc.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_DOC_H__
#define __MUS_DOC_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"

class MusPageFunctor;
class MeiDocument;
class MusDiv;

#include "musobject.h"
#include "musdef.h"
#include "muswwg.h"


//----------------------------------------------------------------------------
// MusDoc
//----------------------------------------------------------------------------

/** 
 * This class is a hold the data and corresponds to the model of a MVC design pattern.
 */
class MusDoc
{
	//friend class MusFileOutputStream;
	friend class MusFileInputStream;

public:
    // constructors and destructors
    MusDoc();
    virtual ~MusDoc();
	
	void AddDiv( MusDiv *div );
	
	void AddLayout( MusLayout *layout );
    
    /*
     * Clear the content of the document.
     */ 
    void Reset();
    
    // moulinette
    void GetNumberOfVoices( int *min_voice, int *max_voice );
    MusLaidOutStaff *GetVoice( int i );

    MeiDocument *GetMeiDocument();
    void SetMeiDocument(MeiDocument *doc);
    
private:
    
public:
    /** nom complet du fichier */
    wxString m_fname;
    
    /** The layouts */
    ArrayOfMusLayouts m_layouts;
    ArrayOfMusDivs m_divs;
    
    MusEnv m_env;
	
    /** Data loaded from the Wolfgang files but unused **/
	MusWWGData m_wwgData;
    


private:
    MeiDocument *m_meidoc;
    
	
};


//----------------------------------------------------------------------------
// MusFileOutputStream
//----------------------------------------------------------------------------

/** 
 * This class is a base class for file output stream classes.
 * It is not an abstract class but should not be instanciate directly.
 */ 
class MusFileOutputStream: public wxFileOutputStream
{
public:
    // constructors and destructors
    MusFileOutputStream( MusDoc *doc, wxString filename );
	MusFileOutputStream( MusDoc *doc, int fd );
    MusFileOutputStream() {};
	//MusFileOutputStream( MusDoc *file, wxFile *wxfile );
    virtual ~MusFileOutputStream();
    
    bool ExportFile( ) { return true; }
	/*
    virtual bool WriteFileHeader( const MusFileHeader *header ) { return true; }
	virtual bool WriteParametersMidi( ) { return true; }
	virtual bool WriteParameters2( const MusEnv *param ) { return true; }
	virtual bool WriteFonts( ) { return true; }
	virtual bool WriteSeparator( ) { return true; }
	virtual bool WritePage( const MusPage *page ) { return true; }
    virtual bool WriteSystem( const MusSystem *system ) { return true; }
	virtual bool WriteStaff( const MusLaidOutStaff *staff ) { return true; }
	virtual bool WriteNote( const MusNote1 *note ) { return true; }
	virtual bool WriteSymbol( const MusSymbol1 *symbol ) { return true; }
	virtual bool WriteElementAttr( const MusElement *element ) { return true; }
	virtual bool WriteDebord( const MusElement *element ) { return true; }
	virtual bool WriteLyric( const MusElement * element ) { return true; }
	virtual bool WritePagination( const MusWWGData *pagination ) { return true; }
	virtual bool WriteHeaderFooter( const MusWWGData *headerfooter) { return true; }
    */
    
public:
    
protected:
    MusDoc *m_doc;

};

//----------------------------------------------------------------------------
// MusFileInputStream
//----------------------------------------------------------------------------

/** 
 * This class is a base class for file input stream classes.
 * It is not an abstract class but should not be instanciate directly.
 */ 
class MusFileInputStream: public wxFileInputStream
{
public:
    // constructors and destructors
    MusFileInputStream( MusDoc *doc, wxString filename );
    MusFileInputStream( MusDoc *doc, int fr );
    MusFileInputStream() {};
    virtual ~MusFileInputStream();
    
    // read
    bool ImportFile( ) { return true; }
    /*
	virtual bool ReadFileHeader( MusFileHeader *header ) { return true; }
	virtual bool ReadParametersMidi( ) { return true; }
	virtual bool ReadParameters2( MusEnv *param ) { return true; }
	virtual bool ReadFonts( ) { return true; }
	virtual bool ReadSeparator( ) { return true; }
	virtual bool ReadPage( MusPage *page ) { return true; }
    virtual bool ReadSystem( MusSystem *system ) { return true; }
	virtual bool ReadStaff( MusLaidOutStaff *staff ) { return true; }
	virtual bool ReadNote( MusNote1 *note ) { return true; }
	virtual bool ReadSymbol( MusSymbol1 *symbol ) { return true; }
	virtual bool ReadElementAttr( MusElement *element ) { return true; }
	virtual bool ReadDebord( MusElement *element ) { return true; }
	virtual bool ReadLyric( MusElement * element ) { return true; }
	virtual bool ReadPagination( MusWWGData *pagination ) { return true; }
	virtual bool ReadHeaderFooter( MusWWGData *headerfooter) { return true; }
    */
    
public:
    
protected:
    MusDoc *m_doc;

};


#endif
