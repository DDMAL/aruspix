/////////////////////////////////////////////////////////////////////////////
// Name:        recbookctrl.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __recbookctrl_H__
#define __recbookctrl_H__

#ifdef AX_RECOGNITION

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "recbookctrl.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "rec_wdr.h"

#include "app/axctrl.h"

class RecEnv;
class RecBookFile;
class RecBookCtrl;

class AxBookFileItem;


//----------------------------------------------------------------------------
// RecBookDataDlg
//----------------------------------------------------------------------------

class RecBookDataDlg: public wxDialog
{
public:
    // constructors and destructors
    RecBookDataDlg( wxWindow *parent, wxWindowID id, const wxString &title,
        RecBookFile *recBookFile );
    
        wxTextCtrl* Axfiles()  { return (wxTextCtrl*) FindWindow( ID4_BOOK_AXFILES ); }
    wxTextCtrl* Images()  { return (wxTextCtrl*) FindWindow( ID4_BOOK_IMAGES ); }
    wxTextCtrl* Library()  { return (wxTextCtrl*) FindWindow( ID4_LIBRARY ); }
    wxTextCtrl* Year()  { return (wxTextCtrl*) FindWindow( ID4_YEAR ); }
    wxTextCtrl* Printer()  { return (wxTextCtrl*) FindWindow( ID4_PRINTER ); }
    wxTextCtrl* Title()  { return (wxTextCtrl*) FindWindow( ID4_TITLE ); }
    wxTextCtrl* Composer()  { return (wxTextCtrl*) FindWindow( ID4_COMPOSER ); }
    wxTextCtrl* RISM()  { return (wxTextCtrl*) FindWindow( ID4_RISM ); }
    wxChoice* PageBinarizationMethod() { return (wxChoice*) FindWindow ( ID4_PAGE_BIN_CHOICE ); }
	wxSpinCtrl* BinarizationRegionSize() { return (wxSpinCtrl*) FindWindow ( ID4_RGN_SIZE_SPINCTRL ); }
	wxCheckBox* BinarizationSelect() { return (wxCheckBox*) FindWindow ( ID4_BIN_SELECT_CHECKBOX ); }
	//virtual bool Validate();
    //virtual bool TransferDataToWindow();
    //virtual bool TransferDataFromWindow();
    
private:
        RecBookFile *m_recBookFile;
    bool m_loadAxfiles;
    bool m_loadImages;
    
private:
        void OnBrowseAxfiles( wxCommandEvent &event );
    void OnBrowseImages( wxCommandEvent &event );
    void OnOk( wxCommandEvent &event );
    //void OnCancel( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// RecBookPanel
//----------------------------------------------------------------------------

class RecBookPanel: public wxPanel
{
public:
    // constructors and destructors
    RecBookPanel( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    
        RecBookCtrl* GetTree()  { return (RecBookCtrl*) FindWindow( ID4_TREEBOOK ); }
    wxStaticBitmap* GetPreview()  { return (wxStaticBitmap*) FindWindow( ID4_PREVIEW ); }
    wxCheckBox* GetPreviewCB()  { return (wxCheckBox*) FindWindow( ID4_CB_PREVIEW ); }
    
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
// RecBookCtrl
//----------------------------------------------------------------------------

class RecBookCtrl: public AxCtrl
{
public:
    // constructors and destructors
    RecBookCtrl( wxWindow *parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0 );
	virtual ~RecBookCtrl();
    
        void SetBookFile( RecBookFile *bookFile ) { m_recBookFilePtr = bookFile; }
    void SetEnv( RecEnv *recEnv ) { m_recEnvPtr = recEnv; }
	void SetBookPanel( RecBookPanel *recBookPanel ) { m_recBookPanelPtr = recBookPanel; }
    void Build( );
    void Update( );
	virtual void SaveDisplay( );
	virtual void LoadDisplay( );
    
private:
        RecBookFile *m_recBookFilePtr;
    RecEnv *m_recEnvPtr;
	RecBookPanel *m_recBookPanelPtr;
    // id
    wxTreeItemId m_rootId;
    wxTreeItemId m_bookId;
    wxTreeItemId m_axFilesId;
    wxTreeItemId m_imgFilesId;
	wxTreeItemId m_optFilesId;
    
    
    
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

#endif //AX_RECOGNITION

#endif
