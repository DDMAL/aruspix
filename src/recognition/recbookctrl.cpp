/////////////////////////////////////////////////////////////////////////////
// Name:        recbookctrl.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_RECOGNITION

#if defined(__GNUG__) && ! defined(__APPLE__)
	#pragma implementation "recbookctrl.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

//#include "wx/file.h"
//#include "wx/filename.h"
#include "wx/imaglist.h"
#include "wx/valgen.h"

#include "recbookctrl.h"
#include "recbookfile.h"
#include "recfile.h"
#include "rec.h"

#include "app/axoptionsdlg.h"


//----------------------------------------------------------------------------
// RecBookDataDlg
//----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(RecBookDataDlg,wxDialog)
    EVT_BUTTON( wxID_OK, RecBookDataDlg::OnOk )
    //EVT_BUTTON( wxID_CANCEL, RecBookDataDlg::OnCancel )
    EVT_BUTTON( ID4_ON_BOOK_IMAGES, RecBookDataDlg::OnBrowseImages )
    EVT_BUTTON( ID4_ON_BOOK_AXFILES, RecBookDataDlg::OnBrowseAxfiles )
END_EVENT_TABLE()

RecBookDataDlg::RecBookDataDlg( wxWindow *parent, wxWindowID id, const wxString &title,
    RecBookFile *recBookFile ) :
    wxDialog( parent, id, title )
{
        RecBookDataFunc4( this, TRUE );
    m_recBookFile = recBookFile;
    m_loadAxfiles = false;
    m_loadImages = false;
    
	PageBinarizationMethod()->SetString(0, BRINK_2CLASSES_DESCRIPTION);
	PageBinarizationMethod()->SetString(1, SAUVOLA_DESCRIPTION);
	PageBinarizationMethod()->SetString(2, BRINK_3CLASSES_DESCRIPTION);
	
    this->RISM()->SetValidator(
        wxTextValidator( wxFILTER_ASCII, &recBookFile->m_RISM ));   
    this->Composer()->SetValidator(
        wxTextValidator( wxFILTER_ASCII, &recBookFile->m_Composer ));
    this->Title()->SetValidator(
        wxTextValidator( wxFILTER_ASCII, &recBookFile->m_Title ));
    this->Printer()->SetValidator(
        wxTextValidator( wxFILTER_ASCII, &recBookFile->m_Printer ));
    this->Year()->SetValidator(
        wxTextValidator( wxFILTER_NUMERIC, &recBookFile->m_Year ));
    this->Library()->SetValidator(
        wxTextValidator( wxFILTER_ASCII, &recBookFile->m_Library ));
    // files
    this->Axfiles()->SetValidator(
        wxTextValidator( wxFILTER_NONE, &recBookFile->m_axFileDir ));
    this->Images()->SetValidator(
        wxTextValidator( wxFILTER_NONE, &recBookFile->m_imgFileDir ));
	// binarization
	this->PageBinarizationMethod()->SetValidator( 
		wxGenericValidator( &m_recBookFile->m_pre_page_binarization_method ) );	
	this->BinarizationRegionSize()->SetValidator( 
		wxGenericValidator( &m_recBookFile->m_pre_page_binarization_method_size ) );	
	this->BinarizationSelect()->SetValidator( 
		wxGenericValidator( &m_recBookFile->m_pre_page_binarization_select ) );
}

/*
bool RecBookDataDlg::Validate()
{
    return TRUE;
}

bool RecBookDataDlg::TransferDataToWindow()
{
    return TRUE;
}
*/

/*
bool RecBookDataDlg::TransferDataFromWindow()
{
    return true;
}
*/


void RecBookDataDlg::OnBrowseAxfiles( wxCommandEvent &event )
{
    wxString start = this->Axfiles()->GetValue( ).IsEmpty() ? wxGetApp().m_lastDirAX0_out : this->Axfiles()->GetValue( );
    wxString input = wxDirSelector( _("Aruspix file folder"), start );
    if ( input.empty() )
        return;
    
    wxGetApp().m_lastDirAX0_out = input;
    this->Axfiles()->SetValue(input);
    m_loadAxfiles = true;
}

void RecBookDataDlg::OnBrowseImages( wxCommandEvent &event )
{
    wxString start = this->Images()->GetValue( ).IsEmpty() ? wxGetApp().m_lastDirTIFF_in : this->Images()->GetValue( );
    wxString input = wxDirSelector( _("Images folder"), start );
    if ( input.empty() )
        return;

    wxGetApp().m_lastDirTIFF_in = input;
    this->Images()->SetValue(input);
    m_loadImages = true;
}


