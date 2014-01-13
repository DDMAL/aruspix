/////////////////////////////////////////////////////////////////////////////
// Name:        rec.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_RECOGNITION

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

//#include "wx/config.h"
#include "wx/valgen.h"
#include "wx/file.h"
//#include "wx/filename.h"
//#include "wx/dir.h"
#include "wx/log.h"

#include "rec.h"
#include "recim.h"
#include "recmus.h"
#include "recfile.h"
#include "recbookfile.h"
#include "recbookctrl.h"
#include "recmodels.h"
#include "recmlfbmp.h"

#include "app/axapp.h"
#include "app/axframe.h"
#include "app/axoptionsdlg.h"

#include "im/impage.h"

#include "mus/musdoc.h"
#include "mus/musiomei.h"

#include "musapp/mustoolpanel.h"
#include "musapp/musiowwg.h"

// statics
bool RecEnv::s_check = true;
bool RecEnv::s_deskew = true;
bool RecEnv::s_staves_position = true;
bool RecEnv::s_crop = true;
bool RecEnv::s_binarize_and_clean = true;
bool RecEnv::s_save_images = true;
//
int RecEnv::s_last_batch = 0; // batch processing
int RecEnv::s_book_sash = 200; // batch processing
//
bool RecEnv::s_find_borders = true;
bool RecEnv::s_find_ornate_letters = true;
bool RecEnv::s_find_text = true;
bool RecEnv::s_find_text_in_staves = true;

wxString RecEnv::s_rec_typ_model = "";
wxString RecEnv::s_rec_mus_model = "";

double RecEnv::s_rec_phone_pen = 1.0; // am_phone_del_pen
wxString RecEnv::s_rec_wrdtrns = ""; // wrdtrns_fname
double RecEnv::s_rec_end_prune = 0.0; // dec_end_prune_window
double RecEnv::s_rec_int_prune = 0.0; // dec_int_prune_window
double RecEnv::s_rec_word_pen = 0.0; // dec_word_entr_pen
bool RecEnv::s_rec_delayed = true; // dec_delayed_lm
int RecEnv::s_rec_lm_order = 2; // lm_ngram_order
double RecEnv::s_rec_lm_scaling = 1.0; // lm_scaling_factor

// training
wxString RecEnv::s_train_dir;
bool RecEnv::s_train_mlf;
bool RecEnv::s_train_positions;

// tree
bool RecEnv::s_expand_root = true;
bool RecEnv::s_expand_book = true;
bool RecEnv::s_expand_img = true;  
bool RecEnv::s_expand_ax = true;
bool RecEnv::s_expand_opt = true;





//----------------------------------------------------------------------------
// RecSplitterWindow
//----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(RecSplitterWindow,wxSplitterWindow)
    EVT_SPLITTER_SASH_POS_CHANGED( -1, RecSplitterWindow::OnSashChanged)
END_EVENT_TABLE()

RecSplitterWindow::RecSplitterWindow( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxSplitterWindow( parent, id, position, size, style )
{
    m_envPtr = NULL;
}

void RecSplitterWindow::SetEnv( RecEnv *env )
{
    m_envPtr = env;
}


void RecSplitterWindow::OnSashChanged( wxSplitterEvent &event )
{
    UpdateSize();
    if ( m_envPtr )
    {
       m_envPtr->UpdateViews( 0 );
    }
    event.Skip();
}


//----------------------------------------------------------------------------
// MusPanel
//----------------------------------------------------------------------------

class MusPanel: public wxPanel
{
public:
    // constructors and destructors
    MusPanel( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    
        
private:
        
private:
    
private:
    DECLARE_EVENT_TABLE()
};



//----------------------------------------------------------------------------
// MusPanel
//----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(MusPanel,wxPanel)
END_EVENT_TABLE()

MusPanel::MusPanel( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
    wxFlexGridSizer *item0 = new wxFlexGridSizer( 1, 0, 0 );
    item0->AddGrowableCol( 0 );
    item0->AddGrowableRow( 1 );

    MusToolPanel *item1 = new MusToolPanel( this, ID4_TOOLPANEL, wxDefaultPosition, wxSize(10,10), 0 );
    item0->Add( item1, 0, wxGROW, 0 );

    RecMusController *item2 = new RecMusController( this, ID4_MUSPANEL, wxDefaultPosition, wxSize(200,160), wxSUNKEN_BORDER );
    item0->Add( item2, 0, wxGROW|wxALL, 0 );


	this->SetSizer( item0 );
	item0->SetSizeHints( this );
}





//----------------------------------------------------------------------------
// RecEnv
//----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(RecEnv,AxEnv)
    // copy paste 
    EVT_MENU( ID_PASTE, RecEnv::OnPaste )
    EVT_MENU( ID_COPY, RecEnv::OnCopy )
    EVT_MENU( ID_CUT, RecEnv::OnCut )
    EVT_MENU( ID_UNDO, RecEnv::OnUndo )
    EVT_MENU( ID_REDO, RecEnv::OnRedo )
    EVT_UPDATE_UI( ID_PASTE , RecEnv::OnUpdateUI )
    EVT_UPDATE_UI( ID_COPY , RecEnv::OnUpdateUI )
    EVT_UPDATE_UI( ID_CUT , RecEnv::OnUpdateUI )
    // file
    EVT_MENU( ID_CLOSE, RecEnv::OnClose )
    EVT_MENU( ID_SAVE_AS, RecEnv::OnSaveAs )
    EVT_MENU( ID_SAVE, RecEnv::OnSave )
    //EVT_MENU( ID_OPEN, RecEnv::OnOpen )
    EVT_MENU( ID_NEW, RecEnv::OnNew )   
    // other
    EVT_MENU( ID4_PROCESS, RecEnv::OnRun )
    EVT_UPDATE_UI_RANGE( wxID_LOWEST, ENV_IDS_MAX , RecEnv::OnUpdateUI )
    EVT_MENU( ID4_CANCEL_REC, RecEnv::OnCancelRecognition )
    EVT_MENU( ID4_ZOOM_OUT, RecEnv::OnZoom )
    EVT_MENU( ID4_ZOOM_IN, RecEnv::OnZoom )
    EVT_MENU( ID4_ADJUST, RecEnv::OnAdjust )
    EVT_MENU( ID4_ADJUST_H, RecEnv::OnAdjust )
    EVT_MENU( ID4_ADJUST_V, RecEnv::OnAdjust )
    EVT_MENU( ID4_EXPORT_IMAGE, RecEnv::OnExportImage )
    EVT_MENU( ID4_EXPORT_CMME, RecEnv::OnExportCmme )
    EVT_MENU( ID4_EXPORT_WWG, RecEnv::OnExportWWG )
    EVT_MENU( ID4_SHOW_STAFF_BMP, RecEnv::OnStaffCorrespondence )
    EVT_MENU_RANGE( ID4_INSERT_MODE, ID4_SYMBOLES, RecEnv::OnTools )
    EVT_MENU( ID4_SAVE_BOOK, RecEnv::OnSaveBook )
    EVT_MENU( ID4_SAVE_AS_BOOK, RecEnv::OnSaveBookAs )
    EVT_MENU( ID4_NEW_BOOK, RecEnv::OnNewBook )
    EVT_MENU( ID4_OPEN_BOOK, RecEnv::OnOpenBook )
    EVT_MENU( ID4_CLOSE_BOOK, RecEnv::OnCloseBook )
	EVT_MENU( ID4_IMPORT_MEI, RecEnv::OnOpenMEI )
    EVT_MENU( ID4_EXPORT_MEI, RecEnv::OnSaveMEI )
    EVT_MENU( ID4_BOOK_LOAD, RecEnv::OnBookLoad )
    EVT_MENU( ID4_BOOK_PRE, RecEnv::OnBookPreprocess )
    EVT_MENU( ID4_BOOK_REC, RecEnv::OnBookRecognize )
    EVT_MENU( ID4_BOOK_FULL, RecEnv::OnBookFullProcess )
    EVT_MENU( ID4_BOOK_ADAPT, RecEnv::OnBookOptimize )
    EVT_MENU( ID4_POPUP_TREE_LOAD, RecEnv::OnBookLoad )
    EVT_MENU( ID4_BOOK_EDIT, RecEnv::OnBookEdit )
    EVT_MENU( ID4_POPUP_TREE_PRE, RecEnv::OnBookPreprocess )
    EVT_MENU( ID4_POPUP_TREE_REC, RecEnv::OnBookPreprocess )
    EVT_MENU( ID4_POPUP_TREE_ADAPT, RecEnv::OnBookOptimize )
    EVT_MENU( ID4_POPUP_TREE_BOOK_EDIT, RecEnv::OnBookEdit )
    EVT_MENU( ID4_BOOK_EXPORT_MEI, RecEnv::OnBookExportMEI )
    EVT_MENU( ID4_BOOK_EXPORT_MODELS, RecEnv::OnBookExportModels )
    EVT_MENU( ID4_BOOK_IMPORT_MODELS, RecEnv::OnBookImportModels )
    EVT_MENU( ID4_BOOK_RESET_ADAPT, RecEnv::OnResetAdaptation )
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(RecEnv,AxEnv)

RecEnv::RecEnv():
    AxEnv()
{
    m_pageSplitterPtr = NULL;
    m_bookSplitterPtr = NULL;
    m_imControlPtr = NULL;
    m_imViewPtr = NULL;
    m_musControlPtr = NULL;
    m_musViewPtr = NULL;
    m_recBookPtr = NULL;
    m_recBookPanelPtr = NULL;

    m_recFilePtr = new RecFile( "rec_env_file", this );
    m_recBookFilePtr = new RecBookFile( "rec_book_file", this );
    //m_recBookFilePtr->New();
    m_typModelPtr = NULL;
    m_musModelPtr = NULL;

    this->m_envMenuBarFuncPtr = MenuBarFunc4;
}

RecEnv::~RecEnv()
{
    if (m_envWindowPtr) m_envWindowPtr->Destroy();
    m_envWindowPtr = NULL;

    if ( m_recFilePtr )
        delete m_recFilePtr; 
    if ( m_recBookFilePtr )
        delete m_recBookFilePtr; 
    if ( m_typModelPtr )
        delete m_typModelPtr;
    if ( m_musModelPtr )
        delete m_musModelPtr;
}

void RecEnv::LoadWindow()
{
    /*wxFlexGridSizer *item0 = new wxFlexGridSizer( 1, 0, 0 );
    item0->AddGrowableCol( 0 );
    item0->AddGrowableRow( 0 );

    wxSplitterWindow *item1 = new wxSplitterWindow( parent, ID3_SPLITTER1, wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item1, 0, wxGROW|wxALL, 0 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;*/
        
    m_bookSplitterPtr = new wxSplitterWindow( m_framePtr, -1 );
    this->m_envWindowPtr = m_bookSplitterPtr;
    if (!m_envWindowPtr)
        return;
        
    m_recBookPanelPtr = new RecBookPanel( m_bookSplitterPtr, -1);
    m_recBookPtr = m_recBookPanelPtr->GetTree();
    m_recBookPtr->SetBookFile( m_recBookFilePtr );
    m_recBookPtr->SetEnv( this );
    m_recBookPtr->SetBookPanel( m_recBookPanelPtr );

    m_pageSplitterPtr = new RecSplitterWindow( m_bookSplitterPtr, -1 );
    if (!m_pageSplitterPtr)
        return;
    m_bookSplitterPtr->SetMinimumPaneSize( 100 );
    m_bookSplitterPtr->SplitVertically( m_recBookPanelPtr, m_pageSplitterPtr, RecEnv::s_book_sash );
    m_bookSplitterPtr->Unsplit( m_recBookPanelPtr );

    m_pageSplitterPtr->SetWindowStyleFlag( wxSP_FULLSASH );
    m_pageSplitterPtr->SetMinimumPaneSize( 100 );

    m_imControlPtr = new RecImController( m_pageSplitterPtr, ID4_CONTROLLER, wxDefaultPosition, wxDefaultSize, 0,
        CONTROLLER_NO_TOOLBAR );
    m_imControlPtr->SetEnv( this );
    m_imViewPtr = new RecImWindow( m_imControlPtr, ID4_VIEW , wxDefaultPosition, 
        wxDefaultSize, wxHSCROLL| wxVSCROLL | wxNO_BORDER  /* | wxSUNKEN_BORDER */);
    m_imViewPtr->SetEnv( this );
    m_imViewPtr->m_popupMenu.AppendSeparator();
    m_imControlPtr->Init( this, m_imViewPtr );

    m_musPanelPtr = new MusPanel( m_pageSplitterPtr, ID4_DISPLAY );
	//m_musPanelPtr = new wxPanel( m_pageSplitterPtr, ID4_DISPLAY );
    //wxFlexGridSizer *mussizer = (wxFlexGridSizer*)MusOutputFunc4( m_musPanelPtr, TRUE );
    m_musControlPtr = (RecMusController*)m_envWindowPtr->FindWindowById( ID4_MUSPANEL );
    m_musViewPtr = new RecMusWindow( m_musControlPtr, ID4_MUSWINDOW, wxDefaultPosition,
            wxDefaultSize, wxHSCROLL |wxVSCROLL | wxNO_BORDER  /*| wxSIMPLE_BORDER */ , false);
    m_musViewPtr->SetEnv( this );
    m_musControlPtr->Init( this, m_musViewPtr );
    
	// cross pointers on views and controller and pointer to the file
    m_musControlPtr->SetImViewAndController( m_imViewPtr, m_imControlPtr );
    m_musControlPtr->SetRecFile( m_recFilePtr );
    m_imControlPtr->SetMusViewAndController( m_musViewPtr, m_musControlPtr );
    m_imControlPtr->SetRecFile( m_recFilePtr );
   
    m_toolpanel = (MusToolPanel*)m_envWindowPtr->FindWindowById( ID4_TOOLPANEL );
    wxASSERT_MSG( m_toolpanel, "Tool Panel cannot be NULL ");

    m_pageSplitterPtr->SetEnv( this );
	
    if ( wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE) == *wxWHITE )
        m_musControlPtr->SetBackgroundColour( *wxLIGHT_GREY );
    else
        m_musControlPtr->SetBackgroundColour( wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE) );


    //m_pageSplitterPtr->SplitHorizontally(m_imControlPtr, m_musControlPtr,  800 );
    m_pageSplitterPtr->SplitHorizontally(m_imControlPtr, m_musPanelPtr );
    m_pageSplitterPtr->Unsplit();
    
    ResetModels();
}


