/////////////////////////////////////////////////////////////////////////////
// Name:        axctrl.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && ! defined(__APPLE__)
#pragma implementation "axctrl.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

//#include "wx/file.h"
//#include "wx/filename.h"
#include "wx/imaglist.h"

#include "axctrl.h"
#include "axapp.h"



//----------------------------------------------------------------------------
// AxCtrl
//----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(AxCtrl,wxGenericTreeCtrl)
END_EVENT_TABLE()

AxCtrl::AxCtrl( wxWindow *parent, wxWindowID id,
                          const wxPoint &position, const wxSize& size, long style ) :
wxGenericTreeCtrl( parent, id,  position, size, wxTR_HAS_BUTTONS | wxSUNKEN_BORDER | wxTR_MULTIPLE )
{ 
    wxImageList *images = new wxImageList(16, 16, true, NB_IMAGES );
    
    // change NB_IMAGES if more types...
    images->Add(wxBitmap( wxGetApp().m_resourcesPath + "/tree/folder_grey.png", wxBITMAP_TYPE_PNG ) );
    images->Add(wxBitmap( wxGetApp().m_resourcesPath + "/tree/folder_blue.png", wxBITMAP_TYPE_PNG ) );
    images->Add(wxBitmap( wxGetApp().m_resourcesPath + "/tree/folder_grey_open.png", wxBITMAP_TYPE_PNG ) );
    images->Add(wxBitmap( wxGetApp().m_resourcesPath + "/tree/folder_blue_open.png", wxBITMAP_TYPE_PNG ) );
    images->Add(wxBitmap( wxGetApp().m_resourcesPath + "/tree/axz_grey.png", wxBITMAP_TYPE_PNG ) );
    images->Add(wxBitmap( wxGetApp().m_resourcesPath + "/tree/axz.png", wxBITMAP_TYPE_PNG ) );
    images->Add(wxBitmap( wxGetApp().m_resourcesPath + "/tree/axz_ok_grey.png", wxBITMAP_TYPE_PNG ) );
    images->Add(wxBitmap( wxGetApp().m_resourcesPath + "/tree/axz_ok.png", wxBITMAP_TYPE_PNG ) );
    images->Add(wxBitmap( wxGetApp().m_resourcesPath + "/tree/axtyp_grey.png", wxBITMAP_TYPE_PNG ) );
    images->Add(wxBitmap( wxGetApp().m_resourcesPath + "/tree/axtyp.png", wxBITMAP_TYPE_PNG ) );
    images->Add(wxBitmap( wxGetApp().m_resourcesPath + "/tree/axmus_grey.png", wxBITMAP_TYPE_PNG ) );
    images->Add(wxBitmap( wxGetApp().m_resourcesPath + "/tree/axmus.png", wxBITMAP_TYPE_PNG ) );
    images->Add(wxBitmap( wxGetApp().m_resourcesPath + "/tree/doc_grey.png", wxBITMAP_TYPE_PNG ) );
    images->Add(wxBitmap( wxGetApp().m_resourcesPath + "/tree/doc.png", wxBITMAP_TYPE_PNG ) );
    images->Add(wxBitmap( wxGetApp().m_resourcesPath + "/tree/text_grey.png", wxBITMAP_TYPE_PNG ) );
    images->Add(wxBitmap( wxGetApp().m_resourcesPath + "/tree/text.png", wxBITMAP_TYPE_PNG ) );
    
    AssignImageList(images);
}

AxCtrl::~AxCtrl( )
{
	this->SaveDisplay( );
}

void AxCtrl::SetTypeImages( wxTreeItemId id, int type )
{
    if ( !id.IsOk() || GetImageList()->GetImageCount() < NB_IMAGES )
        return;
    
    if ( type == IMG_FOLDER )
    {
        SetItemImage( id, IMG_FOLDER, wxTreeItemIcon_Normal );
        SetItemImage( id, IMG_FOLDER_S, wxTreeItemIcon_Selected );
        SetItemImage( id, IMG_FOLDER_EX, wxTreeItemIcon_Expanded );
        SetItemImage( id, IMG_FOLDER_S_EX, wxTreeItemIcon_SelectedExpanded ); 
    }
}

bool AxCtrl::ItemIsChildOf( wxTreeItemId id, wxTreeItemId child )
{
    if ( !id.IsOk() || !child.IsOk() )
        return false;

    if ( !child.IsOk() || child == GetRootItem() )
        return false;
        
    while ( GetItemParent( child ) != id )
    {
        child = GetItemParent( child );
        if ( child == GetRootItem() )
            return false;
    }
    return true;
} 

bool AxCtrl::SelectionIsChildOf( wxTreeItemId id )
{
    if ( !id.IsOk() )
        return false;

    wxArrayTreeItemIds array;
    size_t count = GetSelections(array);
    for ( size_t n = 0; n < count; n++ )
    {
        wxTreeItemId child = array.Item(n);
        if ( !child.IsOk() || child == GetRootItem() )
            return false;
        
        while ( GetItemParent( child ) != id )
        {
            child = GetItemParent( child );
            if ( child == GetRootItem() )
                return false;
        
        }
    }
    return true;
}


