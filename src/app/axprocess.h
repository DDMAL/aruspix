/////////////////////////////////////////////////////////////////////////////
// Name:        axprocess.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
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
    
    bool Start( );
	int GetPid( ) { return m_pid; }
	void SetLog( wxString log_fname );

public:
	int m_status;
    
private:
    wxTextOutputStream *m_log;
	wxFileOutputStream *m_logStream;
	wxString m_cmd;
    
private:
    
private:

};


#endif