void RecEnv::RealizeToolbar( )
{
    wxToolBar *toolbar =  m_framePtr->GetToolBar();
    
    toolbar->InsertTool( 0, ID4_NEW_BOOK, _T("New"), m_framePtr->GetToolbarBitmap( "book_new.png" ), wxNullBitmap, wxITEM_NORMAL, _("New book"), _("New book") );
    toolbar->InsertTool( 1, ID4_OPEN_BOOK, _T("Open"), m_framePtr->GetToolbarBitmap( "book_open.png" ), wxNullBitmap, wxITEM_NORMAL, _("Open book"), _("Open book") );
	//toolbar->InsertTool( 4, ID4_IMPORT_MEI, _("Import MEI"), m_framePtr->GetToolbarBitmap( "folder_yellow_mei.png" ) , wxNullBitmap, wxITEM_NORMAL, _("Import MEI"), _("Import an MEI file") );
    //toolbar->InsertTool( 7, ID4_EXPORT_MEI, _("Export MEI"), m_framePtr->GetToolbarBitmap( "filesave_mei.png" ) , wxNullBitmap, wxITEM_NORMAL, _("MeiEx"), _("Export an MEI file") );
    toolbar->InsertSeparator( 2 );
    
    toolbar->AddTool( ID4_ZOOM_OUT, _T("Zoom out"), m_framePtr->GetToolbarBitmap( "viewmag-.png" ), wxNullBitmap, wxITEM_NORMAL, _("Zoom out"), _("Zoom out") );
    toolbar->AddTool( ID4_ZOOM_IN, _T("Zoom in"), m_framePtr->GetToolbarBitmap( "viewmag+.png" ), wxNullBitmap, wxITEM_NORMAL, _("Zoom in"), _("Zoom in") );
    toolbar->AddTool( ID4_ADJUST, _T("Fit"), m_framePtr->GetToolbarBitmap( "viewmagfit.png" ), wxNullBitmap, wxITEM_CHECK, _("Adjust"), _("Adjust to fit the window") );
    //toolbar->AddTool( ID4_ADJUST_V, _T(""), BitmapsFunc4( 10 ), wxNullBitmap, wxITEM_CHECK, _("Adjust vertically"), _("Adjust to fit the window vertically") );
    //toolbar->AddTool( ID4_ADJUST_H, _T(""), BitmapsFunc4( 11 ), wxNullBitmap, wxITEM_CHECK, _("Adjust horizontally"), _("Adjust to fit the window horizontally") );
    //toolbar->AddSeparator();
    //toolbar->AddTool( ID4_SHOW_STAFF_BMP, _("Staff correspondence"), BitmapsFunc4( 12 ), wxNullBitmap, wxITEM_CHECK, _("Staff correspondence"), _("Show staff correspondence on image") );
    toolbar->AddSeparator();
    toolbar->AddTool( ID4_PROCESS, _T("Run"), m_framePtr->GetToolbarBitmap( "noatun.png" ), wxNullBitmap, wxITEM_NORMAL, _("Process"), _("Process the current page") );
    //parent->AddTool( ID4_RECOGNIZE, _T(""), BitmapsFunc4( 5 ), wxNullBitmap, wxITEM_NORMAL, _("Recognize"), _("Recognize current page") );
    toolbar->AddSeparator();
    //toolbar->AddTool( ID4_BATCH, _T("Batch"), m_framePtr->GetToolbarBitmap( "bookcase.png" ), wxNullBitmap, wxITEM_NORMAL, _("Batch pre-processing"), _("Execute batch pre-processing") );
    
    toolbar->Realize();
}

