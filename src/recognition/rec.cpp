/////////////////////////////////////////////////////////////////////////////
// Name:        rec.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_RECOGNITION

#ifdef __GNUG__
    #pragma implementation "rec.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//#include "wx/config.h"
//#include "wx/valgen.h"
//#include "wx/file.h"
//#include "wx/filename.h"
//#include "wx/dir.h"
#include "wx/log.h"

#include "rec.h"
#include "recim.h"
#include "recwg.h"
#include "recfile.h"
#include "recbookfile.h"
#include "recbookctrl.h"
#include "recmodels.h"
#include "recmlfbmp.h"

#include "app/axapp.h"
#include "app/axframe.h"

#include "im/impage.h"

#include "mus/musfile.h"
#include "mus/musiowwg.h"
#include "mus/mustoolpanel.h"

// statics
bool RecEnv::s_check = true;
bool RecEnv::s_deskew = true;
bool RecEnv::s_staves_position = true;
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
int RecEnv::s_pre_margin_top = 150;
int RecEnv::s_pre_margin_bottom = 120;
int RecEnv::s_pre_margin_left = 30;
int RecEnv::s_pre_margin_right = 20;
int RecEnv::s_pre_threshold_method_resize = PRE_BINARIZATION_OTSU;
int RecEnv::s_pre_threshold_method = PRE_BINARIZATION_BRINK;
int RecEnv::s_pre_threshold_region_size = 15;



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


// WDR: class implementations

//----------------------------------------------------------------------------
// RecSplitterWindow
//----------------------------------------------------------------------------

// WDR: event table for RecSplitterWindow

BEGIN_EVENT_TABLE(RecSplitterWindow,wxSplitterWindow)
    EVT_SPLITTER_DCLICK( -1, RecSplitterWindow::OnSashDoubleClick )
    EVT_SPLITTER_SASH_POS_CHANGED( -1, RecSplitterWindow::OnSashChanged)
END_EVENT_TABLE()

