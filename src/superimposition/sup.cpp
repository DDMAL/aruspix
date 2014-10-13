/////////////////////////////////////////////////////////////////////////////
// Name:        sup.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_SUPERIMPOSITION

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include "wx/valgen.h"

#include "sup.h"
#include "supim.h"
#include "supfile.h"
#include "supbookfile.h"
#include "supbookctrl.h"

#include "app/axapp.h"
#include "app/axprogressdlg.h"
#include "app/axframe.h"

// statics
int SupEnv::s_subWindowLevel = 2;
int SupEnv::s_split_x = 4;
int SupEnv::s_split_y = 8;
int SupEnv::s_corr_x = 30;
int SupEnv::s_corr_y = 50;
int SupEnv::s_interpolation = 0;
bool SupEnv::s_filter1 = true;
bool SupEnv::s_filter2 = true;

int SupEnv::s_book_sash = 200; // batch processing
int SupEnv::s_page_viewer_sash = 0;
int SupEnv::s_page_images_sash = 0;

bool SupEnv::s_expand_root = true;
bool SupEnv::s_expand_book = true;
bool SupEnv::s_expand_img1 = true;  
bool SupEnv::s_expand_img2 = true;   
bool SupEnv::s_expand_ax = true; 


DEFINE_EVENT_TYPE(AX_PUT_POINTS_EVT)

//----------------------------------------------------------------------------
// SupEnv
//----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(SupEnv,AxEnv)
	// copy paste 
    EVT_MENU( ID_PASTE, SupEnv::OnPaste )
    EVT_MENU( ID_COPY, SupEnv::OnCopy )
    EVT_MENU( ID_CUT, SupEnv::OnCut )
	EVT_UPDATE_UI( ID_PASTE , SupEnv::OnUpdateUI )
	EVT_UPDATE_UI( ID_COPY , SupEnv::OnUpdateUI )
	EVT_UPDATE_UI( ID_CUT , SupEnv::OnUpdateUI )
    // file
    EVT_MENU( ID_CLOSE, SupEnv::OnClose )
    EVT_MENU( ID_SAVE_AS, SupEnv::OnSaveAs )
    EVT_MENU( ID_SAVE, SupEnv::OnSave )
    EVT_MENU( ID_NEW, SupEnv::OnNew )   
    // other
    EVT_MENU( ID2_PROCESS, SupEnv::OnRun )
    EVT_UPDATE_UI_RANGE( wxID_LOWEST, ENV_IDS_MAX , SupEnv::OnUpdateUI )
    EVT_MENU( ID2_ZOOM_OUT, SupEnv::OnZoom )
    EVT_MENU( ID2_ZOOM_IN, SupEnv::OnZoom )
    EVT_MENU( ID2_ZOOM_OUT1, SupEnv::OnZoom )
    EVT_MENU( ID2_ZOOM_IN1, SupEnv::OnZoom )
    EVT_MENU( ID2_ZOOM_OUT2, SupEnv::OnZoom )
    EVT_MENU( ID2_ZOOM_IN2, SupEnv::OnZoom )
    EVT_MENU( ID2_ADJUST_DLG, SupEnv::OnAdjustDlg )
    EVT_MENU( ID2_MANUAL_POINTS, SupEnv::OnPutPoints )
    EVT_MENU( ID2_CANCEL_SUP, SupEnv::OnCancelSuperimposition )
    EVT_MENU( ID2_EXPORT_IMAGE, SupEnv::OnExportImage )
    EVT_MENU( ID2_SAVE_BOOK, SupEnv::OnSaveBook )
    EVT_MENU( ID2_SAVE_AS_BOOK, SupEnv::OnSaveBookAs )
    EVT_MENU( ID2_NEW_BOOK, SupEnv::OnNewBook )
    EVT_MENU( ID2_OPEN_BOOK, SupEnv::OnOpenBook )
    EVT_MENU( ID2_CLOSE_BOOK, SupEnv::OnCloseBook )
    EVT_MENU( ID2_BOOK_LOAD, SupEnv::OnBookLoad )
    EVT_MENU( ID2_BOOK_SUP, SupEnv::OnBookSuperimpose )
    EVT_MENU( ID2_POPUP_TREE_LOAD, SupEnv::OnBookLoad )
    EVT_MENU( ID2_BOOK_EDIT, SupEnv::OnBookEdit )
    EVT_MENU( ID2_POPUP_TREE_SUP, SupEnv::OnBookSuperimpose )
    EVT_MENU( ID2_POPUP_TREE_BOOK_EDIT, SupEnv::OnBookEdit )
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(SupEnv,AxEnv)

SupEnv::SupEnv():
    AxEnv()
{
	m_bookSplitterPtr = NULL;
	m_pageSplitterPtr = NULL;
	m_srcSplitterPtr = NULL;
	
    m_imControl1Ptr = NULL;
    m_imControl2Ptr = NULL;
    m_imView1Ptr = NULL;
    m_imView2Ptr = NULL;
	
    m_srcControl1Ptr = NULL;
    m_srcControl2Ptr = NULL;
    m_srcView1Ptr = NULL;
    m_srcView2Ptr = NULL;
	
    m_supFilePtr = new SupFile( "sup_env_file", this );
    m_supBookFilePtr = new SupBookFile( "sup_book_file", this );

    this->m_envMenuBarFuncPtr = MenuBarFunc2;
}

