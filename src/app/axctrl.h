/////////////////////////////////////////////////////////////////////////////
// Name:        axctrl.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __axctrl_H__
#define __axctrl_H__

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "axctrl.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "rec_wdr.h"

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


// WDR: class declarations

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
    
    // WDR: method declarations for AxCtrl
	virtual void SaveDisplay( ) {};
	virtual void LoadDisplay( ) {};
    
private:
    // WDR: member variable declarations for AxCtrl
    
protected:
    void SetTypeImages( wxTreeItemId id, int type );
    bool SelectionIsChildOf( wxTreeItemId id );
    bool ItemIsChildOf( wxTreeItemId id, wxTreeItemId child );
    // WDR: handler declarations for AxCtrl

private:
    DECLARE_EVENT_TABLE()
};

#endif