RecSplitterWindow::RecSplitterWindow( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxSplitterWindow( parent, id, position, size, style )
{
    m_envPtr = NULL;
}

void RecSplitterWindow::SetEnv( RecEnv *env, wxFlexGridSizer *sizer, WgToolPanel *toolpanel,    RecWgController *wgControlPtr )
{
    m_envPtr = env;
    m_wgsizer = sizer;
    m_toolpanel = toolpanel;
    m_wgControlPtr = wgControlPtr;
}

void RecSplitterWindow::ChangeOrientation( )
{
    /*
    if ( !IsSplit() )
        return;
        
    bool vertical = true;
    if ( this->GetSplitMode() == wxSPLIT_VERTICAL )
        vertical = false;

    wxWindow *win1 = this->GetWindow1();
    wxWindow *win2 = this->GetWindow2();
    this->Unsplit();
    win2->Show();

    if (vertical)
        this->SplitVertically( win1, win2 );
    else
        this->SplitHorizontally( win1, win2 );
    
    m_toolpanel->SetDirection( vertical );
    int cols = ( vertical ) ? 0 : 1;
    int rows = ( vertical ) ? 1 : 0;

    m_wgsizer->AddGrowableCol( rows );
    m_wgsizer->RemoveGrowableCol( cols );
    m_wgsizer->AddGrowableRow( cols );
    m_wgsizer->RemoveGrowableRow( rows );

    m_wgsizer->SetCols( cols );
    m_wgsizer->SetRows( rows );
    m_wgsizer->Layout();
    //m_wgsizer->SetSizeHints( win2 );


    if ( m_envPtr )
    {
        m_envPtr->SyncZoom();
    }
    */
}

// WDR: handler implementations for RecSplitterWindow

void RecSplitterWindow::OnSashChanged( wxSplitterEvent &event )
{
    UpdateSize();
    if ( m_envPtr )
    {
       // m_envPtr->SyncZoom();
    }
    event.Skip();
}

void RecSplitterWindow::OnSashDoubleClick( wxSplitterEvent &event )
{
    this->ChangeOrientation();
}


//----------------------------------------------------------------------------
// RecPanel
//----------------------------------------------------------------------------

// WDR: event table for RecPanel

BEGIN_EVENT_TABLE(RecPanel,wxPanel)
END_EVENT_TABLE()

RecPanel::RecPanel( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
    WindowFunc4( this, TRUE ); 
}

// WDR: handler implementations for RecPanel




//----------------------------------------------------------------------------
// RecEnv
//----------------------------------------------------------------------------

// WDR: event table for RecEnv

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
    EVT_MENU( ID4_SHOW_STAFF_BMP, RecEnv::OnStaffCorrespondence )
    EVT_MENU_RANGE( ID4_INSERT_MODE, ID4_SYMBOLES, RecEnv::OnTools )
    EVT_MENU( ID4_SAVE_BOOK, RecEnv::OnSaveBook )
    EVT_MENU( ID4_SAVE_AS_BOOK, RecEnv::OnSaveBookAs )
    EVT_MENU( ID4_NEW_BOOK, RecEnv::OnNewBook )
    EVT_MENU( ID4_OPEN_BOOK, RecEnv::OnOpenBook )
    EVT_MENU( ID4_CLOSE_BOOK, RecEnv::OnCloseBook )
    EVT_MENU( ID4_BOOK_LOAD, RecEnv::OnBookLoad )
    EVT_MENU( ID4_BOOK_PRE, RecEnv::OnBookPreprocess )
    EVT_MENU( ID4_BOOK_REC, RecEnv::OnBookRecognize )
    EVT_MENU( ID4_BOOK_ADAPT, RecEnv::OnBookOptimize )
    EVT_MENU( ID4_POPUP_TREE_LOAD, RecEnv::OnBookLoad )
    EVT_MENU( ID4_BOOK_EDIT, RecEnv::OnBookEdit )
    EVT_MENU( ID4_POPUP_TREE_PRE, RecEnv::OnBookPreprocess )
    EVT_MENU( ID4_POPUP_TREE_REC, RecEnv::OnBookPreprocess )
    EVT_MENU( ID4_POPUP_TREE_ADAPT, RecEnv::OnBookOptimize )
    EVT_MENU( ID4_POPUP_TREE_BOOK_EDIT, RecEnv::OnBookEdit )
    EVT_MENU( ID4_EXPORT_AXMUS, RecEnv::OnExportAxmus )
    EVT_MENU( ID4_EXPORT_AXTYP, RecEnv::OnExportAxtyp )
    EVT_MENU( ID4_BOOK_RESET_ADAPT, RecEnv::OnResetAdaptation )
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(RecEnv,AxEnv)

RecEnv::RecEnv():
    AxEnv()
{
    m_splitterPtr = NULL;
    m_vsplitterPtr = NULL;
    m_imControlPtr = NULL;
    m_imViewPtr = NULL;
    m_wgControlPtr = NULL;
    m_wgViewPtr = NULL;
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
        
    m_vsplitterPtr = new wxSplitterWindow( m_framePtr, -1 );
    this->m_envWindowPtr = m_vsplitterPtr;
    if (!m_envWindowPtr)
        return;
        
    m_recBookPanelPtr = new RecBookPanel( m_vsplitterPtr, -1);
    m_recBookPtr = m_recBookPanelPtr->GetTree();
    m_recBookPtr->SetBookFile( m_recBookFilePtr );
    m_recBookPtr->SetEnv( this );
    m_recBookPtr->SetBookPanel( m_recBookPanelPtr );

    m_splitterPtr = new RecSplitterWindow( m_vsplitterPtr, -1 );
    if (!m_splitterPtr)
        return;
    m_vsplitterPtr->SetMinimumPaneSize( 100 );
    m_vsplitterPtr->SplitVertically( m_recBookPanelPtr, m_splitterPtr, RecEnv::s_book_sash );
    m_vsplitterPtr->Unsplit( m_recBookPanelPtr );

    m_splitterPtr->SetWindowStyleFlag( wxSP_FULLSASH );
    m_splitterPtr->SetMinimumPaneSize( 100 );

    m_imControlPtr = new RecImController( m_splitterPtr, ID4_CONTROLLER, wxDefaultPosition, wxDefaultSize, 0,
        CONTROLLER_NO_TOOLBAR );
    m_imControlPtr->SetEnv( this );
    m_imViewPtr = new RecImWindow( m_imControlPtr, ID4_VIEW , wxDefaultPosition, 
        wxDefaultSize, wxHSCROLL| wxVSCROLL | wxNO_BORDER  /* | wxSUNKEN_BORDER */);
    m_imViewPtr->SetEnv( this );
    m_imViewPtr->m_popupMenu.AppendSeparator();
    m_imControlPtr->Init( this, m_imViewPtr );

    m_wgPanelPtr = new wxPanel( m_splitterPtr, ID4_DISPLAY );
    wxFlexGridSizer *wgsizer = (wxFlexGridSizer*)WgOutputFunc4( m_wgPanelPtr, TRUE );
    m_wgControlPtr = (RecWgController*)m_envWindowPtr->FindWindowById( ID4_WGPANEL );
    m_wgViewPtr = new RecWgWindow( m_wgControlPtr, ID4_WGWINDOW, wxDefaultPosition,
            wxDefaultSize, wxHSCROLL |wxVSCROLL | wxNO_BORDER  /*| wxSIMPLE_BORDER */ , false);
    m_wgViewPtr->SetEnv( this );
    m_wgControlPtr->Init( this, m_wgViewPtr );
    
    m_wgControlPtr->SetImViewAndController( m_imViewPtr, m_imControlPtr );
    m_wgControlPtr->SetRecFile( m_recFilePtr );
    m_imControlPtr->SetWgViewAndController( m_wgViewPtr, m_wgControlPtr );
    m_imControlPtr->SetRecFile( m_recFilePtr );
   
    m_toolpanel = (WgToolPanel*)m_envWindowPtr->FindWindowById( ID4_TOOLPANEL );
    wxASSERT_MSG( m_toolpanel, "Tool Panel cannot be NULL ");
    
 //   m_toolpanel->SetDirection( false );

    m_splitterPtr->SetEnv( this, wgsizer, m_toolpanel, m_wgControlPtr );

    if ( wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE) == *wxWHITE )
        m_wgControlPtr->SetBackgroundColour( *wxLIGHT_GREY );
    else
        m_wgControlPtr->SetBackgroundColour( wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE) );


    //m_splitterPtr->SplitHorizontally(m_imControlPtr, m_wgControlPtr,  800 );
    m_splitterPtr->SplitHorizontally(m_imControlPtr, m_wgPanelPtr );
    m_splitterPtr->Unsplit();
    
    ResetModels();
}