SupEnv::~SupEnv()
{
    if (m_envWindowPtr)
		m_envWindowPtr->Destroy();
	m_envWindowPtr = NULL;
	
    if ( m_supFilePtr )
        delete m_supFilePtr; 
    if ( m_supBookFilePtr )
        delete m_supBookFilePtr; 
}

void SupEnv::LoadWindow()
{
	// book
    m_bookSplitterPtr = new wxSplitterWindow( m_framePtr, -1 );
    this->m_envWindowPtr = m_bookSplitterPtr;
    if (!m_envWindowPtr)
        return;
        
    m_supBookPanelPtr = new SupBookPanel( m_bookSplitterPtr, -1);
    m_supBookPtr = m_supBookPanelPtr->GetTree();
    m_supBookPtr->SetBookFile( m_supBookFilePtr );
    m_supBookPtr->SetEnv( this );
    m_supBookPtr->SetBookPanel( m_supBookPanelPtr );

	// page
    m_pageSplitterPtr = new wxSplitterWindow( m_bookSplitterPtr, -1 );
    if (!m_pageSplitterPtr)
        return;

    m_pageSplitterPtr->SetWindowStyleFlag( wxSP_3D ); // wxSP_FULLSASH deprecated in 2.8
    m_pageSplitterPtr->SetMinimumPaneSize( 100 );

    m_imControl1Ptr = new SupImController( m_pageSplitterPtr, ID2_CONTROLLER1  );
    m_imView1Ptr = new SupImWindow( m_imControl1Ptr, ID2_VIEW1 , wxDefaultPosition, 
        wxDefaultSize, wxHSCROLL| wxVSCROLL | wxSUNKEN_BORDER);
    //m_imView1Ptr->m_popupMenu.AppendSeparator();
    //m_imView1Ptr->m_popupMenu.Append(ID2_POINTS,_("Put points"));
    m_imControl1Ptr->SetSupFile( m_supFilePtr );
    m_imControl1Ptr->Init( this, m_imView1Ptr );

    m_imControl2Ptr = new SupImController( m_pageSplitterPtr, ID2_CONTROLLER2 );
    m_imView2Ptr = new SupImWindow( m_imControl2Ptr, ID2_VIEW2 , wxDefaultPosition, 
        wxDefaultSize, wxHSCROLL| wxVSCROLL | wxSUNKEN_BORDER);
    //m_imView2Ptr->m_popupMenu.AppendSeparator();
    //m_imView2Ptr->m_popupMenu.Append(ID2_POINTS,_("Put points"));
    m_imControl2Ptr->SetSupFile( m_supFilePtr );
    m_imControl2Ptr->Init( this, m_imView2Ptr );

	// sources
    m_srcSplitterPtr = new wxSplitterWindow( m_pageSplitterPtr, -1 );
    if (!m_srcSplitterPtr)
        return;

    m_srcSplitterPtr->SetWindowStyleFlag( wxSP_3D );  // wxSP_FULLSASH deprecated in 2.8
    m_srcSplitterPtr->SetMinimumPaneSize( 100 );

    m_srcControl1Ptr = new AxImageController( m_srcSplitterPtr, ID2_SRC_CONTROLLER1  );
    m_srcView1Ptr = new SupImSrcWindow( m_srcControl1Ptr, ID2_SRC_VIEW1 , wxDefaultPosition, 
        wxDefaultSize, wxHSCROLL| wxVSCROLL | wxSUNKEN_BORDER);
    m_srcControl1Ptr->Init( this, m_srcView1Ptr );

    m_srcControl2Ptr = new AxImageController( m_srcSplitterPtr, ID2_SRC_CONTROLLER2 );
    m_srcView2Ptr = new SupImSrcWindow( m_srcControl2Ptr, ID2_SRC_VIEW2 , wxDefaultPosition, 
        wxDefaultSize, wxHSCROLL| wxVSCROLL | wxSUNKEN_BORDER);
    m_srcControl2Ptr->Init( this, m_srcView2Ptr );

	// split - unsplit
	m_srcSplitterPtr->SplitHorizontally( m_srcControl1Ptr, m_srcControl2Ptr);
    m_pageSplitterPtr->SplitVertically( m_imControl1Ptr, m_imControl2Ptr, SupEnv::s_page_images_sash );
	m_pageSplitterPtr->Unsplit( m_imControl1Ptr );
	m_pageSplitterPtr->SplitVertically( m_imControl1Ptr, m_srcSplitterPtr, SupEnv::s_page_viewer_sash  );
	//m_pageSplitterPtr->Unsplit( m_imControl1Ptr );
	
    m_bookSplitterPtr->SetMinimumPaneSize( 100 );
    m_bookSplitterPtr->SplitVertically( m_supBookPanelPtr, m_pageSplitterPtr, SupEnv::s_book_sash );
    //m_bookSplitterPtr->Unsplit( m_supBookPanelPtr );
    
    m_adjustDlg = NULL;
	
	CloseAll( );
}

