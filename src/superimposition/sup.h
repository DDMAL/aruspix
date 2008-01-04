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
#include "app/aximagecontroller.h"
#include "app/axscrolledwindow.h"
#include "im/imoperator.h"

#include "sup_wdr.h"

enum
{
	ID2_CONTROLLER1 = ENV_IDS_LOCAL_SUP,
	ID2_CONTROLLER2,
	ID2_VIEW1,
	ID2_VIEW2
};

struct _imImage;
class SupImController;
class ProgressDlg;

// WDR: class declarations

//----------------------------------------------------------------------------
// SupFile
//----------------------------------------------------------------------------

class SupFile: public ImOperator 
{
public:
    // constructors and destructors
    SupFile( wxString path = "", wxString shortname = "", ProgressDlg *dlg  = NULL );
    virtual ~SupFile();
    
    // WDR: method declarations for SupFile
    bool Superimpose(const SupImController *imController1, 
        const SupImController *imController2, const wxString filename );
    //int GetError( ) { return m_error; }
    
private:
    // WDR: member variable declarations for SupFile
    void DistByCorrelation(_imImage *image1, _imImage *image2,
                                wxSize window, int *decalageX, int *decalageY);
    //void DistByCorrelationFFT(const _imImage *image1, const _imImage *image2,
    //                            wxSize window, int *decalageX, int *decalageY);
    wxPoint CalcPositionAfterRotation( wxPoint point , float rot_alpha, 
                                int w, int h, int new_w, int new_h);
    bool Terminate( int code = 0, ... );
    //void SwapImages( _imImage **image1, _imImage **image2 );
    //void ImageDestroy( _imImage **image );*/

private:
    // WDR: handler declarations for SupFile
    //ProgressDlg *m_progressDlg;
    //int m_error;

    //_imImage *m_imTmp;
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



//----------------------------------------------------------------------------
// SupPanel
//----------------------------------------------------------------------------

class SupPanel: public wxPanel
{
public:
    // constructors and destructors
    SupPanel( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    
    // WDR: method declarations for SupPanel
    wxSplitterWindow* GetSplitter1()  { return (wxSplitterWindow*) FindWindow( ID2_SPLITTER1 ); }
    AxImageController* GetController2()  { return (AxImageController*) FindWindow( ID2_CONTROLLER2 ); }
    AxImageController* GetController1()  { return (AxImageController*) FindWindow( ID2_CONTROLLER1 ); }
    
private:
    // WDR: member variable declarations for SupPanel
    
private:
    // WDR: handler declarations for SupPanel

private:
    DECLARE_EVENT_TABLE()
};

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

private:
    // WDR: member variable declarations for SupEnv
    SupImController *m_imControl1Ptr;
    SupImController *m_imControl2Ptr;
    AxScrolledWindow *m_view1Ptr;
    AxScrolledWindow *m_view2Ptr;

public:
    static int s_segmentSize;
    static int s_split_x;
    static int s_split_y;
    static int s_corr_x;
    static int s_corr_y;
    static int s_interpolation;
    static bool s_filter1;
    static bool s_filter2;
    
private:
    // WDR: handler declarations for SupEnv
    void OnPrevious( wxCommandEvent &event );
    void OnNext( wxCommandEvent &event );
    void OnGoto( wxCommandEvent &event );
    void OnClose( wxCommandEvent &event );
    void OnNextBoth( wxCommandEvent &event );
    void OnPreviousBoth( wxCommandEvent &event );
    void OnPaste( wxCommandEvent &event );
    void OnCopy( wxCommandEvent &event );
    void OnCut( wxCommandEvent &event );
    void OnOpen( wxCommandEvent &event );
    void OnPutPoints( wxCommandEvent &event );
    void OnSuperimpose( wxCommandEvent &event );
    void OnUpdateUI( wxUpdateUIEvent &event );

private:
    DECLARE_DYNAMIC_CLASS(SupEnv)

    DECLARE_EVENT_TABLE()
};

#endif //AX_SUPERIMPOSITION

#endif

