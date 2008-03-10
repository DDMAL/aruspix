/////////////////////////////////////////////////////////////////////////////
// Name:        recmodels.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __recmodels_H__
#define __recmodels_H__

#ifdef AX_RECOGNITION

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "recmodels.cpp"
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

#include "mus/musiomlf.h"
#include "mus/musmlfdic.h"

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
    
    // WDR: method declarations for RecModel	
	virtual void OpenContent( ); // Open content after archive extraction
	virtual void SaveContent( ); // Save content before archive creation
	
	virtual void OpenModelContent( ) {}; // Open content after archive extraction
	virtual void SaveModelContent( ) {}; // Save content before archive creation
	virtual bool AddFile( wxArrayPtrVoid params, AxProgressDlg *dlg ) = 0;
	virtual bool AddFiles( wxArrayPtrVoid params, AxProgressDlg *dlg );

public:
    // WDR: member variable declarations for RecModel
   
protected:
    // WDR: handler declarations for RecModel
	MusMLFOutput *m_mlf;
	MusMLFDictionary m_mlfDic;
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
	
	virtual bool AddFile( wxArrayPtrVoid params, AxProgressDlg *dlg );
	bool Adapt( wxArrayPtrVoid params, AxProgressDlg *dlg );
	bool Train( wxArrayPtrVoid params, AxProgressDlg *dlg ); // train a new model, to be implemented
														   // some of the files (such as mfc) are not needed in
															// the trained typographic model
	bool Commit( AxProgressDlg *dlg );
	void UpdateInputFiles( );
        
public:
    // WDR: member variable declarations for RecTypModel
	
protected:
    // WDR: handler declarations for RecTypModel
	MusMLFOutput *m_mlf_hmms;
   
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
	
	virtual bool AddFile( wxArrayPtrVoid params, AxProgressDlg *dlg );
	bool Adapt( wxArrayPtrVoid params, AxProgressDlg *dlg );
	bool Train( wxArrayPtrVoid params, AxProgressDlg *dlg );
	bool Commit( AxProgressDlg *dlg );

public:
    // WDR: member variable declarations for RecMusModel
   
private:
    // WDR: handler declarations for RecMusModel
	int m_order;
};

#endif //AX_RECOGNITION

#endif
