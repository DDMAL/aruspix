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


#include "progressdlg.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// AxProcess
//----------------------------------------------------------------------------

class AxProcess: public wxProcess
{
public:
    // constructors and destructors
    AxProcess( wxString cmd, wxString arg, ProgressDlg *dlg );
    virtual ~AxProcess();

	// end process
	virtual void OnTerminate(int pid, int status);
    
    // WDR: method declarations for AxProcess
	bool Start( );
	void SetPid( const int pid ) { m_pid = pid; }
	int GetPid( ) { return m_pid; }

public:
	int m_status;
	bool m_deleteOnTerminate;
	bool m_canceled;
    
private:
    // WDR: member variable declarations for AxProcess
	int m_pid;
	wxString m_cmd;
    
private:
    // WDR: handler declarations for AxProcess

private:

};


#endif
