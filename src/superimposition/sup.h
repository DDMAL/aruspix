/////////////////////////////////////////////////////////////////////////////
// Name:        sup.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __SUP_H__
#define __SUP_H__

#ifdef AX_SUPERIMPOSITION

#ifdef __GNUG__
    #pragma interface "sup.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "app/axapp.h"
#include "app/axenv.h"

#include "sup_wdr.h"

enum
{
	ID2_CONTROLLER1 = ENV_IDS_LOCAL_SUP,
	ID2_CONTROLLER2,
	ID2_VIEW1,
	ID2_VIEW2,
	ID2_SRC_CONTROLLER1,
	ID2_SRC_CONTROLLER2,
	ID2_SRC_VIEW1,
	ID2_SRC_VIEW2,

    ID2_POPUP_TREE_LOAD,
    ID2_POPUP_TREE_SUP,
    ID2_POPUP_TREE_BOOK_EDIT,
    ID2_POPUP_TREE_IMG_REMOVE1,
    ID2_POPUP_TREE_IMG_DESACTIVATE1,
    ID2_POPUP_TREE_IMG_REMOVE2,
    ID2_POPUP_TREE_IMG_DESACTIVATE2,
    ID2_POPUP_TREE_AX_REMOVE,
    ID2_POPUP_TREE_AX_DESACTIVATE,
    ID2_POPUP_TREE_AX_DELETE
};

struct _imImage;

class AxProgressDlg;
class AxImageController;

class SupImController;
class SupImWindow;
class SupImSrcWindow;
class SupFile;
class SupBookCtrl;
class SupBookFile;
class SupBookPanel;
class SupEnv;

// WDR: class declarations

/*
//----------------------------------------------------------------------------
// SupOldFile
//----------------------------------------------------------------------------

class SupOldFile: public ImOperator 
{
public:
    // constructors and destructors
    SupOldFile( wxString path = "", wxString shortname = "", AxProgressDlg *dlg  = NULL );
    virtual ~SupOldFile();
    
    // WDR: method declarations for SupOldFile
    bool Superimpose(const SupImController *imController1, 
        const SupImController *imController2, const wxString filename );
    //int GetError( ) { return m_error; }
    
private:
    // WDR: member variable declarations for SupOldFile
    void DistByCorrelation(_imImage *image1, _imImage *image2,
                                wxSize window, int *decalageX, int *decalageY);
    //void DistByCorrelationFFT(const _imImage *image1, const _imImage *image2,
    //                            wxSize window, int *decalageX, int *decalageY);
    wxPoint CalcPositionAfterRotation( wxPoint point , float rot_alpha, 
                                int w, int h, int new_w, int new_h);
    bool Terminate( int code = 0, ... );
    //void SwapImages( _imImage **image1, _imImage **image2 );
    //void ImageDestroy( _imImage **image );

private:
    // WDR: handler declarations for SupOldFile
    //AxProgressDlg *m_progressDlg;
    //int m_error;

    //_imImage *m_opImTmp1;
    _imImage *m_im1;
    _imImage *m_im2;
    _imImage *m_im2_original;
    _imImage *m_im_1_2;
    _imImage *m_im3;
    _imImage *m_s_im1;
    _imImage *m_s_im2;
    _imImage *m_new_im2;
    _imImage *m_selection;

};
*/

/*
//----------------------------------------------------------------------------
// SupImController
//----------------------------------------------------------------------------

class SupImController: public AxImageController
{
public:
    // constructors and destructors
    SupImController();
    SupImController( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER, int flags = CONTROLLER_ALL );
    virtual ~SupImController();
    
    // WDR: method declarations for SupImController
    virtual void CloseDraggingSelection(wxPoint start, wxPoint end);
    
private:
    // WDR: member variable declarations for SupImController
    int m_selectCounter;

public:
    wxPoint m_points[4];
    
private:
    // WDR: handler declarations for SupImController

private:
    DECLARE_CLASS(SupImController)
    DECLARE_EVENT_TABLE()
};
*/

DECLARE_EVENT_TYPE(AX_PUT_POINTS_EVT, -1)