void RecEnv::RealizeToolbar( )
{

    //ToolBarFunc4( m_framePtr->GetToolBar() ); // function generated by wxDesigner

    wxToolBar *toolbar =  m_framePtr->GetToolBar();
    
    toolbar->InsertTool( 0, ID4_NEW_BOOK, _T("New"), m_framePtr->GetToolbarBitmap( "book_new.png" ), wxNullBitmap, wxITEM_NORMAL, _("New book"), _("New book") );
    toolbar->InsertTool( 1, ID4_OPEN_BOOK, _T("Open"), m_framePtr->GetToolbarBitmap( "book_open.png" ), wxNullBitmap, wxITEM_NORMAL, _("Open book"), _("Open book") );
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
    
    // cannot be changed from dialog
    RecEnv::s_check = (pConfig->Read("Check",1)==1);
    RecEnv::s_deskew = (pConfig->Read("Deskew",1)==1);
    RecEnv::s_staves_position = (pConfig->Read("Staves position",1)==1);
    RecEnv::s_binarize_and_clean = (pConfig->Read("Binarize",1)==1);
    RecEnv::s_save_images = (pConfig->Read("Save images",1)==1);
    // can be changed from dialog
    RecEnv::s_find_borders = (pConfig->Read("Borders",1)==1);
    RecEnv::s_find_ornate_letters = (pConfig->Read("Text initials",1)==1);
    RecEnv::s_find_text = (pConfig->Read("Text",1)==1);
    RecEnv::s_find_text_in_staves = (pConfig->Read("Text in staves",1)==1);
    RecEnv::s_pre_margin_top = pConfig->Read("Pre margin top", 150);
    RecEnv::s_pre_margin_bottom = pConfig->Read("Pre margin bottom", 120);
    RecEnv::s_pre_margin_left = pConfig->Read("Pre margin left", 30);
    RecEnv::s_pre_margin_right = pConfig->Read("Pre margin right", 20);
    RecEnv::s_pre_threshold_method_resize = pConfig->Read("Binarization method resize", PRE_BINARIZATION_OTSU );
    RecEnv::s_pre_threshold_method = pConfig->Read("Binarization method", PRE_BINARIZATION_BRINK );
    RecEnv::s_pre_threshold_region_size = pConfig->Read("Binarization region size", 15);
    
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

    // superimposition
    /*pConfig->Write("SegmentSize", this->s_segmentSize);
    pConfig->Write("Interpolation", this->s_interpolation);
    pConfig->Write("SplitX", this->s_split_x);
    pConfig->Write("SplitY", this->s_split_y);
    pConfig->Write("CorrX", this->s_corr_x);
    pConfig->Write("CorrY", this->s_corr_y);
    pConfig->Write("Filter1", this->s_filter1);
    pConfig->Write("Filter2", this->s_filter2);*/

    pConfig->Write("Borders", RecEnv::s_find_borders);
    pConfig->Write("Text initials", RecEnv::s_find_ornate_letters);
    pConfig->Write("Text", RecEnv::s_find_text );
    pConfig->Write("Text in staves", RecEnv::s_find_text_in_staves );
    pConfig->Write("Pre margin top", RecEnv::s_pre_margin_top);
    pConfig->Write("Pre margin bottom",RecEnv::s_pre_margin_bottom);
    pConfig->Write("Pre margin left", RecEnv::s_pre_margin_left);
    pConfig->Write("Pre margin right", RecEnv::s_pre_margin_right );
    pConfig->Write("Binarization method resize", RecEnv::s_pre_threshold_method_resize );
    pConfig->Write("Binarization method", RecEnv::s_pre_threshold_method );
    pConfig->Write("Binarization region size", RecEnv::s_pre_threshold_region_size );
    
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

    if ( parser->Found("q") &&  (parser->GetParamCount() == 1) )
    {
        //wxLogNull logNo;;
    
        wxString file = parser->GetParam( 0 );
		                        
        // upgrade file version
        m_recFilePtr->Open( file );
        m_recFilePtr->Modify( );
        m_recFilePtr->Save( false );
        
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
    
        ProgressDlg *dlg = new ProgressDlg( m_framePtr, -1, _("Batch") );
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

    if ( m_vsplitterPtr->IsSplit() ) // keep position if splitted
        RecEnv::s_book_sash = m_vsplitterPtr->GetSashPosition( );
    m_vsplitterPtr->Unsplit( m_recBookPanelPtr );
    return true;
}

bool RecEnv::ResetFile()
{
    wxASSERT_MSG( m_wgViewPtr, "WG Window cannot be NULL ");
    
    if ( !m_recFilePtr->Close( true ) )
        return false;

    m_wgViewPtr->Show( false );
    m_wgViewPtr->SetFile( NULL );
    
    if ( m_imControlPtr->Ok() )
    {
        m_imControlPtr->CancelMagicSelection();
        m_imControlPtr->Close();
    }
    
    m_toolpanel->SetWgWindow( NULL );    
    m_wgControlPtr->CancelShowStaffBitmap();
    m_splitterPtr->Unsplit();
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
        
    m_wgControlPtr->LoadBitmapsForFont( );
}


void RecEnv::UpdateViews( int flags )
{
    if ( m_recFilePtr->IsRecognized() )
    {
        m_splitterPtr->SplitHorizontally(m_imControlPtr, m_wgPanelPtr );
        AxImage img;
        m_recFilePtr->GetImage1( &img );
        m_imControlPtr->ResetImage( img );

        m_wgViewPtr->SetFile( m_recFilePtr->GetWgFile() );
        //m_wgViewPtr->SetEnv( this );
        m_wgViewPtr->SetToolPanel( m_toolpanel );
        m_wgViewPtr->Goto( 1 );
        m_wgControlPtr->SyncZoom();  
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
            m_splitterPtr->Unsplit();
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
        m_vsplitterPtr->SplitVertically( m_recBookPanelPtr, m_splitterPtr, RecEnv::s_book_sash );
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

// WDR: handler implementations for RecEnv

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

void RecEnv::OnExportAxmus( wxCommandEvent &event )
{
    if ( ProgressDlg::s_instance_existing )
        return;

    wxArrayString paths, filenames;
    size_t nbOfFiles;
    
    nbOfFiles = AxFileSelector( AX_FILE_DEFAULT, &filenames, &paths, m_framePtr );  
                
    // input AXZ files : choose a directory of select muliple files
    if ( nbOfFiles == 0 )
        return; 
    
    // name of model to generate - temporary...
    RecMusModel model( "rec_export_mus" );
    model.New();
    
    wxArrayPtrVoid params;
    params.Add( &nbOfFiles );
    params.Add( &paths );
    
    ProgressDlg *dlg = new ProgressDlg( m_framePtr, -1, _("Export music model") );
    dlg->Center( wxBOTH );
    dlg->Show();
    dlg->SetMaxBatchBar( 2 );
    
    bool failed = false;
    
    if ( !failed )
        failed = !model.AddFiles( params, dlg );

    if ( !failed )  
        failed = !model.Commit( dlg );
        
    if ( !failed )  
        failed = !model.Train( params, dlg );
        
    imCounterEnd( dlg->GetCounter() );

    dlg->AxShowModal( failed ); // stop process  ( failed ???? )
    dlg->Destroy();
    
    if ( !failed )
        model.Save();
}

//#define META_BATCH3

void RecEnv::OnExportAxtyp( wxCommandEvent &event )
{
#ifndef META_BATCH3
    if ( ProgressDlg::s_instance_existing )
        return;

    wxArrayString paths, filenames;
    size_t nbOfFiles;
    
    nbOfFiles = AxFileSelector( AX_FILE_DEFAULT, &filenames, &paths, m_framePtr );  
                
    // input AXZ files : choose a directory of select muliple files
    if ( nbOfFiles == 0 )
        return; 
    
    // name of model to generate - temporary...
    RecTypModel model( "rec_export_typ" );
    model.New();
    
    wxArrayPtrVoid params;
    params.Add( &nbOfFiles );
    params.Add( &paths );
    
    ProgressDlg *dlg = new ProgressDlg( m_framePtr, -1, _("Export typographic model") );
    dlg->Center( wxBOTH );
    dlg->Show();
    dlg->SetMaxBatchBar( 3 );
    
    bool failed = false;
    
    if ( !failed )
        failed = !model.AddFiles( params, dlg );

    if ( !failed )  
        failed = !model.Commit( dlg );
        
    if ( !failed )  
        failed = !model.Train( params, dlg );
        
    imCounterEnd( dlg->GetCounter() );

    dlg->AxShowModal( failed ); // stop process  ( failed ???? )
    dlg->Destroy();
    
    wxMessageBox("Training typographic models from scratch is not implemented yet. The exported file won't be valid as is. It has to be trained first.");
    
    if ( !failed )
        model.Save();
#else	
	wxLogMessage("MetaBatch!! Research method");
	BatchAdaptation();
#endif
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
    if ( ProgressDlg::s_instance_existing )
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
                           
    ProgressDlg *dlg = new ProgressDlg( m_framePtr, -1, _("Optimization") );
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

void RecEnv::OnBookRecognize( wxCommandEvent &event )
{
    if ( ProgressDlg::s_instance_existing )
        return;
        
    if ( !m_recBookFilePtr->IsOpened() )
        return;

    wxArrayString paths, filenames;
    size_t nbOfFiles;
    
    nbOfFiles = m_recBookFilePtr->FilesToRecognize( &filenames, &paths );
    if ( nbOfFiles == 0 )
    {
        wxLogMessage( _("Nothing to do! All active files are already recognized") );
        return;  
	}
                           
    ProgressDlg *dlg = new ProgressDlg( m_framePtr, -1, _("Batch recognition") );
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
    if ( ProgressDlg::s_instance_existing )
        return;
        
    if ( !m_recBookFilePtr->IsOpened() )
        return;

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
    
    ProgressDlg *dlg = new ProgressDlg( m_framePtr, -1, _("Batch preprocessing") );
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

void RecEnv::OnBookLoad( wxCommandEvent &event )
{
    m_recBookFilePtr->LoadImages();
    m_recBookFilePtr->LoadAxfiles();
    m_recBookPtr->Update();
}


void RecEnv::OnSaveBookAs( wxCommandEvent &event )
{
    m_recBookFilePtr->SaveAs();  
}

void RecEnv::OnSaveBook( wxCommandEvent &event )
{
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
        m_vsplitterPtr->SplitVertically( m_recBookPanelPtr, m_splitterPtr, RecEnv::s_book_sash );
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
    if ( ProgressDlg::s_instance_existing )
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
        RecTypModel model( "rec_batch3_model" );
        model.New();    

        //Reset();

        wxArrayPtrVoid params;
        params.Add( &nbOfFiles );
        params.Add( &paths );
    
        ProgressDlg *dlg = new ProgressDlg( m_framePtr, -1, _("Batch") );
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
    
        model.SaveAs( filename + ".axz" );
    
        cont = dir.GetNext(&filename);
    }
#endif // META_BATCH3
}



/*
void RecEnv::OnBatch4( wxCommandEvent &event )
{
    if ( ProgressDlg::s_instance_existing )
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
    
    ProgressDlg *dlg = new ProgressDlg( m_framePtr, -1, _("Batch") );
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
    wxASSERT_MSG( m_wgViewPtr, "WG Window cannot be NULL ");

    if ( event.GetId() == ID4_INSERT_MODE )
        m_wgViewPtr->SetInsertMode( event.IsChecked() );
    else 
    {
        m_wgViewPtr->SetInsertMode( true );
        if ( event.GetId() == ID4_NOTES )
            m_wgViewPtr->SetToolType( MUS_TOOLS_NOTES );
        else if ( event.GetId() == ID4_KEYS)
            m_wgViewPtr->SetToolType( MUS_TOOLS_CLEFS );
        else if ( event.GetId() == ID4_SIGNS )
            m_wgViewPtr->SetToolType( MUS_TOOLS_SIGNS );
        else if ( event.GetId() == ID4_SYMBOLES )
            m_wgViewPtr->SetToolType( MUS_TOOLS_OTHER );
    }
}

void RecEnv::OnStaffCorrespondence( wxCommandEvent &event )
{
    m_wgControlPtr->InverseShowStaffBitmap( );
}


/* 
    Methode de travail !!!!!!!!! Pas DU TOUT plombee !!!!
  */ 
void RecEnv::OnExportImage( wxCommandEvent &event )
{
    wxString filename;
    filename = wxFileSelector( _("Open"), wxGetApp().m_lastDirTIFF_out, _T(""), NULL, IMAGE_FILES, wxOPEN);
    if (filename.IsEmpty())
        return;
        
    wxGetApp().m_lastDirTIFF_out = wxPathOnly( filename );

    wxMemoryDC memDC;
    wxBitmap bitmap( m_wgViewPtr->ToZoom( m_wgViewPtr->pageFormatHor + 30 )  ,
        m_wgViewPtr->ToZoom( m_wgViewPtr->pageFormatVer + 10 )); // marges bricolees ...
    memDC.SelectObject(bitmap);
    memDC.SetBackground(*wxWHITE_BRUSH);
    memDC.Clear();
    memDC.SetLogicalOrigin( m_wgViewPtr->mrgG,10 );
    //memDC.SetPen(*wxRED_PEN);
    //memDC.SetBrush(*wxTRANSPARENT_BRUSH);
    m_wgViewPtr->m_page->DrawPage( &memDC, false );
    memDC.SelectObject(wxNullBitmap);
    wxImage image = bitmap.ConvertToImage().ConvertToMono( 255 ,255 , 255 );
    image.SaveFile( filename );
    //bitmap.SaveFile( filename, wxBITMAP_TYPE_BMP );

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
    m_wgControlPtr->SyncZoom();      
}

void RecEnv::OnZoom( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imViewPtr, "View cannot be NULL ");

    if ( (event.GetId() == ID4_ZOOM_OUT) && m_imViewPtr->CanZoomOut() )
        m_imViewPtr->ZoomOut(); 
    else if ( (event.GetId() == ID4_ZOOM_IN) && m_imViewPtr->CanZoomIn() )
        m_imViewPtr->ZoomIn();
    
    m_imControlPtr->SyncSelectionBitmap();
    m_wgControlPtr->SyncZoom();
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
    
    if ( !m_recFilePtr->IsPreprocessed() )
        this->Preprocess();
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
        
    if ( ProgressDlg::s_instance_existing )
        return;
        
    ProgressDlg *dlg = new ProgressDlg( m_framePtr, -1, _("Recognition") );
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
        
    if ( ProgressDlg::s_instance_existing )
        return;
        
    // if a book is opened, check if the file has to be preprocessed
    if ( m_recBookFilePtr->IsOpened() && !m_recBookFilePtr->HasToBePreprocessed( m_imControlPtr->GetFilename() ) )
        return;
        
    ProgressDlg *dlg = new ProgressDlg( m_framePtr, -1, _("Preprocessing") );
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
    else if (m_wgViewPtr && (win->GetId() == ID4_WGWINDOW))
        m_wgViewPtr->Paste();
}

void RecEnv::OnCopy( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControlPtr, wxT("Image controller  cannot be NULL") );

    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
        return;

    if (win->GetId() == ID4_VIEW)
        m_imControlPtr->Copy();
    else if (m_wgViewPtr && (win->GetId() == ID4_WGWINDOW))
        m_wgViewPtr->Copy();
}

void RecEnv::OnCut( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControlPtr, wxT("Image controller cannot be NULL") );

    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
        return;

    if (win->GetId() == ID4_VIEW)
        m_imControlPtr->Cut();
    else if (m_wgViewPtr && (win->GetId() == ID4_WGWINDOW))
        m_wgViewPtr->Cut();
}


void RecEnv::OnUndo( wxCommandEvent &event )
{
    wxASSERT_MSG( m_recFilePtr, wxT("RecFile cannot be NULL") );

    if ( m_recFilePtr->IsRecognized() )
    {
       if (m_wgViewPtr && m_wgViewPtr->CanUndo() )
        m_wgViewPtr->Undo();
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
        if (m_wgViewPtr && m_wgViewPtr->CanRedo() )
            m_wgViewPtr->Redo();
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


void RecEnv::OnUpdateUI( wxUpdateUIEvent &event )
{
    wxASSERT_MSG( m_imControlPtr, wxT("Image controller cannot be NULL") );
    wxASSERT_MSG( m_imViewPtr, wxT("View cannot be NULL") );
    wxASSERT_MSG( m_wgViewPtr, "WG Window cannot be NULL ");
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
        else if (m_wgViewPtr && (win->GetId() == ID4_WGWINDOW))
            event.Enable( (m_wgViewPtr && m_wgViewPtr->CanCut()));
        else
            event.Enable( false );
            
    }
    else if (id == ID_COPY)
    {
        if (win->GetId() == ID4_VIEW)
           event.Enable( m_imControlPtr->CanCopy() );
        else if (m_wgViewPtr && (win->GetId() == ID4_WGWINDOW))
            event.Enable( (m_wgViewPtr && m_wgViewPtr->CanCopy()));
        else
            event.Enable( false );
    }
    else if (id == ID_PASTE)
    {
        if (win->GetId() == ID4_VIEW)
           event.Enable( m_imControlPtr->CanPaste() );
        else if (m_wgViewPtr && (win->GetId() == ID4_WGWINDOW))
            event.Enable( (m_wgViewPtr && m_wgViewPtr->CanPaste()));
        else
            event.Enable( false );
    }
    else if (id == ID_UNDO)
    {
        if ( m_recFilePtr->IsRecognized() && m_wgViewPtr && m_wgViewPtr->CanUndo() )
            event.Enable( true );
        else if ( m_recFilePtr->IsPreprocessed() && m_recFilePtr->m_imPagePtr && m_recFilePtr->m_imPagePtr->CanUndo() )
            event.Enable( true );
        else
            event.Enable( false );
    }
    else if (id == ID_REDO)
    {
        if ( m_recFilePtr->IsRecognized() && m_wgViewPtr && m_wgViewPtr->CanRedo() )
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
    else if (id == ID4_BOOK_ADAPT )
        event.Enable( m_recBookFilePtr->IsOpened() );
    else if (id == ID4_BOOK_RESET_ADAPT )
        event.Enable( m_recBookFilePtr->IsOpened() );

    else if (id == ID4_SHOW_STAFF_BMP )
    {
        event.Enable( m_recFilePtr->IsRecognized() );
        event.Check( m_wgControlPtr->ShowStaffBitmap() );
    }
    else if ( event.GetId() == ID4_INSERT_MODE )
    {
        event.Enable( m_wgViewPtr->IsShown() );
        event.Check( m_wgViewPtr && !m_wgViewPtr->m_editElement );
    }
    else
        event.Enable(true);
}

#endif // AX_SUPERIMPOSITION

