/////////////////////////////////////////////////////////////////////////////
// Name:        cmp.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_RECOGNITION
	#ifdef AX_COMPARISON

#ifdef __GNUG__
    #pragma implementation "cmp.cpp"
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

#include "cmp.h"
#include "cmpim.h"
#include "cmpwg.h"
#include "cmpfile.h"
//#include "recognition/recbookfile.h"
#include "cmpctrl.h"
//#include "models.h"

#include "app/axapp.h"
#include "app/axframe.h"

#include "im/impage.h"

#include "wg/wgfile.h"
#include "wg/iowwg.h"
//#include "wg/mlfbmp.h"
//#include "wg/wgtoolpanel.h"

int CmpEnv::s_book_sash = 200; // batch processing
int CmpEnv::s_view_sash = 0;

bool CmpEnv::s_expand_root = true;
bool CmpEnv::s_expand_book = true;
bool CmpEnv::s_expand_cmp = true;  

// WDR: class implementations

/*
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

void RecSplitterWindow::SetEnv( CmpEnv *env, wxFlexGridSizer *sizer, WgToolPanel *toolpanel,    CmpWgController *wgControlPtr )
{
    m_envPtr = env;
    m_wgsizer = sizer;
    m_toolpanel = toolpanel;
    m_wgControlPtr = wgControlPtr;
}

void RecSplitterWindow::ChangeOrientation( )
{
    / *
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
    * /
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

*/


//----------------------------------------------------------------------------
// CmpEnv
//----------------------------------------------------------------------------

// WDR: event table for CmpEnv

BEGIN_EVENT_TABLE(CmpEnv,AxEnv)
    // copy paste 
    EVT_MENU( ID_PASTE, CmpEnv::OnPaste )
    EVT_MENU( ID_COPY, CmpEnv::OnCopy )
    EVT_MENU( ID_CUT, CmpEnv::OnCut )
    EVT_MENU( ID_UNDO, CmpEnv::OnUndo )
    EVT_MENU( ID_REDO, CmpEnv::OnRedo )
    EVT_UPDATE_UI( ID_PASTE , CmpEnv::OnUpdateUI )
    EVT_UPDATE_UI( ID_COPY , CmpEnv::OnUpdateUI )
    EVT_UPDATE_UI( ID_CUT , CmpEnv::OnUpdateUI )
    // file
    EVT_MENU( ID_CLOSE, CmpEnv::OnClose )
    EVT_MENU( ID_SAVE_AS, CmpEnv::OnSaveAs )
    EVT_MENU( ID_SAVE, CmpEnv::OnSave )
    //EVT_MENU( ID_OPEN, CmpEnv::OnOpen )
    EVT_MENU( ID_NEW, CmpEnv::OnNew )   
    // other
    EVT_MENU( ID6_COLLATE, CmpEnv::OnCollate )
    EVT_UPDATE_UI_RANGE( wxID_LOWEST, ENV_IDS_MAX , CmpEnv::OnUpdateUI )
    EVT_MENU( ID6_ZOOM_OUT, CmpEnv::OnZoom )
    EVT_MENU( ID6_ZOOM_IN, CmpEnv::OnZoom )
    EVT_MENU( ID6_ADJUST, CmpEnv::OnAdjust )
    EVT_MENU( ID6_ADJUST_H, CmpEnv::OnAdjust )
    EVT_MENU( ID6_ADJUST_V, CmpEnv::OnAdjust )
    EVT_MENU( ID6_NEXT, CmpEnv::OnNext )
    EVT_MENU( ID6_PREVIOUS, CmpEnv::OnPrevious )
    EVT_MENU( ID6_GOTO, CmpEnv::OnGoto )
    //EVT_MENU_RANGE( ID4_INSERT_MODE, ID4_SYMBOLES, CmpEnv::OnTools )
    EVT_MENU( ID6_OPEN_CMP, CmpEnv::OnOpen )
    EVT_MENU( ID6_CMP_LOAD, CmpEnv::OnCmpLoad )
    EVT_MENU( ID6_POPUP_TREE_LOAD, CmpEnv::OnCmpLoad )
    EVT_MENU( ID6_CMP_EDIT, CmpEnv::OnCmpEdit )
    EVT_MENU( ID6_POPUP_TREE_EDIT, CmpEnv::OnCmpEdit )
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(CmpEnv,AxEnv)