//----------------------------------------------------------------------------
// SupEnv
//----------------------------------------------------------------------------

class SupEnv: public AxEnv
{
public:
    // constructors and destructors
    SupEnv::SupEnv();
    virtual SupEnv::~SupEnv();
    
    // WDR: method declarations for SupEnv
    virtual void LoadWindow();
    static void LoadConfig();
    static void SaveConfig();
    virtual void ParseCmd( wxCmdLineParser *parser );
    virtual void UpdateTitle( );
	virtual void RealizeToolbar( );
    virtual void Open( wxString filename, int type );
    virtual bool CloseAll( );
    //
    bool ResetFile(); // must be called when new file is created or opened
    bool ResetBookFile(); // must be called when new file is created or opened
    void OpenFile( wxString filename, int type );
    void OpenBookFile( wxString filename );
    void UpdateViews( int flags );
	
private:
    // WDR: member variable declarations for SupEnv
    SupImController *m_imControl1Ptr;
    SupImController *m_imControl2Ptr;	
    SupImWindow *m_imView1Ptr;
    SupImWindow *m_imView2Ptr;
	// sources for displaying results
	AxImageController *m_srcControl1Ptr;
    AxImageController *m_srcControl2Ptr;
    SupImSrcWindow *m_srcView1Ptr;
    SupImSrcWindow *m_srcView2Ptr;
	
    wxSplitterWindow *m_bookSplitterPtr;
	wxSplitterWindow *m_pageSplitterPtr;
	wxSplitterWindow *m_srcSplitterPtr;
    // superimposition elements
    SupFile *m_supFilePtr;
    // 
    SupBookCtrl *m_supBookPtr; // tree
    SupBookFile *m_supBookFilePtr;
    SupBookPanel *m_supBookPanelPtr;

public:
    static int s_segmentSize;
    static int s_split_x;
    static int s_split_y;
    static int s_corr_x;
    static int s_corr_y;
    static int s_interpolation;
    static bool s_filter1;
    static bool s_filter2;
    // sash
    static int s_book_sash;
	static int s_page_viewer_sash;
	static int s_page_images_sash;
	// tree
    static bool s_expand_root;
	static bool s_expand_book;
    static bool s_expand_img1;
	static bool s_expand_img2;   
	static bool s_expand_ax;
    
private:
    // WDR: handler declarations for SupEnv
    void OnBookEdit( wxCommandEvent &event );
    void OnBookOptimize( wxCommandEvent &event );
    void OnBookSuperimpose( wxCommandEvent &event );
    void OnBookLoad( wxCommandEvent &event );
    void OnCloseBook( wxCommandEvent &event );
    void OnOpenBook( wxCommandEvent &event );
    void OnNewBook( wxCommandEvent &event );
    void OnSaveBookAs( wxCommandEvent &event );
    void OnSaveBook( wxCommandEvent &event );
    void OnNew( wxCommandEvent &event );
    void OnSave( wxCommandEvent &event );
    void OnSaveAs( wxCommandEvent &event );
    void OnClose( wxCommandEvent &event );
    void OnExportImage( wxCommandEvent &event );
    void OnAdjust( wxCommandEvent &event );
    void OnZoom( wxCommandEvent &event );
    void OnPrevious( wxCommandEvent &event );
    void OnNext( wxCommandEvent &event );
    void OnGoto( wxCommandEvent &event );
    void OnNextBoth( wxCommandEvent &event );
    void OnPreviousBoth( wxCommandEvent &event );
    void OnPaste( wxCommandEvent &event );
    void OnCopy( wxCommandEvent &event );
    void OnCut( wxCommandEvent &event );
    void OnPutPoints( wxCommandEvent &event );
    void OnRun( wxCommandEvent &event );
    void OnUpdateUI( wxUpdateUIEvent &event );
	// custom event
	void OnEndPutPoints( wxCommandEvent &event );

private:
    DECLARE_DYNAMIC_CLASS(SupEnv)

    DECLARE_EVENT_TABLE()
};

#endif //AX_SUPERIMPOSITION

#endif

