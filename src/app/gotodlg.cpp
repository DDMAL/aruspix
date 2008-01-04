/////////////////////////////////////////////////////////////////////////////
// Name:        gotodlg.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "gotodlg.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/spinctrl.h"

#include "gotodlg.h"
#include "defs.h"


// WDR: class implementations

//----------------------------------------------------------------------------
// GotoDlg
//----------------------------------------------------------------------------

// WDR: event table for GotoDlg

BEGIN_EVENT_TABLE(GotoDlg,wxDialog)
    EVT_BUTTON( wxID_OK, GotoDlg::OnOk )
    EVT_BUTTON( wxID_CANCEL, GotoDlg::OnCancel )
    EVT_SPIN( ID_SC_NO_GOTO, GotoDlg::OnSpinCtrl )
	EVT_TEXT( ID_SC_NO_GOTO, GotoDlg::OnSpinCtrlText )
END_EVENT_TABLE()

GotoDlg::GotoDlg( wxWindow *parent, wxWindowID id, const wxString &title,
    int nbOfPages, int currentPage,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style )
{
    GotoDlgFunc( this, TRUE );

    m_pageNo = currentPage + 1;
    m_nbOfPages = nbOfPages;
    this->GetScNoGoto()->SetRange( 1, nbOfPages );
    this->GetScNoGoto()->SetValue( m_pageNo );
}

// WDR: handler implementations for GotoDlg

void GotoDlg::OnSpinCtrlText( wxCommandEvent &event )
{
	this->GetRbNoGoto()->SetValue( true );    
}

void GotoDlg::OnSpinCtrl( wxSpinEvent &event )
{
	this->GetRbNoGoto()->SetValue( true );    
}

void GotoDlg::OnOk(wxCommandEvent &event)
{
	if ( this->GetRbFirstGoto()->GetValue() )
		m_pageNo = 1;
	else if ( this->GetRbLastGoto()->GetValue() )
		m_pageNo = m_nbOfPages;
	else
		m_pageNo = min( m_nbOfPages, this->GetScNoGoto()->GetValue() );
    event.Skip();
}

void GotoDlg::OnCancel(wxCommandEvent &event)
{
    event.Skip();
}