CmpEnv::CmpEnv():
    AxEnv()
{
    m_splitterPtr = NULL;
    m_vsplitterPtr = NULL;
	m_isplitterPtr = NULL;
    m_imControlPtr1 = NULL;
    m_imViewPtr1 = NULL;
    m_imControlPtr2 = NULL;
    m_imViewPtr2 = NULL;
    m_wgControlPtr = NULL;
    m_wgViewPtr = NULL;
    m_cmpFilePtr = NULL;
    m_cmpCtrlPanelPtr = NULL;
	m_cmpCollationPtr = NULL;

    m_cmpFilePtr = new CmpFile( "cmp_file", this );

    this->m_envMenuBarFuncPtr = MenuBarFunc6;
}

CmpEnv::~CmpEnv()
{
    if (m_envWindowPtr) m_envWindowPtr->Destroy();
    m_envWindowPtr = NULL;

    if ( m_cmpFilePtr )
        delete m_cmpFilePtr; 
}

void CmpEnv::LoadWindow()
{
	// splitter project / viewer
    m_vsplitterPtr = new wxSplitterWindow( m_framePtr, -1 );
    this->m_envWindowPtr = m_vsplitterPtr;
    if (!m_envWindowPtr)
        return;
	m_vsplitterPtr->SetMinimumPaneSize( 100 );
        
	// project
    m_cmpCtrlPanelPtr = new CmpCtrlPanel( m_vsplitterPtr, -1);
    m_cmpCtrlPtr = m_cmpCtrlPanelPtr->GetTree();
    m_cmpCtrlPtr->SetBookFile( m_cmpFilePtr );
    m_cmpCtrlPtr->SetEnv( this );
    m_cmpCtrlPtr->SetBookPanel( m_cmpCtrlPanelPtr );

	// viewer: splitter collation / images
    m_splitterPtr = new wxSplitterWindow( m_vsplitterPtr, -1 );
    m_splitterPtr->SetWindowStyleFlag( wxSP_FULLSASH );
    m_splitterPtr->SetMinimumPaneSize( 100 );
		
	// collation
	m_wgControlPtr = new CmpWgController( m_splitterPtr, ID6_DISPLAY );
    m_wgViewPtr = new CmpWgWindow( m_wgControlPtr, ID6_WGWINDOW, wxDefaultPosition,
            wxDefaultSize, wxHSCROLL|wxVSCROLL|wxSIMPLE_BORDER , false);
    m_wgViewPtr->SetEnv( this );
    m_wgControlPtr->Init( this, m_wgViewPtr );

	// images: splitter image / image
	m_isplitterPtr = new wxSplitterWindow( m_splitterPtr, -1 );
    m_isplitterPtr->SetWindowStyleFlag( wxSP_FULLSASH );
    m_isplitterPtr->SetMinimumPaneSize( 100 );

    m_imControlPtr1 = new CmpImController( m_isplitterPtr, ID6_CONTROLLER1, wxDefaultPosition, wxDefaultSize, 0,
        CONTROLLER_NO_TOOLBAR );
    m_imControlPtr1->SetEnv( this );
    m_imViewPtr1 = new CmpImWindow( m_imControlPtr1, ID6_VIEW1 , wxDefaultPosition, 
        wxDefaultSize, wxHSCROLL| wxVSCROLL | wxSUNKEN_BORDER);
    m_imViewPtr1->SetEnv( this );
    m_imControlPtr1->Init( this, m_imViewPtr1 );

    m_imControlPtr2 = new CmpImController( m_isplitterPtr, ID6_CONTROLLER2, wxDefaultPosition, wxDefaultSize, 0,
        CONTROLLER_NO_TOOLBAR );
    m_imControlPtr2->SetEnv( this );
    m_imViewPtr2 = new CmpImWindow( m_imControlPtr2, ID6_VIEW2 , wxDefaultPosition, 
        wxDefaultSize, wxHSCROLL| wxVSCROLL | wxSUNKEN_BORDER);
    m_imViewPtr2->SetEnv( this );
    m_imControlPtr2->Init( this, m_imViewPtr2 );
	
	m_wgControlPtr->SetImViewAndController( m_imViewPtr1, m_imControlPtr1, m_imViewPtr2, m_imControlPtr2 );

    //m_wgControlPtr->SetImViewAndController( m_imViewPtr, m_imControlPtr );
    //m_wgControlPtr->SetRecFile( m_recFilePtr );
    //m_imControlPtr->SetWgViewAndController( m_wgViewPtr, m_wgControlPtr );
    //m_imControlPtr->SetRecFile( m_recFilePtr );
   
    //m_toolpanel = (WgToolPanel*)m_envWindowPtr->FindWindowById( ID4_TOOLPANEL );
   // wxASSERT_MSG( m_toolpanel, "Tool Panel cannot be NULL ");
    
 //   m_toolpanel->SetDirection( false );

    //m_splitterPtr->SetEnv( this, wgsizer, m_toolpanel, m_wgControlPtr );

    if ( wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE) == *wxWHITE )
        m_wgControlPtr->SetBackgroundColour( *wxLIGHT_GREY );
    else
        m_wgControlPtr->SetBackgroundColour( wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE) );

	// load splitter
    m_vsplitterPtr->SplitVertically( m_cmpCtrlPanelPtr, m_splitterPtr, CmpEnv::s_book_sash );
	//m_vsplitterPtr->SplitHorizontally( m_cmpCtrlPanelPtr, m_splitterPtr, CmpEnv::s_book_sash );
    m_vsplitterPtr->Unsplit( m_cmpCtrlPanelPtr );

    //m_splitterPtr->SplitHorizontally( m_wgControlPtr , m_isplitterPtr, CmpEnv::s_view_sash);
	m_splitterPtr->SplitVertically( m_wgControlPtr , m_isplitterPtr, CmpEnv::s_view_sash);
    m_splitterPtr->Unsplit();
	
	//m_isplitterPtr->SplitVertically( m_imControlPtr1 , m_imControlPtr2);
	m_isplitterPtr->SplitHorizontally( m_imControlPtr1 , m_imControlPtr2);
    
}


