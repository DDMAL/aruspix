/////////////////////////////////////////////////////////////////////////////
// Name:        supbookctrl.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_SUPERIMPOSITION

#if defined(__GNUG__) && ! defined(__APPLE__)
	#pragma implementation "supbookctrl.h"
#endif


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/imaglist.h"

#include "supbookctrl.h"
#include "supbookfile.h"
#include "supfile.h"
#include "sup.h"


// WDR: class implementations

//----------------------------------------------------------------------------
// SupBookDataDlg
//----------------------------------------------------------------------------

// WDR: event table for SupBookDataDlg

BEGIN_EVENT_TABLE(SupBookDataDlg,wxDialog)
    EVT_BUTTON( wxID_OK, SupBookDataDlg::OnOk )
    //EVT_BUTTON( wxID_CANCEL, SupBookDataDlg::OnCancel )
    EVT_BUTTON( ID2_ON_BOOK_IMAGES1, SupBookDataDlg::OnBrowseImages1 )
	EVT_BUTTON( ID2_ON_BOOK_IMAGES2, SupBookDataDlg::OnBrowseImages2 )
    EVT_BUTTON( ID2_ON_BOOK_AXFILES, SupBookDataDlg::OnBrowseAxfiles )
END_EVENT_TABLE()

SupBookDataDlg::SupBookDataDlg( wxWindow *parent, wxWindowID id, const wxString &title,
    SupBookFile *supBookFile ) :
    wxDialog( parent, id, title )
{
    // WDR: dialog function BookDataFunc4 for SupBookDataDlg
    SupBookDataFunc2( this, TRUE );
    m_supBookFile = supBookFile;
    m_loadAxfiles = false;
    m_loadImages1 = false;
	m_loadImages2 = false;
    
    this->RISM()->SetValidator(
        wxTextValidator( wxFILTER_ASCII, &supBookFile->m_RISM ));   
    this->Composer()->SetValidator(
        wxTextValidator( wxFILTER_ASCII, &supBookFile->m_Composer ));
    this->Title()->SetValidator(
        wxTextValidator( wxFILTER_ASCII, &supBookFile->m_Title ));
    this->Printer()->SetValidator(
        wxTextValidator( wxFILTER_ASCII, &supBookFile->m_Printer ));
    this->Year()->SetValidator(
        wxTextValidator( wxFILTER_NUMERIC, &supBookFile->m_Year ));
    this->Library1()->SetValidator(
        wxTextValidator( wxFILTER_ASCII, &supBookFile->m_Library1 ));
    this->Library2()->SetValidator(
        wxTextValidator( wxFILTER_ASCII, &supBookFile->m_Library2 ));
    // files
    this->Axfiles()->SetValidator(
        wxTextValidator( wxFILTER_NONE, &supBookFile->m_axFileDir ));
    this->Images1()->SetValidator(
        wxTextValidator( wxFILTER_NONE, &supBookFile->m_imgFileDir1 ));
    this->Images2()->SetValidator(
        wxTextValidator( wxFILTER_NONE, &supBookFile->m_imgFileDir2 ));		
}

/*
bool SupBookDataDlg::Validate()
{
    return TRUE;
}

bool SupBookDataDlg::TransferDataToWindow()
{
    return TRUE;
}
*/

/*
bool SupBookDataDlg::TransferDataFromWindow()
{
    return true;
}
*/

// WDR: handler implementations for SupBookDataDlg

void SupBookDataDlg::OnBrowseAxfiles( wxCommandEvent &event )
{
    wxString start = this->Axfiles()->GetValue( ).IsEmpty() ? wxGetApp().m_lastDirAX0_out : this->Axfiles()->GetValue( );
    wxString input = wxDirSelector( _("Aruspix file folder"), start );
    if ( input.empty() )
        return;
    
    wxGetApp().m_lastDirAX0_out = input;
    this->Axfiles()->SetValue(input);
    m_loadAxfiles = true;
}