void RecBookDataDlg::OnOk(wxCommandEvent &event)
{
    if ( Validate() && TransferDataFromWindow() )
    {
        if ( m_loadImages )
            m_recBookFile->LoadImages( );
        if ( m_loadAxfiles)
            m_recBookFile->LoadAxfiles( );
        if ( IsModal() )
            EndModal(wxID_OK);
        else
        {
            SetReturnCode(wxID_OK);
            this->Show(false);
        }
    }
}

/*
void RecBookDataDlg::OnCancel(wxCommandEvent &event)
{
    event.Skip();
}
*/


//----------------------------------------------------------------------------
// RecBookPanel
//----------------------------------------------------------------------------

#define PREVIEW_WIDTH 200
#define PREVIEW_HEIGHT 200


BEGIN_EVENT_TABLE(RecBookPanel,wxPanel)
    EVT_CHECKBOX( ID4_CB_PREVIEW, RecBookPanel::OnPreview )
END_EVENT_TABLE()

RecBookPanel::RecBookPanel( wxWindow *parent, wxWindowID id,
                            const wxPoint &position, const wxSize& size, long style ) :
wxPanel( parent, id, position, size, style )
{
        RecBookFunc4( this, TRUE );
    this->GetPreviewCB( )->SetValue( true );
	m_show_preview = true;
	m_filename = "";
	
	m_nopreview.Create( PREVIEW_WIDTH, PREVIEW_HEIGHT );
	wxMemoryDC dest;
	dest.SelectObject( m_nopreview );
	dest.SetBrush( wxBrush( wxSystemSettings::GetColour( wxSYS_COLOUR_MENU ) ) );
    dest.DrawRectangle( 0, 0, PREVIEW_WIDTH, PREVIEW_HEIGHT );
	
	m_loading.LoadFile( wxGetApp().m_resourcesPath + "/tree/loading.png", wxBITMAP_TYPE_PNG ) ;
	dest.SelectObject( m_loading );
    dest.SetBrush( wxBrush( *wxWHITE, wxTRANSPARENT ) );
    dest.DrawRectangle( 0, 0, PREVIEW_WIDTH, PREVIEW_HEIGHT );
	
	UpdatePreview( );
}

void RecBookPanel::UpdatePreview( )
{
    if ( m_show_preview )
	{
		LoadPreview( );
		this->GetPreview()->SetBitmap( m_preview );
	}
	else
        this->GetPreview()->SetBitmap( m_nopreview );

}

void RecBookPanel::Preview( wxString filename )
{
	m_filename = filename;
	UpdatePreview( );
}	
	
void RecBookPanel::LoadPreview( )
{
	if ( m_filename.IsEmpty() || !wxFileExists( m_filename ) )
	{
		m_preview = m_nopreview;
		return;		
	}
	
	this->GetPreview()->SetBitmap( m_loading );
	this->GetPreview()->Update();
	wxGetApp().Yield();
	
	wxString filename = m_filename;
	if ( wxFileName( m_filename ).GetExt() == "axz" ) // Aruspix preview
	{	
		filename = AxFile::GetPreview( m_filename, "img0.tif" );
		if ( filename.IsEmpty( ) )
		{
			m_preview = m_nopreview;
			return;		
		}
	}
		
	int newWidth, newHeight;
    double scaleX, scaleY, scale;
        
    wxImage img( filename );
	if ( !img.IsOk() )
	{
		m_preview = m_nopreview;
		return;		
	}
        
	// two steps approach to get a resonable quality preview
    // firt a quick resize to 4 * PREVIEW with NORMAL interpolation
    // add use HIGH quality interpolation to resize to PREVIEW
    scaleX = ((double) 4*PREVIEW_WIDTH)/((double) img.GetWidth());
    scaleY = ((double) 4*PREVIEW_HEIGHT)/((double) img.GetHeight());
    scale = scaleX < scaleY ? scaleX : scaleY;
    if ( scale < 1.0 )
    {
        newWidth = (int) (scale * img.GetWidth());
        newHeight = (int) (scale * img.GetHeight());
        img.Rescale(newWidth, newHeight, wxIMAGE_QUALITY_NORMAL );
    }
       
    scaleX = ((double) PREVIEW_WIDTH)/((double) img.GetWidth());
    scaleY = ((double) PREVIEW_HEIGHT)/((double) img.GetHeight());
    scale = scaleX < scaleY ? scaleX : scaleY;
    newWidth = (int) (scale * img.GetWidth());
    newHeight = (int) (scale * img.GetHeight());
    img.Rescale(newWidth, newHeight, wxIMAGE_QUALITY_HIGH );
        
	m_preview.Create(  PREVIEW_WIDTH, PREVIEW_HEIGHT );
	wxMemoryDC dest;
    dest.SelectObject( m_preview );
    dest.DrawRectangle( 0, 0, PREVIEW_WIDTH, PREVIEW_HEIGHT );
    dest.DrawBitmap( wxBitmap( img ), (PREVIEW_WIDTH - newWidth) / 2, (PREVIEW_HEIGHT - newHeight) / 2 );
    dest.SetBrush( wxBrush( *wxWHITE, wxTRANSPARENT ) );
    dest.DrawRectangle( 0, 0, PREVIEW_WIDTH, PREVIEW_HEIGHT );
}


