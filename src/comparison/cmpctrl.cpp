/////////////////////////////////////////////////////////////////////////////
// Name:        cmpctrl.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_RECOGNITION
	#ifdef AX_COMPARISON

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

//#include "wx/file.h"
//#include "wx/filename.h"
#include "wx/imaglist.h"

#include "cmpctrl.h"
#include "cmpfile.h"
#include "cmp.h"

#include "app/axapp.h"

#include "recognition/recbookfile.h"
#include "recognition/recfile.h"


enum {
    CMP_TREE_ROOT = 0,
    CMP_TREE_BOOKS,
    CMP_TREE_BOOK_AXFILE,
    CMP_TREE_BOOK_PARTS,
    CMP_TREE_BOOK_PART_PAGE,
    CMP_TREE_BOOK_PART_PAGE_START,
    CMP_TREE_BOOK_PART_PAGE_END,
    CMP_TREE_BOOK_PART_PAGE_START_END,
    CMP_TREE_COLLATIONS,
    CMP_TREE_COLLATION,
    CMP_TREE_COLLATION_RESULT
};

//----------------------------------------------------------------------------
// CmpCtrlPanel
//----------------------------------------------------------------------------

#define PREVIEW_WIDTH 200
#define PREVIEW_HEIGHT 200


BEGIN_EVENT_TABLE(CmpCtrlPanel,wxPanel)
    EVT_CHECKBOX( ID6_CB_PREVIEW, CmpCtrlPanel::OnPreview )
END_EVENT_TABLE()

CmpCtrlPanel::CmpCtrlPanel( wxWindow *parent, wxWindowID id,
                            const wxPoint &position, const wxSize& size, long style ) :
