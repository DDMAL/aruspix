/////////////////////////////////////////////////////////////////////////////
// Name:        rec.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __REC_H__
#define __REC_H__

#ifdef AX_RECOGNITION

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#ifdef AX_CMDLINE

//----------------------------------------------------------------------------
// Commandline replacement class
//----------------------------------------------------------------------------

class RecEnv
{
public:
    // constructors and destructors
    RecEnv() {};
    
public:
    // these values cannot be changed from the interface
    static bool s_check;
    static bool s_deskew;
    static bool s_staves_position;
    static bool s_crop;
    static bool s_binarize_and_clean;
    static bool s_save_images;
    // batch
    static int s_last_batch;
    static int s_book_sash;
    //
    static bool s_find_borders;
    static bool s_find_text;
    static bool s_find_ornate_letters;
    static bool s_find_text_in_staves;
    // decoder
    static wxString s_rec_typ_model;
    static wxString s_rec_mus_model;
    
    // static values not changed
    static double s_rec_phone_pen; // am_phone_del_pen
    static double s_rec_end_prune; // dec_end_prune_window
    static double s_rec_word_pen; // dec_word_entr_pen
    static double s_rec_int_prune; // dec_int_prune_window
    // training - idem
    static wxString s_train_dir;
    static bool s_train_mlf;
    static bool s_train_positions;
    //
    static wxString s_rec_wrdtrns;
    static bool s_rec_delayed; // dec_delayed_lm
    static int s_rec_lm_order; // lm_ngram_order
    static double s_rec_lm_scaling; // lm_scaling_factor
    
};

#else

//----------------------------------------------------------------------------
// Gui
//----------------------------------------------------------------------------

#include "app/axapp.h"
#include "app/axenv.h"

#include "rec_wdr.h"

enum
{
    ID4_CONTROLLER = ENV_IDS_LOCAL_REC,
    ID4_VIEW,
    ID4_DISPLAY,
    ID4_MUSWINDOW,
    
    ID4_POPUP_MENU_MUSIC, 
    ID4_POPUP_MENU_TITLE,
    ID4_POPUP_MENU_LYRICS, 
    ID4_POPUP_MENU_ORNATE_LETTER, 
    ID4_POPUP_MENU_TEXT_IN_STAFF,
    ID4_POPUP_MENU_VOID,
    
    ID4_POPUP_TREE_LOAD,
    ID4_POPUP_TREE_PRE,
    ID4_POPUP_TREE_REC,
    ID4_POPUP_TREE_ADAPT,
    ID4_POPUP_TREE_BOOK_EDIT,
    ID4_POPUP_TREE_IMG_REMOVE,
    ID4_POPUP_TREE_IMG_DESACTIVATE,
    ID4_POPUP_TREE_AX_REMOVE,
    ID4_POPUP_TREE_AX_DESACTIVATE,
    ID4_POPUP_TREE_AX_DELETE
};

#define REC_UPDATE_DEFAULT 0
#define REC_UPDATE_FAST 1
#define REC_UPDATE_MEI 2

class AxImageController;
class RecImController;
class RecMusController;
class RecImWindow;
class RecMusWindow;

class MusToolPanel;

class RecFile;
class RecBookCtrl;
class RecBookFile;
class RecBookPanel;
class RecEnv;

class RecTypModel;
class RecMusModel;


//----------------------------------------------------------------------------
// RecSplitterWindow
//----------------------------------------------------------------------------

class RecSplitterWindow: public wxSplitterWindow
{
public:
    // constructors and destructors
    RecSplitterWindow( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0 );
    void SetEnv( RecEnv *env );
    
        
protected:
        RecEnv *m_envPtr;
    
private:
        void OnSashChanged( wxSplitterEvent &event );

private:
    DECLARE_EVENT_TABLE()
};

/*
//----------------------------------------------------------------------------
// RecPanel
//----------------------------------------------------------------------------

class RecPanel: public wxPanel
{
public:
    // constructors and destructors
    RecPanel( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    
        RecSplitterWindow* GetSplitter1()  { return (RecSplitterWindow*) FindWindow( ID4_SPLITTER1 ); }
    AxImageController* GetController()  { return (AxImageController*) FindWindow( ID4_CONTROLLER ); }
    
private:
        
private:
    
private:
    DECLARE_EVENT_TABLE()
};
*/


//----------------------------------------------------------------------------
// RecEnv
//----------------------------------------------------------------------------

class RecEnv: public AxEnv
{
public:
    // constructors and destructors
    RecEnv();
    virtual ~RecEnv();
    