void SupEnv::RealizeToolbar( )
{    
    wxToolBar *toolbar =  m_framePtr->GetToolBar(); 
    
    toolbar->InsertTool( 0, ID2_NEW_BOOK, _T("New"), m_framePtr->GetToolbarBitmap( "sup_new.png" ), wxNullBitmap, wxITEM_NORMAL, _("New book"), _("New book") );
    toolbar->InsertTool( 1, ID2_OPEN_BOOK, _T("Open"), m_framePtr->GetToolbarBitmap( "sup_open.png" ), wxNullBitmap, wxITEM_NORMAL, _("Open book"), _("Open book") );
    
    toolbar->InsertSeparator( 2 );
    
    m_toolCount = toolbar->GetToolsCount(); 
    
    if ( m_supFilePtr && m_supFilePtr->IsSuperimposed() ) {
        RealizeSupToolbar( true );
    } 
    else {
        RealizeSupToolbar( false );
    }
}

void SupEnv::RealizeSupToolbar( bool superimposed )
{
    wxToolBar *toolbar =  m_framePtr->GetToolBar(); 
    int i;
    for ( i = toolbar->GetToolsCount(); i > m_toolCount; i--) {
        toolbar->DeleteToolByPos(i - 1); 
    }
    
    if ( superimposed ) {
 
        toolbar->AddTool( ID2_ZOOM_OUT, _T("Zoom out"), m_framePtr->GetToolbarBitmap( "viewmag-.png" ), wxNullBitmap, wxITEM_NORMAL, _("Zoom out"), _("Zoom out") );
        toolbar->AddTool( ID2_ZOOM_IN, _T("Zoom in"), m_framePtr->GetToolbarBitmap( "viewmag+.png" ), wxNullBitmap, wxITEM_NORMAL, _("Zoom in"), _("Zoom in") );
        
        toolbar->AddSeparator(); 
        
        toolbar->AddTool( ID2_ADJUST_DLG, _T("Contrast / Bright."), m_framePtr->GetToolbarBitmap( "mix_recmon.png" ), wxNullBitmap, wxITEM_NORMAL, _("Contrast and Brightness"), _("Adjust brightness and contrast") );  
    }
    else {
        
        toolbar->AddTool( ID2_MANUAL_POINTS, _T("Manual"), m_framePtr->GetToolbarBitmap( "points1.png" ), wxNullBitmap, wxITEM_CHECK, _("Set superimposition points manually"), _("Set superimposition points") );     
      
        toolbar->AddSeparator(); 
        
        toolbar->AddTool( ID2_PROCESS, _T("Run"), m_framePtr->GetToolbarBitmap( "noatun.png" ), wxNullBitmap, wxITEM_NORMAL, _("Process"), _("Process the current page") );   
        
        toolbar->AddTool( ID2_ZOOM_OUT1, _T("Zoom out"), m_framePtr->GetToolbarBitmap( "viewmag-1.png" ), wxNullBitmap, wxITEM_NORMAL, _("Zoom out source 1"), _("Zoom out source 1") );
        toolbar->AddTool( ID2_ZOOM_IN1, _T("Zoom in"), m_framePtr->GetToolbarBitmap( "viewmag+1.png" ), wxNullBitmap, wxITEM_NORMAL, _("Zoom in source 1"), _("Zoom in source 1") );
        
        toolbar->AddSeparator();
        
        toolbar->AddTool( ID2_ZOOM_OUT2, _T("Zoom out"), m_framePtr->GetToolbarBitmap( "viewmag-2.png" ), wxNullBitmap, wxITEM_NORMAL, _("Zoom out source 2"), _("Zoom out source 2") );
        toolbar->AddTool( ID2_ZOOM_IN2, _T("Zoom in"), m_framePtr->GetToolbarBitmap( "viewmag+2.png" ), wxNullBitmap, wxITEM_NORMAL, _("Zoom in source 2"), _("Zoom in source 2") );
        
    }

    toolbar->AddSeparator();
    
    toolbar->Realize();
}

