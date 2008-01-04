/////////////////////////////////////////////////////////////////////////////
// Name:        gotodlg.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __gotodlg_H__
#define __gotodlg_H__

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "gotodlg.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "app_wdr.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// GotoDlg
//----------------------------------------------------------------------------

class GotoDlg: public wxDialog
{
public:
    // constructors and destructors
    GotoDlg( wxWindow *parent, wxWindowID id, const wxString &title,
        int nbOfPages, int currentPage,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE );
    
    // WDR: method declarations for GotoDlg
	int GetPage() { return m_pageNo -1; }
    wxSpinCtrl* GetScNoGoto()  { return (wxSpinCtrl*) FindWindow( ID_SC_NO_GOTO ); }
    wxRadioButton* GetRbNoGoto()  { return (wxRadioButton*) FindWindow( ID_RB_NO_GOTO ); }
    wxRadioButton* GetRbLastGoto()  { return (wxRadioButton*) FindWindow( ID_RB_LAST_GOTO ); }
    wxRadioButton* GetRbFirstGoto()  { return (wxRadioButton*) FindWindow( ID_RB_FIRST_GOTO ); }
    
private:
    // WDR: member variable declarations for GotoDlg
    int m_nbOfPages;
    int m_pageNo;
    
private:
    // WDR: handler declarations for GotoDlg
	void OnSpinCtrl( wxSpinEvent &event );
	void OnSpinCtrlText( wxCommandEvent &event );
    void OnOk( wxCommandEvent &event );
    void OnCancel( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};

#endif
