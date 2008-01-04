/////////////////////////////////////////////////////////////////////////////
// Name:        models.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __models_H__
#define __models_H__

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "models.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/zipstrm.h"
#include "wx/wfstream.h"

#include "app/axfile.h"

class MLFOutput;

// WDR: class declarations


/*

//----------------------------------------------------------------------------
// RecModel
//----------------------------------------------------------------------------

class RecModel
{
public:
    // constructors and destructors
    RecModel::RecModel( wxString filename );
    virtual RecModel::~RecModel();
	
    // WDR: method declarations for RecModel
	void RecModel::Read();
	void RecModel::Write();
	
	virtual void RecModel::ReadContent( wxZipInputStream *zip, wxString name ) {};
	virtual void RecModel::WriteContent( wxZipOutputStream *zip, wxFFileOutputStream *file, wxString name ) {};

    
public:
    // WDR: member variable declarations for RecModel
	wxString m_filename; 
	wxString m_shortname;
	wxString m_basename; // name of the directory where the model is extracted
	bool m_isLoaded;
   
private:
    // WDR: handler declarations for RecModel

};

*/


//----------------------------------------------------------------------------
// RecTypModel
//----------------------------------------------------------------------------

class RecTypModel: public AxFile
{
public:
    // constructors and destructors
    RecTypModel::RecTypModel( wxString filename );
    virtual RecTypModel::~RecTypModel();
    
    // WDR: method declarations for RecTypModel
	virtual void RecTypModel::NewContent(); // Create content for a new file
	virtual void RecTypModel::OpenContent( ) {}; // Open content after archive extraction
	virtual void RecTypModel::SaveContent( ) {}; // Save content before archive creation
	virtual void RecTypModel::CloseContent( ) {}; // Desactivate content before deletion
	
	bool AddFile( wxArrayPtrVoid params, ProgressDlg *dlg );
	bool AddFiles( wxArrayPtrVoid params, ProgressDlg *dlg );
	bool Adapt( wxArrayPtrVoid params, ProgressDlg *dlg );
	bool Commit( ProgressDlg *dlg );
	void UpdateInputFiles( );
        
public:
    // WDR: member variable declarations for RecTypModel
   
private:
    // WDR: handler declarations for RecTypModel
	MLFOutput *m_mlfoutput;
	int m_nbfiles;

};


//----------------------------------------------------------------------------
// RecMusModel
//----------------------------------------------------------------------------

class RecMusModel: public AxFile
{
public:
    // constructors and destructors
    RecMusModel::RecMusModel( wxString filename );
    virtual RecMusModel::~RecMusModel();
    
    // WDR: method declarations for RecMusModel
	virtual void RecMusModel::NewContent(); // Create content for a new file
	virtual void RecMusModel::OpenContent( ) {}; // Open content after archive extraction
	virtual void RecMusModel::SaveContent( ) {}; // Save content before archive creation
	virtual void RecMusModel::CloseContent( ) {}; // Desactivate content before deletion
	
	bool AddFiles( wxArrayPtrVoid params, ProgressDlg *dlg );
	bool AddFile( wxArrayPtrVoid params, ProgressDlg *dlg );
	bool Commit( ProgressDlg *dlg );

public:
    // WDR: member variable declarations for RecMusModel
   
private:
    // WDR: handler declarations for RecMusModel
	MLFOutput *m_mlf_no_mapping;
	MLFOutput *m_mlf_symb;
	MLFOutput *m_mlf_duration;
	MLFOutput *m_mlf_pitch;
	MLFOutput *m_mlf_intervals;
	int m_nbfiles;
	int m_order;


};


#endif