void SupEnv::LoadConfig()
{
    wxConfigBase *pConfig = wxConfigBase::Get();
    wxASSERT_MSG( pConfig, wxT("pConfig cannot be NULL") );
    pConfig->SetPath("/Superimposition");
	
	// sash
    SupEnv::s_book_sash = pConfig->Read("Book Sash", 200 );
	SupEnv::s_page_viewer_sash = pConfig->Read("Page Viewer Sash", 0L );
	SupEnv::s_page_images_sash = pConfig->Read("Page Images Sash", 0L );
    // superimposition
    SupEnv::s_subWindowLevel = pConfig->Read("SubWindowLevel", 2);
    SupEnv::s_interpolation = pConfig->Read("Interpolation", 0L);
    SupEnv::s_split_x = pConfig->Read("SplitX", 9);
    SupEnv::s_split_y = pConfig->Read("SplitY", 9);
    SupEnv::s_corr_x = pConfig->Read("CorrX", 50);
    SupEnv::s_corr_y = pConfig->Read("CorrY", 50);
    SupEnv::s_filter1 = (pConfig->Read("Filter1",1)==1);
    SupEnv::s_filter2 = (pConfig->Read("Filter2",1)==1);
	// tree
	SupEnv::s_expand_root = (pConfig->Read("Tree root",1)==1);
	SupEnv::s_expand_book = (pConfig->Read("Tree book",1)==1);
	SupEnv::s_expand_img1 = (pConfig->Read("Tree img1",1)==1);
	SupEnv::s_expand_img2 = (pConfig->Read("Tree img2",1)==1);
	SupEnv::s_expand_ax = (pConfig->Read("Tree ax",1)==1);

    pConfig->SetPath("/");
}



void SupEnv::SaveConfig()
{
    wxConfigBase *pConfig = wxConfigBase::Get();
    wxASSERT_MSG( pConfig, wxT("pConfig cannot be NULL") );
    pConfig->SetPath("/Superimposition");

	// sash
    pConfig->Write("Book Sash", SupEnv::s_book_sash);
	pConfig->Write("Page Viewer Sash", SupEnv::s_page_viewer_sash);
	pConfig->Write("Page Images Sash", SupEnv::s_page_images_sash);
    // superimposition
    pConfig->Write("SubWindowLevel", SupEnv::s_subWindowLevel);
    pConfig->Write("Interpolation", SupEnv::s_interpolation);
    pConfig->Write("SplitX", SupEnv::s_split_x);
    pConfig->Write("SplitY", SupEnv::s_split_y);
    pConfig->Write("CorrX", SupEnv::s_corr_x);
    pConfig->Write("CorrY", SupEnv::s_corr_y);
    pConfig->Write("Filter1", SupEnv::s_filter1);
    pConfig->Write("Filter2", SupEnv::s_filter2);
	// tree
	pConfig->Write("Tree root", SupEnv::s_expand_root );
	pConfig->Write("Tree book", SupEnv::s_expand_book );
	pConfig->Write("Tree img1", SupEnv::s_expand_img1 );
	pConfig->Write("Tree img2", SupEnv::s_expand_img2 );
	pConfig->Write("Tree ax", SupEnv::s_expand_ax );

    pConfig->SetPath("/");
}

void SupEnv::ParseCmd( wxCmdLineParser *parser )
{
}

void SupEnv::UpdateTitle( )
{
	/*
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    wxString msg;
    if ( m_imControl1Ptr->Ok() )
    {
        msg += m_imControl1Ptr->GetTitleMsg().c_str();
    }

    if ( m_imControl2Ptr->Ok() )
    {
        msg += ( msg.IsEmpty() ) ? "" : " - ";
        msg += m_imControl2Ptr->GetTitleMsg().c_str();
    }

    SetTitle( "%s", msg.c_str() );
	*/
}

bool SupEnv::ResetBookFile()
{
    if ( !m_supBookFilePtr->Close( true ) )
        return false;

    if ( m_bookSplitterPtr->IsSplit() ) // keep position if splitted
        SupEnv::s_book_sash = m_bookSplitterPtr->GetSashPosition( );
		
    m_bookSplitterPtr->Unsplit( m_supBookPanelPtr );
    return true;
}

bool SupEnv::ResetFile()
{
    if ( m_supFilePtr->IsOpened() && m_pageSplitterPtr->IsSplit() )
	{
		if ( m_supFilePtr->IsSuperimposed() ) // keep position if splitted
			SupEnv::s_page_viewer_sash = m_pageSplitterPtr->GetSashPosition( );
		else
			SupEnv::s_page_images_sash = m_pageSplitterPtr->GetSashPosition( );
	}
	
    if ( !m_supFilePtr->Close( true ) )
        return false;
    
    if ( m_imControl1Ptr->Ok() )
        m_imControl1Ptr->Close();
    if ( m_imControl2Ptr->Ok() )
        m_imControl2Ptr->Close();
    if ( m_srcControl1Ptr->Ok() )
        m_srcControl1Ptr->Close();
    if ( m_srcControl2Ptr->Ok() )
        m_srcControl2Ptr->Close();
		
	m_imControl1Ptr->SetControllers( NULL, NULL );
    m_imControl1Ptr->SetViews( NULL, NULL );
    
    m_pageSplitterPtr->Unsplit();
    UpdateTitle( );
    
    if ( m_adjustDlg ) {
        m_adjustDlg->Show( false );
    }
    
    return true;
}

