/////////////////////////////////////////////////////////////////////////////
// Name:        cmp.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_RECOGNITION
	#ifdef AX_COMPARISON

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "wx/log.h"

#include "cmp.h"
#include "cmpmus.h"
#include "cmpfile.h"
#include "cmpctrl.h"

#include "app/axapp.h"
#include "app/axframe.h"

#include "im/impage.h"

#include "recognition/recfile.h"

#include "mus/doc.h"

#include "musapp/musiowwg.h"

int CmpEnv::s_book_sash = 200; // batch processing
int CmpEnv::s_view_sash = 0;

bool CmpEnv::s_expand_root = true;
bool CmpEnv::s_expand_book = true;
bool CmpEnv::s_expand_cmp = true;  


//----------------------------------------------------------------------------
// CmpEnv
//----------------------------------------------------------------------------


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
    EVT_MENU( ID6_SET_DELIMITER, CmpEnv::OnSetAsDelimiter )
    //EVT_MENU_RANGE( ID4_INSERT_MODE, ID4_SYMBOLES, CmpEnv::OnTools )
    EVT_MENU( ID6_OPEN_CMP, CmpEnv::OnOpen )
    EVT_MENU( ID6_CMP_LOAD, CmpEnv::OnCmpLoad )
    EVT_MENU( ID6_POPUP_TREE_LOAD, CmpEnv::OnCmpLoad )
    EVT_MENU( ID6_POPUP_TREE_ADD_BOOK, CmpEnv::OnCmpAddBook )
    EVT_MENU( ID6_POPUP_TREE_ADD_COLLATION, CmpEnv::OnCmpAddCollation )
    EVT_MENU( ID6_VIEW_IMAGE, CmpEnv::OnViewImage )
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(CmpEnv,AxEnv)

CmpEnv::CmpEnv():
    AxEnv()
{
    m_bookSplitterPtr = NULL;
    m_pageSplitterPtr = NULL;
	m_srcSplitterPtr = NULL;
    m_imControlPtr1 = NULL;
    m_imViewPtr1 = NULL;
    m_imControlPtr2 = NULL;
    m_imViewPtr2 = NULL;
    m_musControlPtr = NULL;
    m_musViewPtr = NULL;
    m_cmpFilePtr = NULL;
    m_cmpCtrlPanelPtr = NULL;
    m_cmpCollationPtr = NULL;
	m_cmpCollationPartPtr = NULL;
    m_cmpPartPage = NULL;

    m_cmpFilePtr = new CmpFile( "cmp_file", this );
    m_recFilePtr = new RecFile( "cmp_rec_file" );

    this->m_envMenuBarFuncPtr = MenuBarFunc6;
}

CmpEnv::~CmpEnv()
{
    if (m_envWindowPtr) m_envWindowPtr->Destroy();
    m_envWindowPtr = NULL;

    if ( m_cmpFilePtr )
        delete m_cmpFilePtr; 
    
    if ( m_recFilePtr )
        delete m_recFilePtr; 
}

