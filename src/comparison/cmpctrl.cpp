/////////////////////////////////////////////////////////////////////////////
// Name:        cmpctrl.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_RECOGNITION
	#ifdef AX_COMPARISON

#if defined(__GNUG__) && ! defined(__APPLE__)
#pragma implementation "cmpctrl.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

//#include "wx/file.h"
//#include "wx/filename.h"
#include "wx/imaglist.h"

#include "cmpctrl.h"
#include "cmpfile.h"
#include "cmp.h"

#include "app/axapp.h"

#include "recognition/recbookfile.h"
#include "recognition/recfile.h"

// WDR: class implementations

/*

//----------------------------------------------------------------------------
// CmpDataDlg
//----------------------------------------------------------------------------

// WDR: event table for CmpDataDlg

BEGIN_EVENT_TABLE(CmpDataDlg,wxDialog)
    EVT_BUTTON( wxID_OK, CmpDataDlg::OnOk )
    //EVT_BUTTON( wxID_CANCEL, CmpDataDlg::OnCancel )
    //EVT_BUTTON( ID6_ON_BOOK_IMAGES, CmpDataDlg::OnBrowseImages )
    //EVT_BUTTON( ID6_ON_BOOK_AXFILES, CmpDataDlg::OnBrowseAxfiles )
END_EVENT_TABLE()

CmpDataDlg::CmpDataDlg( wxWindow *parent, wxWindowID id, const wxString &title,
    RecBookFile *recBookFile ) :
    wxDialog( parent, id, title )
{
    // WDR: dialog function BookDataFunc4 for CmpDataDlg
    BookDataFunc4( this, TRUE );
    m_recBookFile = recBookFile;
    m_loadAxfiles = false;
    m_loadImages = false;
    
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
}

/ *
bool CmpDataDlg::Validate()
{
    return TRUE;
}

bool CmpDataDlg::TransferDataToWindow()
{
    return TRUE;
}
* /

/ *
bool CmpDataDlg::TransferDataFromWindow()
{
    return true;
}
* /

// WDR: handler implementations for CmpDataDlg

void CmpDataDlg::OnBrowseAxfiles( wxCommandEvent &event )
{
    wxString start = this->Axfiles()->GetValue( ).IsEmpty() ? wxGetApp().m_lastDirAX0_out : this->Axfiles()->GetValue( );
    wxString input = wxDirSelector( _("Aruspix file folder"), start );
    if ( input.empty() )
        return;
    
    wxGetApp().m_lastDirAX0_out = input;
    this->Axfiles()->SetValue(input);
    m_loadAxfiles = true;
}

void CmpDataDlg::OnBrowseImages( wxCommandEvent &event )
{
    wxString start = this->Images()->GetValue( ).IsEmpty() ? wxGetApp().m_lastDirTIFF_in : this->Images()->GetValue( );
    wxString input = wxDirSelector( _("Images folder"), start );
    if ( input.empty() )
        return;

    wxGetApp().m_lastDirTIFF_in = input;
    this->Images()->SetValue(input);
    m_loadImages = true;
}


void CmpDataDlg::OnOk(wxCommandEvent &event)
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

/ *
void CmpDataDlg::OnCancel(wxCommandEvent &event)
{
    event.Skip();
}
* /

*/


//----------------------------------------------------------------------------
// CmpCtrlPanel
//----------------------------------------------------------------------------

#define PREVIEW_WIDTH 200
#define PREVIEW_HEIGHT 200

// WDR: event table for CmpCtrlPanel

BEGIN_EVENT_TABLE(CmpCtrlPanel,wxPanel)
    EVT_CHECKBOX( ID6_CB_PREVIEW, CmpCtrlPanel::OnPreview )
END_EVENT_TABLE()

CmpCtrlPanel::CmpCtrlPanel( wxWindow *parent, wxWindowID id,
                            const wxPoint &position, const wxSize& size, long style ) :
