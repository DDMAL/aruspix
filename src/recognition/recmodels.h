/////////////////////////////////////////////////////////////////////////////
// Name:        recmodels.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
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



//----------------------------------------------------------------------------
// RecModel
//----------------------------------------------------------------------------

class RecModel: public AxFile
{
public:
    // constructors and destructors
    RecModel( wxString filename, int type );
    virtual ~RecModel();
    
    virtual void OpenContent( ); // Open content after archive extraction
	virtual void SaveContent( ); // Save content before archive creation
	
	virtual void OpenModelContent( ) {}; // Open content after archive extraction
	virtual void SaveModelContent( ) {}; // Save content before archive creation
	virtual bool AddFile( wxArrayPtrVoid params, AxProgressDlg *dlg ) = 0;
	virtual bool AddFiles( wxArrayPtrVoid params, AxProgressDlg *dlg );

public:
       
protected:
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
    RecTypModel( wxString filename );
    virtual ~RecTypModel();
    
    virtual void NewContent(); // Create content for a new file
	virtual void OpenModelContent( ); // Open content after archive extraction
	virtual void SaveModelContent( ); // Save content before archive creation
	virtual void CloseContent( ); // Desactivate content before deletion
	
	virtual bool AddFile( wxArrayPtrVoid params, AxProgressDlg *dlg );
	bool Adapt( wxArrayPtrVoid params, AxProgressDlg *dlg );
	bool Train( wxArrayPtrVoid params, AxProgressDlg *dlg ); // train a new model, to be implemented
														   // some of the files (such as mfc) are not needed in
															// the trained typographic model
	bool Commit( AxProgressDlg *dlg );
	void UpdateInputFiles( );
        
public:
    
protected:
    MusMLFOutput *m_mlf_hmms;
   
private:
    };


//----------------------------------------------------------------------------
// RecMusModel
//----------------------------------------------------------------------------

class RecMusModel: public RecModel
{
public:
    // constructors and destructors
    RecMusModel( wxString filename );
    virtual ~RecMusModel();
    
    virtual void NewContent(); // Create content for a new file
	virtual void OpenModelContent( ); // Open content after archive extraction
	virtual void SaveModelContent( ); // Save content before archive creation
	virtual void CloseContent( ); // Desactivate content before deletion
	
	virtual bool AddFile( wxArrayPtrVoid params, AxProgressDlg *dlg );
	bool Adapt( wxArrayPtrVoid params, AxProgressDlg *dlg );
	bool Train( wxArrayPtrVoid params, AxProgressDlg *dlg );
	bool Commit( AxProgressDlg *dlg );

public:
       
private:
    int m_order;
};

#endif //AX_RECOGNITION

#endif
