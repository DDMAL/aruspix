/////////////////////////////////////////////////////////////////////////////
// Name:        axprocess.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __axprocess_H__
#define __axprocess_H__

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "axprocess.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/process.h"

#include "axprogressdlg.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// AxProcess
//----------------------------------------------------------------------------

class AxProcess: public wxProcess
{
public:
    // constructors and destructors
    AxProcess( wxString cmd, wxString arg, AxProgressDlg *dlg );
    virtual ~AxProcess();

	// end process
	virtual void OnTerminate(int pid, int status);
	virtual bool HasEnded( );
    
    // WDR: method declarations for AxProcess
	bool Start( );
	int GetPid( ) { return m_pid; }
	void SetLog( wxString log_fname );

public:
	int m_status;
    
private:
    // WDR: member variable declarations for AxProcess
	wxTextOutputStream *m_log;
	wxFileOutputStream *m_logStream;
	wxString m_cmd;
    
private:
    // WDR: handler declarations for AxProcess

private:

};


#endif