void SupEnv::UpdateViews( int flags )
{
	m_pageSplitterPtr->Unsplit();
	
    if ( m_supFilePtr->IsSuperimposed() )
    {
        RealizeSupToolbar( true );        
        m_pageSplitterPtr->SplitVertically( m_imControl1Ptr, m_srcSplitterPtr, SupEnv::s_page_viewer_sash );
		m_imControl1Ptr->SetControllers( m_srcControl1Ptr, m_srcControl2Ptr );
		m_imControl1Ptr->SetViews( m_srcView1Ptr , m_srcView2Ptr );
        AxImage img;
        m_supFilePtr->GetResult( &img );
        m_imControl1Ptr->ResetImage( img );
        m_imControl1Ptr->UpdateBrightness();
        //
		m_supFilePtr->GetSrc1( &img );
        m_srcControl1Ptr->ResetImage( img );
		m_supFilePtr->GetSrc2( &img );
        m_srcControl2Ptr->ResetImage( img );
		
		//m_imControl1Ptr->m_red = 10;
		//m_imControl1Ptr->UpdateBrightness( );
    }
    else
    {
        RealizeSupToolbar( false );    
		m_pageSplitterPtr->SplitVertically( m_imControl1Ptr, m_imControl2Ptr, SupEnv::s_page_images_sash );
		m_imControl1Ptr->Open( m_supFilePtr->m_original1 );
		m_imControl2Ptr->Open( m_supFilePtr->m_original2 );
    }
}

void SupEnv::Open( wxString filename, int type )
{
    // everything has been closed by AxFrame::Open
    if ( type == AX_FILE_DEFAULT )
        this->OpenFile( filename, AX_FILE_DEFAULT );
    else if ( type == AX_FILE_PROJECT )
        this->OpenBookFile( filename );
}

// if type == -1, it is and image
void SupEnv::OpenFile( wxString filename, int type )
{
    if ( !this->ResetFile( ) )
        return;

    if ( (type == AX_FILE_DEFAULT) && this->m_supFilePtr->Open( filename ) )
    {
        wxGetApp().AxBeginBusyCursor( );
        UpdateViews( 0 );
        wxGetApp().AxEndBusyCursor();
    }
}


void SupEnv::OpenBookFile( wxString filename  )
{
    //if ( !this->CloseAll( ) )
    //  return;
        
    if ( this->m_supBookFilePtr->Open( filename ) )
    {
        wxGetApp().AxBeginBusyCursor( );
        m_bookSplitterPtr->SplitVertically( m_supBookPanelPtr, m_pageSplitterPtr, SupEnv::s_book_sash );
        m_supBookPtr->Build();
        wxGetApp().AxEndBusyCursor();
    }
}

bool SupEnv::CloseAll( )
{
    if ( !ResetFile() || !ResetBookFile() )
        return false;
        
    return true;
}



void SupEnv::OnBookEdit( wxCommandEvent &event )
{
    SupBookDataDlg dlg( m_framePtr, -1, _("Edit book"), m_supBookFilePtr );
    dlg.Center( wxBOTH );
    if ( dlg.ShowModal() == wxID_OK )
    {
        m_supBookFilePtr->Modify();
        m_supBookPtr->Update();
		if (!m_supBookFilePtr->CreateFiles( true ));
			return;
    }
}


void SupEnv::OnBookSuperimpose( wxCommandEvent &event )
{
    if ( AxProgressDlg::s_instance_existing )
        return;
        
    if ( !m_supBookFilePtr->IsOpened() )
        return;

    wxArrayString filenames, paths;
    size_t nbOfFiles;
    
    nbOfFiles = m_supBookFilePtr->FilesToSuperimpose( &filenames, &paths );
    if ( nbOfFiles == 0 )
    {
        wxLogMessage( _("Nothing to do! All active files are already superimposed") );
        return;  
     }
    
    AxProgressDlg *dlg = new AxProgressDlg( m_framePtr, -1, _("Batch superimposition") );
    dlg->Center( wxBOTH );
    dlg->Show();
    dlg->SetMaxBatchBar( nbOfFiles );
    
    bool failed_once = false;

    for ( int i = 0; i < (int)nbOfFiles; i++ )
    {
        if ( dlg->GetCanceled() )
            break;
			
        wxString outfile = paths[i];

        SupFile supFile( "sup_batch1_file" );
        supFile.New();
            
        bool failed = false;
        
        if ( !failed )
            failed = !supFile.Open( paths[i] );
			
		wxArrayPtrVoid params;
        params.Add( &supFile.m_original1 );
		params.Add( &supFile.m_original2 );

        if ( !failed )
            failed = !supFile.Superimpose( params, dlg );

        if ( !failed )
            failed = !supFile.SaveAs( outfile );
            
        if ( failed )
            failed_once = true;
    }

    imCounterEnd( dlg->GetCounter() );

    dlg->AxShowModal( failed_once  ); // stop process  ( failed ???? )
    dlg->Destroy();
    
    m_supBookPtr->Update();
}

void SupEnv::OnBookLoad( wxCommandEvent &event )
{
    m_supBookFilePtr->LoadImages1();
	m_supBookFilePtr->LoadImages2();
    m_supBookFilePtr->LoadAxfiles();
	m_supBookFilePtr->CreateFiles( true );
    m_supBookPtr->Update();
}


void SupEnv::OnSaveBookAs( wxCommandEvent &event )
{
    m_supBookFilePtr->SaveAs();  
}

void SupEnv::OnSaveBook( wxCommandEvent &event )
{
    m_supBookFilePtr->Save();    
}

