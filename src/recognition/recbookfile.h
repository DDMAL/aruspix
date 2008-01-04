/////////////////////////////////////////////////////////////////////////////
// Name:        recbookfile.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __recbook_H__
#define __recbook_H__

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "recbookfile.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/dynarray.h"
//#include "wx/treectrl.h"

#include "app/axfile.h"

#include "rec_wdr.h"

class RecEnv;
class RecBookFile;
class RecBookCtrl;
class RecModel;

class RecBookFileItem;
WX_DECLARE_OBJARRAY( RecBookFileItem, ArrayOfBookFiles);

// WDR: class declarations

//----------------------------------------------------------------------------
// RecBookFileItem
//----------------------------------------------------------------------------

class RecBookFileItem: public wxObject
{
public:
    // constructors and destructors
    RecBookFileItem() { m_flags = 0; }
    RecBookFileItem( wxString filename, int flags = 0 ) { m_filename = filename; m_flags = flags; }
    ~RecBookFileItem() {};

public:
    wxString m_filename;
    int m_flags;
};

//----------------------------------------------------------------------------
// RecBookFile
//----------------------------------------------------------------------------

class RecBookFile: public AxFile 
{
public:
    // constructors and destructors
    RecBookFile::RecBookFile( wxString name, RecEnv *env = NULL );
    virtual RecBookFile::~RecBookFile();
    
    // WDR: method declarations for RecFile
    virtual void RecBookFile::NewContent(); // Create content for a new file
    virtual void RecBookFile::OpenContent( ); // Open content after archive extraction
    virtual void RecBookFile::SaveContent( ); // Save content before archive creation
    virtual void RecBookFile::CloseContent( ); // Desactivate content before deletion
    
    // operations
    bool LoadImages( );
    bool LoadAxfiles( );
    bool RemoveImage( wxString filename );
    bool DesactivateImage( wxString filename );
    bool RemoveAxfile( wxString filename );
    bool DeleteAxfile( wxString filename );
	bool DesactivateAxfile( wxString filename );
	// files
	int FilesToPreprocess( wxArrayString *filenames, wxArrayString *paths, bool add_axfiles = true );
	int FilesToRecognize( wxArrayString *filenames, wxArrayString *paths );
	int FilesForAdaptation( wxArrayString *filenames, wxArrayString *paths, bool *isCacheOk );
	int RecognizedFiles( wxArrayString *filenames, wxArrayString *paths );
	bool HasToBePreprocessed( wxString imagefile );
	// adaptation
	bool RecBookFile::ResetAdaptation( bool ask_user );
	bool RecBookFile::FastAdaptation( wxArrayPtrVoid params, ProgressDlg *dlg );
	bool RecBookFile::TypAdaptation( wxArrayPtrVoid params, ProgressDlg *dlg );
	bool RecBookFile::MusAdaptation( wxArrayPtrVoid params, ProgressDlg *dlg );
	wxString RecBookFile::GetTypFilename( ) { return m_basename + "book.axtyp";}
	wxString RecBookFile::GetMusFilename( ) { return m_basename + "book.axmus";}
	wxString GetTypCacheFilename( ) { return m_basename + "cache.axtyp";}
	wxString GetMusCacheFilename( ) { return m_basename + "cache.axmus";}
    
    // static on arrays
    static RecBookFileItem *FindFile( ArrayOfBookFiles *array, wxString filename, int* index  );
    
public:
    // WDR: member variable declarations for RecBookFile
    // infos
    wxString m_RISM;
    wxString m_Composer;
    wxString m_Title;
    wxString m_Printer;
    wxString m_Year;
    wxString m_Library;
    // files
    wxString m_axFileDir;
    wxString m_imgFileDir;
    ArrayOfBookFiles m_axFiles;
    ArrayOfBookFiles m_imgFiles;
	//
	bool m_fullOptimized; // a full Adaptation has been performed, don't use fast adaptation
	int m_nbFilesOptimization; // number of files used for full Adaptation
	wxArrayString m_optFiles;

protected:
    // WDR: member variable declarations for RecBookFile
    RecEnv *m_envPtr;


private:
    // WDR: handler declarations for RecBookFile

};

#endif