wxPanel( parent, id, position, size, style )
{
    // WDR: dialog function BookFunc4 for CmpCtrlPanel
    CmpFunc6( this, TRUE );
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

void CmpCtrlPanel::UpdatePreview( )
{
    if ( m_show_preview )
	{
		LoadPreview( );
		this->GetPreview()->SetBitmap( m_preview );
	}
	else
        this->GetPreview()->SetBitmap( m_nopreview );

}

void CmpCtrlPanel::Preview( wxString filename )
{
	m_filename = filename;
	UpdatePreview( );
}	
	
void CmpCtrlPanel::LoadPreview( )
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

// WDR: handler implementations for CmpCtrlPanel

void CmpCtrlPanel::OnPreview( wxCommandEvent &event )
{
    if ( event.IsChecked() )
		m_show_preview = true;
    else
       m_show_preview = false;
	UpdatePreview( );
}


//----------------------------------------------------------------------------
// CmpCtrl
//----------------------------------------------------------------------------


// WDR: event table for CmpCtrl

BEGIN_EVENT_TABLE(CmpCtrl,AxCtrl)
    EVT_TREE_ITEM_MENU( ID6_TREEBOOK, CmpCtrl::OnMenu )
    EVT_MENU( ID6_POPUP_TREE_LOAD, CmpCtrl::OnBook )
    EVT_MENU( ID6_POPUP_TREE_EDIT, CmpCtrl::OnBook )
	EVT_MENU( ID6_POPUP_TREE_ASSEMBLE, CmpCtrl::OnAssembleParts )
	EVT_MENU( ID6_POPUP_TREE_AX_DESACTIVATE, CmpCtrl::OnAxDesactivate )
    EVT_TREE_ITEM_ACTIVATED( ID6_TREEBOOK, CmpCtrl::OnActivate )
    EVT_TREE_SEL_CHANGED( ID6_TREEBOOK, CmpCtrl::OnSelection )
END_EVENT_TABLE()

CmpCtrl::CmpCtrl( wxWindow *parent, wxWindowID id,
                          const wxPoint &position, const wxSize& size, long style ) :
AxCtrl( parent, id,  position, size, style )
{
    m_cmpFilePtr = NULL;
    m_cmpEnvPtr = NULL;
	m_cmpCtrlPanelPtr = NULL;
}

CmpCtrl::~CmpCtrl( )
{
}

void CmpCtrl::SaveDisplay( )
{
    if ( m_rootId.IsOk() && ItemHasChildren( m_rootId ) ) CmpEnv::s_expand_root = IsExpanded( m_rootId );
	if ( m_cmpId.IsOk() && ItemHasChildren( m_cmpId ) ) CmpEnv::s_expand_cmp = IsExpanded( m_cmpId );
	if ( m_booksId.IsOk() && ItemHasChildren( m_booksId ) ) CmpEnv::s_expand_book = IsExpanded( m_booksId );
}

void CmpCtrl::LoadDisplay( )
{

    if ( m_rootId.IsOk() && CmpEnv::s_expand_root ) Expand( m_rootId );
	if ( m_booksId.IsOk() && CmpEnv::s_expand_book ) Expand( m_booksId );
	if ( m_cmpId.IsOk() && CmpEnv::s_expand_book ) ExpandAllChildren( m_cmpId );
}

void CmpCtrl::Build( )
{
    wxASSERT( m_cmpFilePtr );
    wxASSERT( m_cmpEnvPtr );
	
	this->SaveDisplay( );
    this->DeleteAllItems( );
    
    //this->CollapseAll( );
    m_rootId = AddRoot( m_cmpFilePtr->m_shortname );
    SetTypeImages( m_rootId, IMG_FOLDER );
    m_booksId = AppendItem( m_rootId, _("Books") );
    SetTypeImages( m_booksId, IMG_FOLDER );
    m_cmpId = AppendItem( m_rootId, _("Collations") );
    SetTypeImages( m_cmpId, IMG_FOLDER );

    if ( CmpEnv::s_expand_root ) Expand( m_rootId );

    Update( );
}


void CmpCtrl::Update( )
{
    wxASSERT( m_cmpFilePtr );
    wxASSERT( m_cmpEnvPtr );
    
    wxTreeItemId id;
	
	this->SaveDisplay( );
	
	DeleteChildren( m_booksId );
    int books = (int)m_cmpFilePtr->m_bookFiles.GetCount();
    if ( books > 0 )
        SetItemText( m_booksId, wxString::Format( _("Books (%d)"), books ) );
    else
        SetItemText( m_booksId, _("Books") );
    for ( int i = 0; i < books; i++)
    {
		RecBookFile *book = m_cmpFilePtr->m_bookFiles[i].m_recBookFilePtr;
        m_cmpFilePtr->m_bookFiles[i].m_bookId = AppendItem( m_booksId, m_cmpFilePtr->m_bookFiles[i].m_shortname );
		SetTypeImages( m_cmpFilePtr->m_bookFiles[i].m_bookId, IMG_FOLDER );
		
		
		//m_cmpFilePtr->m_bookFiles[i].m_imgFilesId = AppendItem( m_cmpFilePtr->m_bookFiles[i].m_bookId, _("Images") );
		//SetTypeImages( m_cmpFilePtr->m_bookFiles[i].m_imgFilesId, IMG_FOLDER );
		m_cmpFilePtr->m_bookFiles[i].m_axFilesId = AppendItem( m_cmpFilePtr->m_bookFiles[i].m_bookId, _("Aruspix files") );
		SetTypeImages( m_cmpFilePtr->m_bookFiles[i].m_axFilesId, IMG_FOLDER );
		m_cmpFilePtr->m_bookFiles[i].m_partsId = AppendItem( m_cmpFilePtr->m_bookFiles[i].m_bookId, _("Parts") );
		SetTypeImages( m_cmpFilePtr->m_bookFiles[i].m_partsId, IMG_FOLDER );
		
		/*
		//DeleteChildren( m_cmpFilePtr->m_bookFiles[i].m_imgFilesId );
		int img = (int)book->m_imgFiles.GetCount();
		if ( img > 0 )
			SetItemText( m_cmpFilePtr->m_bookFiles[i].m_imgFilesId, wxString::Format( _("Images (%d)"), img ) );
		else
			SetItemText( m_cmpFilePtr->m_bookFiles[i].m_imgFilesId, _("Images") );
		for ( int j = 0; j < img; j++)
		{
			id = AppendItem( m_cmpFilePtr->m_bookFiles[i].m_imgFilesId, book->m_imgFiles[j].m_filename, IMG_DOC, IMG_DOC_S );
			if (  book->m_imgFiles[j].m_flags & FILE_DESACTIVATED ) 
				SetItemTextColour( id , *wxLIGHT_GREY );
			if ( !wxFileExists( book->m_imgFileDir + wxFileName::GetPathSeparator() +  book->m_imgFiles[j].m_filename ) ) 
				SetItemTextColour( id , *wxRED );
		}
		*/
    
		//DeleteChildren( m_cmpFilePtr->m_bookFiles[i].m_axFilesId );
		int ax = (int)book->m_axFiles.GetCount();
		if ( ax > 0 )
			SetItemText( m_cmpFilePtr->m_bookFiles[i].m_axFilesId, wxString::Format( _("Aruspix files (%d)"), ax ) );
		else
			SetItemText( m_cmpFilePtr->m_bookFiles[i].m_axFilesId, _("Aruspix files") );
		for ( int j = 0; j < ax; j++)
		{
			id = AppendItem( m_cmpFilePtr->m_bookFiles[i].m_axFilesId, book->m_axFiles[j].m_filename, IMG_AXZ, IMG_AXZ_S );
			if (  book->m_axFiles[j].m_flags & FILE_DESACTIVATED ) 
				SetItemTextColour( id , *wxLIGHT_GREY );
			if ( !wxFileExists( book->m_axFileDir + wxFileName::GetPathSeparator() +  book->m_axFiles[j].m_filename ) ) 
				SetItemTextColour( id , *wxRED );
			else if ( RecFile::IsRecognized( book->m_axFileDir + wxFileName::GetPathSeparator() +  book->m_axFiles[j].m_filename ) )
			{
				SetItemImage( id, IMG_AXZ_OK ); 
				SetItemImage( id, IMG_AXZ_OK_S,  wxTreeItemIcon_Selected );
			}	
		}
		
		//DeleteChildren( m_cmpFilePtr->m_bookFiles[i].m_partsId );
		int parts = (int)m_cmpFilePtr->m_bookFiles[i].m_bookParts.GetCount();
		if ( parts > 0 )
			SetItemText( m_cmpFilePtr->m_bookFiles[i].m_partsId, wxString::Format( _("Parts (%d)"), parts ) );
		else
			SetItemText( m_cmpFilePtr->m_bookFiles[i].m_partsId, _("Parts") );
		for ( int j = 0; j < parts; j++)
		{
			CmpBookPart *part = &m_cmpFilePtr->m_bookFiles[i].m_bookParts[j];
			wxTreeItemId partid;
			partid = AppendItem( m_cmpFilePtr->m_bookFiles[i].m_partsId, part->m_name );
			SetTypeImages( partid, IMG_FOLDER );
			for ( int k = 0; k < (int)part->m_partpages.GetCount(); k++)
			{
				wxString staves = part->m_partpages[k].m_axfile;
				if ( !part->m_partpages[k].m_staves.IsEmpty() )
				{
					//TODO, something...
				}
				id = AppendItem( partid, staves, IMG_AXZ_OK, IMG_AXZ_OK_S );
			}	
		}
    }
	
    DeleteChildren( m_cmpId );
    int collations = (int)m_cmpFilePtr->m_collations.GetCount();
    if ( collations > 0 )
        SetItemText( m_cmpId, wxString::Format( _("Collations (%d)"), collations ) );
    else
        SetItemText( m_cmpId, _("Collations") );
    for ( int i = 0; i < collations; i++)
    {
		//RecBookFile *book = m_cmpFilePtr->m_bookFiles[i].m_recBookFilePtr;
		m_cmpFilePtr->m_collations[i].m_colId = AppendItem( m_cmpId, m_cmpFilePtr->m_collations[i].m_name );
		SetTypeImages( m_cmpFilePtr->m_collations[i].m_colId, IMG_FOLDER );
		
		//DeleteChildren( m_cmpFilePtr->m_bookFiles[i].m_partsId );
		int parts = (int)m_cmpFilePtr->m_collations[i].m_collationParts.GetCount();
		//if ( parts > 0 )
		//	SetItemText( colid, wxString::Format( _("Parts (%d)"), parts ) );
		//else
		//	SetItemText( colid, _("Parts") );
		for ( int j = 0; j < parts; j++)
		{
			CmpCollationPart *part = &m_cmpFilePtr->m_collations[i].m_collationParts[j];
			wxString label = wxString::Format("%s (%s)", part->m_bookPart->m_bookname.c_str(), part->m_bookPart->m_name.c_str() ); 
			id = AppendItem( m_cmpFilePtr->m_collations[i].m_colId, label, IMG_TEXT, IMG_TEXT_S );
			if (  part->m_flags & PART_REFERENCE ) 
				SetItemBold( id , true );
		}		
		
		
	}
		
	this->LoadDisplay( );
	
    m_cmpFilePtr->Modify();
}

CmpCollation *CmpCtrl::GetSelectedCollation( )
{
	int collations = (int)m_cmpFilePtr->m_collations.GetCount();
    for ( int i = 0; i < collations; i++)
    {
		CmpCollation *item = &m_cmpFilePtr->m_collations[i];
		if ( this->SelectionIsChildOf( item->m_colId ) )
			return item;
	}
	return NULL;
}

CmpBookItem *CmpCtrl::GetSelectedBookItem()
{
	int books = (int)m_cmpFilePtr->m_bookFiles.GetCount();
    for ( int i = 0; i < books; i++)
    {
		CmpBookItem *item = &m_cmpFilePtr->m_bookFiles[i];
		if ( this->SelectionIsChildOf( m_booksId ) )
			return item;
	}
	return NULL;
}

// WDR: handler implementations for CmpCtrl

void CmpCtrl::OnSelection( wxTreeEvent &event )
{
	wxASSERT( m_cmpCtrlPanelPtr );
	
    // show some info about this item
    wxTreeItemId itemId = event.GetItem();
	if ( !itemId.IsOk() )
		return;
		
	CmpBookItem *item = GetSelectedBookItem();
	
	if ( item && ItemIsChildOf( item->m_imgFilesId, itemId ) )
    {   
        m_cmpCtrlPanelPtr->Preview( item->m_recBookFilePtr->m_imgFileDir + wxFileName::GetPathSeparator() +  GetItemText( itemId ) );
    }
    else if ( item && ItemIsChildOf( item->m_axFilesId, itemId ) )
    {
        m_cmpCtrlPanelPtr->Preview( item->m_recBookFilePtr->m_axFileDir + wxFileName::GetPathSeparator() +  GetItemText( itemId ) );
    }
    else if ( item && ItemIsChildOf( item->m_partsId, itemId ) )
    {
        m_cmpCtrlPanelPtr->Preview( item->m_recBookFilePtr->m_axFileDir + wxFileName::GetPathSeparator() +  GetItemText( itemId ) );
    }
	else
		m_cmpCtrlPanelPtr->Preview( "" );
	event.Skip();
}

void CmpCtrl::OnActivate( wxTreeEvent &event )
{
    // show some info about this item
    wxTreeItemId itemId = event.GetItem();

	if ( !itemId.IsOk() )
		return;
		
	CmpCollation *collation = GetSelectedCollation();
	
	if ( collation )
	{
		m_cmpEnvPtr->ViewCollation( collation );
	}
	/*
    else if ( ItemIsChildOf( m_imgFilesId, itemId ) )
    {   
        m_cmpEnvPtr->OpenFile( m_cmpFilePtr->m_imgFileDir + wxFileName::GetPathSeparator() +  GetItemText( itemId ), -1 );
    }
    else if ( ItemIsChildOf( m_axFilesId, itemId ) )
    {
        m_cmpEnvPtr->OpenFile( m_cmpFilePtr->m_axFileDir + wxFileName::GetPathSeparator() +  GetItemText( itemId ), AX_FILE_DEFAULT );
    }
	else if ( ItemIsChildOf( m_optFilesId, itemId ) )
    {
        m_cmpEnvPtr->OpenFile( m_cmpFilePtr->m_axFileDir + wxFileName::GetPathSeparator() +  GetItemText( itemId ), AX_FILE_DEFAULT );
    }
	*/
}

void CmpCtrl::OnAxDesactivate( wxCommandEvent &event )
{
    wxArrayTreeItemIds array;
    size_t count = GetSelections(array);
    for ( size_t n = 0; n < count; n++ )
    {
        wxString name = GetItemText(array.Item(n));
        m_cmpFilePtr->DesactivateAxfile( name );
    }
    Update( );  
}

/*
void CmpCtrl::OnAxRemove( wxCommandEvent &event )
{
    if ( wxMessageBox("The files won't be deleted. Do you want to remove the files?", wxGetApp().GetAppName() , wxYES | wxNO | wxICON_QUESTION ) != wxYES )
        return;

    wxArrayTreeItemIds array;
    size_t count = GetSelections(array);
    for ( size_t n = 0; n < count; n++ )
    {
        wxString name = GetItemText(array.Item(n));
        m_cmpFilePtr->RemoveAxfile( name );
    }
    Update( );     
}


void CmpCtrl::OnAxDelete( wxCommandEvent &event )
{
    if ( wxMessageBox("Are you sure you want to delete the files?", wxGetApp().GetAppName() , wxYES | wxNO | wxICON_QUESTION ) != wxYES )
        return;

    wxArrayTreeItemIds array;
    size_t count = GetSelections(array);
    for ( size_t n = 0; n < count; n++ )
    {
        wxString name = GetItemText(array.Item(n));
        m_cmpFilePtr->DeleteAxfile( name );
    }
    Update( );      
}

void CmpCtrl::OnImgDesactivate( wxCommandEvent &event )
{
    wxArrayTreeItemIds array;
    size_t count = GetSelections(array);
    for ( size_t n = 0; n < count; n++ )
    {
        wxString name = GetItemText(array.Item(n));
        m_cmpFilePtr->DesactivateImage( name );
    }
    Update( );    
}

void CmpCtrl::OnImgRemove( wxCommandEvent &event )
{
    if ( wxMessageBox("The files won't be deleted. Do you want to remove the files?", wxGetApp().GetAppName() , wxYES | wxNO | wxICON_QUESTION ) != wxYES )
        return;

    wxArrayTreeItemIds array;
    size_t count = GetSelections(array);
    for ( size_t n = 0; n < count; n++ )
    {
        wxString name = GetItemText(array.Item(n));
        m_cmpFilePtr->RemoveImage( name );
    }
    Update( );    
}
*/

void CmpCtrl::OnBook( wxCommandEvent &event )
{
    m_cmpEnvPtr->ProcessEvent( event );
}


void CmpCtrl::OnAssembleParts( wxCommandEvent &event )
{	
	CmpBookItem *item = GetSelectedBookItem();
	
	if ( item )
		item->AssembleParts( );
	//event.Skip();
}

void CmpCtrl::OnMenu( wxTreeEvent &event )
{
    wxMenu popup;
    popup.Append( ID6_POPUP_TREE_EDIT, _("Edit") );
    popup.Append( ID6_POPUP_TREE_LOAD, _("Reload files"), _("Reload images and Aruspix files")  );
    //popup.AppendSeparator( );
    //popup.Append( ID6_POPUP_TREE_PRE, _("Batch preprocessing") );
    //popup.Append( ID6_POPUP_TREE_REC, _("Batch recognition") );
    //popup.Append( ID6_POPUP_TREE_ADAPT, _("Optimize") );
    
    
    if ( SelectionIsChildOf( m_booksId ) )
    {
        popup.AppendSeparator( );
        popup.Append( ID6_POPUP_TREE_ASSEMBLE, _("Assemble parts") );
    }
	/*
    if ( SelectionIsChildOf( m_imgFilesId ) )
    {
        popup.AppendSeparator( );
        popup.Append( ID6_POPUP_TREE_IMG_REMOVE, _("Remove file reference from book") );
		popup.AppendSeparator( );
        popup.Append( ID6_POPUP_TREE_IMG_DESACTIVATE, _("Desactivate / reactivate") );
    }
    else if ( SelectionIsChildOf( m_axFilesId ) )
    {
        popup.AppendSeparator( );
        popup.Append( ID6_POPUP_TREE_AX_REMOVE, _("Remove file reference from book") );
        popup.Append( ID6_POPUP_TREE_AX_DELETE, _("Delete file permanently") );
		popup.AppendSeparator( );
		popup.Append( ID6_POPUP_TREE_AX_DESACTIVATE, _("Desactivate / reactivate") );
    }
	*/

    wxPoint clientpt = event.GetPoint();
    wxPoint screenpt = ClientToScreen(clientpt);    
    PopupMenu(&popup, clientpt );
}

	#endif // AX_COMPARISON
#endif // AX_RECOGNITION