wxPanel( parent, id, position, size, style )
{
        CmpFunc6( this, true );
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



BEGIN_EVENT_TABLE(CmpCtrl,AxCtrl)
    EVT_TREE_ITEM_MENU( ID6_TREEBOOK, CmpCtrl::OnMenu )
    EVT_MENU( ID6_POPUP_TREE_LOAD, CmpCtrl::OnCmpEvent )
	EVT_MENU( ID6_POPUP_TREE_ASSEMBLE, CmpCtrl::OnAssembleParts )
    EVT_MENU( ID6_POPUP_TREE_ADD_PART, CmpCtrl::OnAddPart )
    EVT_MENU( ID6_POPUP_TREE_ADD_AXFILE, CmpCtrl::OnAddAxFile )
    EVT_MENU( ID6_POPUP_TREE_ADD_PART_PAGE_START, CmpCtrl::OnAddPartPageStartEnd )
    EVT_MENU( ID6_POPUP_TREE_ADD_PART_PAGE_END, CmpCtrl::OnAddPartPageStartEnd )
    EVT_MENU( ID6_POPUP_TREE_REMOVE_PART_PAGE_START, CmpCtrl::OnRemovePartPageStartEnd )
    EVT_MENU( ID6_POPUP_TREE_REMOVE_PART_PAGE_END, CmpCtrl::OnRemovePartPageStartEnd )
    EVT_MENU( ID6_POPUP_TREE_ADD_AXFILE, CmpCtrl::OnAddAxFile )
    EVT_MENU( ID6_POPUP_TREE_ADD_COLLATION, CmpCtrl::OnCmpEvent )
    EVT_MENU( ID6_POPUP_TREE_ADD_COLLATION_PART, CmpCtrl::OnAddCollationPart )
    EVT_TREE_ITEM_ACTIVATED( ID6_TREEBOOK, CmpCtrl::OnActivate )
    EVT_TREE_SEL_CHANGED( ID6_TREEBOOK, CmpCtrl::OnSelection )
    // Events with no specific method that need to be passed to the CmpEnv object
    EVT_MENU( ID6_POPUP_TREE_ADD_BOOK, CmpCtrl::OnCmpEvent )    
    EVT_MENU( ID6_POPUP_TREE_LOAD, CmpCtrl::OnCmpEvent )
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
    AxTreeItem *axTreeItem = NULL;
	int i, j;
	
	DeleteChildren( m_booksId );
    int books = (int)m_cmpFilePtr->m_bookFiles.GetCount();
    if ( books > 0 )
        SetItemText( m_booksId, wxString::Format( _("Books (%d)"), books ) );
    else
        SetItemText( m_booksId, _("Books") );
    // For specific menu
    axTreeItem = new AxTreeItem( m_booksId, CMP_TREE_BOOKS, NULL ); 
    m_axItems.Add( axTreeItem );
    
    for ( i = 0; i < books; i++)
    {
		RecBookFile *book = m_cmpFilePtr->m_bookFiles[i].m_recBookFilePtr;
        wxTreeItemId m_bookId = AppendItem( m_booksId, m_cmpFilePtr->m_bookFiles[i].m_shortname );
		SetTypeImages( m_bookId, IMG_FOLDER );
		
        // The name of the book
        AppendItem( m_bookId, m_cmpFilePtr->m_bookFiles[i].m_bookname, IMG_TEXT, IMG_TEXT_S );
        
        // The Aruspix files of the book
        m_cmpFilePtr->m_bookFiles[i].m_axFilesId = AppendItem( m_bookId, _("Aruspix files") );
		SetTypeImages( m_cmpFilePtr->m_bookFiles[i].m_axFilesId, IMG_FOLDER );        
		int ax = (int)book->m_axFiles.GetCount();
		if ( ax > 0 )
			SetItemText( m_cmpFilePtr->m_bookFiles[i].m_axFilesId, wxString::Format( _("Aruspix files (%d)"), ax ) );
		else
			SetItemText( m_cmpFilePtr->m_bookFiles[i].m_axFilesId, _("Aruspix files") );
		for ( j = 0; j < ax; j++)
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
                axTreeItem = new AxTreeItem( id, CMP_TREE_BOOK_AXFILE, &m_cmpFilePtr->m_bookFiles[i] ); 
                m_axItems.Add( axTreeItem );
			}	
		}
		
        // The parts of the book
		m_cmpFilePtr->m_bookFiles[i].m_partsId = AppendItem( m_bookId, _("Parts") );
		SetTypeImages( m_cmpFilePtr->m_bookFiles[i].m_partsId, IMG_FOLDER );
        // for the parts we associate the book object
        axTreeItem = new AxTreeItem( m_cmpFilePtr->m_bookFiles[i].m_partsId, CMP_TREE_BOOK_PARTS, &m_cmpFilePtr->m_bookFiles[i] ); 
        m_axItems.Add( axTreeItem );
        // We have a separate method for this
        UpdateParts( &m_cmpFilePtr->m_bookFiles[i] );
    }
	
    DeleteChildren( m_cmpId );
    int collations = (int)m_cmpFilePtr->m_collations.GetCount();
    if ( collations > 0 )
        SetItemText( m_cmpId, wxString::Format( _("Collations (%d)"), collations ) );
    else
        SetItemText( m_cmpId, _("Collations") );
    // For specific menu
    axTreeItem = new AxTreeItem( m_cmpId, CMP_TREE_COLLATIONS, NULL ); 
    m_axItems.Add( axTreeItem );
    
    for ( i = 0; i < collations; i++)
    {
		//RecBookFile *book = m_cmpFilePtr->m_bookFiles[i].m_recBookFilePtr;
		m_cmpFilePtr->m_collations[i].m_colId = AppendItem( m_cmpId, m_cmpFilePtr->m_collations[i].m_name );
		SetTypeImages( m_cmpFilePtr->m_collations[i].m_colId, IMG_FOLDER );
        // for the parts we associate the collation object
        axTreeItem = new AxTreeItem( m_cmpFilePtr->m_collations[i].m_colId, CMP_TREE_COLLATION, &m_cmpFilePtr->m_collations[i] ); 
        m_axItems.Add( axTreeItem );
        // We have a separate method for this
        UpdateCollationParts( &m_cmpFilePtr->m_collations[i] );
	}
	
    m_cmpFilePtr->Modify();
}


