/////////////////////////////////////////////////////////////////////////////
// Name:        axframe.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __axframe_H__
#define __axframe_H__

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "axframe.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

class AxEnv;
class AxEnvRow;

class RtMidiIn;

#include "axapp_wdr.h"

WX_DECLARE_OBJARRAY(AxEnvRow, AxEnvArray);


//----------------------------------------------------------------------------
// AxFrame
//----------------------------------------------------------------------------

class AxFrame: public wxFrame
{
public:
    // constructors and destructors
    AxFrame() {};
    AxFrame( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE );
    virtual ~AxFrame();
    
    private:
	int GetEnvironmentMenuId( const wxString className, int *pos );
    void SetEnvironment( int menuId );
    void LoadConfig();
    void SaveConfig(int lastEnvId);
	void CheckOptions( );
	

public:
	wxBitmap GetToolbarBitmap( wxString name );
	void RealizeToolbar();
    void SetEnvironment( const wxString className );
    void ParseCmd( wxCmdLineParser *parser );
	void Open( wxString filename );
    void InitMidi();
    wxString GetEnvironmentName( );
    // Midi Input
    RtMidiIn *m_midiIn;

private:
        // tmp
    wxMenuBar *m_menuBarPtr;
    wxToolBar *m_toolBarPtr;
    AxEnvArray m_envArray;
    AxEnv *m_env;
    AxEnvRow *m_envRow;

public:
    wxArrayString m_fnames;
    
private:
        void OnHelp( wxCommandEvent &event );
    void OnOptions( wxCommandEvent &event );
    void OnSize( wxSizeEvent &event );
    void OnClose( wxCloseEvent &event );
    void OnEnvironmentMenu( wxCommandEvent &event );
    void OnCustomMenu( wxEvent &event );
    void OnEnvironments( wxCommandEvent &event );   
    void OnQuit( wxCommandEvent &event );
    void OnAbout( wxCommandEvent &event );
    void OnUpdateUIEnvironmentMenu( wxUpdateUIEvent &event );
    void OnUpdateUIEnvs( wxUpdateUIEvent &event );

private:
    DECLARE_DYNAMIC_CLASS(AxFrame)
    DECLARE_EVENT_TABLE()
};

#endif
