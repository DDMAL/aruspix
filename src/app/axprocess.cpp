/////////////////////////////////////////////////////////////////////////////
// Name:        axprocess.cpp
// Author:      Laurent Pugin
// Created:     04/05/25
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "axprocess.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/txtstrm.h"

#include "axprocess.h"
#include "axapp.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// AxProcess
//----------------------------------------------------------------------------

// WDR: event table for AxProcess

AxProcess::AxProcess( wxString cmd, wxString args, AxProgressDlg *dlg )
{
	//Redirect();
	m_pid = 0;
	m_status = 0;
	m_cmd = wxGetApp().m_appPath + "/" + cmd + " "  + args;
	m_deleteOnTerminate = false;
	m_canceled = false;
}

AxProcess::~AxProcess()
{
}


void AxProcess::OnTerminate(int pid, int status)
{
	wxString line;
	m_status = status;

	// redirect in now disabled (see constructor)
	// nothing will happen
	wxTextInputStream tis(*GetInputStream());
    while ( IsInputAvailable() )
	{
		line = tis.ReadLine();
		if ( !line.IsEmpty() )
			wxLogMessage( _("%s"), line.c_str() );
	}

	wxTextInputStream tes(*GetErrorStream());
    while ( IsInputAvailable() )
	{
		line = tes.ReadLine();
		if ( !line.IsEmpty() )
			wxLogError( _("%s"), line.c_str() );
	}

	if (status != 0)
	{
		if ( !m_canceled )
			wxLogError( _T("Process %u terminated with exit code %d"), pid,  status);
		else
			wxLogMessage( _T("Process %u terminated, operation canceled"), pid);
	}

	m_pid = 0;
	
	if ( m_deleteOnTerminate )
		delete this;
}


bool AxProcess::Start( )
{
	int pid = wxExecute(m_cmd, wxEXEC_ASYNC, this);
	if ( !pid )
    {
		wxLogError(_("Execution failed"));
		return false;
	}
	
	m_pid = pid;
	return true;
}

// WDR: handler implementations for AxProcess

