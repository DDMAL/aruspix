/////////////////////////////////////////////////////////////////////////////
// Name:        axfile.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __axfile_H__
#define __axfile_H__

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


// values for AxFile::GetAllFiles
#define IMAGE_FILES 1

// IMPORTANT 
// Any change of MAX_FILE_TYPES must be reported in axfile.cpp where
// the enum values are mapped with const *char

#define MAX_FILE_TYPES 4

enum
{
	AX_FILE_DEFAULT = 0,
	AX_FILE_TYP_MODEL = 1,
	AX_FILE_MUS_MODEL = 2,
	AX_FILE_PROJECT = 3,
	AX_FILE_ZIP
};

// IMPORTANT 
// Any change of MAX_ENV_TYPES must be reported in axfile.cpp where
// the enum values are mapped with const *char

#define MAX_ENV_TYPES 4 

enum
{
	AX_FILE_RECOGNITION = 0,
	AX_FILE_SUPERIMPOSITION,
	AX_FILE_COMPARISON,
    AX_FILE_EDITION
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

class AxBookFileItem;
WX_DECLARE_OBJARRAY( AxBookFileItem, ArrayOfBookFileItems);

int SortBookFileItems( AxBookFileItem **first, AxBookFileItem **second );


//----------------------------------------------------------------------------
// AxBookFileItem
//----------------------------------------------------------------------------

class AxBookFileItem: public wxObject
{
public:
    // constructors and destructors
    AxBookFileItem() { m_flags = 0; }
    AxBookFileItem( wxString filename, int flags = 0 ) { m_filename = filename; m_flags = flags; }
    ~AxBookFileItem() {};
	
    // static on arrays
    static AxBookFileItem *FindFile( ArrayOfBookFileItems *array, wxString filename, int* index  );

public:
    wxString m_filename;
    int m_flags;
};

//----------------------------------------------------------------------------
// AxFile
//----------------------------------------------------------------------------

class AxFile
{
public:
    // constructors and destructors
    AxFile( wxString name, int type, int envtype );
    virtual ~AxFile();
	
    bool New();
	bool Open( wxString filename );
	bool Save( bool askUser = false );
	bool SaveAs( wxString filename = "" ); // overwrite if filename exists
	bool Close( bool askUser = false );
	bool Check( wxString filename ); // check the version, but also type and envtype ; uses the static version Check
	// status
	bool IsOpened() { return m_isOpened; }
    bool IsNew() { return m_filename.IsEmpty(); }
	bool IsModified() { return m_isModified; }
	void Modify() { m_isModified = true; }
	// process
	bool Terminate( int code = 0, ... );
	int GetError() { return m_error; }
	
	virtual void NewContent() {}; // Create content for a new file
	virtual void OpenContent( ) {}; // Open content after archive extraction
	virtual void SaveContent( ) {}; // Save content before archive creation
	virtual void CloseContent( ) {}; // Desactivate content before deletion
	
	// the static methods enable some informations on the file to be obtained before (or without) opening it completely
	// check version, and return type and envtype of a file (given by filename)
	static bool Check( wxString filename, int *type, int *envtype );
	static void GetVersion( TiXmlElement *root, int *vmaj, int *vmin, int *vrev );
	static wxString FormatVersion( int vmaj, int vmin, int vrev );
    static int GetAllFiles( wxString dirname, wxArrayString *files, int file_type );
	// file chooser
	static wxString Open( int file_type );
	//
	static wxString GetEnvName( int envtype );
	static bool ContainsFile( wxString filename, wxString search_filename );
	// unzip the preview file into the working directory and return path to it if success ("" otherwise)
	static wxString GetPreview( wxString filename, wxString preview );

    
public:
    wxString m_filename; 
	wxString m_shortname;
	wxString m_basename; // name of the directory where the file is extracted
	//TiXmlDocument m_dom;
	TiXmlElement *m_xml_root;
   
protected:
    int m_type;
	int m_envtype;
	bool m_isOpened;
	bool m_isModified;
	bool m_isNew;
	int m_error;
	// version
	int m_vmaj;
	int m_vmin;
	int m_vrev;
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
	virtual ~AxFileFunctor() {};

    // override function "Call"
    virtual void Call( AxFile *ptr, wxArrayPtrVoid params, ProcessDlg *dlg )
        { (*ptr.*fpt)( params, dlg );};          // execute member function
};


//----------------------------------------------------------------------------
// File selector function
//----------------------------------------------------------------------------

int AxFileSelector( int type, wxArrayString *filenames, wxArrayString *paths, wxWindow* parent );

#endif
