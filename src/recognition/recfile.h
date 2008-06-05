/////////////////////////////////////////////////////////////////////////////
// Name:        recfile.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __recfile_H__
#define __recfile_H__

#ifdef AX_RECOGNITION

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "recfile.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "app/axfile.h"

class ImPage;
class MusFile;
class RecEnv;
class RecTypModel;
class RecMusModel;
class AxImage;

// WDR: class declarations


//----------------------------------------------------------------------------
// RecFile
//----------------------------------------------------------------------------

class RecFile: public AxFile 
{
public:
    // constructors and destructors
    RecFile::RecFile( wxString name, RecEnv *env = NULL );
    virtual RecFile::~RecFile();
    
    // WDR: method declarations for RecFile
	virtual void RecFile::NewContent(); // Create content for a new file
	virtual void RecFile::OpenContent( ); // Open content after archive extraction
	virtual void RecFile::SaveContent( ); // Save content before archive creation
	virtual void RecFile::CloseContent( ); // Desactivate content before deletion
	
	void RecFile::SetImage( wxString filename );
	void RecFile::SetImage( AxImage *image );
	void RecFile::GetImage0( AxImage *image );
	void RecFile::GetImage1( AxImage *image );
	bool RecFile::CancelRecognition( bool ask_user ); // remove all recognition files
	void RecFile::WriteNoPitchMLF( ); // write MLF Without Pitch using the current page content
	// functor
	bool RecFile::Preprocess( wxArrayPtrVoid params, AxProgressDlg *dlg );
	bool RecFile::Recognize( wxArrayPtrVoid params, AxProgressDlg *dlg );
    bool RecFile::Decode( wxArrayPtrVoid params, AxProgressDlg *dlg );
	bool RecFile::RealizeFromMLF( wxArrayPtrVoid params, AxProgressDlg *dlg );
	bool RecFile::GenerateMFC( wxArrayPtrVoid params, AxProgressDlg *dlg );
    //int DoCorrelation( ImPage *imPage );
	// getters
	MusFile *RecFile::GetWgFile() { return m_musFilePtr; }
	ImPage *RecFile::GetImPage() { return m_imPagePtr; }
	//RecTypModel *RecFile::GetTypModel() { return m_typModelPtr; }
	
	// status
	bool RecFile::IsPreprocessed() { return m_isPreprocessed; }
	bool RecFile::IsRecognized() {  return m_isRecognized;  } 
	
	// backward compatibility to be check when a opening the file
	void RecFile::UpgradeTo_1_4_0();
	
	// status
	static bool RecFile::IsRecognized( wxString filename );
	
	// setter
	void RecFile::SetBinarization( int image_binarization_method, int page_binarization_method, int page_binarization_size );
    
public:
    // WDR: member variable declarations for RecFile
	RecEnv *m_envPtr;
	ImPage *m_imPagePtr;
	MusFile *m_musFilePtr;
	// filename
	// models
	//RecTypModel *m_typModelPtr; // not used yet
	//RecMusModel *m_musModelPtr;	// not used yet
	
    // decoder
    wxString m_rec_typ_model; // am_models_fname
	wxString m_rec_mus_model; // am_models_fname
    //double m_rec_phone_pen; // am_phone_del_pen
    //wxString m_rec_dict; // lex_dict_fname
    wxString m_rec_wrdtrns; // wrdtrns_fname
    //wxString m_rec_output; // output_fname
    //wxString m_rec_lm; // lm_fname
    //double m_rec_end_prune; // dec_end_prune_window
    //double m_rec_word_pen; // dec_word_entr_pen
    bool m_rec_delayed; // dec_delayed_lm
    int m_rec_lm_order; // lm_ngram_order
    double m_rec_lm_scaling; // lm_scaling_factor
	
	// binarization
	int m_pre_image_binarization_method;
	int m_pre_page_binarization_method;
	int m_pre_page_binarization_method_size;

protected:
    // WDR: member variable declarations for RecFile
	bool m_isPreprocessed;
	bool m_isRecognized;
   
private:
    // WDR: handler declarations for RecFile

};

#endif //AX_RECOGNITION

#endif
