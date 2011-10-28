/////////////////////////////////////////////////////////////////////////////
// Name:        supbookctrl.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __supbookctrl_H__
#define __supbookctrl_H__

#ifdef AX_SUPERIMPOSITION

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "supbookctrl.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "sup_wdr.h"

#include "app/axctrl.h"

class SupEnv;
class SupFile;
class SupBookFile;
class SupBookCtrl;

class AxBookFileItem;


//----------------------------------------------------------------------------
// SupBookDataDlg
//----------------------------------------------------------------------------

class SupBookDataDlg: public wxDialog
{
public:
    // constructors and destructors
    SupBookDataDlg( wxWindow *parent, wxWindowID id, const wxString &title,
        SupBookFile *supBookFile );
    
        wxTextCtrl* Axfiles()  { return (wxTextCtrl*) FindWindow( ID2_BOOK_AXFILES ); }
    wxTextCtrl* Images1()  { return (wxTextCtrl*) FindWindow( ID2_BOOK_IMAGES1 ); }
	wxTextCtrl* Images2()  { return (wxTextCtrl*) FindWindow( ID2_BOOK_IMAGES2 ); }
    wxTextCtrl* Library1()  { return (wxTextCtrl*) FindWindow( ID2_LIBRARY1 ); }
	wxTextCtrl* Library2()  { return (wxTextCtrl*) FindWindow( ID2_LIBRARY2 ); }
    wxTextCtrl* Year()  { return (wxTextCtrl*) FindWindow( ID2_YEAR ); }
    wxTextCtrl* Printer()  { return (wxTextCtrl*) FindWindow( ID2_PRINTER ); }
    wxTextCtrl* Title()  { return (wxTextCtrl*) FindWindow( ID2_TITLE ); }
    wxTextCtrl* Composer()  { return (wxTextCtrl*) FindWindow( ID2_COMPOSER ); }
    wxTextCtrl* RISM()  { return (wxTextCtrl*) FindWindow( ID2_RISM ); }
    //virtual bool Validate();
    //virtual bool TransferDataToWindow();
    //virtual bool TransferDataFromWindow();
    
private:
        SupBookFile *m_supBookFile;
    bool m_loadAxfiles;
    bool m_loadImages1;
	bool m_loadImages2;
    
private:
        void OnBrowseAxfiles( wxCommandEvent &event );
    void OnBrowseImages1( wxCommandEvent &event );
	void OnBrowseImages2( wxCommandEvent &event );
    void OnOk( wxCommandEvent &event );
    //void OnCancel( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// SupBookPanel
//----------------------------------------------------------------------------

class SupBookPanel: public wxPanel
{
public:
    // constructors and destructors
    SupBookPanel( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    
        SupBookCtrl* GetTree()  { return (SupBookCtrl*) FindWindow( ID2_TREEBOOK ); }
    wxStaticBitmap* GetPreview()  { return (wxStaticBitmap*) FindWindow( ID2_PREVIEW ); }
    wxCheckBox* GetPreviewCB()  { return (wxCheckBox*) FindWindow( ID2_CB_PREVIEW ); }
    
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
// SupBookCtrl
//----------------------------------------------------------------------------

class SupBookCtrl: public AxCtrl
{
public:
    // constructors and destructors
    SupBookCtrl( wxWindow *parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0 );
	virtual ~SupBookCtrl();
    
        void SetBookFile( SupBookFile *bookFile ) { m_supBookFilePtr = bookFile; }
    void SetEnv( SupEnv *supEnv ) { m_supEnvPtr = supEnv; }
	void SetBookPanel( SupBookPanel *supBookPanel ) { m_supBookPanelPtr = supBookPanel; }
    void Build( );
    void Update( );
	virtual void SaveDisplay( );
	virtual void LoadDisplay( );
    
private:
        SupBookFile *m_supBookFilePtr;
    SupEnv *m_supEnvPtr;
	SupBookPanel *m_supBookPanelPtr;
    // id
    wxTreeItemId m_rootId;
    wxTreeItemId m_bookId;
    wxTreeItemId m_axFilesId;
    wxTreeItemId m_imgFilesId1;
	wxTreeItemId m_imgFilesId2;
    
    
    
private:
        void OnSelection( wxTreeEvent &event );
    void OnActivate( wxTreeEvent &event );
	void OnAxDesactivate( wxCommandEvent &event );
    void OnAxRemove( wxCommandEvent &event );
    void OnAxDelete( wxCommandEvent &event );
    void OnImgDesactivate( wxCommandEvent &event );
    void OnImgRemove( wxCommandEvent &event );
    void OnBookEdit( wxCommandEvent &event );
    void OnBook( wxCommandEvent &event );
    void OnMenu( wxTreeEvent &event );

private:
    DECLARE_EVENT_TABLE()
};

#endif //AX_SUPERIMPOSITION

#endif
