/////////////////////////////////////////////////////////////////////////////
// Name:        axfile.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __axfile_H__
#define __axfile_H__

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "axfile.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/zipstrm.h"
#include "wx/wfstream.h"

#include "axprogressdlg.h"

// TINYXML
#if defined (__WXMSW__)
    #include "tinyxml.h"
#else
    #include "tinyxml/tinyxml.h"
#endif

class ProcessDlg;

//#include "wx/ptr_scpd.h"
//wxDECLARE_SCOPED_PTR(wxZipEntry, wxZipEntryPtr);

enum
{
	AX_FILE_DEFAULT = 0,
	AX_FILE_TYP_MODEL = 1,
	AX_FILE_MUS_MODEL = 2,
	AX_FILE_PROJECT = 3,
	AX_FILE_ZIP
};

enum
{
	AX_FILE_RECOGNITION = 0,
	AX_FILE_SUPERIMPOSITION,
	AX_FILE_COMPARISON
};

enum
{
	AX_FILE_TIFF = 10000
};

enum
{
    FILE_DESACTIVATED = (1<<0)
};

enum
{
    PART_REFERENCE = (1<<0)
};

// WDR: class declarations

//----------------------------------------------------------------------------
// AxFile
//----------------------------------------------------------------------------

class AxFile
{
public:
    // constructors and destructors
    AxFile::AxFile( wxString name, int type, int envtype );
    virtual AxFile::~AxFile();
	
    // WDR: method declarations for AxFile
	bool AxFile::New();
	bool AxFile::Open( wxString filename );
	bool AxFile::Save( bool askUser = false );
	bool AxFile::SaveAs( wxString filename = "" ); // overwrite if filename exists
	bool AxFile::Close( bool askUser = false );
	bool AxFile::Check( wxString filename ); // check the version, but also type and envtype ; uses the static version AxFile::Check
	// status
	bool AxFile::IsOpened() { return m_isOpened; }
	bool AxFile::IsModified() { return m_isModified; }
	void AxFile::Modify() { m_isModified = true; }
	// process
	bool AxFile::Terminate( int code = 0, ... );
	int AxFile::GetError() { return m_error; }
	
	virtual void AxFile::NewContent() {}; // Create content for a new file
	virtual void AxFile::OpenContent( ) {}; // Open content after archive extraction
	virtual void AxFile::SaveContent( ) {}; // Save content before archive creation
	virtual void AxFile::CloseContent( ) {}; // Desactivate content before deletion
	
	// check version, and return type and envtype of a file (given by filename)
	static bool AxFile::Check( wxString filename, int *type, int *envtype );
	// file chooser
	static wxString AxFile::Open( int file_type );
	//
	static wxString AxFile::GetEnvName( int envtype );
	// unzip the preview file into the working directory and return path to it if success ("" otherwise)
	static wxString AxFile::GetPreview( wxString filename, wxString preview );

    
public:
    // WDR: member variable declarations for AxFile
	wxString m_filename; 
	wxString m_shortname;
	wxString m_basename; // name of the directory where the file is extracted
	//TiXmlDocument m_dom;
	TiXmlElement *m_xml_root;
   
protected:
    // WDR: handler declarations for AxFile
	int m_type;
	int m_envtype;
	bool m_isOpened;
	bool m_isModified;
	bool m_isNew;
	int m_error;
};


//----------------------------------------------------------------------------
// abstract base class AxFileFunctor
//----------------------------------------------------------------------------
class AxFileFunctor
{
private:
    void (AxFile::*fpt)( wxArrayPtrVoid params, ProcessDlg *dlg );   // pointer to member function

public:

    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    AxFileFunctor( void(AxFile::*_fpt)(wxArrayPtrVoid, ProcessDlg* )) { fpt=_fpt; };
	virtual AxFileFunctor::~AxFileFunctor() {};

    // override function "Call"
    virtual void Call( AxFile *ptr, wxArrayPtrVoid params, ProcessDlg *dlg )
        { (*ptr.*fpt)( params, dlg );};          // execute member function
};


//----------------------------------------------------------------------------
// File selector function
//----------------------------------------------------------------------------

int AxFileSelector( int type, wxArrayString *filenames, wxArrayString *paths, wxWindow* parent );

#endif