void RecEnv::LoadConfig()
{
    wxConfigBase *pConfig = wxConfigBase::Get();
    wxASSERT_MSG( pConfig, wxT("pConfig cannot be NULL") );
    pConfig->SetPath("/Recognition");
    
    // cannot be changed from dialog - values are not written by SaveConfig
    RecEnv::s_check = (pConfig->Read("Check",1)==1);
    RecEnv::s_deskew = (pConfig->Read("Deskew",1)==1);
    RecEnv::s_staves_position = (pConfig->Read("Staves position",1)==1);
    RecEnv::s_crop = (pConfig->Read("Crop",1)==1);
    RecEnv::s_binarize_and_clean = (pConfig->Read("Binarize",1)==1);
    RecEnv::s_save_images = (pConfig->Read("Save images",1)==1);
    // can be changed from dialog
    RecEnv::s_find_borders = (pConfig->Read("Borders",1)==1);
    RecEnv::s_find_ornate_letters = (pConfig->Read("Text initials",1)==1);
    RecEnv::s_find_text = (pConfig->Read("Text",1)==1);
    RecEnv::s_find_text_in_staves = (pConfig->Read("Text in staves",1)==1);
    // impage statics - not great
    ImPage::s_pre_margin_top = pConfig->Read("Pre margin top", 150);
    ImPage::s_pre_margin_bottom = pConfig->Read("Pre margin bottom", 120);
    ImPage::s_pre_margin_left = pConfig->Read("Pre margin left", 30);
    ImPage::s_pre_margin_right = pConfig->Read("Pre margin right", 20);
    
    RecEnv::s_last_batch = pConfig->Read("Last Batch", 0L );
    RecEnv::s_book_sash = pConfig->Read("Book Sash", 200 );

    RecEnv::s_rec_typ_model = pConfig->Read("RecTypModel", "");
    RecEnv::s_rec_mus_model = pConfig->Read("RecMusModel", "");
    // cannot be changed from dialog
    pConfig->Read("RecPhonePen", &RecEnv::s_rec_phone_pen, 1.0);
    RecEnv::s_rec_wrdtrns = pConfig->Read("RecWrdtrns", "");
    pConfig->Read("RecEndPrune", &RecEnv::s_rec_end_prune, 0.0);
    pConfig->Read("RecWordPen", &RecEnv::s_rec_word_pen, 0.0);
    pConfig->Read("RecIntPrune", &RecEnv::s_rec_int_prune, 0.0);
    // training idem
    RecEnv::s_train_dir = pConfig->Read("TrainDir", "");
    RecEnv::s_train_mlf = (pConfig->Read("TrainMlf",0L)==1);
    RecEnv::s_train_positions = (pConfig->Read("TrainPosition",0L)==1);
    //
    RecEnv::s_rec_delayed = (pConfig->Read("RecLMDelayed",1)==1);
    RecEnv::s_rec_lm_order = pConfig->Read("RecLMOrder", 2);
    pConfig->Read("RecLMScaling", &RecEnv::s_rec_lm_scaling, 1.0);
	// tree
	RecEnv::s_expand_root = (pConfig->Read("Tree root",1)==1);
	RecEnv::s_expand_book = (pConfig->Read("Tree book",1)==1);
	RecEnv::s_expand_img = (pConfig->Read("Tree img",1)==1);
	RecEnv::s_expand_ax = (pConfig->Read("Tree ax",1)==1);
	RecEnv::s_expand_opt = (pConfig->Read("Tree opt",1)==1);

    pConfig->SetPath("/");
}

void RecEnv::SaveConfig()
{
    wxConfigBase *pConfig = wxConfigBase::Get();
    wxASSERT_MSG( pConfig, wxT("pConfig cannot be NULL") );
    pConfig->SetPath("/Recognition");

    pConfig->Write("Borders", RecEnv::s_find_borders);
    pConfig->Write("Text initials", RecEnv::s_find_ornate_letters);
    pConfig->Write("Text", RecEnv::s_find_text );
    pConfig->Write("Text in staves", RecEnv::s_find_text_in_staves );
    pConfig->Write("Pre margin top", ImPage::s_pre_margin_top);
    pConfig->Write("Pre margin bottom", ImPage::s_pre_margin_bottom);
    pConfig->Write("Pre margin left", ImPage::s_pre_margin_left);
    pConfig->Write("Pre margin right", ImPage::s_pre_margin_right );
        
    pConfig->Write("Last Batch", RecEnv::s_last_batch );
    pConfig->Write("Book Sash", RecEnv::s_book_sash);

    pConfig->Write("RecTypModel", RecEnv::s_rec_typ_model);
    pConfig->Write("RecMusModel", RecEnv::s_rec_mus_model);
    // cannot be changed from dialog - useless to write them...
    //pConfig->Write("RecPhonePen", RecEnv::s_rec_phone_pen);
    //pConfig->Write("RecWrdtrns", RecEnv::s_rec_wrdtrns);
    //pConfig->Write("RecEndPrune", RecEnv::s_rec_end_prune);
    //pConfig->Write("RecWordPen", RecEnv::s_rec_word_pen);
    //pConfig->Write("RecIntPrune", RecEnv::s_rec_int_prune);
    //
    pConfig->Write("RecLMDelayed", RecEnv::s_rec_delayed);
    pConfig->Write("RecLMOrder", RecEnv::s_rec_lm_order);
    pConfig->Write("RecLMScaling", RecEnv::s_rec_lm_scaling);
	// tree
	pConfig->Write("Tree root", RecEnv::s_expand_root );
	pConfig->Write("Tree book", RecEnv::s_expand_book );
	pConfig->Write("Tree img", RecEnv::s_expand_img );
	pConfig->Write("Tree ax", RecEnv::s_expand_ax );
	pConfig->Write("Tree opt", RecEnv::s_expand_opt );

    pConfig->SetPath("/");
}

void RecEnv::ParseCmd( wxCmdLineParser *parser )
{
    wxASSERT_MSG( parser, wxT("Parser cannot be NULL") );
    wxASSERT_MSG( m_imControlPtr, wxT("Image controller cannot be NULL") );

    if ( parser->Found("q") &&  (parser->GetParamCount() > 0) )
    {
        // it might be useful to disable logging
        //wxLogNull logNo;;
        
        // first parameter is input file
        wxString file = parser->GetParam( 0 );
        wxLogDebug( file.c_str() );
        
        if ( parser->Found("p") && (parser->GetParamCount() == 2) )
        {
            // with -p, the second parameter is the output filename
            wxString outfile = parser->GetParam( 1 );
            wxLogDebug( outfile.c_str() );
            
            // create a new  file
            m_recFilePtr->New(  );
            
            // parameter array with filename as first parameter
            wxArrayPtrVoid params;
            params.Add( &file );
    
            // fake dialog, we need one even in command line
            AxProgressDlg dlg( m_framePtr, -1, _("Recognition") );
    
            bool failed = false;
    
            // do the pre-processing
            failed = !m_recFilePtr->Preprocess( params, &dlg );
            
            // save if succeed
            if ( !failed )
                failed = !m_recFilePtr->SaveAs( outfile );

        }
        
		                        
        // upgrade file version
        //m_recFilePtr->Open( file );
        //m_recFilePtr->Modify( );
        //m_recFilePtr->Save( false );
		
		// HACK : force MFC generation here
        /*
		m_recFilePtr->Open( file );
		m_recFilePtr->Modify( );
		wxArrayPtrVoid mfc_params;
		wxString mfcfile = "";
		mfc_params.Add( &mfcfile );
		AxProgressDlg dlg( m_framePtr, -1, _("Recognition") );
		m_recFilePtr->GenerateMFC( mfc_params, &dlg ); // 2 operations
        m_recFilePtr->Save( false );
        */
        
        /*
        wxString outfile = file;
        
        // TODO change this dialog to have an option -o with filename
        //if ( wxMessageBox("Overwrite files?", wxGetApp().GetAppName() , wxYES | wxNO | wxICON_QUESTION ) != wxYES )
        //{
        //  output = wxDirSelector( _("Output folder"), wxGetApp().m_lastDirBatch_out );
        //  if ( output.empty() )
        //      return;
        //  wxGetApp().m_lastDirBatch_out = output;
        //  outfile ...;
        //}
    
        AxProgressDlg *dlg = new AxProgressDlg( m_framePtr, -1, _("Batch") );
        dlg->Center( wxBOTH );
        //dlg->Show( );
        wxLog::SetActiveTarget( new wxLogStderr() );
        dlg->SetMaxBatchBar( 1 );
    
        //wxLogDebug("Typographic model %s", m_typModelPtr->m_filename.c_str() );
    
        wxArrayPtrVoid params;
        params.Add( m_typModelPtr );
        params.Add( m_musModelPtr );
        params.Add( &RecEnv::s_rec_delayed );
        params.Add( &RecEnv::s_rec_lm_order );
        params.Add( &RecEnv::s_rec_lm_scaling );
        wxString rec_wrdtrns = "";
        params.Add( &rec_wrdtrns );
    
        RecFile recFilePtr( "rec_batch2_file" );
        recFilePtr.New();
            
        bool failed = false;
        
        if ( !failed )
            failed = !recFilePtr.Open( file );

        if ( !failed )
            failed = !recFilePtr.Recognize( params, dlg );

        if ( !failed )
            failed = !recFilePtr.SaveAs( outfile );
            
        //dlg->AxShowModal( failed_once ); // stop process  ( failed ???? )
        dlg->Destroy();
        */
    }
}

void RecEnv::UpdateTitle( )
{
    wxASSERT_MSG( m_imControlPtr, wxT("Image controller cannot be NULL") );

    wxString msg;
    if ( m_recFilePtr->IsPreprocessed() )
        msg += wxString::Format("%s (%d%%)", m_recFilePtr->m_shortname.c_str(), m_imViewPtr->GetZoomPercent() );
    else if ( m_imControlPtr->Ok() )
        msg += m_imControlPtr->GetTitleMsg().c_str();

    SetTitle( "%s", msg.c_str() );
}


bool RecEnv::ResetBookFile()
{
    if ( !m_recBookFilePtr->Close( true ) )
        return false;

    if ( m_bookSplitterPtr->IsSplit() ) // keep position if splitted
        RecEnv::s_book_sash = m_bookSplitterPtr->GetSashPosition( );
    m_bookSplitterPtr->Unsplit( m_recBookPanelPtr );
    return true;
}

bool RecEnv::ResetFile()
{
    wxASSERT_MSG( m_musViewPtr, "WG Window cannot be NULL ");
    
    if ( !m_recFilePtr->Close( true ) )
        return false;

    m_musViewPtr->Show( false );
    m_musViewPtr->SetDoc( NULL );
    
    if ( m_imControlPtr->Ok() )
    {
        m_imControlPtr->CancelMagicSelection();
        m_imControlPtr->Close();
    }
    
    m_toolpanel->SetMusWindow( NULL );    
    m_musControlPtr->CancelShowStaffBitmap();
    m_pageSplitterPtr->Unsplit();
    UpdateTitle( );
    
    return true;
}

