/////////////////////////////////////////////////////////////////////////////
// Name:        cmp.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __CMP_H__
#define __CMP_H__

#ifdef AX_RECOGNITION
	#ifdef AX_COMPARISON

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "app/axapp.h"
#include "app/axenv.h"

#include "cmp_wdr.h"

enum
{
    ID6_CONTROLLER1 = ENV_IDS_LOCAL_CMP,
    ID6_VIEW1,
	ID6_CONTROLLER2,
    ID6_VIEW2,
    ID6_DISPLAY,
    ID6_MUSWINDOW,
    
    ID6_POPUP_TREE_LOAD,
    ID6_POPUP_TREE_ADD_BOOK,
    ID6_POPUP_TREE_ADD_AXFILE,
    ID6_POPUP_TREE_ADD_PART,
    ID6_POPUP_TREE_ADD_PART_PAGE_START,
    ID6_POPUP_TREE_ADD_PART_PAGE_END,
    ID6_POPUP_TREE_REMOVE_PART_PAGE_START,
    ID6_POPUP_TREE_REMOVE_PART_PAGE_END,
	ID6_POPUP_TREE_ASSEMBLE,
    ID6_POPUP_TREE_ADD_COLLATION,
    ID6_POPUP_TREE_ADD_COLLATION_PART   
};

class CmpImController;
class CmpMusController;
class CmpImWindow;
class CmpMusWindow;

class CmpFile;
class CmpCtrl;
class RecFile;
class CmpCtrlPanel;
class CmpEnv;
class CmpBookItem;
class CmpBookPart;
class CmpPartPage;
class CmpCollation;
class CmpCollationPart;

#define CMP_APP_DEL 1
#define CMP_APP_SUBST 2
#define CMP_APP_INS 3

//----------------------------------------------------------------------------
// CmpEnv
//----------------------------------------------------------------------------

class CmpEnv: public AxEnv
{
public:
    // constructors and destructors
    CmpEnv();
    virtual ~CmpEnv();
    
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
    bool ResetCmpFile(); // must be called when new file is created or opened
    /**
     * Open a collation when double clicking on the tree item
     */
	void ViewCollationPart( CmpCollationPart *collationPart, CmpCollation *collation );
    /**
     * Add a CmpBookPart to a CmpBookItem when right click on the "Parts" subtree.
     * Open a dialog for entering a part name (e.g., Canto)
     */ 
    void AddPart( CmpBookItem *book );
    /**
     * Add a CmpPartPage to a CmpBookPart when right click on a recognized file of a recognition book.
     * Open a dialog for selecting to which part (e.g., Canto) the page has to be added
     */
    void AddAxFile( wxString filename, CmpBookItem *book );
    /**
     * Add a start or end delimiter to a CmpPartPage when right click on a part page in the tree.
     * Open the recognition result, and the delimiter will be added when choosing "Set as delimiter" in "Edit" menu.
     * See CmpEnv::OnSetDelimiter.
     */ 
    void AddPartPageStartEnd( wxString filename, CmpPartPage *page, bool isStart );
    /**
     * Remove a start or end delimiter to a CmpPartPage when right click on a part page in the tree.
     */ 
    void RemovePartPageStartEnd( wxString filename, CmpPartPage *page, bool isStart );
    /**
     * Add a CmpCollationPart to CmpCollation.
     */
    void AddCollationPart( CmpCollation *collation );
    void OpenFile( wxString filename);
    void UpdateViews( int flags );
    CmpCollation *GetCollationPtr() { return m_cmpCollationPtr; }
	CmpCollationPart *GetCollationPartPtr() { return m_cmpCollationPartPtr; }

private:
    wxSplitterWindow *m_bookSplitterPtr;
    wxSplitterWindow *m_pageSplitterPtr;
	wxSplitterWindow *m_srcSplitterPtr;
    // im
    CmpMusController *m_imControlPtr1, *m_imControlPtr2;
    CmpMusWindow *m_imViewPtr1, *m_imViewPtr2;
    // mus    
    CmpMusController *m_musControlPtr;
    CmpMusWindow *m_musViewPtr;
	CmpCollationPart *m_cmpCollationPartPtr;
    CmpCollation *m_cmpCollationPtr;
    // recognition elements - used for selecting delimiters
    RecFile *m_recFilePtr;
    CmpPartPage *m_cmpPartPage;
    bool m_delimiterIsStart;
    // 
    CmpCtrl *m_cmpCtrlPtr; // tree
    CmpFile *m_cmpFilePtr;
    CmpCtrlPanel *m_cmpCtrlPanelPtr;

public:
    static int s_book_sash;
	static int s_view_sash;
	// tree
    static bool s_expand_root;
	static bool s_expand_book;
    static bool s_expand_cmp;
    
private:

    /**
     * Add a CmpBookItem to the CmpFile.
     */
    void OnCmpAddBook( wxCommandEvent &event );
    /**
     * Add a CmpCollation to the CmpFile.
     */
    void OnCmpAddCollation( wxCommandEvent &event );
    void OnCmpLoad( wxCommandEvent &event );
    void OnOpen( wxCommandEvent &event );
    void OnTools( wxCommandEvent &event );
    void OnNew( wxCommandEvent &event );
    void OnSave( wxCommandEvent &event );
    void OnSaveAs( wxCommandEvent &event );
    void OnClose( wxCommandEvent &event );
    void OnAdjust( wxCommandEvent &event );
    void OnZoom( wxCommandEvent &event );
    void OnGoto( wxCommandEvent &event );
    void OnPrevious( wxCommandEvent &event );
    void OnNext( wxCommandEvent &event );
    void OnPaste( wxCommandEvent &event );
    void OnCopy( wxCommandEvent &event );
    void OnCut( wxCommandEvent &event );
    void OnUndo( wxCommandEvent &event );
    void OnRedo( wxCommandEvent &event );
    void OnSetAsDelimiter( wxCommandEvent &event );
    void OnCollate( wxCommandEvent &event );
    void OnViewImage( wxCommandEvent &event );
    virtual void OnUpdateUI( wxUpdateUIEvent &event );

private:
    DECLARE_DYNAMIC_CLASS(CmpEnv)

    DECLARE_EVENT_TABLE()
};

	#endif //AX_COMPARISON
#endif //AX_RECOGNITION

#endif

