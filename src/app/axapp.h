/////////////////////////////////////////////////////////////////////////////
// Name:        axapp.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __axapp_H__
#define __axapp_H__

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "axapp.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/config.h"
#include "wx/ipc.h"     // IPC support
#include "wx/snglinst.h" // single instance checker
#include "wx/cmdline.h"  // command line support
#include "wx/intl.h"     // internationalisation
#include "wx/html/helpctrl.h"
#include "wx/dir.h"
#include "wx/wizard.h"

#include "axapp_wdr.h"
#include "axabout_wdr.h"

/*
 * GESTION DES IDS
 *
 * axapp.wdr 6000
 * axoptions.wdr 8000
 * axabout.wdr 9000
 */
 
 /*******************
 * env.wdr 1x000
 ** sup 12000
 ** dsp 13000
 ** rec 14000
 ** edt 15000
 ** cmp 16000
 *******************
 * mus.wdr 20000
 * aximcontrol.wdr 21000
 * local 22x000
 */
 
#define ENV_IDS_MIN 10000
#define ENV_IDS_MAX 19999
#define ENV_IDS_MIN_SUP 12000
#define ENV_IDS_MAX_SUP 12999
//#define ENV_IDS_MIN_DSP 13000 Free as Dsp doesn't exist anymore
//#define ENV_IDS_MAX_DSP 13999
#define ENV_IDS_MIN_REC 14000
#define ENV_IDS_MAX_REC 14999
#define ENV_IDS_MIN_EDT 15000
#define ENV_IDS_MAX_EDT 15999
#define ENV_IDS_MIN_CMP 16000
#define ENV_IDS_MAX_CMP 16999

#define ID_MIDI_INPUT 19999 // Last ID in mus.wdr

#define ENV_IDS_LOCAL 22000
#define ENV_IDS_LOCAL_SUP ENV_IDS_LOCAL + 200
//#define ENV_IDS_LOCAL_DSP ENV_IDS_LOCAL + 300 Free as Dsp doesn't exist anymore
#define ENV_IDS_LOCAL_REC ENV_IDS_LOCAL + 400
#define ENV_IDS_LOCAL_EDT ENV_IDS_LOCAL + 500
#define ENV_IDS_LOCAL_CMP ENV_IDS_LOCAL + 600
#define ENV_BASE_ID 30000 // base id for environment menu items
#define ENV_MAX 50 // max of environments (used for RANGE_ID event)

// customized event type for midi
DECLARE_EVENT_TYPE( AX_EVT_MIDI, -1 )

class AxFrame;


//----------------------------------------------------------------------------
// AxAboutDlg
//----------------------------------------------------------------------------

class AxAboutDlg: public wxDialog
{
public:
    // constructors and destructors
    AxAboutDlg( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE );
    
        wxStaticBitmap* GetLogo()  { return (wxStaticBitmap*) FindWindow( ID0_LOGO ); }
    wxStaticText* GetTxAppVersion()  { return (wxStaticText*) FindWindow( TX_APP_VERSION ); }
    wxStaticText* GetTxAppBuild()  { return (wxStaticText*) FindWindow( TX_APP_BUILD ); }

private:
        
private:
    
private:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// AxApp
//----------------------------------------------------------------------------

class AxApp: public wxApp
{
    friend class AxIPCConnection;

public:
    // constructors and destructors
    AxApp();
    virtual ~AxApp();
    
        virtual bool OnInit();
    virtual int OnExit();
    //static wxString FindAppPath();
    static bool CheckDir( wxString dirname, int permission );
    void AxBeginBusyCursor();
    void AxEndBusyCursor();
	
#ifdef __WXMAC__
	virtual void MacOpenFile(const wxString &fileName);
#endif

private:
    bool ProcessCmdLine (char** argv, int argc = 0);
    bool ProcessRemote (char** argv, int argc = 0);
    
private:
        wxSingleInstanceChecker *m_singleInstance; 
    wxServerBase *m_serverIPC; //! the wxIPC server
    AxFrame *m_mainFrame;
    int m_busyCursor;

public:
	wxLocale m_locale; // i18n
    int m_language;
    wxString m_appPath;
    wxString m_resourcesPath;
    wxHtmlHelpController *m_help;

    wxString m_docDir;
    wxString m_workingDir;
	wxString m_logDir;
    wxString m_lastDir;
    wxString m_lastDirAX0_in; // default file
    wxString m_lastDirAX0_out;
    wxString m_lastDirAX1_in; // typographic model
    wxString m_lastDirAX1_out;
    wxString m_lastDirAX2_in; // music model
    wxString m_lastDirAX2_out;
    wxString m_lastDirAX3_in; // project
    wxString m_lastDirAX3_out;
    wxString m_lastDirAX4_in; // zip file
    wxString m_lastDirAX4_out;
    wxString m_lastDirTIFF_in;
    wxString m_lastDirTIFF_out;
    wxString m_lastDirBatch_in;
    wxString m_lastDirBatch_out;
    wxString m_lastDirMusModel_in;
    wxString m_lastDirTypModel_in;

    wxString m_musicFontDesc;
	wxString m_neumeFontDesc;
    wxString m_lyricFontDesc;
    int m_fontSizeCorrection;
    int m_fontPosCorrection;
    
    int m_toolbar_toolsize;
	
	// versions
    static int s_version_major;
    static int s_version_minor;
    static int s_version_revision;
    static wxString s_version;
    static wxString s_build_date;
    static wxString s_build_time;

    
private:
    
};

DECLARE_APP(AxApp)

//----------------------------------------------------------------------------
//! IPC connection for application APP_VENDOR-APP_NAME.
//----------------------------------------------------------------------------

class AxIPCConnection : public wxConnection 
{
public:
    //! application IPC connection
    AxIPCConnection();

    //! execute handler
    virtual bool OnExecute (const wxString& WXUNUSED(topic),
                            char *data, int size, wxIPCFormat WXUNUSED(format));

private:
   // character buffer
   char m_buffer[4096];

};


//----------------------------------------------------------------------------
//! IPC server for application APP_VENDOR-APP_NAME.
//----------------------------------------------------------------------------

class AxIPCServer : public wxServer {

public:
    //! accept conncetion handler
    virtual wxConnectionBase *OnAcceptConnection (const wxString& topic);
};



//----------------------------------------------------------------------------
// AxDirTraverser
//----------------------------------------------------------------------------

// remove all files recursively in a directory

class AxDirTraverser : public wxDirTraverser
{

public:
    AxDirTraverser( wxString directory ); // clean directory
	AxDirTraverser( wxString from, wxString to ); // copy directory

    virtual wxDirTraverseResult OnFile(const wxString& filename);
    virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname));
        
private:
    wxArrayString m_names;
};

// ----------------------------------------------------------------------------
// AxSetup
// ----------------------------------------------------------------------------

class AxSetup : public wxWizard
{
public:
    AxSetup( wxBitmap logo );

    wxWizardPage *GetFirstPage() const { return m_page1; }

private:
    wxWizardPageSimple *m_page1;
	
public:
	int m_language;
};

#endif