void RecEnv::ResetModels()
{
    if ( !m_typModelPtr )
        m_typModelPtr = new RecTypModel( "rec_env_typ" );
        
    if ( !m_musModelPtr )
        m_musModelPtr = new RecMusModel( "rec_env_mus" );

    wxString to_use_typ = wxGetApp().m_resourcesPath + "/models/builtin.axtyp";
    wxString to_use_mus = wxGetApp().m_resourcesPath + "/models/builtin.axmus";
    
    //  find the model to use
    // 1- builtin model
    // 2- user-defined model
    if ( !RecEnv::s_rec_typ_model.IsEmpty() && wxFileExists(RecEnv::s_rec_typ_model) )
        to_use_typ = RecEnv::s_rec_typ_model;
    
    // check is this model is the one loaded
    if ( m_typModelPtr && ( m_typModelPtr->m_filename != to_use_typ ) )
        m_typModelPtr->Close();

    // if necessary, open it...
    if ( !m_typModelPtr->IsOpened() )
        m_typModelPtr->Open( to_use_typ );
        
    // idem for the music model
    //  find the model to use
    if ( !RecEnv::s_rec_mus_model.IsEmpty() && wxFileExists(RecEnv::s_rec_mus_model) )
        to_use_mus = RecEnv::s_rec_mus_model;
    
    // check is this model is the one loaded
    if ( m_musModelPtr && ( m_musModelPtr->m_filename != to_use_mus ) )
        m_musModelPtr->Close();

    // if necessary, open it...
    if ( !m_musModelPtr->IsOpened() )
        m_musModelPtr->Open( to_use_mus );
        
    m_musControlPtr->LoadBitmapsForFont( );
}


void RecEnv::UpdateViews( int flags )
{
    if ( m_recFilePtr->IsRecognized() )
    {
        m_pageSplitterPtr->SplitHorizontally(m_imControlPtr, m_musPanelPtr );
        AxImage img;
        m_recFilePtr->GetImage1( &img );
        m_imControlPtr->ResetImage( img );

        m_musViewPtr->SetDoc( m_recFilePtr->m_musDocPtr );
        m_musViewPtr->SetEnv( this );
        m_musViewPtr->SetToolPanel( m_toolpanel );
        m_musViewPtr->LoadPage( 1 );
        m_musControlPtr->SyncZoom();  
    }
    else if ( m_recFilePtr->IsPreprocessed() )
    {
        if ( flags == REC_UPDATE_FAST )
        {
            m_recFilePtr->GetImage0( m_imControlPtr );
            m_imViewPtr->UpdateViewFast( );
        }
        else
        {
            m_pageSplitterPtr->Unsplit();
            AxImage img;
            m_recFilePtr->GetImage0( &img );
            m_imControlPtr->ResetImage( img );
        }
    }
}

void RecEnv::Open( wxString filename, int type )
{
    // everything has been closed by AxFrame::Open
    if ( type == AX_FILE_DEFAULT )
        this->OpenFile( filename, AX_FILE_DEFAULT );
    else if ( type == AX_FILE_PROJECT )
        this->OpenBookFile( filename );
}

// if type == -1, it is and image
void RecEnv::OpenFile( wxString filename, int type )
{
    if ( !this->ResetFile( ) )
        return;

    if ( (type == AX_FILE_DEFAULT) && this->m_recFilePtr->Open( filename ) )
    {
        wxGetApp().AxBeginBusyCursor( );
        UpdateViews( 0 );
        wxGetApp().AxEndBusyCursor();
    }
    else if ( (type == -1) && this->m_recFilePtr->New() )
    {
         m_imControlPtr->Open( filename );
    }
}


void RecEnv::OpenBookFile( wxString filename  )
{
    //if ( !this->CloseAll( ) )
    //  return;
        
    if ( this->m_recBookFilePtr->Open( filename ) )
    {
        wxGetApp().AxBeginBusyCursor( );
        m_bookSplitterPtr->SplitVertically( m_recBookPanelPtr, m_pageSplitterPtr, RecEnv::s_book_sash );
        m_recBookPtr->Build();
        wxGetApp().AxEndBusyCursor();
    }
}

bool RecEnv::CloseAll( )
{
    if ( !ResetFile() || !ResetBookFile() )
        return false;
        
    return true;
}


void RecEnv::OnResetAdaptation( wxCommandEvent &event )
{
   if ( m_recBookFilePtr->IsOpened() )
   {
        if ( !m_recBookFilePtr->ResetAdaptation( true ) )
            return;
        else // just update view
			m_recBookPtr->Update();
    }
}

void RecEnv::OnBookExportModels( wxCommandEvent &event )
{
    if ( AxProgressDlg::s_instance_existing )
        return;

	if ( !m_recBookFilePtr || !m_recBookFilePtr->IsOpened() )
        return;
    
    m_recBookFilePtr->ExportModels();
}


void RecEnv::OnBookExportMEI ( wxCommandEvent &event )
{
	if ( !m_recBookFilePtr || !m_recBookFilePtr->IsOpened() )
        return;
      
    m_recBookFilePtr->ExportPages( REC_BOOK_EXPORT_MEI );
}

void RecEnv::OnBookImportModels( wxCommandEvent &event )
{
    if ( AxProgressDlg::s_instance_existing )
        return;
    
	if ( !m_recBookFilePtr || !m_recBookFilePtr->IsOpened() )
        return;
    
    m_recBookFilePtr->ImportModels();    
}


void RecEnv::OnBookEdit( wxCommandEvent &event )
{
    RecBookDataDlg dlg( m_framePtr, -1, _("Edit book"), m_recBookFilePtr );
    dlg.Center( wxBOTH );
    if ( dlg.ShowModal() == wxID_OK )
    {
        m_recBookFilePtr->Modify();
        m_recBookPtr->Update();
    }
}

void RecEnv::OnBookOptimize( wxCommandEvent &event )
{
    if ( AxProgressDlg::s_instance_existing )
        return;

    if ( !m_recBookFilePtr->IsOpened() )
        return;

    wxArrayString paths, filenames;
    size_t nbOfFiles;
    
	bool isCacheOk;
    nbOfFiles = m_recBookFilePtr->FilesForAdaptation( &filenames, &paths, &isCacheOk );
    if ( nbOfFiles == 0 )
    {
        wxLogMessage( _("Nothing to do! The is no active file for optimization") );
        return;  
	}
                           
    AxProgressDlg *dlg = new AxProgressDlg( m_framePtr, -1, _("Optimization") );
    dlg->Center( wxBOTH );
    dlg->Show();
	dlg->SetMaxBatchBar( 6 );	
		
	bool fast = false;
    wxArrayPtrVoid typ_params;	
	typ_params.Add( &nbOfFiles );
	typ_params.Add( &paths );
	typ_params.Add( &filenames );
	typ_params.Add( &isCacheOk );
	typ_params.Add( &fast ); // bool, fast or not
    typ_params.Add( m_typModelPtr );
	
    wxArrayPtrVoid mus_params;
	mus_params.Add( &nbOfFiles );
	mus_params.Add( &paths );
	mus_params.Add( &isCacheOk );
	mus_params.Add( m_musModelPtr );
	mus_params.Add( m_typModelPtr ); // to get the merged dictionnary
	
	bool failed = false;
	
    if ( !failed )
        failed = !m_recBookFilePtr->TypAdaptation( typ_params, dlg );
		
	if ( !failed ) // load optimized model because we use the merged dictionnary for MusAdaptation
					// not really nice...
		failed = (!m_typModelPtr->Close() || !m_typModelPtr->Open( m_recBookFilePtr->GetTypFilename() ));
		
    if ( !failed )
        failed = !m_recBookFilePtr->MusAdaptation( mus_params, dlg );
		
	if ( !failed )
		m_recBookFilePtr->m_fullOptimized = true;
	
    imCounterEnd( dlg->GetCounter() );

    dlg->AxShowModal( failed ); // stop process
    dlg->Destroy();

    m_recBookPtr->Update(); 
}

/*
void RecEnv::OnBookOptimize( wxCommandEvent &event )
{
    wxLogMessage( _("Temporary method for leave-one-out cross validation") );
    
    wxString outputDir = wxDirSelector( "Choose", wxGetApp().m_lastDirBatch_out );
    if ( outputDir.empty() )
        return;
    
    if ( AxProgressDlg::s_instance_existing )
        return;
    
    if ( !m_recBookFilePtr->IsOpened() )
        return;
    
    wxArrayString paths, filenames;
    size_t nbOfFiles;
    
    // Reset the optimization because cache need to be empty
    m_recBookFilePtr->ResetAdaptation( false );
    
	bool isCacheOk;
    nbOfFiles = m_recBookFilePtr->FilesForAdaptation( &filenames, &paths, &isCacheOk );
    // Force cache to be false here
    isCacheOk = false;
    if ( nbOfFiles == 0 )
    {
        wxLogMessage( _("Nothing to do! The is no active file for optimization") );
        return;  
	}
    
    AxProgressDlg *dlg = new AxProgressDlg( m_framePtr, -1, _("Optimization for leave-one-out cross validation") );
    dlg->Center( wxBOTH );
    dlg->Show();
	dlg->SetMaxBatchBar( 6 * nbOfFiles );	
    
    bool failed = false;

    int i;
    for ( i = 0; i < nbOfFiles; i++ )
    {
        m_recBookFilePtr->ResetAdaptation( false );
        
        wxString modelName = filenames[i];
        filenames.RemoveAt( i );
        
        // we do fast adaption here
        bool fast = true;
        wxArrayPtrVoid typ_params;	
        typ_params.Add( &nbOfFiles );
        typ_params.Add( &paths );
        typ_params.Add( &filenames );
        typ_params.Add( &isCacheOk );
        typ_params.Add( &fast ); // bool, fast or not
        typ_params.Add( m_typModelPtr );
        
        wxArrayPtrVoid mus_params;
        mus_params.Add( &nbOfFiles );
        mus_params.Add( &paths );
        mus_params.Add( &isCacheOk );
        mus_params.Add( m_musModelPtr );
        mus_params.Add( m_typModelPtr ); // to get the merged dictionnary
        
        failed = false;
        
        if ( !failed )
            failed = !m_recBookFilePtr->TypAdaptation( typ_params, dlg );
               
        if ( !failed ) // load optimized model because we use the merged dictionnary for MusAdaptation
            // not really nice...
            failed = (!m_typModelPtr->Close() || !m_typModelPtr->Open( m_recBookFilePtr->GetTypFilename() ));
        
        if ( !failed )
            failed = !m_recBookFilePtr->MusAdaptation( mus_params, dlg );
        
        wxString typName = modelName;
        wxString musName = modelName;
        typName.Replace( ".axz", ".axtyp" );
        musName.Replace( ".axz", ".axmus" );
        
        wxCopyFile( m_recBookFilePtr->GetTypFilename(), outputDir + "/" + typName );
        wxCopyFile( m_recBookFilePtr->GetMusFilename(), outputDir + "/" + musName );
        
        wxLogMessage( modelName );
        
        filenames.Insert( modelName, i );
    }
    
	
    imCounterEnd( dlg->GetCounter() );
    
    dlg->AxShowModal( failed ); // stop process
    dlg->Destroy();
    
}
*/


