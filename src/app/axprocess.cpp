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
#include "wx/wfstream.h"

#include "axprocess.h"
#include "axapp.h"


//----------------------------------------------------------------------------
// AxProcess
//----------------------------------------------------------------------------


AxProcess::AxProcess( wxString cmd, wxString args, AxProgressDlg *dlg )
	//:  wxProcess( dlg )
{
	Redirect();
	m_pid = 0;
	m_status = 0;
	m_cmd = wxGetApp().m_appPath + "/" + cmd + " "  + args;
	m_log = NULL;
	m_logStream = NULL;
}

AxProcess::~AxProcess()
{
	wxLogDebug("AxProcess deleted");
	if ( m_log )
		delete m_log;
	if ( m_logStream )
	{
		m_logStream->Close();
		delete m_logStream;
	}
}


void AxProcess::SetLog( wxString log_fname )
{
	m_logStream = new wxFileOutputStream( log_fname );
	if ( !m_logStream->IsOk() )
	{
		wxLogWarning("Unable to open log file '%s'", log_fname.c_str() );
		return;
	}
	
	m_log = new wxTextOutputStream( *m_logStream );
	m_log->WriteString( m_cmd );
	m_log->WriteString( "\n" );
}

void AxProcess::OnTerminate(int pid, int status)
{
}


bool AxProcess::HasEnded( )
{
	wxString msg;

	// redirect in now disabled (see constructor)
	// nothing will happen
	wxTextInputStream tis(*GetInputStream());
	wxTextInputStream tes(*GetErrorStream());
	
	// we don't know where Error is going to be output
    while ( IsInputAvailable() )
	{
		msg = tis.ReadLine();
		if ( msg.StartsWith("$ Success!") )
		{
			return true;
		}
		else if ( msg.StartsWith("$ Error: ") )
		{
			m_status = 255;
			return true;
		}
		if ( !msg.IsEmpty() && m_log )
			m_log->WriteString( msg + "\n" );
	}
    while ( IsErrorAvailable() )
	{
		msg = tes.ReadLine();
		if ( msg.StartsWith("$ Error: ") )
		{
			m_status = 255;
			return true;
		}
		if ( !msg.IsEmpty() && m_log )
			m_log->WriteString( msg + "\n" );
    }


    return false;
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


