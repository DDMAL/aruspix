/////////////////////////////////////////////////////////////////////////////
// Name:        axgotodlg.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "axgotodlg.h"
#endif

#include <algorithm>
using std::min;
using std::max;

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/spinctrl.h"

#include "axgotodlg.h"

//----------------------------------------------------------------------------
// AxGotoDlg
//----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(AxGotoDlg,wxDialog)
    EVT_BUTTON( wxID_OK, AxGotoDlg::OnOk )
    EVT_BUTTON( wxID_CANCEL, AxGotoDlg::OnCancel )
    EVT_SPIN( ID_SC_NO_GOTO, AxGotoDlg::OnSpinCtrl )
	EVT_TEXT( ID_SC_NO_GOTO, AxGotoDlg::OnSpinCtrlText )
END_EVENT_TABLE()

AxGotoDlg::AxGotoDlg( wxWindow *parent, wxWindowID id, const wxString &title,
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


void AxGotoDlg::OnSpinCtrlText( wxCommandEvent &event )
{
	this->GetRbNoGoto()->SetValue( true );    
}

void AxGotoDlg::OnSpinCtrl( wxSpinEvent &event )
{
	this->GetRbNoGoto()->SetValue( true );    
}

void AxGotoDlg::OnOk(wxCommandEvent &event)
{
	if ( this->GetRbFirstGoto()->GetValue() )
		m_pageNo = 1;
	else if ( this->GetRbLastGoto()->GetValue() )
		m_pageNo = m_nbOfPages;
	else
		m_pageNo = min( m_nbOfPages, this->GetScNoGoto()->GetValue() );
    event.Skip();
}

void AxGotoDlg::OnCancel(wxCommandEvent &event)
{
    event.Skip();
}