void RecEnv::OnBookRecognize( wxCommandEvent &event )
{
    if ( AxProgressDlg::s_instance_existing )
        return;
        
    if ( !m_recBookFilePtr->IsOpened() )
        return;

    if ( m_recBookFilePtr->IsModified() )
        m_recBookFilePtr->Save();

    wxArrayString paths, filenames;
    size_t nbOfFiles;
    
    nbOfFiles = m_recBookFilePtr->FilesToRecognize( &filenames, &paths );
    if ( nbOfFiles == 0 )
    {
        wxLogMessage( _("Nothing to do! All active files are already recognized") );
        return;  
	}
                           
    AxProgressDlg *dlg = new AxProgressDlg( m_framePtr, -1, _("Batch recognition") );
    dlg->Center( wxBOTH );
    dlg->Show();
    dlg->SetMaxBatchBar( nbOfFiles );
     
    wxArrayPtrVoid params;
    params.Add( m_typModelPtr );
    params.Add( m_musModelPtr );
    params.Add( &RecEnv::s_rec_delayed );
    params.Add( &RecEnv::s_rec_lm_order );
    params.Add( &RecEnv::s_rec_lm_scaling );
    wxString rec_wrdtrns = "";
    params.Add( &rec_wrdtrns );
    
    wxArrayPtrVoid adapt_params;
    adapt_params.Add( m_typModelPtr );
	adapt_params.Add( m_musModelPtr );
    
    bool failed_once = false;
    
    if ( m_recBookFilePtr->IsOpened() )
        failed_once = !m_recBookFilePtr->FastAdaptation( adapt_params, dlg );
        
    //wxLogDebug("Typographic model %s", m_typModelPtr->m_filename.c_str() );

    wxString ext, shortname, outfile;
    for ( int i = 0; i < (int)nbOfFiles; i++ )
    {
        if ( dlg->GetCanceled() )
            break;

        wxString outfile = paths[i];

        RecFile recFilePtr( "rec_batch2_file" );
        recFilePtr.New();
            
        bool failed = false;
        
        if ( !failed )
            failed = !recFilePtr.Open( paths[i] );

        if ( !failed )
            failed = !recFilePtr.Recognize( params, dlg );

        if ( !failed )
            failed = !recFilePtr.SaveAs( outfile );
            
        if ( failed )
            failed_once = true;
    }

    imCounterEnd( dlg->GetCounter() );

    dlg->AxShowModal( failed_once ); // stop process  ( failed ???? )
    dlg->Destroy();
    
    m_recBookPtr->Update();
}

void RecEnv::OnBookPreprocess( wxCommandEvent &event )
{
    if ( AxProgressDlg::s_instance_existing )
        return;
        
    if ( !m_recBookFilePtr->IsOpened() )
        return;
    
    if ( m_recBookFilePtr->IsModified() )
        m_recBookFilePtr->Save();

    wxArrayString paths, filenames;
    size_t nbOfFiles;
    
    nbOfFiles = m_recBookFilePtr->FilesToPreprocess( &filenames, &paths );
    if ( nbOfFiles == 0 )
    {
        wxLogMessage( _("Nothing to do! All active files are already preprocessed") );
        return;  
     }

    wxString output = m_recBookFilePtr->m_axFileDir;
    output += wxFileName::GetPathSeparator();
    
    AxProgressDlg *dlg = new AxProgressDlg( m_framePtr, -1, _("Batch preprocessing") );
    dlg->Center( wxBOTH );
    dlg->Show();
    dlg->SetMaxBatchBar( nbOfFiles );
    
    bool failed_once = false;

    wxString ext, shortname, infile;
    for ( int i = 0; i < (int)nbOfFiles; i++ )
    {
        if ( dlg->GetCanceled() )
            break;

        infile = paths[i];
        wxFileName::SplitPath( paths[i], NULL, &shortname, &ext );

        RecFile recFilePtr( "rec_batch1_file" );
        recFilePtr.New();
        
		recFilePtr.SetBinarization( m_recBookFilePtr->m_pre_image_binarization_method, 
									m_recBookFilePtr->m_pre_page_binarization_method,
									m_recBookFilePtr->m_pre_page_binarization_method_size);
		
        wxArrayPtrVoid params;
        params.Add( &infile );
    
        bool failed = false;

        if ( !failed )
            failed = !recFilePtr.Preprocess( params, dlg );

        if ( !failed )
            failed = !recFilePtr.SaveAs( output + shortname + ".axz" );
            
        if ( failed )
            failed_once = true;
    }

    imCounterEnd( dlg->GetCounter() );

    dlg->AxShowModal( failed_once  ); // stop process  ( failed ???? )
    dlg->Destroy();
    
    m_recBookPtr->Update();
}

void RecEnv::OnBookFullProcess( wxCommandEvent &event )
{
    // force dialog to close at the end
    AxProgressDlg::s_close_at_end = true; 
    
    this->OnBookPreprocess( event );
    
    wxGetApp().Yield( );
    // force this, because the previous dialog might have not been deleted yet
    AxProgressDlg::s_instance_existing = false; 
    
    this->OnBookRecognize( event );
}

void RecEnv::OnBookLoad( wxCommandEvent &event )
{
    if ( !m_recBookFilePtr->IsOpened() )
        return;

    m_recBookFilePtr->LoadImages();
    m_recBookFilePtr->LoadAxfiles();
    m_recBookPtr->Update();
}


void RecEnv::OnSaveBookAs( wxCommandEvent &event )
{
    if ( !m_recBookFilePtr->IsOpened() )
        return;
		
    m_recBookFilePtr->SaveAs();  
}

void RecEnv::OnSaveBook( wxCommandEvent &event )
{
    if ( !m_recBookFilePtr->IsOpened() )
        return;

    m_recBookFilePtr->Save();    
}

void RecEnv::OnOpenBook( wxCommandEvent &event )
{
    if ( !this->CloseAll( ) )
        return;

    wxString filename = AxFile::Open( AX_FILE_PROJECT );
    int type, envtype;
    if ( !AxFile::Check( filename, &type, &envtype ) )
        return;
    if  (envtype != AX_FILE_RECOGNITION)
    {
        wxLogError("Invalid file");
        return;
    }
    OpenBookFile( filename );
}

void RecEnv::OnCloseBook( wxCommandEvent &event )
{
    CloseAll( );
}

void RecEnv::OnNewBook( wxCommandEvent &event )
{
    if ( !CloseAll( ) )
        return;
        
    m_recBookFilePtr->New();
    RecBookDataDlg dlg( m_framePtr, -1, _("New book"), m_recBookFilePtr );
    dlg.Center( wxBOTH );
    if ( dlg.ShowModal() == wxID_OK )
    {
        m_recBookFilePtr->Modify();
        wxGetApp().AxBeginBusyCursor( );
        m_bookSplitterPtr->SplitVertically( m_recBookPanelPtr, m_pageSplitterPtr, RecEnv::s_book_sash );
        m_recBookPtr->Build();
        wxGetApp().AxEndBusyCursor();
    }
    
}

void RecEnv::OnNew( wxCommandEvent &event )
{
    if ( !CloseAll( ) )
        return;

    OpenFile( "" , -1 );
}

void RecEnv::OnSave( wxCommandEvent &event )
{
    wxASSERT_MSG( m_recFilePtr, wxT("RecFile cannot be NULL") );

    m_recFilePtr->Save();

    if ( m_recBookFilePtr->IsOpened() )
        m_recBookPtr->Update();
}

void RecEnv::OnSaveAs( wxCommandEvent &event )
{
    wxASSERT_MSG( m_recFilePtr, wxT("RecFile cannot be NULL") );

    m_recFilePtr->SaveAs();
    //UpdateViews( 0 ); // for testing
    UpdateTitle();
    
    if ( m_recBookFilePtr->IsOpened() )
        m_recBookPtr->Update();
}

void RecEnv::OnClose( wxCommandEvent &event )
{
    ResetFile();
}