void CmpEnv::LoadWindow()
{
	// splitter project / viewer
    m_bookSplitterPtr = new wxSplitterWindow( m_framePtr, -1 );
    this->m_envWindowPtr = m_bookSplitterPtr;
    if (!m_envWindowPtr)
        return;
	m_bookSplitterPtr->SetMinimumPaneSize( 100 );
        
	// project
    m_cmpCtrlPanelPtr = new CmpCtrlPanel( m_bookSplitterPtr, -1);
    m_cmpCtrlPtr = m_cmpCtrlPanelPtr->GetTree();
    m_cmpCtrlPtr->SetBookFile( m_cmpFilePtr );
    m_cmpCtrlPtr->SetEnv( this );
    m_cmpCtrlPtr->SetBookPanel( m_cmpCtrlPanelPtr );

	// viewer: splitter collation / images
    m_pageSplitterPtr = new wxSplitterWindow( m_bookSplitterPtr, -1 );
    m_pageSplitterPtr->SetWindowStyleFlag( wxSP_NOBORDER );
    m_pageSplitterPtr->SetMinimumPaneSize( 100 );
		
	// collation
	m_musControlPtr = new CmpMusController( m_pageSplitterPtr, ID6_DISPLAY );
    m_musViewPtr = new CmpMusWindow( m_musControlPtr, ID6_MUSWINDOW, wxDefaultPosition,
            wxDefaultSize, wxHSCROLL|wxVSCROLL|wxSIMPLE_BORDER , false);
    m_musViewPtr->SetEnv( this, true );
    m_musControlPtr->Init( this, m_musViewPtr, true );

	// images: splitter image / image
	m_srcSplitterPtr = new wxSplitterWindow( m_pageSplitterPtr, -1 );
    m_srcSplitterPtr->SetWindowStyleFlag( wxSP_NOBORDER );
    m_srcSplitterPtr->SetMinimumPaneSize( 100 );

    m_imControlPtr1 = new CmpMusController( m_srcSplitterPtr, ID6_CONTROLLER1 );
    //m_imControlPtr1->SetEnv( this );
    m_imViewPtr1 = new CmpMusWindow( m_imControlPtr1, ID6_VIEW1 , wxDefaultPosition, 
        wxDefaultSize, wxHSCROLL| wxVSCROLL | wxSUNKEN_BORDER);
    m_imViewPtr1->SetEnv( this, false );
    m_imControlPtr1->Init( this, m_imViewPtr1, false );

    m_imControlPtr2 = new CmpMusController( m_srcSplitterPtr );
    //m_imControlPtr2->SetEnv( this );
    m_imViewPtr2 = new CmpMusWindow( m_imControlPtr2, ID6_VIEW2 , wxDefaultPosition, 
        wxDefaultSize, wxHSCROLL| wxVSCROLL | wxSUNKEN_BORDER);
    m_imViewPtr2->SetEnv( this, false );
    m_imControlPtr2->Init( this, m_imViewPtr2, false );
	
	m_musControlPtr->SetImViewAndController( m_imViewPtr1, m_imControlPtr1, m_imViewPtr2, m_imControlPtr2 );

    //m_musControlPtr->SetImViewAndController( m_imViewPtr, m_imControlPtr );
    //m_musControlPtr->SetRecFile( m_recFilePtr );
    //m_imControlPtr->SetMusViewAndController( m_musViewPtr, m_musControlPtr );
    //m_imControlPtr->SetRecFile( m_recFilePtr );
   
    //m_toolpanel = (MusToolPanel*)m_envWindowPtr->FindWindowById( ID4_TOOLPANEL );
   // wxASSERT_MSG( m_toolpanel, "Tool Panel cannot be NULL ");
    
 //   m_toolpanel->SetDirection( false );

    //m_pageSplitterPtr->SetEnv( this, mussizer, m_toolpanel, m_musControlPtr );

    wxColour background = *wxLIGHT_GREY;
    if ( wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE) != *wxWHITE ) {
        background = wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE);
    }
    m_musControlPtr->SetBackgroundColour( background );
    m_imControlPtr1->SetBackgroundColour( background );
    m_imControlPtr2->SetBackgroundColour( background );
    
    m_musControlPtr->SetCmpFile( m_cmpFilePtr );
    m_imControlPtr1->SetCmpFile( m_cmpFilePtr );
    m_imControlPtr2->SetCmpFile( m_cmpFilePtr );

	// load splitter
    m_bookSplitterPtr->SplitVertically( m_cmpCtrlPanelPtr, m_pageSplitterPtr, CmpEnv::s_book_sash );
	//m_bookSplitterPtr->SplitHorizontally( m_cmpCtrlPanelPtr, m_pageSplitterPtr, CmpEnv::s_book_sash );
    m_bookSplitterPtr->Unsplit( m_cmpCtrlPanelPtr );

    //m_pageSplitterPtr->SplitHorizontally( m_musControlPtr , m_srcSplitterPtr, CmpEnv::s_view_sash);
	m_pageSplitterPtr->SplitHorizontally( m_musControlPtr , m_srcSplitterPtr, CmpEnv::s_view_sash);
    m_pageSplitterPtr->Unsplit();
	
	//m_srcSplitterPtr->SplitVertically( m_imControlPtr1 , m_imControlPtr2);
	m_srcSplitterPtr->SplitVertically( m_imControlPtr1 , m_imControlPtr2);
    
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
    toolbar->AddTool( ID6_VIEW_IMAGE, _T("Facsimile"), m_framePtr->GetToolbarBitmap( "window_list.png" ), wxNullBitmap, wxITEM_CHECK, _("Facsimile"), _("Show the original facsimile of the sources") );
    toolbar->AddSeparator();
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
    if ( m_cmpCollationPartPtr && m_musViewPtr )
        //msg += wxString::Format("- %s - page %d / %d", 
		//	m_cmpCollationPtr->m_name.c_str(), 
        //    m_musViewPtr->m_npage + 1,
		//	(int)m_cmpCollationPtr->GetMusFile()->m_pages.GetCount()  );
        msg += wxString::Format("- %s", m_cmpCollationPtr->m_name.c_str() );

    SetTitle( _T("%s"), msg.c_str() );
}