void CmpEnv::RealizeToolbar( )
{
    wxToolBar *toolbar =  m_framePtr->GetToolBar();
    //toolbar->InsertTool( 0, ID6_NEW_CMP, _T("New"), m_framePtr->GetToolbarBitmap( "cmp_new.png" ), wxNullBitmap, wxITEM_NORMAL, _("New book"), _("New book") );
    //toolbar->InsertTool( 1, ID6_OPEN_CMP, _T("Open"), m_framePtr->GetToolbarBitmap( "cmp_open.png" ), wxNullBitmap, wxITEM_NORMAL, _("Open book"), _("Open book") );
    //toolbar->InsertSeparator( 2 );
	toolbar->DeleteTool( ID_OPEN );
	toolbar->InsertTool( 1, ID6_OPEN_CMP, _T("Open"), m_framePtr->GetToolbarBitmap( "cmp_open.png" ), wxNullBitmap, wxITEM_NORMAL, _("Open book"), _("Open book") );
    toolbar->AddTool( ID6_ZOOM_OUT, _T("Zoom out"), m_framePtr->GetToolbarBitmap( "viewmag-.png" ), wxNullBitmap, wxITEM_NORMAL, _("Zoom out"), _("Zoom out") );
    toolbar->AddTool( ID6_ZOOM_IN, _T("Zoom in"), m_framePtr->GetToolbarBitmap( "viewmag+.png" ), wxNullBitmap, wxITEM_NORMAL, _("Zoom in"), _("Zoom in") );
    toolbar->AddTool( ID6_ADJUST, _T("Fit"), m_framePtr->GetToolbarBitmap( "viewmagfit.png" ), wxNullBitmap, wxITEM_CHECK, _("Adjust"), _("Adjust to fit the window") );
	toolbar->AddSeparator();
    toolbar->AddTool( ID6_COLLATE, _T("Collate"), m_framePtr->GetToolbarBitmap( "compfile.png" ), wxNullBitmap, wxITEM_NORMAL, _("Collate"), _("Build the collations") );
    toolbar->AddSeparator();
	toolbar->AddTool( ID6_PREVIOUS, _T("Previous"), m_framePtr->GetToolbarBitmap( "previous.png" ), wxNullBitmap, wxITEM_NORMAL, _("Previous"), _("Previous page page") );
	toolbar->AddTool( ID6_NEXT, _T("Next"), m_framePtr->GetToolbarBitmap( "next.png" ), wxNullBitmap, wxITEM_NORMAL, _("Next"), _("Next page") );    
    toolbar->Realize();
}

