/////////////////////////////////////////////////////////////////////////////
// Name:        cmpctrl.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __cmpctrl_H__
#define __cmpctrl_H__

#ifdef AX_RECOGNITION
	#ifdef AX_COMPARISON

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "cmp_wdr.h"

#include "app/axctrl.h"

class CmpEnv;
class CmpFile;
class CmpCtrl;
class CmpBookItem;
class CmpBookPart;
class CmpCollation;

class BookFile;
class AxProgressDlg;

//----------------------------------------------------------------------------
// CmpCtrlPanel
//----------------------------------------------------------------------------

class CmpCtrlPanel: public wxPanel
{
public:
    // constructors and destructors
    CmpCtrlPanel( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    
        CmpCtrl* GetTree()  { return (CmpCtrl*) FindWindow( ID6_TREEBOOK ); }
    wxStaticBitmap* GetPreview()  { return (wxStaticBitmap*) FindWindow( ID6_PREVIEW ); }
    wxCheckBox* GetPreviewCB()  { return (wxCheckBox*) FindWindow( ID6_CB_PREVIEW ); }
    
	void Preview( wxString filename );
	
	
private:
    wxString m_filename;
	wxBitmap m_preview, m_nopreview, m_loading;
	bool m_show_preview;
    
private:
	void LoadPreview( );
	void UpdatePreview( );
        void OnPreview( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// CmpCtrl
//----------------------------------------------------------------------------

class CmpCtrl: public AxCtrl
{
public:
    // constructors and destructors
    CmpCtrl( wxWindow *parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0 );
	virtual ~CmpCtrl();
    
    void SetBookFile( CmpFile *cmpFile ) { m_cmpFilePtr = cmpFile; }
    void SetEnv( CmpEnv *cmpEnv ) { m_cmpEnvPtr = cmpEnv; }
	void SetBookPanel( CmpCtrlPanel *cmpCtrlPanel ) { m_cmpCtrlPanelPtr = cmpCtrlPanel; }
    void Build( );
    void Update( );
    void UpdateParts( CmpBookItem *book );
    void UpdateCollationParts( CmpCollation *collation );
	virtual void SaveDisplay( );
	virtual void LoadDisplay( );
	//
	bool Collate( wxArrayPtrVoid params, AxProgressDlg *dlg );
	//
	CmpBookItem *GetSelectedBookItem();
    
private:
    CmpFile *m_cmpFilePtr;
    CmpEnv *m_cmpEnvPtr;
	CmpCtrlPanel *m_cmpCtrlPanelPtr;
    // id
    wxTreeItemId m_rootId;
    wxTreeItemId m_cmpId;
	wxTreeItemId m_booksId;
    
private:
    /**
     * Item is selected.
     * Loads the preview when available.
     */
    void OnSelection( wxTreeEvent &event );
    /**
     * Item is double clicked or space bar is pressed.
     */
    void OnActivate( wxTreeEvent &event );
    /**
     * Method for event that are processed directly by the CmpEnv.
     */ 
    void OnCmpEvent( wxCommandEvent &event );
    /**
     * Assemble the parts of a CmpBookPart (TODO).
     */
	void OnAssembleParts( wxCommandEvent &event );
    /**
     * Add a CmpPartPage to a CmpBookPart.
     * See CmpEnv::AddAxFile.
     */
    void OnAddAxFile( wxCommandEvent &event );
    /**
     * Add a CmpBookPart to a CmpBookItem.
     * See CmpEnv::AddPart.
     */
    void OnAddPart( wxCommandEvent &event );
    /**
     * Add a start or end delimiter to a CmpPartPage.
     * See CmpEnv::AddPartPageStartEnd.
     */
    void OnAddPartPageStartEnd( wxCommandEvent &event );
    /**
     * Add a CmpCollationPart to a CmpCollation.
     * See CmpEnv::AddCollationPart.
     */
    void OnAddCollationPart( wxCommandEvent &event );
    /**
     * Prepare the popup menu when right click on an item.
     * Uses AxTreeItem::m_type to figure out what menu items to enable.
     */
    void OnMenu( wxTreeEvent &event );

private:
    DECLARE_EVENT_TABLE()
};

	#endif //AX_COMPARISON
#endif //AX_RECOGNITION

#endif