void SupEnv::OnOpenBook( wxCommandEvent &event )
{
    if ( !this->CloseAll( ) )
        return;

    wxString filename = AxFile::Open( AX_FILE_PROJECT );
    int type, envtype;
    if ( !AxFile::Check( filename, &type, &envtype ) )
        return;
    if  (envtype != AX_FILE_SUPERIMPOSITION)
    {
        wxLogError("Invalid file environment");
        return;
    }
    OpenBookFile( filename );
}

void SupEnv::OnCloseBook( wxCommandEvent &event )
{
    CloseAll( );
}

void SupEnv::OnNewBook( wxCommandEvent &event )
{
    if ( !CloseAll( ) )
        return;
        
    m_supBookFilePtr->New();
    SupBookDataDlg dlg( m_framePtr, -1, _("New book"), m_supBookFilePtr );
    dlg.Center( wxBOTH );
    if ( dlg.ShowModal() == wxID_OK )
    {
        m_supBookFilePtr->Modify();
        wxGetApp().AxBeginBusyCursor( );
        m_bookSplitterPtr->SplitVertically( m_supBookPanelPtr, m_pageSplitterPtr, SupEnv::s_book_sash );
        m_supBookPtr->Build();
        wxGetApp().AxEndBusyCursor();
    }
    
}

void SupEnv::OnNew( wxCommandEvent &event )
{
    if ( !CloseAll( ) || !this->m_supFilePtr->New())
        return;
	
	m_pageSplitterPtr->SplitVertically( m_imControl1Ptr, m_imControl2Ptr, SupEnv::s_page_images_sash );
		
	m_imControl1Ptr->Open( "" );
    if ( !m_imControl1Ptr->Ok() || !m_imControl1Ptr->HasFilename() )
        return;
	
	m_imControl2Ptr->Open( "" );
    if ( !m_imControl2Ptr->Ok() || !m_imControl2Ptr->HasFilename() )
        return;
	
	m_supFilePtr->m_original1 = m_imControl1Ptr->GetFilename();
	m_supFilePtr->m_original2 = m_imControl2Ptr->GetFilename();
	m_supFilePtr->Modify();
}

void SupEnv::OnSave( wxCommandEvent &event )
{
    wxASSERT_MSG( m_supFilePtr, wxT("SupFile cannot be NULL") );

    m_supFilePtr->Save();

    if ( m_supBookFilePtr->IsOpened() )
        m_supBookPtr->Update();
}

void SupEnv::OnSaveAs( wxCommandEvent &event )
{
    wxASSERT_MSG( m_supFilePtr, wxT("SupFile cannot be NULL") );

    m_supFilePtr->SaveAs();
    UpdateTitle();
    
    if ( m_supBookFilePtr->IsOpened() )
        m_supBookPtr->Update();
}

void SupEnv::OnClose( wxCommandEvent &event )
{
    ResetFile();
}

void SupEnv::OnPutPoints( wxCommandEvent &event )
{
	wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );
    
    if (m_supFilePtr->IsSuperimposed()) {
        return;
    }
    
    m_supFilePtr->Modify();
 
    if ( event.IsChecked() ) {
        // we test the first point (-1,-1) to see if values where ever set
        if ( m_supFilePtr->m_points1[0].x == -1 ) {
            m_imControl1Ptr->SetInitialPoints();
            m_imControl2Ptr->SetInitialPoints();
        }
        
        m_imView1Ptr->BeginSelection( SHAPE_POINT );   
        m_imView2Ptr->BeginSelection( SHAPE_POINT );  
        m_supFilePtr->m_hasManualPoints1 = true;
    }
    else {
        m_imView1Ptr->EndSelection();
        m_imView2Ptr->EndSelection();
        m_supFilePtr->m_hasManualPoints1 = false;
    }
    m_imView1Ptr->UpdateViewFast();
    m_imView2Ptr->UpdateViewFast();
    
}

void SupEnv::OnRun( wxCommandEvent &event )
{
	wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller1 cannot be NULL") );
	wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller2 cannot be NULL") );
    wxASSERT_MSG( m_supFilePtr, wxT("SupFile cannot be NULL") );
    
    wxASSERT( !m_supFilePtr->IsSuperimposed() );
    if ( m_supFilePtr->IsSuperimposed() ) // do nothing, should not happen
        return;
    
    if ( !m_imControl1Ptr->Ok() || !m_imControl1Ptr->HasFilename() )
        return;
		
    if ( !m_imControl2Ptr->Ok() || !m_imControl2Ptr->HasFilename() )
        return;
        
    if ( AxProgressDlg::s_instance_existing )
        return;
        
    // if a book is opened, check if the file has to be superimposed
	// really ?
    //if ( m_supBookFilePtr->IsOpened() && !m_supBookFilePtr->HasToBeSuperimposed( m_imControlPtr->GetFilename() ) )
    //    return;
        
    AxProgressDlg *dlg = new AxProgressDlg( m_framePtr, -1, _("Preprocessing") );
    dlg->Center( wxBOTH );
    dlg->Show();
    dlg->SetMaxBatchBar( 1 );
    
	wxString infile1 = m_imControl1Ptr->GetFilename();
	wxString infile2 = m_imControl2Ptr->GetFilename();
	
    wxArrayPtrVoid params;
    params.Add( &infile1 );
	params.Add( &infile2 );
    
    bool failed = false;
    
    failed = !m_supFilePtr->Superimpose( params, dlg );

    imCounterEnd( dlg->GetCounter() );

    dlg->AxShowModal( ( failed || m_supFilePtr->GetError() != ERR_NONE ) ); // stop process
    dlg->Destroy();

    if ( failed || m_supFilePtr->GetError() != ERR_NONE ) // operation annulee ou failed
        return;
        
    if ( m_supBookFilePtr->IsOpened() )
    {
        m_supFilePtr->SaveAs( m_supBookFilePtr->m_axFileDir + wxFileName::GetPathSeparator() + m_supFilePtr->m_shortname + ".axz" );
        m_supBookPtr->Update();
    }

    UpdateViews( 0 );
}