void CmpEnv::LoadConfig()
{
    wxConfigBase *pConfig = wxConfigBase::Get();
    wxASSERT_MSG( pConfig, wxT("pConfig cannot be NULL") );
    pConfig->SetPath("/Comparison");
    
    CmpEnv::s_book_sash = pConfig->Read("Book Sash", 200 );
	CmpEnv::s_view_sash = pConfig->Read("View Sash", 0L );

	// tree
	CmpEnv::s_expand_root = (pConfig->Read("Tree root",1)==1);
	CmpEnv::s_expand_book = (pConfig->Read("Tree book",1)==1);
	CmpEnv::s_expand_cmp = (pConfig->Read("Tree cmp",1)==1);

    pConfig->SetPath("/");
}

void CmpEnv::SaveConfig()
{
    wxConfigBase *pConfig = wxConfigBase::Get();
    wxASSERT_MSG( pConfig, wxT("pConfig cannot be NULL") );
    pConfig->SetPath("/Comparison");

    pConfig->Write("Book Sash", CmpEnv::s_book_sash);
	pConfig->Write("View Sash", CmpEnv::s_view_sash);

	// tree
	pConfig->Write("Tree root", CmpEnv::s_expand_root );
	pConfig->Write("Tree book", CmpEnv::s_expand_book );
	pConfig->Write("Tree cmp", CmpEnv::s_expand_cmp );

    pConfig->SetPath("/");
}

void CmpEnv::ParseCmd( wxCmdLineParser *parser )
{
    wxASSERT_MSG( parser, wxT("Parser cannot be NULL") );

    if ( parser->Found("q") &&  (parser->GetParamCount() == 1) )
    { 
        wxString file = parser->GetParam( 0 );
    }
}

void CmpEnv::UpdateTitle( )
{
    wxASSERT_MSG( m_cmpFilePtr, wxT("Cmp file cannot be NULL") );

    wxString msg = m_cmpFilePtr->m_shortname;
    if ( m_cmpCollationPtr && m_wgViewPtr )
        msg += wxString::Format("- %s - page %d / %d", 
			m_cmpCollationPtr->m_name.c_str(), 
			m_wgViewPtr->m_npage + 1,
			(int)m_cmpCollationPtr->GetWgFile()->m_pages.GetCount()  );

    SetTitle( "%s", msg.c_str() );

}


bool CmpEnv::ResetFile()
{
	wxASSERT_MSG( m_wgViewPtr, "WG Window cannot be NULL ");

    if ( !m_cmpFilePtr->Close( true ) )
        return false;

    if ( m_vsplitterPtr->IsSplit() ) // keep position if splitted
        CmpEnv::s_book_sash = m_vsplitterPtr->GetSashPosition( );
    m_vsplitterPtr->Unsplit( m_cmpCtrlPanelPtr );

    m_wgViewPtr->Show( false );
    m_wgViewPtr->SetFile( NULL );
	m_cmpCollationPtr = NULL;
    
    if ( m_imControlPtr1->Ok() )
        m_imControlPtr1->Close(); 
	if ( m_imControlPtr2->Ok() )
        m_imControlPtr2->Close();

    if ( m_splitterPtr->IsSplit() ) // keep position if splitted
        CmpEnv::s_view_sash = m_splitterPtr->GetSashPosition( );
    m_splitterPtr->Unsplit();
    UpdateTitle( );
    
    return true;
}

void CmpEnv::ViewCollation( CmpCollation *collation )
{
	m_cmpCollationPtr = collation;
	UpdateViews( 0 );
}

void CmpEnv::UpdateViews( int flags )
{
    if ( m_cmpCollationPtr && m_cmpCollationPtr->IsCollationLoaded() )
    {
        //m_splitterPtr->SplitHorizontally( m_wgControlPtr , m_isplitterPtr, CmpEnv::s_view_sash );
		m_splitterPtr->SplitVertically( m_wgControlPtr , m_isplitterPtr, CmpEnv::s_view_sash );
        m_wgViewPtr->SetFile( m_cmpCollationPtr->GetWgFile() );
        //m_wgViewPtr->SetEnv( this );
        //m_wgViewPtr->SetToolPanel( m_toolpanel );
        m_wgViewPtr->Goto( 0 );
        m_wgViewPtr->Resize( );  
    }
	UpdateTitle( );
}

void CmpEnv::Open( wxString filename, int type )
{
    // everything has been closed by AxFrame::Open
    if ( type == AX_FILE_PROJECT )
        this->OpenFile( filename );
}