void RecBookPanel::OnPreview( wxCommandEvent &event )
{
    if ( event.IsChecked() )
		m_show_preview = true;
    else
       m_show_preview = false;
	UpdatePreview( );
}


//----------------------------------------------------------------------------
// RecBookCtrl
//----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(RecBookCtrl,AxCtrl)
    EVT_TREE_ITEM_MENU( ID4_TREEBOOK, RecBookCtrl::OnMenu )
    EVT_MENU( ID4_POPUP_TREE_LOAD, RecBookCtrl::OnBook )
    EVT_MENU( ID4_POPUP_TREE_PRE, RecBookCtrl::OnBook )
    EVT_MENU( ID4_POPUP_TREE_REC, RecBookCtrl::OnBook )
    EVT_MENU( ID4_POPUP_TREE_ADAPT, RecBookCtrl::OnBook )
    EVT_MENU( ID4_POPUP_TREE_BOOK_EDIT, RecBookCtrl::OnBook )
    EVT_MENU( ID4_POPUP_TREE_IMG_REMOVE, RecBookCtrl::OnImgRemove )
    EVT_MENU( ID4_POPUP_TREE_IMG_DESACTIVATE, RecBookCtrl::OnImgDesactivate )
    EVT_MENU( ID4_POPUP_TREE_AX_DELETE, RecBookCtrl::OnAxDelete )
    EVT_MENU( ID4_POPUP_TREE_AX_REMOVE, RecBookCtrl::OnAxRemove )
	EVT_MENU( ID4_POPUP_TREE_AX_DESACTIVATE, RecBookCtrl::OnAxDesactivate )
    EVT_TREE_ITEM_ACTIVATED( ID4_TREEBOOK, RecBookCtrl::OnActivate )
    EVT_TREE_SEL_CHANGED( ID4_TREEBOOK, RecBookCtrl::OnSelection )
END_EVENT_TABLE()

RecBookCtrl::RecBookCtrl( wxWindow *parent, wxWindowID id,
                          const wxPoint &position, const wxSize& size, long style ) :
AxCtrl( parent, id,  position, size, style)
{
    m_recBookFilePtr = NULL;
    m_recEnvPtr = NULL;
	m_recBookPanelPtr = NULL;
}

RecBookCtrl::~RecBookCtrl( )
{
}

void RecBookCtrl::SaveDisplay( )
{
    if ( m_rootId.IsOk() && ItemHasChildren( m_rootId ) ) RecEnv::s_expand_root = IsExpanded( m_rootId );
	if ( m_bookId.IsOk() && ItemHasChildren( m_bookId ) ) RecEnv::s_expand_book = IsExpanded( m_bookId );
	if ( m_imgFilesId.IsOk() && ItemHasChildren( m_imgFilesId ) ) RecEnv::s_expand_img = IsExpanded( m_imgFilesId );
	if ( m_axFilesId.IsOk() && ItemHasChildren( m_axFilesId )) RecEnv::s_expand_ax = IsExpanded( m_axFilesId );
	if ( m_optFilesId.IsOk() && ItemHasChildren( m_optFilesId ) ) RecEnv::s_expand_opt = IsExpanded( m_optFilesId );
}

