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
    
private:
        
protected:
    void SetTypeImages( wxTreeItemId id, int type );
    bool SelectionIsChildOf( wxTreeItemId id );
    bool ItemIsChildOf( wxTreeItemId id, wxTreeItemId child );
    
private:
    DECLARE_EVENT_TABLE()
};

#endif