// if type == -1, it is and image
void CmpEnv::OpenFile( wxString filename )
{
    if ( !this->ResetFile( ) )
        return;

    if ( this->m_cmpFilePtr->Open( filename ) )
    {
        wxGetApp().AxBeginBusyCursor( );
        m_vsplitterPtr->SplitVertically( m_cmpCtrlPanelPtr, m_splitterPtr, CmpEnv::s_book_sash );
        m_cmpCtrlPtr->Build();
		UpdateViews( 0 );
        wxGetApp().AxEndBusyCursor();
    }
}

bool CmpEnv::CloseAll( )
{
    if ( !ResetFile() )
        return false;
        
    return true;
}

// WDR: handler implementations for CmpEnv

void CmpEnv::OnCmpEdit( wxCommandEvent &event )
{
	/*
    RecBookDataDlg dlg( m_framePtr, -1, _("Edit book"), m_recBookFilePtr );
    dlg.Center( wxBOTH );
    if ( dlg.ShowModal() == wxID_OK )
    {
        m_recBookFilePtr->Modify();
        m_recBookPtr->Update();
    }
	*/
}

void CmpEnv::OnCmpLoad( wxCommandEvent &event )
{
    if ( ProgressDlg::s_instance_existing )
        return;
	

    wxArrayString paths, filenames;
    size_t nbOfFiles;
    
    //nbOfFiles = m_recBookFilePtr->FilesToRecognize( &filenames, &paths );
	nbOfFiles = m_cmpFilePtr->m_bookFiles.GetCount();
    if ( nbOfFiles == 0 )
    {
        wxLogMessage( _("Nothing to do! No book to collate") );
        return;  
	}
	// OR similary, plus propre
    // if a book is opened, check if the file has to be preprocessed
    //if ( m_recBookFilePtr->IsOpened() && !m_recBookFilePtr->HasToBePreprocessed( m_imControlPtr->GetFilename() ) )
    //    return;
						
	
    ProgressDlg *dlg = new ProgressDlg( m_framePtr, -1, _("Load book") );
    dlg->Center( wxBOTH );
    dlg->Show();
    dlg->SetMaxBatchBar( nbOfFiles );
	
	wxArrayPtrVoid params;
       
    bool failed = false;
    
    failed = !m_cmpFilePtr->LoadBooks( params, dlg );

    imCounterEnd( dlg->GetCounter() );

    dlg->AxShowModal( ( failed || m_cmpFilePtr->GetError() != ERR_NONE ) ); // stop process
    dlg->Destroy();

    if ( failed || m_cmpFilePtr->GetError() != ERR_NONE ) // operation annulee ou failed
        return;

    //UpdateViews( 0 );

    //m_cmpFilePtr->LoadImages();
    //m_cmpFilePtr->LoadAxfiles();
    m_cmpCtrlPtr->Update();
}

void CmpEnv::OnOpen( wxCommandEvent &event )
{
    if ( !this->CloseAll( ) )
        return;

    wxString filename = AxFile::Open( AX_FILE_PROJECT );
    int type, envtype;
    if ( !AxFile::Check( filename, &type, &envtype ) )
        return;
    if  (envtype != AX_FILE_COMPARISON)
    {
        wxLogError("Invalid file");
        return;
    }
    OpenFile( filename );
}

void CmpEnv::OnClose( wxCommandEvent &event )
{
    CloseAll( );
}

void CmpEnv::OnNew( wxCommandEvent &event )
{
    if ( !CloseAll( ) )
        return;
        
    m_cmpFilePtr->New();
	
    /*
	RecBookDataDlg dlg( m_framePtr, -1, _("New book"), m_recBookFilePtr );
    dlg.Center( wxBOTH );
    if ( dlg.ShowModal() == wxID_OK )
    {
        m_recBookFilePtr->Modify();
        wxGetApp().AxBeginBusyCursor( );
        m_vsplitterPtr->SplitVertically( m_recBookPanelPtr, m_splitterPtr, CmpEnv::s_book_sash );
        m_recBookPtr->Build();
        wxGetApp().AxEndBusyCursor();
    }
	*/
    
}

void CmpEnv::OnSave( wxCommandEvent &event )
{
    m_cmpFilePtr->Save();  
}

void CmpEnv::OnSaveAs( wxCommandEvent &event )
{
	m_cmpFilePtr->SaveAs();  
}

void CmpEnv::OnTools( wxCommandEvent &event )
{
}