void SupEnv::OnCancelSuperimposition( wxCommandEvent &event )
{
	wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller1 cannot be NULL") );
	wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller2 cannot be NULL") );
    wxASSERT_MSG( m_supFilePtr, wxT("SupFile cannot be NULL") );
    
    //wxASSERT( m_supFilePtr->IsSuperimposed() );
    if ( !m_supFilePtr->IsSuperimposed() ) // do nothing, should not happen
        return;
    
    if ( AxProgressDlg::s_instance_existing )
        return;
    
    // if a book is opened, check if the file has to be superimposed
	// really ?
    //if ( m_supBookFilePtr->IsOpened() && !m_supBookFilePtr->HasToBeSuperimposed( m_imControlPtr->GetFilename() ) )
    //    return;
    

    if ( !m_supFilePtr->CancelSuperimposition( true ) )
        return;
    else // just update view
    { 
        UpdateViews( 0 );
        m_supFilePtr->Save();
        if ( m_supBookFilePtr->IsOpened() )
            m_supBookPtr->Update();
    }
}


/* 
    Methode de travail !!!!!!!!! Pas DU TOUT plombee !!!!
  */ 
void SupEnv::OnExportImage( wxCommandEvent &event )
{
/*
    wxString filename;
    filename = wxFileSelector( _("Open"), wxGetApp().m_lastDirTIFF_out, _T(""), _T(""), IMAGE_FILES_SELECTION, wxFD_OPEN);
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
*/
}

void SupEnv::OnZoom( wxCommandEvent &event )
{
    if ( (event.GetId() == ID2_ZOOM_OUT1) && m_imView1Ptr->CanZoomOut() ) {
        m_imView1Ptr->ZoomOut(); 
    }
    if ( (event.GetId() == ID2_ZOOM_IN1) && m_imView1Ptr->CanZoomIn() ) {
        m_imView1Ptr->ZoomIn(); 
    }
    if ( (event.GetId() == ID2_ZOOM_OUT2) && m_imView2Ptr->CanZoomOut() ) {
        m_imView2Ptr->ZoomOut(); 
    }
    if ( (event.GetId() == ID2_ZOOM_IN2) && m_imView2Ptr->CanZoomIn() ) {
        m_imView2Ptr->ZoomIn(); 
    }
    // superimposed sources
    if ( (event.GetId() == ID2_ZOOM_OUT) && m_imView1Ptr->CanZoomOut() ) {
        m_imView1Ptr->ZoomOut(); 
    }
    if ( (event.GetId() == ID2_ZOOM_IN) && m_imView1Ptr->CanZoomIn() ) {
        m_imView1Ptr->ZoomIn(); 
    }
}

void SupEnv::OnAdjustDlg( wxCommandEvent &event )
{
    if ( !m_adjustDlg ) {
        m_adjustDlg = new SupAdjustDlg( m_framePtr, -1, _("Adjust brightness and contrast"), m_imControl1Ptr );
    }
    m_adjustDlg->Show();
}


void SupEnv::OnPaste( wxCommandEvent &event )
{
/*
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
        return;

    if (win->GetId() == ID2_VIEW1)
        m_imControl1Ptr->Paste();
    else if (win->GetId() == ID2_VIEW2)
        m_imControl2Ptr->Paste();
*/
}

void SupEnv::OnCopy( wxCommandEvent &event )
{

    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
        return;

    if (win->GetId() == ID2_VIEW1)
        m_imControl1Ptr->Copy();
    else if (win->GetId() == ID2_VIEW2)
        m_imControl2Ptr->Copy();
}

void SupEnv::OnCut( wxCommandEvent &event )
{
/*
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
        return;

    if (win->GetId() == ID2_VIEW1)
        m_imControl1Ptr->Cut();
    else if (win->GetId() == ID2_VIEW2)
        m_imControl2Ptr->Cut();
*/
}

