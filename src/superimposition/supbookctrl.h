/////////////////////////////////////////////////////////////////////////////
// Name:        supbookctrl.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
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

// WDR: class declarations

//----------------------------------------------------------------------------
// SupBookDataDlg
//----------------------------------------------------------------------------

class SupBookDataDlg: public wxDialog
{
public:
    // constructors and destructors
    SupBookDataDlg( wxWindow *parent, wxWindowID id, const wxString &title,
        SupBookFile *supBookFile );
    
    // WDR: method declarations for SupBookDataDlg
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
    // WDR: member variable declarations for SupBookDataDlg
    SupBookFile *m_supBookFile;
    bool m_loadAxfiles;
    bool m_loadImages1;
	bool m_loadImages2;
    
private:
    // WDR: handler declarations for SupBookDataDlg
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
    
    // WDR: method declarations for SupBookPanel
    SupBookCtrl* GetTree()  { return (SupBookCtrl*) FindWindow( ID2_TREEBOOK ); }
    wxStaticBitmap* GetPreview()  { return (wxStaticBitmap*) FindWindow( ID2_PREVIEW ); }
    wxCheckBox* GetPreviewCB()  { return (wxCheckBox*) FindWindow( ID2_CB_PREVIEW ); }
    
	void Preview( wxString filename );
	
	
private:
    // WDR: member variable declarations for SupBookPanel
	wxString m_filename;
	wxBitmap m_preview, m_nopreview, m_loading;
	bool m_show_preview;
    
private:
	void LoadPreview( );
	void UpdatePreview( );
    // WDR: handler declarations for SupBookPanel
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
    
    // WDR: method declarations for SupBookCtrl
    void SetBookFile( SupBookFile *bookFile ) { m_supBookFilePtr = bookFile; }
    void SetEnv( SupEnv *supEnv ) { m_supEnvPtr = supEnv; }
	void SetBookPanel( SupBookPanel *supBookPanel ) { m_supBookPanelPtr = supBookPanel; }
    void Build( );
    void Update( );
	virtual void SaveDisplay( );
	virtual void LoadDisplay( );
    
private:
    // WDR: member variable declarations for SupBookCtrl
    SupBookFile *m_supBookFilePtr;
    SupEnv *m_supEnvPtr;
	SupBookPanel *m_supBookPanelPtr;
    // id
    wxTreeItemId m_rootId;
    wxTreeItemId m_bookId;
    wxTreeItemId m_axFilesId;
    wxTreeItemId m_imgFilesId;
    
    
    
private:
    // WDR: handler declarations for SupBookCtrl
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