bool CmpEnv::ResetFile()
{
	wxASSERT_MSG( m_musViewPtr, "WG Window cannot be NULL ");

    if ( !m_cmpFilePtr->Close( true ) )
        return false;

    if ( m_bookSplitterPtr->IsSplit() ) // keep position if splitted
        CmpEnv::s_book_sash = m_bookSplitterPtr->GetSashPosition( );
    m_bookSplitterPtr->Unsplit( m_cmpCtrlPanelPtr );

    m_musViewPtr->Show( false );
    m_musViewPtr->SetDoc( NULL );
	m_cmpCollationPartPtr = NULL;
    m_cmpCollationPtr = NULL;
    
    /*
    if ( m_imControlPtr1->Ok() )
        m_imControlPtr1->Close(); 
	if ( m_imControlPtr2->Ok() )
        m_imControlPtr2->Close();
    */

    if ( m_pageSplitterPtr->IsSplit() ) // keep position if splitted
        CmpEnv::s_view_sash = m_pageSplitterPtr->GetSashPosition( );
    m_pageSplitterPtr->Unsplit();
    UpdateTitle( );
    
    return true;
}

void CmpEnv::ViewCollationPart( CmpCollationPart *collationPart, CmpCollation *collation )
{
	m_cmpCollationPartPtr = collationPart;
    m_cmpCollationPtr = collation;
	UpdateViews( 0 );
}

void CmpEnv::AddPart( CmpBookItem *book )
{
 
    wxString partname = wxGetTextFromUser( "Enter the name for the book", "" );
    if (partname.Length() == 0 ) {
        partname = "[unspecified]";
    }
    
    char uuidStr[37];
    uuid_t id;
    uuid_generate( id );
    uuid_unparse( id, uuidStr ); 
    
    CmpBookPart *part = new CmpBookPart( uuidStr, partname, book );
    book->m_bookParts.Add( part );
    m_cmpCtrlPtr->UpdateParts( book );
    UpdateViews( 0 );
}

void CmpEnv::AddAxFile( wxString filename, CmpBookItem *book )
{
    wxASSERT( book );
    
    int i;
    wxArrayString names;
    for (i = 0; i < (int)book->m_bookParts.GetCount(); i++ ) {
        names.Add( book->m_bookParts[i].m_name );
    }
    
    i = wxGetSingleChoiceIndex( "Select the part", "", names );
    if (i ==  -1) {
        return;
    }
    
    CmpBookPart *part = &book->m_bookParts[i];
    CmpPartPage *page = new CmpPartPage( filename, part );
    part->m_partpages.Add( page );
    m_cmpCtrlPtr->UpdateParts( book );
    UpdateViews( 0 );
}