void SupBookDataDlg::OnBrowseImages1( wxCommandEvent &event )
{
    wxString start = this->Images1()->GetValue( ).IsEmpty() ? wxGetApp().m_lastDirTIFF_in : this->Images1()->GetValue( );
    wxString input = wxDirSelector( _("Images folder"), start );
    if ( input.empty() )
        return;

    wxGetApp().m_lastDirTIFF_in = input;
    this->Images1()->SetValue(input);
    m_loadImages1 = true;
}

void SupBookDataDlg::OnBrowseImages2( wxCommandEvent &event )
{
    wxString start = this->Images2()->GetValue( ).IsEmpty() ? wxGetApp().m_lastDirTIFF_in : this->Images2()->GetValue( );
    wxString input = wxDirSelector( _("Images folder"), start );
    if ( input.empty() )
        return;

    wxGetApp().m_lastDirTIFF_in = input;
    this->Images2()->SetValue(input);
    m_loadImages2 = true;
}


void SupBookDataDlg::OnOk(wxCommandEvent &event)
{
    if ( Validate() && TransferDataFromWindow() )
    {
        if ( m_loadImages1 )
            m_supBookFile->LoadImages1( );
        if ( m_loadImages2 )
            m_supBookFile->LoadImages2( );
        if ( m_loadAxfiles)
            m_supBookFile->LoadAxfiles( );
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
void SupBookDataDlg::OnCancel(wxCommandEvent &event)
{
    event.Skip();
}
*/


//----------------------------------------------------------------------------
// SupBookPanel
//----------------------------------------------------------------------------

#define PREVIEW_WIDTH 200
#define PREVIEW_HEIGHT 200

// WDR: event table for SupBookPanel

BEGIN_EVENT_TABLE(SupBookPanel,wxPanel)
    EVT_CHECKBOX( ID2_CB_PREVIEW, SupBookPanel::OnPreview )
END_EVENT_TABLE()

SupBookPanel::SupBookPanel( wxWindow *parent, wxWindowID id,
                            const wxPoint &position, const wxSize& size, long style ) :
wxPanel( parent, id, position, size, style )
{
    // WDR: dialog function BookFunc4 for SupBookPanel
    SupBookFunc2( this, TRUE );
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

void SupBookPanel::UpdatePreview( )
{
    if ( m_show_preview )
	{
		LoadPreview( );
		this->GetPreview()->SetBitmap( m_preview );
	}
	else
        this->GetPreview()->SetBitmap( m_nopreview );

}

void SupBookPanel::Preview( wxString filename )
{
	m_filename = filename;
	UpdatePreview( );
}	
	
void SupBookPanel::LoadPreview( )
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
		filename = AxFile::GetPreview( m_filename, "result.tif" );
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

// WDR: handler implementations for SupBookPanel

void SupBookPanel::OnPreview( wxCommandEvent &event )
{
    if ( event.IsChecked() )
		m_show_preview = true;
    else
       m_show_preview = false;
	UpdatePreview( );
}


//----------------------------------------------------------------------------
// SupBookCtrl
//----------------------------------------------------------------------------

// WDR: event table for SupBookCtrl

BEGIN_EVENT_TABLE(SupBookCtrl,AxCtrl)
    EVT_TREE_ITEM_MENU( ID2_TREEBOOK, SupBookCtrl::OnMenu )
    EVT_MENU( ID2_POPUP_TREE_LOAD, SupBookCtrl::OnBook )
    EVT_MENU( ID2_POPUP_TREE_SUP, SupBookCtrl::OnBook )
    EVT_MENU( ID2_POPUP_TREE_BOOK_EDIT, SupBookCtrl::OnBook )
    EVT_MENU( ID2_POPUP_TREE_IMG_REMOVE1, SupBookCtrl::OnImgRemove )
    EVT_MENU( ID2_POPUP_TREE_IMG_DESACTIVATE1, SupBookCtrl::OnImgDesactivate )
	EVT_MENU( ID2_POPUP_TREE_IMG_REMOVE2, SupBookCtrl::OnImgRemove )
    EVT_MENU( ID2_POPUP_TREE_IMG_DESACTIVATE2, SupBookCtrl::OnImgDesactivate )
    EVT_MENU( ID2_POPUP_TREE_AX_DELETE, SupBookCtrl::OnAxDelete )
    EVT_MENU( ID2_POPUP_TREE_AX_REMOVE, SupBookCtrl::OnAxRemove )
	EVT_MENU( ID2_POPUP_TREE_AX_DESACTIVATE, SupBookCtrl::OnAxDesactivate )
    EVT_TREE_ITEM_ACTIVATED( ID2_TREEBOOK, SupBookCtrl::OnActivate )
    EVT_TREE_SEL_CHANGED( ID2_TREEBOOK, SupBookCtrl::OnSelection )
END_EVENT_TABLE()

SupBookCtrl::SupBookCtrl( wxWindow *parent, wxWindowID id,
                          const wxPoint &position, const wxSize& size, long style ) :
AxCtrl( parent, id,  position, size, style)
{
    m_supBookFilePtr = NULL;
    m_supEnvPtr = NULL;
	m_supBookPanelPtr = NULL;
}

SupBookCtrl::~SupBookCtrl( )
{
}

void SupBookCtrl::SaveDisplay( )
{
    if ( m_rootId.IsOk() && ItemHasChildren( m_rootId ) ) SupEnv::s_expand_root = IsExpanded( m_rootId );
	if ( m_bookId.IsOk() && ItemHasChildren( m_bookId ) ) SupEnv::s_expand_book = IsExpanded( m_bookId );
	if ( m_imgFilesId1.IsOk() && ItemHasChildren( m_imgFilesId1 ) ) SupEnv::s_expand_img1 = IsExpanded( m_imgFilesId1 );
	if ( m_imgFilesId2.IsOk() && ItemHasChildren( m_imgFilesId2 ) ) SupEnv::s_expand_img2 = IsExpanded( m_imgFilesId2 );
	if ( m_axFilesId.IsOk() && ItemHasChildren( m_axFilesId )) SupEnv::s_expand_ax = IsExpanded( m_axFilesId );
}

void SupBookCtrl::LoadDisplay( )
{
    if ( m_rootId.IsOk() && SupEnv::s_expand_root ) Expand( m_rootId );
	if ( m_bookId.IsOk() && SupEnv::s_expand_book ) Expand( m_bookId );
	if ( m_imgFilesId1.IsOk() && SupEnv::s_expand_img1 ) Expand( m_imgFilesId1 );
	if ( m_imgFilesId2.IsOk() && SupEnv::s_expand_img2 ) Expand( m_imgFilesId2 );
	if ( m_axFilesId.IsOk() && SupEnv::s_expand_ax ) Expand( m_axFilesId );
}

void SupBookCtrl::Build( )
{
    wxASSERT( m_supBookFilePtr );
    wxASSERT( m_supEnvPtr );
	
	this->SaveDisplay( );
    this->DeleteAllItems( );
    
    //this->CollapseAll( );
    m_rootId = AddRoot( m_supBookFilePtr->m_shortname );
    SetTypeImages( m_rootId, IMG_FOLDER );
    m_bookId = AppendItem( m_rootId, _("Book") );
    SetTypeImages( m_bookId, IMG_FOLDER );
    m_imgFilesId1 = AppendItem( m_rootId, _("Images 1") );
    SetTypeImages( m_imgFilesId1, IMG_FOLDER );
    m_imgFilesId2 = AppendItem( m_rootId, _("Images 2") );	
    SetTypeImages( m_imgFilesId2, IMG_FOLDER );
    m_axFilesId = AppendItem( m_rootId, _("Aruspix files") );
    SetTypeImages( m_axFilesId, IMG_FOLDER );

    if ( SupEnv::s_expand_root ) Expand( m_rootId );

    Update( );
}


void SupBookCtrl::Update( )
{
    wxASSERT( m_supBookFilePtr );
    wxASSERT( m_supEnvPtr );
    
    wxTreeItemId id;
	int i;
	
	this->SaveDisplay( );
    
    DeleteChildren( m_bookId );
    if ( !m_supBookFilePtr->m_RISM.IsEmpty() )
        AppendItem( m_bookId, m_supBookFilePtr->m_RISM, IMG_TEXT, IMG_TEXT_S );
    if ( !m_supBookFilePtr->m_Composer.IsEmpty() )
        AppendItem( m_bookId, m_supBookFilePtr->m_Composer, IMG_TEXT, IMG_TEXT_S );
    if ( !m_supBookFilePtr->m_Title.IsEmpty() )
        AppendItem( m_bookId, m_supBookFilePtr->m_Title, IMG_TEXT, IMG_TEXT_S );
    if ( !m_supBookFilePtr->m_Printer.IsEmpty() )
        AppendItem( m_bookId, m_supBookFilePtr->m_Printer, IMG_TEXT, IMG_TEXT_S );
    if ( !m_supBookFilePtr->m_Year.IsEmpty() )
        AppendItem( m_bookId, m_supBookFilePtr->m_Year, IMG_TEXT, IMG_TEXT_S );
    if ( !m_supBookFilePtr->m_Library1.IsEmpty() )
        AppendItem( m_bookId, m_supBookFilePtr->m_Library1, IMG_TEXT, IMG_TEXT_S );
    if ( !m_supBookFilePtr->m_Library2.IsEmpty() )
        AppendItem( m_bookId, m_supBookFilePtr->m_Library2, IMG_TEXT, IMG_TEXT_S );
        
    DeleteChildren( m_imgFilesId1 );
    int img1 = (int)m_supBookFilePtr->m_imgFiles1.GetCount();
    if ( img1 )
        SetItemText( m_imgFilesId1, wxString::Format( _("Images 1 (%d)"), img1 ) );
    else
        SetItemText( m_imgFilesId1, _("Images 1") );
    for ( i = 0; i < img1; i++)
    {
        id = AppendItem( m_imgFilesId1, m_supBookFilePtr->m_imgFiles1[i].m_filename, IMG_DOC, IMG_DOC_S );
        if (  m_supBookFilePtr->m_imgFiles1[i].m_flags & FILE_DESACTIVATED ) 
            SetItemTextColour( id , *wxLIGHT_GREY );
        if ( !wxFileExists( m_supBookFilePtr->m_imgFileDir1 + wxFileName::GetPathSeparator() +  m_supBookFilePtr->m_imgFiles1[i].m_filename ) ) 
            SetItemTextColour( id , *wxRED );
    }
    DeleteChildren( m_imgFilesId2 );
	int img2 = (int)m_supBookFilePtr->m_imgFiles2.GetCount();
    if ( img2 > 0 )
        SetItemText( m_imgFilesId2, wxString::Format( _("Images 2 (%d)"), img2 ) );
    else
        SetItemText( m_imgFilesId2, _("Images 2") );
    for ( i = 0; i < img2; i++)
    {
        id = AppendItem( m_imgFilesId2, m_supBookFilePtr->m_imgFiles2[i].m_filename, IMG_DOC, IMG_DOC_S );
        if (  m_supBookFilePtr->m_imgFiles2[i].m_flags & FILE_DESACTIVATED ) 
            SetItemTextColour( id , *wxLIGHT_GREY );
        if ( !wxFileExists( m_supBookFilePtr->m_imgFileDir2 + wxFileName::GetPathSeparator() +  m_supBookFilePtr->m_imgFiles2[i].m_filename ) ) 
            SetItemTextColour( id , *wxRED );
    }
        
    DeleteChildren( m_axFilesId );
    int ax = (int)m_supBookFilePtr->m_axFiles.GetCount();
    if ( ax > 0 )
        SetItemText( m_axFilesId, wxString::Format( _("Aruspix files (%d)"), ax ) );
    else
        SetItemText( m_axFilesId, _("Aruspix files") );
    for ( i = 0; i < ax; i++)
    {
        id = AppendItem( m_axFilesId, m_supBookFilePtr->m_axFiles[i].m_filename, IMG_AXZ, IMG_AXZ_S );
        if (  m_supBookFilePtr->m_axFiles[i].m_flags & FILE_DESACTIVATED ) 
			SetItemTextColour( id , *wxLIGHT_GREY );
        if ( !wxFileExists( m_supBookFilePtr->m_axFileDir + wxFileName::GetPathSeparator() +  m_supBookFilePtr->m_axFiles[i].m_filename ) ) 
            SetItemTextColour( id , *wxRED );
		else if ( SupFile::IsSuperimposed( m_supBookFilePtr->m_axFileDir + wxFileName::GetPathSeparator() +  m_supBookFilePtr->m_axFiles[i].m_filename ) )
		//else if ( m_supBookFilePtr->m_axFiles[i].m_flags & FILE_FULLY_PROCESSED ) 
		{
			SetItemImage( id, IMG_AXZ_OK ); 
			SetItemImage( id, IMG_AXZ_OK_S,  wxTreeItemIcon_Selected );
		}	
    }
	
	this->LoadDisplay( );
	
    m_supBookFilePtr->Modify();
}  

// WDR: handler implementations for SupBookCtrl

void SupBookCtrl::OnSelection( wxTreeEvent &event )
{
	wxASSERT( m_supBookPanelPtr );
	
    // show some info about this item
    wxTreeItemId itemId = event.GetItem();
	if ( !itemId.IsOk() )
		return;

	if ( ItemIsChildOf( m_imgFilesId1, itemId ) )
    {   
        m_supBookPanelPtr->Preview( m_supBookFilePtr->m_imgFileDir1 + wxFileName::GetPathSeparator() +  GetItemText( itemId ) );
    }
	else if ( ItemIsChildOf( m_imgFilesId2, itemId ) )
    {   
        m_supBookPanelPtr->Preview( m_supBookFilePtr->m_imgFileDir2 + wxFileName::GetPathSeparator() +  GetItemText( itemId ) );
    }
    else if ( ItemIsChildOf( m_axFilesId, itemId ) )
    {
        m_supBookPanelPtr->Preview( m_supBookFilePtr->m_axFileDir + wxFileName::GetPathSeparator() +  GetItemText( itemId ) );
    }
	else
		m_supBookPanelPtr->Preview( "" );
	event.Skip();
}

void SupBookCtrl::OnActivate( wxTreeEvent &event )
{
    // show some info about this item
    wxTreeItemId itemId = event.GetItem();

    if ( ItemIsChildOf( m_bookId, itemId ) )
    {
        // edition
    }
    else if ( ItemIsChildOf( m_imgFilesId1, itemId ) )
    {   
        //m_supEnvPtr->OpenFile( m_supBookFilePtr->m_imgFileDir1 + wxFileName::GetPathSeparator() +  GetItemText( itemId ), -1 );
    }
    else if ( ItemIsChildOf( m_imgFilesId2, itemId ) )
    {   
        //m_supEnvPtr->OpenFile( m_supBookFilePtr->m_imgFileDir2 + wxFileName::GetPathSeparator() +  GetItemText( itemId ), -1 );
    }
    else if ( ItemIsChildOf( m_axFilesId, itemId ) )
    {
        m_supEnvPtr->OpenFile( m_supBookFilePtr->m_axFileDir + wxFileName::GetPathSeparator() +  GetItemText( itemId ), AX_FILE_DEFAULT );
    }
}

void SupBookCtrl::OnAxDesactivate( wxCommandEvent &event )
{
    wxArrayTreeItemIds array;
    size_t count = GetSelections(array);
    for ( size_t n = 0; n < count; n++ )
    {
        wxString name = GetItemText(array.Item(n));
        m_supBookFilePtr->DesactivateAxfile( name );
    }
    Update( );    
}

void SupBookCtrl::OnAxRemove( wxCommandEvent &event )
{
    if ( wxMessageBox("The files won't be deleted. Do you want to remove the files?", wxGetApp().GetAppName() , wxYES | wxNO | wxICON_QUESTION ) != wxYES )
        return;

    wxArrayTreeItemIds array;
    size_t count = GetSelections(array);
    for ( size_t n = 0; n < count; n++ )
    {
        wxString name = GetItemText(array.Item(n));
        m_supBookFilePtr->RemoveAxfile( name );
    }
    Update( );     
}

void SupBookCtrl::OnAxDelete( wxCommandEvent &event )
{
    if ( wxMessageBox("Are you sure you want to delete the files?", wxGetApp().GetAppName() , wxYES | wxNO | wxICON_QUESTION ) != wxYES )
        return;

    wxArrayTreeItemIds array;
    size_t count = GetSelections(array);
    for ( size_t n = 0; n < count; n++ )
    {
        wxString name = GetItemText(array.Item(n));
        m_supBookFilePtr->DeleteAxfile( name );
    }
    Update( );      
}

void SupBookCtrl::OnImgDesactivate( wxCommandEvent &event )
{
	int book_no = (event.GetId() == ID2_POPUP_TREE_IMG_DESACTIVATE1) ? 1 : 2;
    wxArrayTreeItemIds array;
    size_t count = GetSelections(array);
    for ( size_t n = 0; n < count; n++ )
    {
        wxString name = GetItemText(array.Item(n));
        m_supBookFilePtr->DesactivateImage( name, book_no );
    }
    Update( );    
}

void SupBookCtrl::OnImgRemove( wxCommandEvent &event )
{
    if ( wxMessageBox("The files won't be deleted. Do you want to remove the files?", wxGetApp().GetAppName() , wxYES | wxNO | wxICON_QUESTION ) != wxYES )
        return;

	int book_no = (event.GetId() == ID2_POPUP_TREE_IMG_REMOVE1) ? 1 : 2;
    wxArrayTreeItemIds array;
    size_t count = GetSelections(array);
    for ( size_t n = 0; n < count; n++ )
    {
        wxString name = GetItemText(array.Item(n));
        m_supBookFilePtr->RemoveImage( name, book_no );
    }
    Update( );    
}

void SupBookCtrl::OnBook( wxCommandEvent &event )
{
    m_supEnvPtr->ProcessEvent( event );
}

void SupBookCtrl::OnMenu( wxTreeEvent &event )
{
    wxMenu popup;
    popup.Append( ID2_POPUP_TREE_BOOK_EDIT, _("Edit") );
    popup.Append( ID2_POPUP_TREE_LOAD, _("Reload files"), _("Reload images and Aruspix files into the book")  );
    popup.AppendSeparator( );
    popup.Append( ID2_POPUP_TREE_SUP, _("Batch superimposition") );
    
    
    //if ( SelectionIsChildOf( book ) )
    //{
    //    popup.AppendSeparator( );
    //    popup.Append( ID2_POPUP_TREE_BOOK_EDIT, _("Edit") );
    //}
	   
    if ( SelectionIsChildOf( m_imgFilesId1 ) )
    {
        popup.AppendSeparator( );
        popup.Append( ID2_POPUP_TREE_IMG_REMOVE1, _("Remove file reference from book") );
		popup.AppendSeparator( );
        popup.Append( ID2_POPUP_TREE_IMG_DESACTIVATE1, _("Desactivate / reactivate") );
    }
	
    if ( SelectionIsChildOf( m_imgFilesId2 ) )
    {
        popup.AppendSeparator( );
        popup.Append( ID2_POPUP_TREE_IMG_REMOVE2, _("Remove file reference from book") );
		popup.AppendSeparator( );
        popup.Append( ID2_POPUP_TREE_IMG_DESACTIVATE2, _("Desactivate / reactivate") );
    }
	
	if ( SelectionIsChildOf( m_axFilesId ) )
    {
        popup.AppendSeparator( );
        popup.Append( ID2_POPUP_TREE_AX_REMOVE, _("Remove file reference from book") );
        popup.Append( ID2_POPUP_TREE_AX_DELETE, _("Delete file permanently") );
		popup.AppendSeparator( );
		popup.Append( ID2_POPUP_TREE_AX_DESACTIVATE, _("Desactivate / reactivate") );
    }

    wxPoint clientpt = event.GetPoint();
    wxPoint screenpt = ClientToScreen(clientpt);    
    PopupMenu(&popup, clientpt );
}

#endif // AX_SUPERIMPOSITION