// meta batch
void RecEnv::BatchAdaptation( )
{
#ifdef META_BATCH3
    if ( AxProgressDlg::s_instance_existing )
        return;

    wxArrayString paths, filenames;
    size_t nbOfFiles;
    
    wxString input = wxDirSelector( "Choose", wxGetApp().m_lastDirBatch_in );
    if ( input.empty() )
            return;
            
    wxDir dir( input );

    if ( !dir.IsOpened() )
        return;

    wxString filename;

    bool cont = dir.GetFirst(&filename, "", wxDIR_DIRS );
    while ( cont )
    {
        wxLogMessage(filename);
        paths.Clear();
        filenames.Clear();
        nbOfFiles = wxDir::GetAllFiles( dir.GetName() + "/" + filename, &paths, "*.axz" );
    
        //nbOfFiles = AxFileSelector( AX_FILE_DEFAULT, &filenames, &paths, m_framePtr );    
        
        for ( int i = 0; i < (int)nbOfFiles; i++ )
        {
            wxFileName name( paths[i] );
            filenames.Add( name.GetFullName() );
        }
                
        // input AXZ files : choose a directory of select muliple files
        if ( nbOfFiles == 0 )
            continue;   
    
        // name of model to generate - temporary...
        //
#ifdef META_BATCH_MUS    
        RecMusModel model( "rec_batch3_model" );
#else
		RecTypModel model( "rec_batch3_model" );
#endif
		model.New();    

        //Reset();

        wxArrayPtrVoid params;
        params.Add( &nbOfFiles );
        params.Add( &paths );
    
        AxProgressDlg *dlg = new AxProgressDlg( m_framePtr, -1, _("Batch") );
        dlg->Center( wxBOTH );
        dlg->Show();
        dlg->SetMaxBatchBar( 2 );
    
        bool failed = false;
    
        if ( !failed )
            failed = !model.AddFiles( params, dlg );

        if ( !failed )  
            failed = !model.Commit( dlg );

#ifdef META_BATCH_MUS
	    if ( !failed )  
			failed = !model.Train( params, dlg );
#endif
		
		imCounterEnd( dlg->GetCounter() );

        dlg->AxShowModal( ); // stop process  ( failed ???? )
        dlg->Destroy();

#ifdef META_BATCH_MUS    
        model.SaveAs( filename + ".axmus" );
#else
		model.SaveAs( filename + ".axtyp" );
#endif
    
        cont = dir.GetNext(&filename);
    }
#endif // META_BATCH3
}



/*
void RecEnv::OnBatch4( wxCommandEvent &event )
{
    if ( AxProgressDlg::s_instance_existing )
        return;

    wxArrayString paths, filenames;
    size_t nbOfFiles;
    
    nbOfFiles = AxFileSelector( AX_FILE_DEFAULT, &filenames, &paths, m_framePtr );  
                
    // input AXZ files : choose a directory of select muliple files
    if ( nbOfFiles == 0 )
        return; 
    
    // name of model to generate - temporary...
    RecMusModel model( "rec_batch4_model" );
    model.New();    

    //Reset();
    
    wxArrayPtrVoid params;
    params.Add( &nbOfFiles );
    params.Add( &paths );
    
    AxProgressDlg *dlg = new AxProgressDlg( m_framePtr, -1, _("Batch") );
    dlg->Center( wxBOTH );
    dlg->Show();
    dlg->SetMaxBatchBar( 2 );
    
    bool failed = false;
    
    if ( !failed )
        failed = !model.AddFiles( params, dlg );

    if ( !failed )  
        failed = !model.Commit( dlg );

    imCounterEnd( dlg->GetCounter() );

    dlg->AxShowModal( ); // stop process  ( failed ???? )
    dlg->Destroy();
    
    if ( !failed )
        failed = !model.Save();
}
*/

void RecEnv::OnTools( wxCommandEvent &event )
{
    wxASSERT_MSG( m_musViewPtr, "WG Window cannot be NULL ");

    if ( event.GetId() == ID4_INSERT_MODE )
        
        m_musViewPtr->SetEditorMode( event.IsChecked() ? MUS_EDITOR_INSERT : MUS_EDITOR_EDIT );
    else 
    {
        m_musViewPtr->SetEditorMode( MUS_EDITOR_INSERT );
        if ( event.GetId() == ID4_NOTES )
            m_musViewPtr->SetToolType( MUS_TOOLS_NOTES );
        else if ( event.GetId() == ID4_KEYS)
            m_musViewPtr->SetToolType( MUS_TOOLS_CLEFS );
        else if ( event.GetId() == ID4_SIGNS )
            m_musViewPtr->SetToolType( MUS_TOOLS_PROPORTIONS );
        else if ( event.GetId() == ID4_SYMBOLES )
            m_musViewPtr->SetToolType( MUS_TOOLS_OTHER );
    }
}

void RecEnv::OnStaffCorrespondence( wxCommandEvent &event )
{
    m_musControlPtr->InverseShowStaffBitmap( );
}


/* 
    Methode de travail !!!!!!!!! Pas DU TOUT plombee !!!!
  */ 
  
//#include "mus/mussvg.h"

/*
void RecEnv::OnExportImage( wxCommandEvent &event )
{
    wxString filename;
    filename = wxFileSelector( _("Save"), wxGetApp().m_lastDirTIFF_out, _T(""), NULL, IMAGE_FILES_SELECTION, wxFD_SAVE);
    if (filename.IsEmpty())
        return;
        
    wxGetApp().m_lastDirTIFF_out = wxPathOnly( filename );

    wxMemoryDC memDC;
    wxBitmap bitmap( m_musViewPtr->ToRendererX( m_musViewPtr->m_pageWidth + 30 )  ,
        m_musViewPtr->ToRendererX( m_musViewPtr->m_paperHeight + 10 )); // marges bricolees ...
    memDC.SelectObject(bitmap);
    memDC.SetBackground(*wxWHITE_BRUSH);
    memDC.Clear();
    memDC.SetLogicalOrigin( m_musViewPtr->m_leftMargin,10 );
    //memDC.SetPen(*wxRED_PEN);
    //memDC.SetBrush(*wxTRANSPARENT_BRUSH);
    m_musViewPtr->m_page->DrawPage( &memDC, false );
    memDC.SelectObject(wxNullBitmap);
    wxImage image = bitmap.ConvertToImage().ConvertToMono( 255 ,255 , 255 );
    image.SaveFile( filename );
    //bitmap.SaveFile( filename, wxBITMAP_TYPE_BMP );
}*/

void RecEnv::OnExportImage( wxCommandEvent &event )
{
    /*
    wxString filename;
    filename = wxFileSelector( _("Save"), wxGetApp().m_lastDirTIFF_out, _T(""), NULL, IMAGE_FILES_SELECTION, wxFD_SAVE);
    if (filename.IsEmpty())
        return;
        
    wxGetApp().m_lastDirTIFF_out = wxPathOnly( filename );
    
    MusSVGFileDC svgDC (filename, m_musViewPtr->ToRendererX( m_musViewPtr->m_pageWidth + 30 )  ,
        m_musViewPtr->ToRendererX( m_musViewPtr->m_paperHeight + 10 )) ;
	svgDC.SetTextForeground( *wxBLACK );
	//dc.SetMapMode( wxMM_TEXT );
	svgDC.SetAxisOrientation( true, false );
   m_musViewPtr->m_page->DrawPage( &svgDC, false );
   */
   wxLogWarning( "Not implemented");
}
  

void RecEnv::OnExportCmme( wxCommandEvent &event )
{
    /*
   	if ( !m_recFilePtr->IsRecognized() )
		return;
   
    wxString filename;
    filename = wxFileSelector( _("Save"), wxGetApp().m_lastDirAX0_out, m_recFilePtr->m_shortname + ".xml", "xml", "CMME XML|*.xml", wxFD_SAVE);
    if (filename.IsEmpty())
        return;
        
    wxGetApp().m_lastDirAX0_out = wxPathOnly( filename );
    
    // save
    MusCmmeOutput *cmme_output = new MusCmmeOutput( m_recFilePtr->m_musDocPtr, filename );
    cmme_output->ExportFile();
    delete cmme_output;
    */
    wxLogWarning( "Not implemented");
}

void RecEnv::OnExportWWG( wxCommandEvent &event )
{
   	if ( !m_recFilePtr->IsRecognized() )
		return;
   
    wxString filename;
    filename = wxFileSelector( _("Save"), wxGetApp().m_lastDirAX0_out, m_recFilePtr->m_shortname + ".wwg", "wwg", "Wolfgang WWG|*.wwg", wxSAVE);
    if (filename.IsEmpty())
        return;
        
    wxGetApp().m_lastDirAX0_out = wxPathOnly( filename );
    
    // save
    MusWWGOutput *wwg_output = new MusWWGOutput( m_recFilePtr->m_musDocPtr, filename.c_str() );
    wwg_output->ExportFile();
    delete wwg_output;
}

void RecEnv::OnAdjust( wxCommandEvent &event )
{
    if (!m_imViewPtr)
        return;

    if ( event.GetId() == ID4_ADJUST)
        m_imViewPtr->SetAdjustMode( ADJUST_BOTH );
    else if ( event.GetId() == ID4_ADJUST_V)
        m_imViewPtr->SetAdjustMode( ADJUST_VERTICAL );
    else if ( event.GetId() == ID4_ADJUST_H)
        m_imViewPtr->SetAdjustMode( ADJUST_HORIZONTAL );

    m_imControlPtr->SyncSelectionBitmap();
    m_musControlPtr->SyncZoom();      
}

void RecEnv::OnZoom( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imViewPtr, "View cannot be NULL ");

    if ( (event.GetId() == ID4_ZOOM_OUT) && m_imViewPtr->CanZoomOut() )
        m_imViewPtr->ZoomOut(); 
    else if ( (event.GetId() == ID4_ZOOM_IN) && m_imViewPtr->CanZoomIn() )
        m_imViewPtr->ZoomIn();
    
    m_imControlPtr->SyncSelectionBitmap();
    m_musControlPtr->SyncZoom();
}

void RecEnv::OnCancelRecognition( wxCommandEvent &event )
{
    if ( m_recFilePtr->IsRecognized() )
    {
        if ( !m_recFilePtr->CancelRecognition( true ) )
            return;
        else // just update view
        { 
            UpdateViews( 0 );
            m_recFilePtr->Save();
            if ( m_recBookFilePtr->IsOpened() )
                m_recBookPtr->Update();
        }
    }
}

void RecEnv::OnRun( wxCommandEvent &event )
{
    if ( !m_recFilePtr->IsPreprocessed() ){
		this->Preprocess();
	}
	else if ( !m_recFilePtr->IsRecognized() )
        this->Recognize();
}