void CmpCtrl::UpdateParts( CmpBookItem *book )
{
    int j, k;
    
    if ( !book->m_partsId.IsOk() ) {
        wxLogDebug( "wxTreeItemId should be Ok" );
        return;
    }
    DeleteChildren( book->m_partsId );
    
    int parts = (int)book->m_bookParts.GetCount();
    if ( parts > 0 )
        SetItemText( book->m_partsId, wxString::Format( _("Parts (%d)"), parts ) );
    else
        SetItemText( book->m_partsId, _("Parts") );
    for ( j = 0; j < parts; j++)
    {
        CmpBookPart *part = &book->m_bookParts[j];
        wxTreeItemId partid;
        partid = AppendItem( book->m_partsId, part->m_name );
        SetTypeImages( partid, IMG_FOLDER );
        for ( k = 0; k < (int)part->m_partpages.GetCount(); k++)
        {
            wxString staves = part->m_partpages[k].m_axfile;
            int img = IMG_DOC;
            int itemType = CMP_TREE_BOOK_PART_PAGE;
            if ( part->m_partpages[k].HasStart() && part->m_partpages[k].HasEnd() ) {
                img = IMG_DOC_START_END;
                itemType = CMP_TREE_BOOK_PART_PAGE_START_END;
            }
            else if ( part->m_partpages[k].HasStart() ) {
                img = IMG_DOC_START;
                itemType = CMP_TREE_BOOK_PART_PAGE_START;
            }
            else if ( part->m_partpages[k].HasEnd() ) {
                img = IMG_DOC_END;
                itemType = CMP_TREE_BOOK_PART_PAGE_END;
            }
            wxTreeItemId id = AppendItem( partid, staves, img, img );
            AxTreeItem *axTreeItem = new AxTreeItem( id, itemType, &part->m_partpages[k] ); 
            m_axItems.Add( axTreeItem );
        }	
    }
}

