/////////////////////////////////////////////////////////////////////////////
// Name:        axctrl.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __axctrl_H__
#define __axctrl_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/treectrl.h>

#ifndef wxGenericTreeCtrl
#define wxGenericTreeCtrl wxTreeCtrl
//#define sm_classwxTreeCtrl sm_classwxGenericTreeCtrl
#endif

#define NB_IMAGES 16

enum 
{
    IMG_FOLDER = 0,
	IMG_FOLDER_S,
	IMG_FOLDER_EX,
	IMG_FOLDER_S_EX,
    IMG_AXZ,
	IMG_AXZ_S,
    IMG_AXZ_OK,
	IMG_AXZ_OK_S,
    IMG_AXTYP,
	IMG_AXTYP_S,
    IMG_AXMUS,
	IMG_AXMUS_S,
    IMG_DOC,
	IMG_DOC_S,
    IMG_TEXT,
	IMG_TEXT_S
};


class AxTreeItem;
WX_DECLARE_OBJARRAY( AxTreeItem, ArrayOfTreeItems );

//----------------------------------------------------------------------------
// AxTreeItem
//----------------------------------------------------------------------------

/**
 * This class hold an tree item id together with a wxObject (or two) associated with the row.
 */
class AxTreeItem: public wxObject
{
public:
    // constructors and destructors
    AxTreeItem();
    AxTreeItem( wxTreeItemId id, wxObject *object, wxObject *secondaryObject = NULL );
    virtual ~AxTreeItem() {};
    
public:
    wxTreeItemId m_id;
    /** A pointer to an object */
	wxObject *m_object;
    /** A pointer to an object when we need a second one */
    wxObject *m_secondaryObject;
    
private:
    
	DECLARE_DYNAMIC_CLASS(AxTreeItem)
};



//----------------------------------------------------------------------------
// AxCtrl
//----------------------------------------------------------------------------

class AxCtrl: public wxGenericTreeCtrl
{
public:
    // constructors and destructors
    AxCtrl( wxWindow *parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0 );
	virtual ~AxCtrl();
    
    virtual void SaveDisplay( ) {};
	virtual void LoadDisplay( ) {};
    
protected:
    wxObject *GetObject( wxTreeItemId, bool secondary = false );
    
private:
        
public:
    
protected:
    void SetTypeImages( wxTreeItemId id, int type );
    bool SelectionIsChildOf( wxTreeItemId id );
    bool ItemIsChildOf( wxTreeItemId id, wxTreeItemId child );
    
    ArrayOfTreeItems m_axItems;
    
private:
    DECLARE_EVENT_TABLE()
};

#endif