void RecEnv::Recognize(  )
{
    wxASSERT_MSG( m_imControlPtr, wxT("Image controller cannot be NULL") );
    wxASSERT_MSG( m_imViewPtr, wxT("View cannot be NULL") );
    wxASSERT_MSG( m_recFilePtr, wxT("RecFile cannot be NULL") );
    wxASSERT_MSG( m_typModelPtr, wxT("RecTypModel cannot be NULL") );
    wxASSERT_MSG( m_musModelPtr, wxT("RecMusModel cannot be NULL") );
       
    wxASSERT( !m_recFilePtr->IsRecognized() );
    if ( m_recFilePtr->IsRecognized() ) // do nothing, should not happen
        return;
        
    if ( AxProgressDlg::s_instance_existing )
        return;
        
    AxProgressDlg *dlg = new AxProgressDlg( m_framePtr, -1, _("Recognition") );
    dlg->Center( wxBOTH );
    dlg->Show();
    dlg->SetMaxBatchBar( 1 );
    
    UpdateViews( 0 );
        
    m_imControlPtr->CancelMagicSelection();
    ResetModels();
    
    wxArrayPtrVoid adapt_params;
    adapt_params.Add( m_typModelPtr );
	adapt_params.Add( m_musModelPtr );
        
    // params 0: RecTypModel: typModelPtr
    // params 1: RecMusModel: musModelPtr
    // params 2: bool: rec_delayed
    // params 3: int: rec_lm_order
    // params 4: double: rec_lm_scaling
    // params 5: wxString: rec_wrdtrns
    wxArrayPtrVoid params;
    params.Add( m_typModelPtr );
    params.Add( m_musModelPtr );
    params.Add( &RecEnv::s_rec_delayed );
    params.Add( &RecEnv::s_rec_lm_order );
    params.Add( &RecEnv::s_rec_lm_scaling );
    wxString rec_wrdtrns = "";
    params.Add( &rec_wrdtrns );
    
    bool failed = false;
    
    if ( m_recBookFilePtr->IsOpened() )
        failed = !m_recBookFilePtr->FastAdaptation( adapt_params, dlg );
    
    failed = !m_recFilePtr->Recognize( params, dlg );

    imCounterEnd( dlg->GetCounter() );

    dlg->AxShowModal( ( failed || m_recFilePtr->GetError() != ERR_NONE ) ); // stop process
    dlg->Destroy();

    if ( failed || m_recFilePtr->GetError() != ERR_NONE ) // operation annulee ou failed
        return;
        
    if ( m_recBookFilePtr->IsOpened() )
    {
        m_recFilePtr->SaveAs( m_recFilePtr->m_filename );
        m_recBookPtr->Update();
    }
           
    UpdateViews( 0 );
}


void RecEnv::Preprocess( )
{
    wxASSERT_MSG( m_imControlPtr, wxT("Image controller cannot be NULL") );
    wxASSERT_MSG( m_imViewPtr, wxT("View cannot be NULL") );
    wxASSERT_MSG( m_recFilePtr, wxT("RecFile cannot be NULL") );
    
    wxASSERT( !m_recFilePtr->IsPreprocessed() );
    if ( m_recFilePtr->IsPreprocessed() ) // do nothing, should not happen
        return;
    
    if ( !m_imControlPtr->Ok() || !m_imControlPtr->HasFilename() )
        return;
        
    if ( AxProgressDlg::s_instance_existing )
        return;
        
    // if a book is opened, check if the file has to be preprocessed
    if ( m_recBookFilePtr->IsOpened() && !m_recBookFilePtr->HasToBePreprocessed( m_imControlPtr->GetFilename() ) )
        return;
	
	// if a book is opened, use the book binarization parameters
	if ( m_recBookFilePtr->IsOpened() && m_recBookFilePtr->m_pre_page_binarization_select == true ){
		RecBinSelectDlg *bindlg = new RecBinSelectDlg( m_framePtr, -1, _("Binarization Selection"), m_recFilePtr, m_recBookFilePtr );
		bindlg->Center( wxBOTH );
		bindlg->ShowModal();
		bindlg->Destroy();
	} else if ( !m_recBookFilePtr->IsOpened() && ImPage::s_pre_page_binarization_select == true ){
		RecBinSelectDlg *bindlg = new RecBinSelectDlg( m_framePtr, -1, _("Binarization Selection"), m_recFilePtr, NULL );
		bindlg->Center( wxBOTH );
		bindlg->ShowModal();
		bindlg->Destroy();
	}
	
    AxProgressDlg *dlg = new AxProgressDlg( m_framePtr, -1, _("Preprocessing") );
    dlg->Center( wxBOTH );
    dlg->Show();
    dlg->SetMaxBatchBar( 1 );
    
    wxString file = m_imControlPtr->GetFilename();
    wxFileName::SplitPath( file, NULL, &m_recFilePtr->m_shortname, NULL );
    
    wxArrayPtrVoid params;
    params.Add( &file );
    
    bool failed = false;
    
    failed = !m_recFilePtr->Preprocess( params, dlg );

    imCounterEnd( dlg->GetCounter() );

    dlg->AxShowModal( ( failed || m_recFilePtr->GetError() != ERR_NONE ) ); // stop process
    dlg->Destroy();

    if ( failed || m_recFilePtr->GetError() != ERR_NONE ) // operation annulee ou failed
        return;
        
    if ( m_recBookFilePtr->IsOpened() )
    {
        m_recFilePtr->SaveAs( m_recBookFilePtr->m_axFileDir + wxFileName::GetPathSeparator() + m_recFilePtr->m_shortname + ".axz" );
        m_recBookPtr->Update();
    }

    UpdateViews( 0 );
}

void RecEnv::OnPaste( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControlPtr, wxT("Image controller cannot be NULL") );

    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
        return;

    if (win->GetId() == ID4_VIEW)
        m_imControlPtr->Paste();
    else if (m_musViewPtr && (win->GetId() == ID4_MUSWINDOW))
        m_musViewPtr->Paste();
}

void RecEnv::OnCopy( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControlPtr, wxT("Image controller  cannot be NULL") );

    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
        return;

    if (win->GetId() == ID4_VIEW)
        m_imControlPtr->Copy();
    else if (m_musViewPtr && (win->GetId() == ID4_MUSWINDOW))
        m_musViewPtr->Copy();
}

void RecEnv::OnCut( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControlPtr, wxT("Image controller cannot be NULL") );

    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
        return;

    if (win->GetId() == ID4_VIEW)
        m_imControlPtr->Cut();
    else if (m_musViewPtr && (win->GetId() == ID4_MUSWINDOW))
        m_musViewPtr->Cut();
}


void RecEnv::OnUndo( wxCommandEvent &event )
{
    wxASSERT_MSG( m_recFilePtr, wxT("RecFile cannot be NULL") );

    if ( m_recFilePtr->IsRecognized() )
    {
       if (m_musViewPtr && m_musViewPtr->CanUndo() )
        m_musViewPtr->Undo();
    }
    else
    {
        if (m_recFilePtr->m_imPagePtr && m_recFilePtr->m_imPagePtr->CanUndo() )
        {
            m_recFilePtr->m_imPagePtr->Undo();
            UpdateViews( REC_UPDATE_FAST );
        }
    }
}

void RecEnv::OnRedo( wxCommandEvent &event )
{
    wxASSERT_MSG( m_recFilePtr, wxT("RecFile cannot be NULL") );

    if ( m_recFilePtr->IsRecognized() )
    {
        if (m_musViewPtr && m_musViewPtr->CanRedo() )
            m_musViewPtr->Redo();
    }
    else
    {
        if (m_recFilePtr->m_imPagePtr && m_recFilePtr->m_imPagePtr->CanRedo() )
        {
            m_recFilePtr->m_imPagePtr->Redo();
            UpdateViews( REC_UPDATE_FAST );
        }
    }
}

void RecEnv::OnOpenMEI( wxCommandEvent &event )
{
/*
    wxASSERT_MSG( m_musPanelPtr, "Panel cannot be NULL ");
	
    //if ( !this->ResetFile( ) )
    //    return;
	m_recFilePtr->New();
	
    m_imControlPtr->Open();
	
	if ( !m_recFilePtr->IsPreprocessed() ){
		this->Preprocess();
	}
	else if ( !m_recFilePtr->IsRecognized() )
        this->Recognize();
	
	m_musControlPtr->SetForMEI();
	
	wxString filename = wxFileSelector( _("Import MEI - Choose MEI"), wxGetApp().m_lastDir, _T(""), _T(""), "MEI Files|*.mei|XML Files|*.xml", wxFD_OPEN);
    if ( filename.IsEmpty() )
        return;
    wxGetApp().m_lastDir = wxPathOnly( filename );
    
    //m_recFilePtr->m_musDocPtr->New();
	//m_recFilePtr->m_musDocPtr->m_pages.Clear();
	m_recFilePtr->SetforMEI(); //may be unnecessary
	
    MeiInput meiinput( m_recFilePtr->m_musDocPtr, filename );
	if ( !meiinput.ImportFile() )
		return;
	
	m_recFilePtr->m_musDocPtr->m_parameters.m_notationMode = MUS_NEUMATIC_MODE; //temporary for liber usualis project
	
	m_musViewPtr->SetEditorMode(MUS_EDITOR_EDIT);
    
    */
    wxLogMessage("Fix me"); // ax2
	
	/*MeiElement *root = m_recFilePtr->m_musDocPtr->GetMeiDocument()->getRootElement();
	FacsTable table;
	meiinput.ReadFacsTable(root, &table);
	
	m_recFilePtr->m_musDocPtr->m_parameters.param.m_paperWidth = m_recFilePtr->m_imPagePtr->m_size.GetWidth() / 10;
	m_recFilePtr->m_musDocPtr->m_parameters.param.m_paperHeight = m_recFilePtr->m_imPagePtr->m_size.GetHeight() / 10;
	
	for (int i = 0; i < (int)m_recFilePtr->m_musDocPtr->m_pages[0].GetStaffCount(); i++)
	{
		MeiElement *element = m_recFilePtr->m_musDocPtr->m_pages[0].m_staves[i].m_meiref;
		m_musViewPtr->kPos[i].yp = m_recFilePtr->m_musDocPtr->m_parameters.param.m_paperHeight - table.GetUY(element->getAttribute("systemref")->getValue())/20 + m_musViewPtr->m_staffSize[m_recFilePtr->m_musDocPtr->m_pages[0].m_staves[i].staffSize];
	}
	
    int x1 = 5, x2 = 195;
    m_recFilePtr->m_imPagePtr->CalcLeftRight( &x1, &x2 ); 
	//x1 = 0; // force it, indentation will be calculated staff by staff
    m_recFilePtr->m_musDocPtr->m_parameters.param.m_leftMarginOddPage = x1 / 10;
    m_recFilePtr->m_musDocPtr->m_parameters.param.m_leftMarginEvenPage = x1 / 10;
    m_recFilePtr->m_musDocPtr->m_pages[0].lrg_lign = (x2 - x1) / 10;*/
	
    UpdateViews( 0 );
}