void RecBookCtrl::LoadDisplay( )
{
    if ( m_rootId.IsOk() && RecEnv::s_expand_root ) Expand( m_rootId );
	if ( m_bookId.IsOk() && RecEnv::s_expand_book ) Expand( m_bookId );
	if ( m_imgFilesId.IsOk() && RecEnv::s_expand_img ) Expand( m_imgFilesId );
	if ( m_axFilesId.IsOk() && RecEnv::s_expand_ax ) Expand( m_axFilesId );
	if ( m_optFilesId.IsOk() && RecEnv::s_expand_opt ) Expand( m_optFilesId );
}

void RecBookCtrl::Build( )
{
    wxASSERT( m_recBookFilePtr );
    wxASSERT( m_recEnvPtr );
	
	this->SaveDisplay( );
    this->DeleteAllItems( );
    
    //this->CollapseAll( );
    m_rootId = AddRoot( m_recBookFilePtr->m_shortname );
    SetTypeImages( m_rootId, IMG_FOLDER );
    m_bookId = AppendItem( m_rootId, _("Book") );
    SetTypeImages( m_bookId, IMG_FOLDER );
    m_imgFilesId = AppendItem( m_rootId, _("Images") );
    SetTypeImages( m_imgFilesId, IMG_FOLDER );
    m_axFilesId = AppendItem( m_rootId, _("Aruspix files") );
    SetTypeImages( m_axFilesId, IMG_FOLDER );
    m_optFilesId = AppendItem( m_rootId, _("Optimization files") );
    SetTypeImages( m_optFilesId, IMG_FOLDER );

    if ( RecEnv::s_expand_root ) Expand( m_rootId );

    Update( );
}


void RecBookCtrl::Update( )
{
    wxASSERT( m_recBookFilePtr );
    wxASSERT( m_recEnvPtr );
    
    wxTreeItemId id;
	int i;
	
	this->SaveDisplay( );
    
    DeleteChildren( m_bookId );
    if ( !m_recBookFilePtr->m_RISM.IsEmpty() )
        AppendItem( m_bookId, m_recBookFilePtr->m_RISM, IMG_TEXT, IMG_TEXT_S );
    if ( !m_recBookFilePtr->m_Composer.IsEmpty() )
        AppendItem( m_bookId, m_recBookFilePtr->m_Composer, IMG_TEXT, IMG_TEXT_S );
    if ( !m_recBookFilePtr->m_Title.IsEmpty() )
        AppendItem( m_bookId, m_recBookFilePtr->m_Title, IMG_TEXT, IMG_TEXT_S );
    if ( !m_recBookFilePtr->m_Printer.IsEmpty() )
        AppendItem( m_bookId, m_recBookFilePtr->m_Printer, IMG_TEXT, IMG_TEXT_S );
    if ( !m_recBookFilePtr->m_Year.IsEmpty() )
        AppendItem( m_bookId, m_recBookFilePtr->m_Year, IMG_TEXT, IMG_TEXT_S );
    if ( !m_recBookFilePtr->m_Library.IsEmpty() )
        AppendItem( m_bookId, m_recBookFilePtr->m_Library, IMG_TEXT, IMG_TEXT_S );
        
    DeleteChildren( m_imgFilesId );
    int img = (int)m_recBookFilePtr->m_imgFiles.GetCount();
    if ( img > 0 )
        SetItemText( m_imgFilesId, wxString::Format( _("Images (%d)"), img ) );
    else
        SetItemText( m_imgFilesId, _("Images") );
    for ( i = 0; i < img; i++)
    {
        id = AppendItem( m_imgFilesId, m_recBookFilePtr->m_imgFiles[i].m_filename, IMG_DOC, IMG_DOC_S );
        if (  m_recBookFilePtr->m_imgFiles[i].m_flags & FILE_DESACTIVATED ) 
            SetItemTextColour( id , *wxLIGHT_GREY );
        if ( !wxFileExists( m_recBookFilePtr->m_imgFileDir + wxFileName::GetPathSeparator() +  m_recBookFilePtr->m_imgFiles[i].m_filename ) ) 
            SetItemTextColour( id , *wxRED );
    }
    
    DeleteChildren( m_axFilesId );
    int ax = (int)m_recBookFilePtr->m_axFiles.GetCount();
    if ( ax > 0 )
        SetItemText( m_axFilesId, wxString::Format( _("Aruspix files (%d)"), ax ) );
    else
        SetItemText( m_axFilesId, _("Aruspix files") );
    for ( i = 0; i < ax; i++)
    {
        id = AppendItem( m_axFilesId, m_recBookFilePtr->m_axFiles[i].m_filename, IMG_AXZ, IMG_AXZ_S );
        if (  m_recBookFilePtr->m_axFiles[i].m_flags & FILE_DESACTIVATED ) 
			SetItemTextColour( id , *wxLIGHT_GREY );
        if ( !wxFileExists( m_recBookFilePtr->m_axFileDir + wxFileName::GetPathSeparator() +  m_recBookFilePtr->m_axFiles[i].m_filename ) ) 
            SetItemTextColour( id , *wxRED );
		else if ( RecFile::IsRecognized( m_recBookFilePtr->m_axFileDir + wxFileName::GetPathSeparator() +  m_recBookFilePtr->m_axFiles[i].m_filename ) )
		{
			SetItemImage( id, IMG_AXZ_OK ); 
			SetItemImage( id, IMG_AXZ_OK_S,  wxTreeItemIcon_Selected );
		}	
    }
	
    DeleteChildren( m_optFilesId );
    int opt = (int)m_recBookFilePtr->m_optFiles.GetCount();
    if ( opt > 0 )
	{
		if ( m_recBookFilePtr->m_fullOptimized )
			SetItemText( m_optFilesId, wxString::Format( _("Files used for full optimization (%d)"), opt ) );
		else
			SetItemText( m_optFilesId, wxString::Format( _("Files used for fast optimization (%d)"), opt ) );
	}
	else
        SetItemText( m_optFilesId, _("Optimization files") );
    for ( i = 0; i < opt; i++)
    {
        id = AppendItem( m_optFilesId, m_recBookFilePtr->m_optFiles[i], IMG_AXZ_OK, IMG_AXZ_OK_S );
        if ( !wxFileExists( m_recBookFilePtr->m_axFileDir + wxFileName::GetPathSeparator() +  m_recBookFilePtr->m_optFiles[i] ) ) 
            SetItemTextColour( id , *wxRED );	
    }
    
	this->LoadDisplay( );
	
    m_recBookFilePtr->Modify();
}  