void CmpEnv::AddPartPageStartEnd( wxString filename, CmpPartPage *page, bool isStart )
{
    wxASSERT( m_recFilePtr );
    
    RecBookFile *book = page->m_part->m_book->m_recBookFilePtr;
    wxString file = book->m_axFileDir + wxFileName::GetPathSeparator() +  filename;	
    
    // we need to check that the file is at least version 2.0.0
    int type, envtype, vmaj, vmin, vrev;
    if ( !AxFile::Check( file, &type, &envtype, &vmaj, &vmin, &vrev ) ) 
    {
        wxLogError(_("File '%s' could not be verified"), file.c_str() );
        return;
    }
    
    if ( AxFile::FormatVersion( vmaj, vmin, vrev) < AxFile::FormatVersion(2, 0, 0) )
    {
        if ( wxMessageBox(_("The file needs to be upgraded to the current version of Aruspix\nDo you want to open it?")
                          , wxGetApp().GetAppName() , wxYES | wxNO | wxICON_QUESTION ) == wxYES )
		{
            m_framePtr->Open( book->m_axFileDir + wxFileName::GetPathSeparator() +  filename );
        }
        return;
    }
    
    m_recFilePtr->New();
    if ( !m_recFilePtr->Open( file ) || !m_recFilePtr->IsRecognized() ) 
    {
        wxLogError(_("File '%s' could not be open or is not a recognized file"), file.c_str() );
        return;
    }
    
    // unset and unsplit
    m_pageSplitterPtr->Unsplit();
    m_cmpCollationPartPtr = NULL;
    m_cmpCollationPtr = NULL;
    
    // keep data for when delimiter is selected
    m_cmpPartPage = page;
    m_delimiterIsStart = isStart;
    
    // set and update view
    m_musViewPtr->SetDoc( m_recFilePtr->m_musDocPtr );
    m_musViewPtr->Resize( ); 
    UpdateViews( 0 );
}

void CmpEnv::RemovePartPageStartEnd( wxString filename, CmpPartPage *page, bool isStart )
{
    wxASSERT( page );
    
    page->SetStartEnd( NULL, isStart );
    m_cmpCtrlPtr->UpdateParts( page->m_part->m_book );
    
    m_cmpPartPage = NULL;
    UpdateViews( 0 );
}

void CmpEnv::AddCollationPart( CmpCollation *collation )
{
    int i, j;
    wxArrayString names;
    wxArrayString idrefs;
    for (i = 0; i < (int)m_cmpFilePtr->m_bookFiles.GetCount(); i++ ) {
        CmpBookItem *book = &m_cmpFilePtr->m_bookFiles[i];
        for (j = 0; j < (int)book->m_bookParts.GetCount(); j++ ) {
            names.Add( book->m_bookname + " - " + book->m_bookParts[j].m_name );
            idrefs.Add( book->m_bookParts[i].m_id );
        }
    }
    
    i = wxGetSingleChoiceIndex( "Select the part", "", names );
    if (i ==  -1) {
        return;
    }
    
    int flags = 0;
    if ( wxMessageBox(_("Do you want to use this part as reference for the collation?")
                      , wxGetApp().GetAppName() , wxYES | wxNO | wxICON_QUESTION ) == wxYES )
    {
        flags = 1;
    }
    
    CmpBookPart *bookpart = m_cmpFilePtr->FindBookPart( idrefs[i] );
    if ( !bookpart )
    {
        wxLogError(_("The part could not be loaded" ) );
        return;         
    }
     
    CmpCollationPart *part = new CmpCollationPart( bookpart, flags );
    collation->m_collationParts.Add( part );    
    
    m_cmpCtrlPtr->UpdateCollationParts( collation );
    UpdateViews( 0 );
}