void RecEnv::OnSaveMEI( wxCommandEvent &event )
{

	if (  !m_musPanelPtr || !m_recFilePtr )
        return;
    
    wxString filename;
    filename = wxFileSelector( _("Save"), wxGetApp().m_lastDirAX0_out, m_recFilePtr->m_shortname + ".mei", "wwg", "MEI Files|*.mei|XML Files|*.xml", wxFD_SAVE);
    if (filename.IsEmpty())
        return;
    
    MeiOutput *mei_output = new MeiOutput( m_recFilePtr->m_musDocPtr, filename.c_str() );
    mei_output->ExportFile();
    delete mei_output;
}


void RecEnv::OnUpdateUI( wxUpdateUIEvent &event )
{
    wxASSERT_MSG( m_imControlPtr, wxT("Image controller cannot be NULL") );
    wxASSERT_MSG( m_imViewPtr, wxT("View cannot be NULL") );
    wxASSERT_MSG( m_musViewPtr, "WG Window cannot be NULL ");
    wxASSERT_MSG( m_recFilePtr, "RecFile cannot be NULL ");
	wxASSERT_MSG( m_recBookFilePtr, "RecBookFile cannot be NULL ");
    
    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
    {
        // wxLogDebug("RecEnv::OnUpdateUI : no focus window" );
        return;
    }

    int id = event.GetId();

    if (id == ID_CUT)
    {
    
        //event.Enable( true );
        //wxLogDebug("RecEnv::OnUpdateUI : update cut" );
        if (win->GetId() == ID4_VIEW)
           event.Enable( m_imControlPtr->CanCut() );
        else if (m_musViewPtr && (win->GetId() == ID4_MUSWINDOW))
            event.Enable( (m_musViewPtr && m_musViewPtr->CanCut()));
        else
            event.Enable( false );
            
    }
    else if (id == ID_COPY)
    {
        if (win->GetId() == ID4_VIEW)
           event.Enable( m_imControlPtr->CanCopy() );
        else if (m_musViewPtr && (win->GetId() == ID4_MUSWINDOW))
            event.Enable( (m_musViewPtr && m_musViewPtr->CanCopy()));
        else
            event.Enable( false );
    }
    else if (id == ID_PASTE)
    {
        if (win->GetId() == ID4_VIEW)
           event.Enable( m_imControlPtr->CanPaste() );
        else if (m_musViewPtr && (win->GetId() == ID4_MUSWINDOW))
            event.Enable( (m_musViewPtr && m_musViewPtr->CanPaste()));
        else
            event.Enable( false );
    }
    else if (id == ID_UNDO)
    {
        if ( m_recFilePtr->IsRecognized() && m_musViewPtr && m_musViewPtr->CanUndo() )
            event.Enable( true );
        else if ( m_recFilePtr->IsPreprocessed() && m_recFilePtr->m_imPagePtr && m_recFilePtr->m_imPagePtr->CanUndo() )
            event.Enable( true );
        else
            event.Enable( false );
    }
    else if (id == ID_REDO)
    {
        if ( m_recFilePtr->IsRecognized() && m_musViewPtr && m_musViewPtr->CanRedo() )
            event.Enable( true );
        else if ( m_recFilePtr->IsPreprocessed() && m_recFilePtr->m_imPagePtr && m_recFilePtr->m_imPagePtr->CanRedo() )
            event.Enable( true );
        else
            event.Enable( false );
    }
    else if (id == ID4_PROCESS)
        event.Enable( m_imControlPtr->Ok() && !m_recFilePtr->IsRecognized() );
    else if (id == ID4_CANCEL_REC)
        event.Enable( m_recFilePtr->IsRecognized() );
    else if (id == ID4_ZOOM_OUT)
        event.Enable( m_imViewPtr->CanZoomOut() );
    else if (id == ID4_ZOOM_IN)
        event.Enable( m_imViewPtr->CanZoomIn() );
    else if (id == ID4_ADJUST)
        event.Check( m_imViewPtr->GetAdjustMode() == ADJUST_BOTH );
    else if (id == ID4_ADJUST_V)
        event.Check( m_imViewPtr->GetAdjustMode() == ADJUST_VERTICAL );
    else if (id == ID4_ADJUST_H)
        event.Check( m_imViewPtr->GetAdjustMode() == ADJUST_HORIZONTAL );
    else if (id == ID_SAVE )
        event.Enable( m_recFilePtr->IsModified() );
    else if (id == ID_SAVE_AS )
        event.Enable( m_recFilePtr->IsPreprocessed() );
    else if (id == ID_CLOSE )
        event.Enable( m_recFilePtr->IsPreprocessed() );
    else if (id == ID4_EXPORT_IMAGE )
        event.Enable( false ); // not implemented
        //event.Enable( m_recFilePtr->IsRecognized() );
    else if (id == ID4_EXPORT_CMME )
        event.Enable( false ); // not implemented
        //event.Enable( m_recFilePtr->IsRecognized() );
    else if (id == ID4_EXPORT_WWG )
        event.Enable( m_recFilePtr->IsRecognized() );
    else if (id == ID4_EXPORT_MEI )
        event.Enable( m_recFilePtr->IsRecognized() );
	// book
    else if (id == ID4_CLOSE_BOOK )
        event.Enable( m_recBookFilePtr->IsOpened() );
    else if (id == ID4_SAVE_BOOK )
        event.Enable( m_recBookFilePtr->IsOpened() );
    else if (id == ID4_SAVE_AS_BOOK )
        event.Enable( m_recBookFilePtr->IsOpened() );
    else if (id == ID4_BOOK_EDIT )
        event.Enable( m_recBookFilePtr->IsOpened() );
    else if (id == ID4_BOOK_LOAD )
        event.Enable( m_recBookFilePtr->IsOpened() );
    else if (id == ID4_BOOK_PRE )
        event.Enable( m_recBookFilePtr->IsOpened() );
    else if (id == ID4_BOOK_REC )
        event.Enable( m_recBookFilePtr->IsOpened() );
    else if (id == ID4_BOOK_FULL )
        event.Enable( m_recBookFilePtr->IsOpened() );
    else if (id == ID4_BOOK_ADAPT )
        event.Enable( m_recBookFilePtr->IsOpened() );
    else if (id == ID4_BOOK_RESET_ADAPT )
        event.Enable( m_recBookFilePtr->IsOpened() );
    else if (id == ID4_BOOK_EXPORT_MODELS )
         event.Enable( m_recBookFilePtr->IsOpened() && wxFileExists( m_recBookFilePtr->GetTypFilename() ) && wxFileExists( m_recBookFilePtr->GetMusFilename() ) );
    else if (id == ID4_BOOK_IMPORT_MODELS )
        event.Enable( m_recBookFilePtr->IsOpened() );
    else if (id == ID4_BOOK_EXPORT_MEI )
        event.Enable( m_recBookFilePtr->IsOpened() );

    else if (id == ID4_SHOW_STAFF_BMP )
    {
        event.Enable( m_recFilePtr->IsRecognized() );
        event.Check( m_musControlPtr->ShowStaffBitmap() );
    }
    else if ( event.GetId() == ID4_INSERT_MODE )
    {
        event.Enable( m_musViewPtr->IsShown() );
        event.Check( m_musViewPtr && m_musViewPtr->m_editorMode == MUS_EDITOR_INSERT );
    }
    else
        event.Enable(true);
}



//----------------------------------------------------------------------------
// RecBinSelectDlg
//----------------------------------------------------------------------------


RecBinSelectDlg::RecBinSelectDlg(wxWindow *parent, wxWindowID id, const wxString &title, RecFile *recfile, RecBookFile *recbookfile ) : 
wxDialog( parent, id, title )
{
	RecBinSelectDlgFunc( this, TRUE );

	GetCPageBin()->SetString(0, BRINK_2CLASSES_DESCRIPTION);
	GetCPageBin()->SetString(1, SAUVOLA_DESCRIPTION);
	GetCPageBin()->SetString(2, BRINK_3CLASSES_DESCRIPTION);
	
	this->GetCPageBin()->SetValidator( wxGenericValidator( &recfile->m_pre_page_binarization_method ) );
	this->GetScBinRgnSize()->SetValidator( wxGenericValidator( &recfile->m_pre_page_binarization_method_size ) );
	if ( recbookfile == NULL )
		this->GetCbDeactivateDlg()->SetValidator( wxGenericValidator( &ImPage::s_pre_page_binarization_select ) );
	else 
		this->GetCbDeactivateDlg()->SetValidator( wxGenericValidator( &recbookfile->m_pre_page_binarization_select ) );
}


#endif // AX_RECOGNITION