void RecBookCtrl::OnSelection( wxTreeEvent &event )
{
	wxASSERT( m_recBookPanelPtr );
	
    // show some info about this item
    wxTreeItemId itemId = event.GetItem();
	if ( !itemId.IsOk() )
		return;

	if ( ItemIsChildOf( m_imgFilesId, itemId ) )
    {   
        m_recBookPanelPtr->Preview( m_recBookFilePtr->m_imgFileDir + wxFileName::GetPathSeparator() +  GetItemText( itemId ) );
    }
    else if ( ItemIsChildOf( m_axFilesId, itemId ) )
    {
        m_recBookPanelPtr->Preview( m_recBookFilePtr->m_axFileDir + wxFileName::GetPathSeparator() +  GetItemText( itemId ) );
    }
    else if ( ItemIsChildOf( m_optFilesId, itemId ) )
    {
        m_recBookPanelPtr->Preview( m_recBookFilePtr->m_axFileDir + wxFileName::GetPathSeparator() +  GetItemText( itemId ) );
    }
	else
		m_recBookPanelPtr->Preview( "" );
	event.Skip();
}

void RecBookCtrl::OnActivate( wxTreeEvent &event )
{
    // show some info about this item
    wxTreeItemId itemId = event.GetItem();

    if ( ItemIsChildOf( m_bookId, itemId ) )
    {
        // edition
    }
    else if ( ItemIsChildOf( m_imgFilesId, itemId ) )
    {   
        m_recEnvPtr->OpenFile( m_recBookFilePtr->m_imgFileDir + wxFileName::GetPathSeparator() +  GetItemText( itemId ), -1 );
    }
    else if ( ItemIsChildOf( m_axFilesId, itemId ) )
    {
        m_recEnvPtr->OpenFile( m_recBookFilePtr->m_axFileDir + wxFileName::GetPathSeparator() +  GetItemText( itemId ), AX_FILE_DEFAULT );
    }
	else if ( ItemIsChildOf( m_optFilesId, itemId ) )
    {
        m_recEnvPtr->OpenFile( m_recBookFilePtr->m_axFileDir + wxFileName::GetPathSeparator() +  GetItemText( itemId ), AX_FILE_DEFAULT );
    }
}