void CmpEnv::UpdateViews( int flags )
{
    if ( m_cmpCollationPartPtr && m_cmpCollationPtr && m_cmpCollationPtr->IsCollationLoaded( m_cmpCollationPartPtr ) )
    {
		m_pageSplitterPtr->SplitHorizontally( m_musControlPtr , m_srcSplitterPtr, CmpEnv::s_view_sash );
        
        // The last layout is the Raw layout created in CmpCollation::IsCollationLoaded
        m_musViewPtr->SetDoc( m_cmpCollationPtr->GetMusDoc() );
        m_musViewPtr->Resize( );  
        m_imViewPtr1->SetDoc( m_cmpCollationPtr->GetMusDocSrc1() );
        m_imViewPtr1->Resize();
        m_imViewPtr2->SetDoc( m_cmpCollationPtr->GetMusDocSrc2() );
        m_imViewPtr2->Resize();
    }
	UpdateTitle( );
}

void CmpEnv::OnViewImage( wxCommandEvent &event )
{
    if ( event.IsChecked() ) {
        m_imViewPtr1->m_viewImage = true;
        m_imViewPtr2->m_viewImage = true;
    }
    else {
        m_imViewPtr1->m_viewImage = false;
        m_imViewPtr2->m_viewImage = false;
    }
    m_imViewPtr1->Refresh();
    m_imViewPtr2->Refresh();
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
        m_bookSplitterPtr->SplitVertically( m_cmpCtrlPanelPtr, m_pageSplitterPtr, CmpEnv::s_book_sash );
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



void CmpEnv::OnSetAsDelimiter( wxCommandEvent &event )
{
    wxASSERT( m_cmpPartPage && m_musViewPtr && m_musViewPtr->m_currentElement );
    
    m_cmpPartPage->SetStartEnd( m_musViewPtr->m_currentElement, m_delimiterIsStart );
    m_cmpCtrlPtr->UpdateParts( m_cmpPartPage->m_part->m_book );

    m_cmpPartPage = NULL;
    m_recFilePtr->Close();
    m_musViewPtr->Show( false );
    m_musViewPtr->SetDoc( NULL );
    UpdateViews( 0 );

}

void CmpEnv::OnCmpAddBook( wxCommandEvent &event )
{
	wxString filename = AxFile::Open( AX_FILE_PROJECT );
    int type, envtype;
    if ( !AxFile::Check( filename, &type, &envtype ) )
        return;
    if  (envtype != AX_FILE_RECOGNITION)
    {
        wxLogError("Invalid file");
        return;
    }
    
    wxString bookname = wxGetTextFromUser( "Enter the name for the book", "" );
    if (bookname.Length() == 0 ) {
        bookname = "[unspecified]";
    }
    
    CmpBookItem *book = new CmpBookItem( filename, bookname, 0 );
    m_cmpFilePtr->m_bookFiles.Add( book );
    m_cmpCtrlPtr->Build();
    UpdateViews( 0 );
}

void CmpEnv::OnCmpAddCollation( wxCommandEvent &event )
{
    wxString colname = wxGetTextFromUser( "Enter the name for the collation", "" );
    if (colname.Length() == 0 ) {
        colname = "[unspecified]";
    }
    
    char uuidStr[37];
    uuid_t id;
    uuid_generate( id );
    uuid_unparse( id, uuidStr );
    
    CmpCollation *collation = new CmpCollation( uuidStr, colname, m_cmpFilePtr->m_basename );
    m_cmpFilePtr->m_collations.Add( collation );
    m_cmpCtrlPtr->Build();
    UpdateViews( 0 );
}

void CmpEnv::OnCmpLoad( wxCommandEvent &event )
{
    if ( AxProgressDlg::s_instance_existing )
        return;
	

    wxArrayString paths, filenames;
    size_t nbOfFiles;
    
	nbOfFiles = m_cmpFilePtr->m_bookFiles.GetCount();
    if ( nbOfFiles == 0 )
    {
        wxLogMessage( _("Nothing to do! No book to collate") );
        return;  
	}
	
    AxProgressDlg *dlg = new AxProgressDlg( m_framePtr, -1, _("Load book") );
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
        wxLogError("Invalid file project file");
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
    
    m_bookSplitterPtr->SplitVertically( m_cmpCtrlPanelPtr, m_pageSplitterPtr, CmpEnv::s_book_sash );
    m_cmpCtrlPtr->Build();
    UpdateViews( 0 );
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
    m_musControlPtr->SyncZoom();  
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
    m_musControlPtr->SyncZoom();
	*/
}



void CmpEnv::OnCollate( wxCommandEvent &event )
{
    if ( AxProgressDlg::s_instance_existing )
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
						
	
    AxProgressDlg *dlg = new AxProgressDlg( m_framePtr, -1, _("Collation") );
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
    if ( !m_musViewPtr )
        return;
    if (m_musViewPtr->CanGoto(  ) )
        m_musViewPtr->Goto(  );
	UpdateTitle( );
}

void CmpEnv::OnPrevious( wxCommandEvent &event )
{
    if ( !m_musViewPtr )
        return;
    if (m_musViewPtr->HasNext( false ) )
        m_musViewPtr->Next( false );
	UpdateTitle( );
}

void CmpEnv::OnNext( wxCommandEvent &event )
{
    if ( !m_musViewPtr )
        return;
    if (m_musViewPtr->HasNext( true ) )
        m_musViewPtr->Next( true );
	UpdateTitle( );
}

void CmpEnv::OnUpdateUI( wxUpdateUIEvent &event )
{
	
    wxASSERT_MSG( m_imControlPtr1, wxT("Source controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imViewPtr1, wxT("Source view 1 cannot be NULL") );
    wxASSERT_MSG( m_imControlPtr2, wxT("Source controller 2 cannot be NULL") );
    wxASSERT_MSG( m_imViewPtr1, wxT("Source view 2 cannot be NULL") );
    wxASSERT_MSG( m_musControlPtr, "Collation controller cannot be NULL ");
    wxASSERT_MSG( m_musViewPtr, "Collation view cannot be NULL ");
    wxASSERT_MSG( m_cmpFilePtr, "CmpFile cannot be NULL ");
    
    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
    {
        // wxLogDebug("CmpEnv::OnUpdateUI : no focus window" );
        return;
    }

    int id = event.GetId();

    if (id == ID_CUT)
    {
        event.Enable( false );
    }
    else if (id == ID_COPY)
    {
        event.Enable( false );
    }
    else if (id == ID_PASTE)
    {
        event.Enable( false );
    }
    else if (id == ID_UNDO)
    {
        event.Enable( false );
    }
    else if (id == ID_REDO)
    {
        event.Enable( false );
    }
    else if (id == ID6_ZOOM_OUT)
        event.Enable( false );
    else if (id == ID6_ZOOM_IN)
        event.Enable( false );
    else if (id == ID6_ADJUST)
        event.Check( false);
    else if (id == ID6_ADJUST_V)
        event.Check( false );
    else if (id == ID6_ADJUST_H)
        event.Check( false );
    else if (id == ID_SAVE )
        event.Enable( m_cmpFilePtr->IsModified() );
    else if (id == ID_SAVE_AS )
        event.Enable( true );
    else if (id == ID_CLOSE )
        event.Enable( true );
    else if (id == ID6_SET_DELIMITER)
        event.Enable( m_cmpPartPage && m_musViewPtr && m_musViewPtr->CanCopy() );
    else if (id == ID_NEW )
        event.Enable( true );
    // no new file for now - because we always show the image and not the transcription
    else if (id == ID6_VIEW_IMAGE )
        event.Enable( false );
    // default
    else
        event.Enable(true);

}

	#endif // AX_COMPARISON
#endif // AX_RECOGNITION

