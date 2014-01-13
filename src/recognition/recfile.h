/////////////////////////////////////////////////////////////////////////////
// Name:        recfile.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __recfile_H__
#define __recfile_H__

#ifdef AX_RECOGNITION

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "app/axfile.h"

class ImPage;
class Doc;
class RecEnv;
class RecTypModel;
class RecMusModel;
class AxImage;



//----------------------------------------------------------------------------
// RecFile
//----------------------------------------------------------------------------

class RecFile: public AxFile 
{
public:
    // constructors and destructors
    RecFile( wxString name, RecEnv *env = NULL );
    virtual ~RecFile();
    
    /** @name File methods
     * Standard file operation (new, open, save, close) methods for this AxFile child class.
     */
    ///@{
    virtual void NewContent(); // Create content for a new file
    virtual void OpenContent( ); // Open content after archive extraction
    virtual void SaveContent( ); // Save content before archive creation
    virtual void CloseContent( ); // Desactivate content before deletion
    ///@}
	
	void SetforMEI(); //needs to be fooled into thinking it's been preprocessed and recognized
	void SetImage( wxString filename );
	void SetImage( AxImage *image );
	void GetImage0( AxImage *image );
	void GetImage1( AxImage *image );
	bool CancelRecognition( bool ask_user ); // remove all recognition files
	// functor
	bool Preprocess( wxArrayPtrVoid params, AxProgressDlg *dlg );
	bool Recognize( wxArrayPtrVoid params, AxProgressDlg *dlg );
    bool Decode( wxArrayPtrVoid params, AxProgressDlg *dlg );
	bool RealizeFromMLF( wxArrayPtrVoid params, AxProgressDlg *dlg );
	bool GenerateMFC( wxArrayPtrVoid params, AxProgressDlg *dlg );
    //int DoCorrelation( ImPage *imPage );
	// getters
	Doc *GetMusFile() { return m_musDocPtr; }
	ImPage *GetImPage() { return m_imPagePtr; }
	//RecTypModel *GetTypModel() { return m_typModelPtr; }
	
	// status
	bool IsPreprocessed() { return m_isPreprocessed; }
	bool IsRecognized() {  return m_isRecognized;  } 
	
	// backward compatibility to be check when a opening the file
	void UpgradeTo_1_4_0();
    void UpgradeTo_1_5_0();
    void UpgradeTo_2_0_0();
    void UpgradeTo_2_1_0();
    void UpgradeTo_2_3_0();
	
	// status
	static bool IsRecognized( wxString filename );
	
	// setter
	void SetBinarization( int image_binarization_method, int page_binarization_method, int page_binarization_size );
    
public:
    RecEnv *m_envPtr;
	ImPage *m_imPagePtr;
	Doc *m_musDocPtr;
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
    bool m_isPreprocessed;
	bool m_isRecognized;
   
private:
    
};

#endif //AX_RECOGNITION

#endif
