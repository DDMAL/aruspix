/////////////////////////////////////////////////////////////////////////////
// Name:        cmpctrl.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __cmpctrl_H__
#define __cmpctrl_H__

#ifdef AX_RECOGNITION
	#ifdef AX_COMPARISON

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "cmpctrl.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "cmp_wdr.h"

#include "app/axctrl.h"

class CmpEnv;
class CmpFile;
class CmpCtrl;
class CmpBookItem;
class CmpCollation;

class BookFile;
class AxProgressDlg;

// WDR: class declarations

/*
//----------------------------------------------------------------------------
// CmpDataDlg
//----------------------------------------------------------------------------

class CmpDataDlg: public wxDialog
{
public:
    // constructors and destructors
    CmpDataDlg( wxWindow *parent, wxWindowID id, const wxString &title,
        RecBookFile *recBookFile );
    
    // WDR: method declarations for CmpDataDlg
    wxTextCtrl* Axfiles()  { return (wxTextCtrl*) FindWindow( ID6_BOOK_AXFILES ); }
    wxTextCtrl* Images()  { return (wxTextCtrl*) FindWindow( ID6_BOOK_IMAGES ); }
    wxTextCtrl* Library()  { return (wxTextCtrl*) FindWindow( ID6_LIBRARY ); }
    wxTextCtrl* Year()  { return (wxTextCtrl*) FindWindow( ID6_YEAR ); }
    wxTextCtrl* Printer()  { return (wxTextCtrl*) FindWindow( ID6_PRINTER ); }
    wxTextCtrl* Title()  { return (wxTextCtrl*) FindWindow( ID6_TITLE ); }
    wxTextCtrl* Composer()  { return (wxTextCtrl*) FindWindow( ID6_COMPOSER ); }
    wxTextCtrl* RISM()  { return (wxTextCtrl*) FindWindow( ID6_RISM ); }
    //virtual bool Validate();
    //virtual bool TransferDataToWindow();
    //virtual bool TransferDataFromWindow();
    
private:
    // WDR: member variable declarations for CmpDataDlg
    RecBookFile *m_recBookFile;
    bool m_loadAxfiles;
    bool m_loadImages;
    
private:
    // WDR: handler declarations for CmpDataDlg
    void OnBrowseAxfiles( wxCommandEvent &event );
    void OnBrowseImages( wxCommandEvent &event );
    void OnOk( wxCommandEvent &event );
    //void OnCancel( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};
*/

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
    
    // WDR: method declarations for CmpCtrlPanel
    CmpCtrl* GetTree()  { return (CmpCtrl*) FindWindow( ID6_TREEBOOK ); }
    wxStaticBitmap* GetPreview()  { return (wxStaticBitmap*) FindWindow( ID6_PREVIEW ); }
    wxCheckBox* GetPreviewCB()  { return (wxCheckBox*) FindWindow( ID6_CB_PREVIEW ); }
    
	void Preview( wxString filename );
	
	
private:
    // WDR: member variable declarations for CmpCtrlPanel
	wxString m_filename;
	wxBitmap m_preview, m_nopreview, m_loading;
	bool m_show_preview;
    
private:
	void LoadPreview( );
	void UpdatePreview( );
    // WDR: handler declarations for CmpCtrlPanel
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
    
    // WDR: method declarations for CmpCtrl
    void SetBookFile( CmpFile *cmpFile ) { m_cmpFilePtr = cmpFile; }
    void SetEnv( CmpEnv *cmpEnv ) { m_cmpEnvPtr = cmpEnv; }
	void SetBookPanel( CmpCtrlPanel *cmpCtrlPanel ) { m_cmpCtrlPanelPtr = cmpCtrlPanel; }
    void Build( );
    void Update( );
	virtual void SaveDisplay( );
	virtual void LoadDisplay( );
	//
	bool Collate( wxArrayPtrVoid params, AxProgressDlg *dlg );
	//
	CmpBookItem *GetSelectedBookItem();
	CmpCollation *CmpCtrl::GetSelectedCollation( );
    
private:
    // WDR: member variable declarations for CmpCtrl
    CmpFile *m_cmpFilePtr;
    CmpEnv *m_cmpEnvPtr;
	CmpCtrlPanel *m_cmpCtrlPanelPtr;
    // id
    wxTreeItemId m_rootId;
    wxTreeItemId m_cmpId;
	wxTreeItemId m_booksId;
    
private:
    // WDR: handler declarations for CmpCtrl
    void OnSelection( wxTreeEvent &event );
    void OnActivate( wxTreeEvent &event );
	void OnAxDesactivate( wxCommandEvent &event );
    //void OnAxRemove( wxCommandEvent &event );
    //void OnAxDelete( wxCommandEvent &event );
    //void OnImgDesactivate( wxCommandEvent &event );
    //void OnImgRemove( wxCommandEvent &event );
    void OnBookEdit( wxCommandEvent &event );
    void OnBook( wxCommandEvent &event );
	void OnAssembleParts( wxCommandEvent &event );
    void OnMenu( wxTreeEvent &event );

private:
    DECLARE_EVENT_TABLE()
};

	#endif //AX_COMPARISON
#endif //AX_RECOGNITION

#endif
