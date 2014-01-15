/////////////////////////////////////////////////////////////////////////////
// Name:        axoptionsdlg.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __axoptionsdlg_H__
#define __axoptionsdlg_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "axoptions_wdr.h"
#include "musapp/muswindow.h"

#include "vrv/doc.h"

// XXX: Doesn't compile if AX_RECOGNITION isn't set
#ifdef AX_RECOGNITION
	#include "recognition/recfile.h"
	#include "recognition/recbookfile.h"
#endif

#define SAUVOLA_DESCRIPTION _("Uneven lighting")
#define BRINK_2CLASSES_DESCRIPTION _("Clean")
#define BRINK_3CLASSES_DESCRIPTION _("Bleedthough")

class AxOptMusWindow;

//----------------------------------------------------------------------------
// AxOptionsDlg
//----------------------------------------------------------------------------

class AxOptionsDlg: public wxDialog
{
public:
    // constructors and destructors
    AxOptionsDlg( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE );
    
    wxPanel* GetMusPanel()  { return (wxPanel*) FindWindow( ID_PANEL_WINKEY ); }
    wxSpinCtrl* GetScMusOffset()  { return (wxSpinCtrl*) FindWindow( ID_SC_MUS_OFFSET ); }
    wxSpinCtrl* GetScMusSize()  { return (wxSpinCtrl*) FindWindow( ID_SC_MUS_SIZE ); }
    wxStaticText* GetFontName()  { return (wxStaticText*) FindWindow( ID_FONT_NAME ); }
	wxStaticText* GetLyricFontName()  { return (wxStaticText*) FindWindow( ID_LYRIC_FONT_NAME ); }
	wxTextCtrl* GetDocDir()  { return (wxTextCtrl*) FindWindow( ID_DOCUMENTS_DIR ); }
    wxTextCtrl* GetWorkingDir()  { return (wxTextCtrl*) FindWindow( ID_WORKING_DIR ); }
    wxComboBox* GetCbLanguageOps()  { return (wxComboBox*) FindWindow( ID_CB_LANGUAGE_OPS ); }
    wxCheckBox* GetCbGrayOpi()  { return (wxCheckBox*) FindWindow( ID_CB_GRAY_OPI ); }
    wxTextCtrl* GetTcMaxsizeOpi()  { return (wxTextCtrl*) FindWindow( ID_TC_MAXSIZE_OPI ); }
    wxCheckBox* GetCbReduceOpi()  { return (wxCheckBox*) FindWindow( ID_CB_REDUCE_OPI ); }
	wxCheckBox* GetCbNegativeOpi()  { return (wxCheckBox*) FindWindow( ID_CB_NEGATIVE_OPI ); }
	wxChoice* GetRbImageBin()  { return (wxChoice*) FindWindow( ID_C_IMAGE_BINARIZATION ); }
	
#ifdef AX_SUPERIMPOSITION
    wxCheckBox* GetChFilter2Ops1()  { return (wxCheckBox*) FindWindow( ID2_CH_FILTER2_OPS1 ); }
    wxCheckBox* GetChFilter1Ops1()  { return (wxCheckBox*) FindWindow( ID2_CH_FILTER1_OPS1 ); }
    wxSpinCtrl* GetScCorryOps1()  { return (wxSpinCtrl*) FindWindow( ID2_SC_CORRY_OPS1 ); }
    wxSpinCtrl* GetScCorrxOps1()  { return (wxSpinCtrl*) FindWindow( ID2_SC_CORRX_OPS1 ); }
    wxSpinCtrl* GetScSplityOps1()  { return (wxSpinCtrl*) FindWindow( ID2_SC_SPLITY_OPS1 ); }
    wxSpinCtrl* GetScSplitxOps1()  { return (wxSpinCtrl*) FindWindow( ID2_SC_SPLITX_OPS1 ); }
    wxChoice* GetCcInterpOps1()  { return (wxChoice*) FindWindow( ID2_CC_INTERP_OPS1 ); }
    wxSpinCtrl* GetScSubWindowOps1()  { return (wxSpinCtrl*) FindWindow( ID2_SC_SUBWINDOW_OPS1 ); }
#endif
	
//#ifdef AX_RECOGNITION	
    wxTextCtrl* GetLMScaling()  { return (wxTextCtrl*) FindWindow( ID4_LMSCALING ); }
    wxSpinCtrl* GetLMOrder()  { return (wxSpinCtrl*) FindWindow( ID4_REC_LMORDER ); }
    wxCheckBox* GetDelayed()  { return (wxCheckBox*) FindWindow( ID4_REC_DELAYED ); }
    wxTextCtrl* GetTypModel()  { return (wxTextCtrl*) FindWindow( ID4_REC_TYP_MODELS ); }
	wxTextCtrl* GetMusModel()  { return (wxTextCtrl*) FindWindow( ID4_REC_MUS_MODELS ); }
    wxSpinCtrl* GetScMarginTopOps3()  { return (wxSpinCtrl*) FindWindow( ID4_MARGIN_TOP ); }
    wxSpinCtrl* GetScMarginBottomOps3()  { return (wxSpinCtrl*) FindWindow( ID4_MARGIN_BOTTOM ); }
    wxSpinCtrl* GetScMarginLeftOps3()  { return (wxSpinCtrl*) FindWindow( ID4_MARGIN_LEFT ); }
    wxSpinCtrl* GetScMarginRightOps3()  { return (wxSpinCtrl*) FindWindow( ID4_MARGIN_RIGHT ); }
    wxCheckBox* GetChBordersPosOps3()  { return (wxCheckBox*) FindWindow( ID4_CHB_BORDERS_POS ); }
    wxCheckBox* GetChTextInitOps3()  { return (wxCheckBox*) FindWindow( ID4_CHB_TEXT_INIT ); }
    wxCheckBox* GetChStavesSegOps3()  { return (wxCheckBox*) FindWindow( ID4_CHB_STAVES_SEG ); }
    wxCheckBox* GetChTextOps3()  { return (wxCheckBox*) FindWindow( ID4_CHB_TEXT_POS ); }
	wxSpinCtrl* GetScBinSizeOps3()  { return (wxSpinCtrl*) FindWindow( ID4_BIN_REGION_SIZE ); }
	wxChoice* GetCBinOps3()  { return (wxChoice*) FindWindow( ID4_BINARIZATION ); }
	wxCheckBox* GetCbBinSelect() { return (wxCheckBox*) FindWindow( ID4_BINARIZATION_SELECT ); }
//#endif

private:
	void UpdateFontCorrections( int eventID );
    
private:
    wxString m_imageSizeToReduceStr;
    bool m_changeLanguage;
    wxFont m_font;
	wxFont m_lyric_font;
    bool m_changeFont;
	AxOptMusWindow *m_musWinPtr;
	bool m_ignoreSpinEvent;
	int m_previousFontPosCorrection; // keep if canceled
	int m_previousFontSizeCorrection;
	
#ifdef AX_SUPERIMPOSITION
    int m_interpolation_index;
#endif
	
#ifdef AX_RECOGNITION		
    wxString m_lmScalingStr;
#endif
    
private:
	void OnSpinCtrl( wxSpinEvent &event );
	void OnSpinCtrlText( wxCommandEvent &event );
    void OnChooseFont( wxCommandEvent &event );
	void OnChooseLyricFont( wxCommandEvent &event );
	void OnPreDefault( wxCommandEvent &event );
	void OnBrowseDocDir( wxCommandEvent &event );
    void OnBrowseWorkDir( wxCommandEvent &event );
    void OnChangeLanguage( wxCommandEvent &event );
    void OnOk( wxCommandEvent &event );
    void OnCancel( wxCommandEvent &event );
	// Standard tabs
	void OptionsDlgStandard( wxNotebook *notebook );
	// Superimposition tabs
	void OptionsDlgSuperimposition( wxNotebook *notebook );
	// Recognition tabs
	void OptionsDlgRecognition( wxNotebook *notebook );
	wxSize m_min_size; // keep min size to layout notebook; bug in auto layout ??? can't get it...
	
#ifdef AX_SUPERIMPOSITION

#endif
	
#ifdef AX_RECOGNITION		
    void OnBrowse4( wxCommandEvent &event );
#endif

private:
    DECLARE_EVENT_TABLE()
	
public:
	wxNotebook *m_notebook;
	static int s_last_open_tab;
};


//----------------------------------------------------------------------------
// AxOptMusWindow
//----------------------------------------------------------------------------

class AxOptMusWindow: public MusWindow
{
public:
    // constructors and destructors
    AxOptMusWindow();
    AxOptMusWindow( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxScrolledWindowStyle, bool center = true );
    virtual ~AxOptMusWindow();
    
protected:
    vrv::Doc *m_docPtr;
    
private:
    void OnMouse( wxMouseEvent &event );

private:
    DECLARE_CLASS(AxOptMusWindow)
    DECLARE_EVENT_TABLE()
};

#endif

