/////////////////////////////////////////////////////////////////////////////
// Name:        recbookfile.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __recbook_H__
#define __recbook_H__

#ifdef AX_RECOGNITION

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




//----------------------------------------------------------------------------
// RecBookFile
//----------------------------------------------------------------------------

class RecBookFile: public AxFile 
{
public:
    // constructors and destructors
    RecBookFile( wxString name, RecEnv *env = NULL );
    virtual ~RecBookFile();
    
        virtual void NewContent(); // Create content for a new file
    virtual void OpenContent( ); // Open content after archive extraction
    virtual void SaveContent( ); // Save content before archive creation
    virtual void CloseContent( ); // Desactivate content before deletion
    
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
	bool ResetAdaptation( bool ask_user );
	bool FastAdaptation( wxArrayPtrVoid params, AxProgressDlg *dlg );
	bool TypAdaptation( wxArrayPtrVoid params, AxProgressDlg *dlg );
	bool MusAdaptation( wxArrayPtrVoid params, AxProgressDlg *dlg );
	wxString GetTypFilename( ) { return m_basename + "book.axtyp";}
	wxString GetMusFilename( ) { return m_basename + "book.axmus";}
	wxString GetTypCacheFilename( ) { return m_basename + "cache.axtyp";}
	wxString GetMusCacheFilename( ) { return m_basename + "cache.axmus";}
    
public:
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
    ArrayOfBookFileItems m_axFiles;
    ArrayOfBookFileItems m_imgFiles;
	//
	bool m_fullOptimized; // a full Adaptation has been performed, don't use fast adaptation
	int m_nbFilesOptimization; // number of files used for full Adaptation
	wxArrayString m_optFiles;
	
	// binarization
	int m_pre_image_binarization_method;
	int m_pre_page_binarization_method;
	int m_pre_page_binarization_method_size;
	bool m_pre_page_binarization_select;

protected:
        RecEnv *m_envPtr;

private:
    
};

#endif //AX_RECOGNITION

#endif