void CmpCtrl::UpdateCollationParts( CmpCollation *collation )
{
    int j;
    
    if ( !collation->m_colId.IsOk() ) {
        wxLogDebug( "wxTreeItemId should be Ok" );
        return;
    }
    DeleteChildren( collation->m_colId ); 
    
    int parts = (int)collation->m_collationParts.GetCount();
    //if ( parts > 0 )
    //	SetItemText( colid, wxString::Format( _("Parts (%d)"), parts ) );
    //else
    //	SetItemText( colid, _("Parts") );
    for ( j = 0; j < parts; j++)
    {
        CmpCollationPart *part = &collation->m_collationParts[j];
        wxString label = wxString::Format("%s (%s)", part->m_bookPart->m_book->m_bookname.c_str(), part->m_bookPart->m_name.c_str() ); 
        wxTreeItemId id = AppendItem( collation->m_colId, label, IMG_TEXT, IMG_TEXT_S );
        if (  part->m_flags & PART_REFERENCE ) {
            SetItemBold( id , true );
        }
        else {
            AxTreeItem *axTreeItem = new AxTreeItem( id, CMP_TREE_COLLATION_RESULT, &collation->m_collationParts[j], collation ); 
            m_axItems.Add( axTreeItem );
        }
    }		
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


void CmpCtrl::OnSelection( wxTreeEvent &event )
{
	wxASSERT( m_cmpCtrlPanelPtr );
	
    // show some info about this item
    wxTreeItemId itemId = event.GetItem();
	if ( !itemId.IsOk() )
		return;
		
	CmpBookItem *item = GetSelectedBookItem();
	
	if ( item )
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
    
    // we should use type here
    wxObject *object = GetObject( itemId );
    if ( object ) {
        if ( dynamic_cast<CmpCollationPart*>(object) ) {
            // we have click on a collation part
            wxObject *secondaryObject = GetObject( itemId, true );
            m_cmpEnvPtr->ViewCollationPart( dynamic_cast<CmpCollationPart*>(object), dynamic_cast<CmpCollation*>(secondaryObject) );
        }
    }
}

void CmpCtrl::OnCmpEvent( wxCommandEvent &event )
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

void CmpCtrl::OnAddPart( wxCommandEvent &event )
{    
    wxObject *object = GetObject( this->GetSelection() );
    
    if ( object && dynamic_cast<CmpBookItem*>(object) ) {
        m_cmpEnvPtr->AddPart( dynamic_cast<CmpBookItem*>(object) );
    }
    
}

void CmpCtrl::OnAddAxFile( wxCommandEvent &event )
{    
    wxObject *object = GetObject( this->GetSelection() );
    
    if ( object && dynamic_cast<CmpBookItem*>(object) ) {
        m_cmpEnvPtr->AddAxFile( this->GetItemText( this->GetSelection() ), dynamic_cast<CmpBookItem*>(object) );
    }
    
}

void CmpCtrl::OnAddPartPageStartEnd( wxCommandEvent &event )
{    
    wxObject *object = GetObject( this->GetSelection() );
    
    if ( object && dynamic_cast<CmpPartPage*>(object) ) {
        m_cmpEnvPtr->AddPartPageStartEnd( this->GetItemText( this->GetSelection() ),
            dynamic_cast<CmpPartPage*>(object), (event.GetId() == ID6_POPUP_TREE_ADD_PART_PAGE_START)  );
    }
    
}

void CmpCtrl::OnRemovePartPageStartEnd( wxCommandEvent &event )
{    
    wxObject *object = GetObject( this->GetSelection() );
    
    if ( object && dynamic_cast<CmpPartPage*>(object) ) {
        m_cmpEnvPtr->RemovePartPageStartEnd( this->GetItemText( this->GetSelection() ),
            dynamic_cast<CmpPartPage*>(object), (event.GetId() == ID6_POPUP_TREE_REMOVE_PART_PAGE_START)  );
    }
    
}

void CmpCtrl::OnAddCollationPart( wxCommandEvent &event )
{    
    wxObject *object = GetObject( this->GetSelection() );
    
    if ( object && dynamic_cast<CmpCollation*>(object) ) {
        m_cmpEnvPtr->AddCollationPart( dynamic_cast<CmpCollation*>(object) );
    }
    
}

void CmpCtrl::OnMenu( wxTreeEvent &event )
{
    wxMenu popup;
    
    popup.Append( ID6_POPUP_TREE_LOAD, _("Reload files"), _("Reload images and Aruspix files")  );
    // Books
    popup.AppendSeparator( );
    popup.Append( ID6_POPUP_TREE_ADD_BOOK, _("Add book") );   
    popup.Enable( ID6_POPUP_TREE_ADD_BOOK, false );
    // Book submenu
    // I am not absolutely sure it we gets deleted, so potential very smal memory leak
    wxMenu *bookSubmenu = new wxMenu(); 
    bookSubmenu->Append( ID6_POPUP_TREE_ASSEMBLE, _("Assemble parts") );
    bookSubmenu->Enable( ID6_POPUP_TREE_ASSEMBLE, false );
    bookSubmenu->AppendSeparator( );
    bookSubmenu->Append( ID6_POPUP_TREE_ADD_PART, _("Add part to book") );
    bookSubmenu->Enable( ID6_POPUP_TREE_ADD_PART, false );
    bookSubmenu->Append( ID6_POPUP_TREE_ADD_AXFILE, _("Add file to part") );
    bookSubmenu->Enable( ID6_POPUP_TREE_ADD_AXFILE, false );
    // Page submenu in Book submenu
    wxMenu *pageSubmenu = new wxMenu();
    pageSubmenu->Append( ID6_POPUP_TREE_ADD_PART_PAGE_START, _("Add start delimiter for a page") );
    pageSubmenu->Enable( ID6_POPUP_TREE_ADD_PART_PAGE_START, false );
    pageSubmenu->Append( ID6_POPUP_TREE_ADD_PART_PAGE_END, _("Add end delimiter for a page") );
    pageSubmenu->Enable( ID6_POPUP_TREE_ADD_PART_PAGE_END, false );
    pageSubmenu->Append( ID6_POPUP_TREE_REMOVE_PART_PAGE_START, _("Remove start delimiter for a page") );
    pageSubmenu->Enable( ID6_POPUP_TREE_REMOVE_PART_PAGE_START, false );
    pageSubmenu->Append( ID6_POPUP_TREE_REMOVE_PART_PAGE_END, _("Remove end delimiter for a page") );
    pageSubmenu->Enable( ID6_POPUP_TREE_REMOVE_PART_PAGE_END, false );
    bookSubmenu->AppendSubMenu( pageSubmenu,  _("Part page") );
    popup.AppendSubMenu( bookSubmenu, _("Book") );
    // Collations
    popup.AppendSeparator( );
    popup.Append( ID6_POPUP_TREE_ADD_COLLATION, _("Add collation") );   
    popup.Enable( ID6_POPUP_TREE_ADD_COLLATION, false );
    // Collation submenu
    wxMenu *colSubmenu = new wxMenu(); 
    colSubmenu->Append( ID6_POPUP_TREE_ADD_COLLATION_PART, _("Add part to collation") );
    colSubmenu->Enable( ID6_POPUP_TREE_ADD_COLLATION_PART, false );
    popup.AppendSubMenu( colSubmenu, _("Collation") );
    
    int itemType = GetType( event.GetItem() );

    // first menu according to wxTreeItemId
    if ( itemType == CMP_TREE_BOOKS )
    {
        popup.Enable( ID6_POPUP_TREE_ADD_BOOK, true );
    }
    else if ( itemType == CMP_TREE_BOOK_PARTS ) {
        // we have click Parts and want to add a part to a book
        popup.Enable( ID6_POPUP_TREE_ADD_PART, true );
        popup.Enable( ID6_POPUP_TREE_ASSEMBLE, true );
    }
    else if ( itemType == CMP_TREE_BOOK_AXFILE ) {
        // we have click Aruspix file of a book and want to add it to a part
        popup.Enable( ID6_POPUP_TREE_ADD_AXFILE, true );
    }
    else if ( itemType == CMP_TREE_COLLATIONS )
    {
        popup.Enable( ID6_POPUP_TREE_ADD_COLLATION, true );
    }
    else if ( itemType == CMP_TREE_COLLATION ) {
        // we have click a collation and want to add a part to a it
        popup.Enable( ID6_POPUP_TREE_ADD_COLLATION_PART, true );
    }
    
    // bitfields would be better...
    if ( (itemType == CMP_TREE_BOOK_PART_PAGE) || (itemType == CMP_TREE_BOOK_PART_PAGE_END)  ) {
        // we have click a page and want to add a start delimiter
        popup.Enable( ID6_POPUP_TREE_ADD_PART_PAGE_START, true );
    }
    if ( (itemType == CMP_TREE_BOOK_PART_PAGE) || (itemType == CMP_TREE_BOOK_PART_PAGE_START)  ) {
        // we have click a page and want to add a end delimiter
        popup.Enable( ID6_POPUP_TREE_ADD_PART_PAGE_END, true );
    }
    if ( (itemType == CMP_TREE_BOOK_PART_PAGE_START) || (itemType == CMP_TREE_BOOK_PART_PAGE_START_END)  ) {
        // we have click a page and want to remove a start delimiter
        popup.Enable( ID6_POPUP_TREE_REMOVE_PART_PAGE_START, true );
    }
    if ( (itemType == CMP_TREE_BOOK_PART_PAGE_END) || (itemType == CMP_TREE_BOOK_PART_PAGE_START_END)  ) {
        // we have click a page and want to remove a end delimiter
        popup.Enable( ID6_POPUP_TREE_REMOVE_PART_PAGE_END, true );
    }
    
    wxPoint clientpt = event.GetPoint();
    wxPoint screenpt = ClientToScreen(clientpt);    
    PopupMenu(&popup, clientpt );
}

	#endif // AX_COMPARISON
#endif // AX_RECOGNITION