void CmpEnv::OnAdjust( wxCommandEvent &event )
{
	/*
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
	*/    
}

void CmpEnv::OnZoom( wxCommandEvent &event )
{
	/*
    wxASSERT_MSG( m_imViewPtr, "View cannot be NULL ");

    if ( (event.GetId() == ID4_ZOOM_OUT) && m_imViewPtr->CanZoomOut() )
        m_imViewPtr->ZoomOut(); 
    else if ( (event.GetId() == ID4_ZOOM_IN) && m_imViewPtr->CanZoomIn() )
        m_imViewPtr->ZoomIn();
    
    m_imControlPtr->SyncSelectionBitmap();
    m_wgControlPtr->SyncZoom();
	*/
}



void CmpEnv::OnCollate( wxCommandEvent &event )
{
    if ( ProgressDlg::s_instance_existing )
        return;
	

    wxArrayString paths, filenames;
    size_t nbOfFiles;
    
    //nbOfFiles = m_recBookFilePtr->FilesToRecognize( &filenames, &paths );
	nbOfFiles = m_cmpFilePtr->m_collations.GetCount();
    if ( nbOfFiles == 0 )
    {
        wxLogMessage( _("Nothing to do! No collation to build") );
        return;  
	}
	// OR similary, plus propre
    // if a book is opened, check if the file has to be preprocessed
    //if ( m_recBookFilePtr->IsOpened() && !m_recBookFilePtr->HasToBePreprocessed( m_imControlPtr->GetFilename() ) )
    //    return;
						
	
    ProgressDlg *dlg = new ProgressDlg( m_framePtr, -1, _("Collation") );
    dlg->Center( wxBOTH );
    dlg->Show();
    dlg->SetMaxBatchBar( nbOfFiles );
	
	wxArrayPtrVoid params;
       
    bool failed = false;
    
    failed = !m_cmpFilePtr->Collate( params, dlg );

    imCounterEnd( dlg->GetCounter() );

    dlg->AxShowModal( ( failed || m_cmpFilePtr->GetError() != ERR_NONE ) ); // stop process
    dlg->Destroy();

    if ( failed || m_cmpFilePtr->GetError() != ERR_NONE ) // operation annulee ou failed
        return;

    UpdateViews( 0 );
}

void CmpEnv::OnPaste( wxCommandEvent &event )
{
}

void CmpEnv::OnCopy( wxCommandEvent &event )
{

}

void CmpEnv::OnCut( wxCommandEvent &event )
{
}


void CmpEnv::OnUndo( wxCommandEvent &event )
{
}

void CmpEnv::OnRedo( wxCommandEvent &event )
{
}

void CmpEnv::OnGoto( wxCommandEvent &event )
{
    if ( !m_wgViewPtr )
        return;
    if (m_wgViewPtr->CanGoto(  ) )
        m_wgViewPtr->Goto(  );
	UpdateTitle( );
}

void CmpEnv::OnPrevious( wxCommandEvent &event )
{
    if ( !m_wgViewPtr )
        return;
    if (m_wgViewPtr->HasNext( false ) )
        m_wgViewPtr->Next( false );
	UpdateTitle( );
}

void CmpEnv::OnNext( wxCommandEvent &event )
{
    if ( !m_wgViewPtr )
        return;
    if (m_wgViewPtr->HasNext( true ) )
        m_wgViewPtr->Next( true );
	UpdateTitle( );
}

void CmpEnv::OnUpdateUI( wxUpdateUIEvent &event )
{
	/*
    wxASSERT_MSG( m_imControlPtr, wxT("Image controller cannot be NULL") );
    wxASSERT_MSG( m_imViewPtr, wxT("View cannot be NULL") );
    wxASSERT_MSG( m_wgViewPtr, "WG Window cannot be NULL ");
    wxASSERT_MSG( m_recFilePtr, "RecFile cannot be NULL ");
	wxASSERT_MSG( m_recBookFilePtr, "RecBookFile cannot be NULL ");
    
    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
    {
        // wxLogDebug("CmpEnv::OnUpdateUI : no focus window" );
        return;
    }

    int id = event.GetId();

    if (id == ID_CUT)
    {
    
        //event.Enable( true );
        //wxLogDebug("CmpEnv::OnUpdateUI : update cut" );
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
	*/
}

	#endif // AX_COMPARISON
#endif // AX_RECOGNITION