void SupEnv::OnUpdateUI( wxUpdateUIEvent &event )
{

    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
        return;

    int id = event.GetId();

    
    if (id == ID_CUT)
    {
        /* if (win->GetId() == ID2_VIEW1)
            event.Enable( m_imControl1Ptr->CanCut() );
        else if (win->GetId() == ID2_VIEW2)
            event.Enable( m_imControl2Ptr->CanCut() );
        else */
        event.Enable(false);
    }
    else if (id == ID_COPY)
    {
        if (win->GetId() == ID2_VIEW1)
            event.Enable( m_imControl1Ptr->CanCopy() );
        else if (win->GetId() == ID2_VIEW2)
            event.Enable( m_imControl2Ptr->CanCopy() );
        else
            event.Enable(false);
    }
    else if (id == ID_PASTE)
    {
        /* if (win->GetId() == ID2_VIEW1)
            event.Enable( m_imControl1Ptr->CanPaste() );
        else if (win->GetId() == ID2_VIEW2)
            event.Enable( m_imControl2Ptr->CanPaste() );
        else */
            event.Enable(false);
    }
    else if (id == ID2_ZOOM_OUT) {
        event.Enable( m_supFilePtr->IsSuperimposed() && m_imView1Ptr->CanZoomOut() );
    }
    else if (id == ID2_ZOOM_IN) {
        event.Enable( m_supFilePtr->IsSuperimposed() && m_imView1Ptr->CanZoomIn() );
    }
    else if (id == ID2_ZOOM_OUT1) {
        event.Enable( !m_supFilePtr->IsSuperimposed() && m_imView1Ptr->CanZoomOut() );
    }
    else if (id == ID2_ZOOM_IN1) {
        event.Enable( !m_supFilePtr->IsSuperimposed() && m_imView1Ptr->CanZoomIn() );
    }
    else if (id == ID2_ZOOM_OUT2) {
        event.Enable( !m_supFilePtr->IsSuperimposed() && m_imView2Ptr->CanZoomOut() );
    }
    else if (id == ID2_ZOOM_IN2) {
        event.Enable( !m_supFilePtr->IsSuperimposed() && m_imView2Ptr->CanZoomIn() );
    }
    //
    else if (id == ID2_MANUAL_POINTS)
    {
        event.Enable( m_imControl1Ptr->Ok() && m_imControl2Ptr->Ok() );
    }
    else if (id == ID2_PROCESS)
    {
        event.Enable( m_imControl1Ptr->Ok() && m_imControl2Ptr->Ok() );
    }
    else if (id == ID2_ADJUST_DLG)
    {
        event.Enable( m_supFilePtr->IsSuperimposed() );
    }
    else {
        event.Enable(true);
    }

}


//----------------------------------------------------------------------------
// SupAdjustDlg
//----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(SupAdjustDlg,wxDialog)
    EVT_BUTTON( wxID_OK, SupAdjustDlg::OnClose )
    EVT_BUTTON( wxID_CANCEL, SupAdjustDlg::OnCancel )
    EVT_BUTTON( ID2_RESET_ADJ, SupAdjustDlg::OnReset )
    EVT_COMMAND_SCROLL( ID2_SRC1_B, SupAdjustDlg::OnSlider)
    EVT_COMMAND_SCROLL( ID2_SRC1_C, SupAdjustDlg::OnSlider)
    EVT_COMMAND_SCROLL( ID2_SRC2_B, SupAdjustDlg::OnSlider)
    EVT_COMMAND_SCROLL( ID2_SRC2_C, SupAdjustDlg::OnSlider)
END_EVENT_TABLE()

SupAdjustDlg::SupAdjustDlg( wxWindow *parent, wxWindowID id, const wxString &title,
                     SupImController *controller,
                     const wxPoint &position, const wxSize& size, long style ) :
wxDialog( parent, id, title, position, size, style )
{
    SupAdjustDlgFunc2( this, true );
    
    this->GetSrc1B()->SetValidator(
        wxGenericValidator(&controller->m_greenBrightness));
    this->GetSrc1C()->SetValidator(
        wxGenericValidator(&controller->m_greenContrast));
    this->GetSrc2B()->SetValidator(
        wxGenericValidator(&controller->m_redBrightness));
    this->GetSrc2C()->SetValidator(
        wxGenericValidator(&controller->m_redContrast));
    
    m_controller = controller;
}

void SupAdjustDlg::OnSlider( wxScrollEvent &event )
{
    this->TransferDataFromWindow();
    m_controller->UpdateBrightness();
}

void SupAdjustDlg::OnReset(wxCommandEvent &event)
{
    this->GetSrc1B()->SetValue( 0 );
    this->GetSrc1C()->SetValue( 0 );
    this->GetSrc2B()->SetValue( 0 );
    this->GetSrc2C()->SetValue( 0 );
    this->TransferDataFromWindow();
    m_controller->UpdateBrightness();
}

void SupAdjustDlg::OnCancel(wxCommandEvent &event)
{
    this->OnReset( event );
    this->Show( false );
}

void SupAdjustDlg::OnClose(wxCommandEvent &event)
{
    this->Show( false );
}

#endif // AX_SUPERIMPOSITION