void RecBookCtrl::OnAxDesactivate( wxCommandEvent &event )
{
    wxArrayTreeItemIds array;
    size_t count = GetSelections(array);
    for ( size_t n = 0; n < count; n++ )
    {
        wxString name = GetItemText(array.Item(n));
        m_recBookFilePtr->DesactivateAxfile( name );
    }
    Update( );    
}

void RecBookCtrl::OnAxRemove( wxCommandEvent &event )
{
    if ( wxMessageBox("The files won't be deleted. Do you want to remove the files?", wxGetApp().GetAppName() , wxYES | wxNO | wxICON_QUESTION ) != wxYES )
        return;

    wxArrayTreeItemIds array;
    size_t count = GetSelections(array);
    for ( size_t n = 0; n < count; n++ )
    {
        wxString name = GetItemText(array.Item(n));
        m_recBookFilePtr->RemoveAxfile( name );
    }
    Update( );     
}

void RecBookCtrl::OnAxDelete( wxCommandEvent &event )
{
    if ( wxMessageBox("Are you sure you want to delete the files?", wxGetApp().GetAppName() , wxYES | wxNO | wxICON_QUESTION ) != wxYES )
        return;

    wxArrayTreeItemIds array;
    size_t count = GetSelections(array);
    for ( size_t n = 0; n < count; n++ )
    {
        wxString name = GetItemText(array.Item(n));
        m_recBookFilePtr->DeleteAxfile( name );
    }
    Update( );      
}

void RecBookCtrl::OnImgDesactivate( wxCommandEvent &event )
{
    wxArrayTreeItemIds array;
    size_t count = GetSelections(array);
    for ( size_t n = 0; n < count; n++ )
    {
        wxString name = GetItemText(array.Item(n));
        m_recBookFilePtr->DesactivateImage( name );
    }
    Update( );    
}

void RecBookCtrl::OnImgRemove( wxCommandEvent &event )
{
    if ( wxMessageBox("The files won't be deleted. Do you want to remove the files?", wxGetApp().GetAppName() , wxYES | wxNO | wxICON_QUESTION ) != wxYES )
        return;

    wxArrayTreeItemIds array;
    size_t count = GetSelections(array);
    for ( size_t n = 0; n < count; n++ )
    {
        wxString name = GetItemText(array.Item(n));
        m_recBookFilePtr->RemoveImage( name );
    }
    Update( );    
}

void RecBookCtrl::OnBook( wxCommandEvent &event )
{
    m_recEnvPtr->ProcessEvent( event );
}

void RecBookCtrl::OnMenu( wxTreeEvent &event )
{
    wxMenu popup;
    popup.Append( ID4_POPUP_TREE_BOOK_EDIT, _("Edit") );
    popup.Append( ID4_POPUP_TREE_LOAD, _("Reload files"), _("Reload images and Aruspix files into the book")  );
    popup.AppendSeparator( );
    popup.Append( ID4_POPUP_TREE_PRE, _("Batch preprocessing") );
    popup.Append( ID4_POPUP_TREE_REC, _("Batch recognition") );
    popup.Append( ID4_POPUP_TREE_ADAPT, _("Optimize") );
    
    
    //if ( SelectionIsChildOf( book ) )
    //{
    //    popup.AppendSeparator( );
    //    popup.Append( ID4_POPUP_TREE_BOOK_EDIT, _("Edit") );
    //}   
    if ( SelectionIsChildOf( m_imgFilesId ) )
    {
        popup.AppendSeparator( );
        popup.Append( ID4_POPUP_TREE_IMG_REMOVE, _("Remove file reference from book") );
		popup.AppendSeparator( );
        popup.Append( ID4_POPUP_TREE_IMG_DESACTIVATE, _("Desactivate / reactivate") );
    }
    else if ( SelectionIsChildOf( m_axFilesId ) )
    {
        popup.AppendSeparator( );
        popup.Append( ID4_POPUP_TREE_AX_REMOVE, _("Remove file reference from book") );
        popup.Append( ID4_POPUP_TREE_AX_DELETE, _("Delete file permanently") );
		popup.AppendSeparator( );
		popup.Append( ID4_POPUP_TREE_AX_DESACTIVATE, _("Desactivate / reactivate") );
    }

    wxPoint clientpt = event.GetPoint();
    wxPoint screenpt = ClientToScreen(clientpt);    
    PopupMenu(&popup, clientpt );
}

#endif //AX_RECOGNITION
