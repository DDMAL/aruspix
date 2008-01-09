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


#define TYP_THRESHOLD 0.01
#define TYP_ITER 100;
#define TYP_N_GAUSSIANS 8

#define MUS_NGRAM_ORDER 3

class MLFOutput;

// WDR: class declarations


//----------------------------------------------------------------------------
// RecModel
//----------------------------------------------------------------------------

class RecModel: public AxFile
{
public:
    // constructors and destructors
    RecModel::RecModel( wxString filename, int type );
    virtual RecModel::~RecModel();
    
    // WDR: method declarations for RecTypModel	
	virtual void OpenContent( ); // Open content after archive extraction
	virtual void SaveContent( ); // Save content before archive creation
	
	virtual void OpenModelContent( ) {}; // Open content after archive extraction
	virtual void SaveModelContent( ) {}; // Save content before archive creation
	virtual bool AddFile( wxArrayPtrVoid params, ProgressDlg *dlg ) = 0;
	virtual bool AddFiles( wxArrayPtrVoid params, ProgressDlg *dlg );

public:
    // WDR: member variable declarations for RecTypModel
   
protected:
    // WDR: handler declarations for RecTypModel
	MLFOutput *m_mlfoutput;
	int m_nbfiles;
	wxArrayString m_files;
};


//----------------------------------------------------------------------------
// RecTypModel
//----------------------------------------------------------------------------

class RecTypModel: public RecModel
{
public:
    // constructors and destructors
    RecTypModel::RecTypModel( wxString filename );
    virtual RecTypModel::~RecTypModel();
    
    // WDR: method declarations for RecTypModel
	virtual void RecTypModel::NewContent(); // Create content for a new file
	virtual void RecTypModel::OpenModelContent( ); // Open content after archive extraction
	virtual void RecTypModel::SaveModelContent( ); // Save content before archive creation
	virtual void RecTypModel::CloseContent( ); // Desactivate content before deletion
	
	virtual bool AddFile( wxArrayPtrVoid params, ProgressDlg *dlg );
	bool Adapt( wxArrayPtrVoid params, ProgressDlg *dlg );
	bool Train( wxArrayPtrVoid params, ProgressDlg *dlg ); // train a new model, to be implemented
														   // some of the files (such as mfc) are not needed in
															// the trained typographic model
	bool Commit( ProgressDlg *dlg );
	void UpdateInputFiles( );
        
public:
    // WDR: member variable declarations for RecTypModel
   
private:
    // WDR: handler declarations for RecTypModel
};


//----------------------------------------------------------------------------
// RecMusModel
//----------------------------------------------------------------------------

class RecMusModel: public RecModel
{
public:
    // constructors and destructors
    RecMusModel::RecMusModel( wxString filename );
    virtual RecMusModel::~RecMusModel();
    
    // WDR: method declarations for RecMusModel
	virtual void RecMusModel::NewContent(); // Create content for a new file
	virtual void RecMusModel::OpenModelContent( ); // Open content after archive extraction
	virtual void RecMusModel::SaveModelContent( ); // Save content before archive creation
	virtual void RecMusModel::CloseContent( ); // Desactivate content before deletion
	
	virtual bool AddFile( wxArrayPtrVoid params, ProgressDlg *dlg );
	bool Adapt( wxArrayPtrVoid params, ProgressDlg *dlg );
	bool Train( wxArrayPtrVoid params, ProgressDlg *dlg );
	bool Commit( ProgressDlg *dlg );

public:
    // WDR: member variable declarations for RecMusModel
   
private:
    // WDR: handler declarations for RecMusModel
	int m_order;
};


#endif