    virtual void LoadWindow();
    static void LoadConfig();
    static void SaveConfig();
    virtual void ParseCmd( wxCmdLineParser *parser );
    virtual void UpdateTitle( );
    virtual void RealizeToolbar( );
    virtual void Open( wxString filename, int type );
    virtual bool CloseAll( );
    //
    bool ResetFile(); // must be called when new file is created or opened
    bool ResetBookFile(); // must be called when new file is created or opened
    void OpenFile( wxString filename, int type );
    void OpenBookFile( wxString filename );
    void ResetModels( ); // check if models are loaded and up to date
    void UpdateViews( int flags );
	void BatchAdaptation( ); // temporary method, used for research only
    // recognition elements access
    //ImPage *GetImagePage() { return m_pagePtr; }

private:
        RecSplitterWindow *m_pageSplitterPtr;
    wxSplitterWindow *m_bookSplitterPtr;
    // im
    RecImController *m_imControlPtr;
    RecImWindow *m_imViewPtr;
    // mus    
    RecMusController *m_musControlPtr;
    RecMusWindow *m_musViewPtr;
    MusToolPanel *m_toolpanel;
    wxPanel *m_musPanelPtr; // panel complet: contient toolpanel et recmuscontoller
    // recognition elements
    RecFile *m_recFilePtr;
    // 
    RecBookCtrl *m_recBookPtr; // tree
    RecBookFile *m_recBookFilePtr;
    RecBookPanel *m_recBookPanelPtr;

public:
    // recognition elements
    RecTypModel *m_typModelPtr;
    RecMusModel *m_musModelPtr;

    // these values cannot be changed from the interface
    static bool s_check;
    static bool s_deskew;
    static bool s_staves_position;
    static bool s_crop;
    static bool s_binarize_and_clean;
    static bool s_save_images;
    // batch
    static int s_last_batch;
    static int s_book_sash;
    //
    static bool s_find_borders;
    static bool s_find_text;
    static bool s_find_ornate_letters;
    static bool s_find_text_in_staves;
    // decoder
    static wxString s_rec_typ_model;
    static wxString s_rec_mus_model;    

    // static values not changed
    static double s_rec_phone_pen; // am_phone_del_pen
    static double s_rec_end_prune; // dec_end_prune_window
    static double s_rec_word_pen; // dec_word_entr_pen
    static double s_rec_int_prune; // dec_int_prune_window
    // training - idem
    static wxString s_train_dir;
    static bool s_train_mlf;
    static bool s_train_positions;
    //
    static wxString s_rec_wrdtrns;
    static bool s_rec_delayed; // dec_delayed_lm
    static int s_rec_lm_order; // lm_ngram_order
    static double s_rec_lm_scaling; // lm_scaling_factor
	
    // tree
    static bool s_expand_root;
	static bool s_expand_book;
    static bool s_expand_img;   
	static bool s_expand_ax;
	static bool s_expand_opt;
    
private:
    void Recognize( );
    void Preprocess( );

    void OnResetAdaptation( wxCommandEvent &event );
    void OnBookImportModels( wxCommandEvent &event );
    void OnBookExportModels( wxCommandEvent &event );
    void OnBookExportMEI ( wxCommandEvent &event );
    void OnBookEdit( wxCommandEvent &event );
    void OnBookOptimize( wxCommandEvent &event );
    void OnBookFullProcess( wxCommandEvent &event );
    void OnBookRecognize( wxCommandEvent &event );
    void OnBookPreprocess( wxCommandEvent &event );
    void OnBookLoad( wxCommandEvent &event );
    void OnCloseBook( wxCommandEvent &event );
    void OnOpenBook( wxCommandEvent &event );
    void OnNewBook( wxCommandEvent &event );
    void OnSaveBookAs( wxCommandEvent &event );
    void OnSaveBook( wxCommandEvent &event );
    void OnCancelRecognition( wxCommandEvent &event );
    void OnTools( wxCommandEvent &event );
    void OnStaffCorrespondence( wxCommandEvent &event );
    void OnNew( wxCommandEvent &event );
    void OnSave( wxCommandEvent &event );
    void OnSaveAs( wxCommandEvent &event );
    void OnClose( wxCommandEvent &event );
    void OnExportImage( wxCommandEvent &event );
    void OnExportCmme( wxCommandEvent &event );
    void OnExportWWG( wxCommandEvent &event );
    void OnExportMusicXML( wxCommandEvent &event );
    void OnAdjust( wxCommandEvent &event );
    void OnZoom( wxCommandEvent &event );
    void OnPaste( wxCommandEvent &event );
    void OnCopy( wxCommandEvent &event );
    void OnCut( wxCommandEvent &event );
    void OnUndo( wxCommandEvent &event );
    void OnRedo( wxCommandEvent &event );
    void OnRun( wxCommandEvent &event );
	void OnOpenMEI( wxCommandEvent &event );
	void OnSaveMEI( wxCommandEvent &event );
    virtual void OnUpdateUI( wxUpdateUIEvent &event );

private:
    DECLARE_DYNAMIC_CLASS(RecEnv)

    DECLARE_EVENT_TABLE( )
};



//-------------------------------------------------------------------------------
// RecBinSelectDlg
//-------------------------------------------------------------------------------

class RecBinSelectDlg: public wxDialog
{
public:
	// constructors and destructors
	RecBinSelectDlg( wxWindow *parent, wxWindowID id, const wxString &title, RecFile *recfile, RecBookFile *recbookfile );
	
	wxChoice* GetCPageBin()  { return (wxChoice*) FindWindow( ID4_BIN_CHOICE ); }
	wxSpinCtrl* GetScBinRgnSize()  { return (wxSpinCtrl*) FindWindow( ID4_RGN_SIZE_SPINCTRL ); }
	wxCheckBox* GetCbDeactivateDlg()  { return (wxCheckBox*) FindWindow( ID4_DEACTIVATE_DIALOG_CHECKBOX ); }
};

#endif // AX_CMDLINE

#endif //AX_RECOGNITION

#endif

