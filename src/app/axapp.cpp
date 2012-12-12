/////////////////////////////////////////////////////////////////////////////
// Name:        axapp.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "wx/file.h"
#include "wx/filename.h"
#include "wx/intl.h"
#include "wx/fs_zip.h"   // ZIP filesystem support
#include "wx/hyperlink.h"
#include "wx/stdpaths.h"
#include "wx/splash.h"
#include "wx/tokenzr.h"
#include "wx/txtstrm.h"
#include "wx/url.h"
#include "wx/protocol/http.h"

#include "axapp.h"
#include "axframe.h"
#include "axfile.h"
#include "axundo.h"

#define APPNAME "Aruspix"

const wxString IPC_START = "StartOther";

int AxApp::s_version_major = 2;
int AxApp::s_version_minor = 0;
int AxApp::s_version_revision = 6;
wxString AxApp::s_version = wxString::Format("%d.%d.%d", AxApp::s_version_major, AxApp::s_version_minor, AxApp::s_version_revision);
wxString AxApp::s_build_date = __DATE__;
wxString AxApp::s_build_time = __TIME__;

#define COPYRIGHT "Copyright © 2004-2012 Laurent Pugin and others"
#define LICENSE "Published under the GNU General Public License 3"
#define CONTRIBUTORS "John Ashley Burgoyne;Greg Eustace;Andrew Hankinson;Tristan Himmelman;Tristan Matthews;Christopher Niven;Alastair Porter;Marnie Reckenberg;Gabriel Vigliensoni;Rodolfo Zitellini"
#define UPDATE_URL "http://www.aruspix.net/downloads/changes.txt"

#if defined(__WXMSW__)
    // To be tested
    #define UPDATE_FILE "http://www.aruspix.net/downloads/AruspixInstaller%d.%d.%d.exe"
#else // OS X
    #define UPDATE_FILE "http://www.aruspix.net/downloads/Aruspix%d.%d.%d.dmg"
#endif

IMPLEMENT_APP(AxApp)

/*
#if defined(__WXMSW__)
    #if !defined (_DEBUG) 
        #define APPDIR "Application Data/Aruspix/"
    #else
        #define APPDIR "D:/Projects/Aruspix/distrib/"
    #endif //_DEGUG
#else
    #define APPDIR ".Aruspix/"
#endif
*/


//----------------------------------------------------------------------------
// AxApp
//----------------------------------------------------------------------------

AxApp::AxApp()
{
    m_language = 0;
    m_help = NULL;
    m_busyCursor = 0;
}

AxApp::~AxApp()
{
}


bool AxApp::OnInit()
{
    wxInitAllImageHandlers();
    
    
    SetVendorName("Aruspix");
    SetAppName(APPNAME); // not needed, it's the default value

    m_singleInstance = NULL;
    m_serverIPC = NULL;
    m_mainFrame = NULL;

#ifndef __WXMAC__
    // Set and check for single instance running
    wxString name = APPNAME + wxString::Format ("%s", wxGetUserId().c_str());
    wxLogNull *logNo = new wxLogNull(); // hide error messages
	m_singleInstance = new wxSingleInstanceChecker( );
    if ( m_singleInstance->Create( name ) )
    {
        if (m_singleInstance->IsAnotherRunning()) {
            wxClient client;
            wxConnectionBase *conn = client.MakeConnection ("", name, IPC_START);
            if (conn) {
                wxString dataStr;
                int i;
                for (i = 0; i < argc; ++i) {
                    dataStr.Append (argv[i]);
                    dataStr.Append (" ");
                }
                char data[4096];
                strcpy (data, dataStr.c_str());
                int size = 0;
                for (i = 0; i < argc; ++i) {
                    size += strlen (argv[i]);
                    data[size] = '\0';
                    size += 1;
                }
                data[size] = '\0';
                size += 1;
                if (conn->Execute (data, size)) return false;
            }
            delete conn;
        }

        // IPC server
        m_serverIPC = new AxIPCServer ();
        if (!m_serverIPC->Create ( name ) ) {
            delete m_serverIPC;
            m_serverIPC = NULL;
        }
    }
    else 
    {   
        delete m_singleInstance;
        m_singleInstance = NULL;
    }
    delete logNo;
#endif


	m_appPath = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath (wxPATH_GET_VOLUME);
#if defined(__WXMSW__)
    m_resourcesPath = m_appPath;
#else // OS X
	m_resourcesPath = wxStandardPaths::Get().GetResourcesDir();
#endif

#if defined(__linux__)
	wxString xdg_cache;
	if (wxGetEnv("XDG_CACHE_HOME", &xdg_cache)) {
		m_logDir = xdg_cache + "/aruspix";
	} else {
		wxString home_dir;
		wxGetEnv("HOME", &home_dir);
		m_logDir = home_dir + "/.cache/aruspix";
	}
#else
	m_logDir = wxStandardPaths::Get().GetUserDataDir();
#endif

    // configuration that has to be loaded before building the frame
    // we need language and toolbar toolsize
	// TODO: Would be nice to use $XDG_CONFIG_HOME on linux
    wxConfigBase *pConfig = wxConfigBase::Get();
    wxASSERT_MSG( pConfig, wxT("pConfig cannot be NULL") );
    pConfig->SetRecordDefaults();
    pConfig->SetPath("/");

    // language
    pConfig->Read("ToolbarToolsize", &m_toolbar_toolsize, 16);
    pConfig->Read("Language",&m_language,-1);
    
#ifndef __AXDEBUG__
	wxSplashScreen* splash = NULL; // we don't want to show the splash screen in Debug mod
#endif
    if ( m_language == -1 ) // never choosen before, it is the first time the user run aruspix, run setup
    {
		SetExitOnFrameDelete(FALSE);
#if defined(__WXMSW__)
    AxSetup setup( wxBitmap( m_resourcesPath + "/logo.win.png", wxBITMAP_TYPE_PNG ) );
#else // OS X
		AxSetup setup( wxBitmap( m_resourcesPath + "/logo.png", wxBITMAP_TYPE_PNG ) );
#endif
		
		setup.RunWizard(setup.GetFirstPage());

        if ( setup.m_language == -1 ) //canceled
            m_language = 0;
        else
            m_language = setup.m_language;
			
		SetExitOnFrameDelete(TRUE);
    }
	else // splash screen,but not shown in Debug mode
	{
		wxBitmap bitmap;
		if (bitmap.LoadFile( m_resourcesPath + "/splash.png", wxBITMAP_TYPE_PNG))
		{
			wxMemoryDC dest;
			dest.SelectObject( bitmap );
#ifndef __AXDEBUG__
    #if defined(__WXMSW__)
			long splash_style = wxSIMPLE_BORDER | wxSTAY_ON_TOP;
			dest.SetFont( *wxNORMAL_FONT );
    #else
			long splash_style = wxSIMPLE_BORDER; // stay on top did not work on OS X
			dest.SetFont( *wxSMALL_FONT );		
    #endif
            wxString version = wxString::Format( "Version %s", AxApp::s_version.c_str() );
            wxString build = wxString::Format( "Build %s - %s", AxApp::s_build_date.c_str() , AxApp::s_build_time.c_str() );
            int x = 140;
            dest.DrawText( version , x, 230 );
            dest.DrawText( build , x, 245 );
            dest.DrawText( COPYRIGHT , x, 260 );
            dest.DrawText( LICENSE , x, 275 );
            dest.SelectObject( wxNullBitmap );

            splash = new wxSplashScreen(bitmap,
                    wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
                    4000, NULL, -1, wxDefaultPosition, wxDefaultSize, splash_style );
#endif
		}
	}
	
    switch (m_language)
    {
        case 0 : m_locale.Init( wxLANGUAGE_ENGLISH ); break;
        case 1 : m_locale.Init( wxLANGUAGE_FRENCH ); break;
    }
    m_locale.AddCatalogLookupPathPrefix( m_resourcesPath );
    m_locale.AddCatalog( "aruspix" );
    wxString lang = m_locale.GetName();
    lang = lang.Truncate( 2 );

    // initializes help (Html help needs the Zip filestytem!?!)
    //wxFileSystem::AddHandler (new wxZipFSHandler);
    m_help = new wxHtmlHelpController;

    wxASSERT_MSG( wxGetApp().m_help , "Help controller cannot be NULL ");

    if (!m_help->Initialize ( m_resourcesPath + "/" + lang + "/aruspix") )
        m_help->Initialize ( m_resourcesPath + "/en/aruspix" );

#ifdef AX_SUPERIMPOSITION
    m_locale.AddCatalog( "superimposition" );
    if (!m_help->Initialize ( m_resourcesPath + "/" + lang + "/superimposition") )
        m_help->Initialize ( m_resourcesPath + "/en/superimposition" );
#endif //AX_SUPERIMPOSITION

#ifdef AX_RECOGNITION
    m_locale.AddCatalog( "recognition" );
    if (!m_help->Initialize ( m_resourcesPath + "/" + lang + "/recognition") )
        m_help->Initialize ( m_resourcesPath + "/en/recognition" );
#endif //AX_RECOGNITION

#ifdef AX_EDITION
    m_locale.AddCatalog( "edt" );
    if (!m_help->Initialize ( m_resourcesPath + "/" + lang + "/edt") )
        m_help->Initialize ( m_resourcesPath + "/en/edt" );
#endif //AX_EDITION

#ifdef AX_COMPARISON
    m_locale.AddCatalog( "comparison" );
    if (!m_help->Initialize ( m_resourcesPath + "/" + lang + "/comparison") )
        m_help->Initialize ( m_resourcesPath + "/en/comparison" );
#endif //AX_COMPARISON

    // main frame
    m_mainFrame = new AxFrame((wxFrame*)NULL,-1,_("Aruspix") );
    //SetTopWindow(m_mainFrame);
    // undo
    AxUndo::InitUndos();
    
    // if false, means that option -q (quiet) was used, i.e. command line mode
    if ( !ProcessCmdLine (argv, argc) ) {
        return false;
        //m_mainFrame->Close();
    }

    m_mainFrame->InitMidi();
    m_mainFrame->Show(TRUE);
 
#if !defined(__WXMSW__)
	m_mainFrame->Lower( ); 	// again, stay on top did not work for OS X	
#endif
 
    AxUpdateDlg *dlg = new AxUpdateDlg( NULL ,-1, _("Update"));
    if ( dlg->CheckForUpdate() ) {
        dlg->Center(wxBOTH);
        dlg->ShowModal();
    }
    dlg->Destroy();
 
    return TRUE;
}

int AxApp::OnExit()
{
    // delete single instance checker
    if (m_singleInstance) delete m_singleInstance;

    // delete IPC server
    if (m_serverIPC) delete m_serverIPC;

    // delete help
    if (m_help) delete m_help;

    delete wxConfigBase::Set((wxConfigBase *) NULL);
    return 0;
	
	// clean the working directory in Release mode only
#ifndef __AXDEBUG__		
	if ( wxDirExists( wxGetApp().m_workingDir ) )
		AxDirTraverser clean( wxGetApp().m_workingDir );
#endif
}

void AxApp::AxBeginBusyCursor()
{
    if ( m_busyCursor == 0 )
    {
        wxBeginBusyCursor( );
    }
            
    m_busyCursor++;
}


void AxApp::AxEndBusyCursor()
{
    m_busyCursor--;
    
    wxASSERT( m_busyCursor >= 0 );

    if ( m_busyCursor == 0 )
        wxEndBusyCursor();
}


bool AxApp::ProcessCmdLine (char** argv, int argc)
{
    wxASSERT(m_mainFrame);

    // get and process command line
    static const wxCmdLineEntryDesc cmdLineDesc[] = {
        { wxCMD_LINE_SWITCH, _T("h"), _T("help"), _T("show this help message"),
            wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        {wxCMD_LINE_PARAM,  NULL, NULL, _("input files"),
            wxCMD_LINE_VAL_STRING,
            wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE},
        {wxCMD_LINE_OPTION,  "e", "environment", _("environment name"),
            wxCMD_LINE_VAL_STRING },
        {wxCMD_LINE_SWITCH, "q", "quiet", "command line mode, do not show main frame" },
        {wxCMD_LINE_SWITCH, "p", "preprocess", "preprocessing the image in recognition environment (output file is require)"} ,
        {wxCMD_LINE_SWITCH, "m", "mei2svg", "the mei file to be converted to svg (output file is require)"} ,
        {wxCMD_LINE_NONE}
    };
    wxCmdLineParser parser (cmdLineDesc, argc, argv);
    wxString logo = wxString::Format( "Aruspix version %s - Build %s %s\nCopyright 2004-2011 Laurent Pugin, All Rights Reserved",
        AxApp::s_version.c_str(), AxApp::s_build_date.c_str() , AxApp::s_build_time.c_str() );
    parser.SetLogo( logo );

    // get filenames from the commandline
    m_mainFrame->m_fnames.Clear();

    if ( parser.Parse() != 0 ) // help message (-1) or syntax error
        return false;
        
    bool show_frame = true; 
    if ( parser.Found("q") )
        show_frame = false;
    m_mainFrame->ParseCmd( &parser );
    return show_frame;

}


bool AxApp::ProcessRemote (char** argv, int argc) {

    // process remote request
    ProcessCmdLine (argv, argc);
    //wxMessageBox("ok");
    /*AppFrame* frame = (AppFrame*)wxTheApp->GetTopWindow();
    frame->FileOpen (*m_fnames);
    frame->Maximize (false);*/

    return true;
}

/*
wxString AxApp::FindAppPath()
{
    wxString argv0 = wxGetApp().argv[0];
    wxString cwd = wxGetCwd();
    wxString str;

    if (wxIsAbsolutePath(argv0))
        return wxPathOnly(argv0);
    else
    {
        // Is it a relative path?
        wxString currentDir(cwd);
        if (currentDir.Last() != wxFILE_SEP_PATH)
            currentDir += wxFILE_SEP_PATH;

        str = currentDir + argv0;
        if (wxFileExists(str))
            return wxPathOnly(str);
    }

    // OK, it's neither an absolute path nor a relative path.
    // Search PATH.

    wxPathList pathList;
    pathList.AddEnvList(wxT("PATH"));
    str = pathList.FindAbsoluteValidPath(argv0);
    if (!str.IsEmpty())
        return wxPathOnly(str);

    // Failed
    return wxEmptyString;
}
*/


    
bool AxApp::CheckDir( wxString dirname, int permission )
{
    // check if exists
    wxLogNull *logNo = new wxLogNull();
    if ( !wxDirExists( dirname ) && !wxMkdir( dirname, permission ) )
    {
        delete logNo;
        return false;
    }

    // check if writable
    wxString test_filename = dirname + "/.test";
    wxFile test_file( test_filename, wxFile::write );
    if ( !test_file.IsOpened() )
    {
        delete logNo;
        return false;
    }
    test_file.Close();
    wxRemoveFile( test_filename );
    delete logNo;
    return true;
}



#ifdef __WXMAC__
void AxApp::MacOpenFile(const wxString &fileName)
{
	//wxLogMessage( fileName );
    m_mainFrame->Open( fileName );
}
#endif


//----------------------------------------------------------------------------
//! IPC connection for application APP_VENDOR-APP_NAME.
//----------------------------------------------------------------------------

AxIPCConnection::AxIPCConnection()
    : wxConnection (m_buffer, WXSIZEOF(m_buffer))
{

}

bool AxIPCConnection::OnExecute(const wxString& WXUNUSED(topic),
                            char *data, int size, wxIPCFormat WXUNUSED(format))
{
    char** argv;
    int argc = 0;
    int i;
    for (i=0; i<size; ++i) {
        if ((i > 0) && (data[i] == '\0') && (data[i-1] == '\0')) break;
        if (data[i] == '\0') ++argc;
    }
    argv = new char*[argc];
    int p = 0;
    char* temp = data;
    for (i=0; i<argc; ++i) {
        argv[i] = new char [strlen (temp) + 1];
        strcpy (argv[i], temp);
        p = strlen (temp) + 1;
        temp += p;
    }
    bool ok = wxGetApp().ProcessRemote (argv, argc);
    for (i=0; i<argc; ++i) {
        delete [] argv[i];
    }
    delete [] argv;
    return ok;
}



//----------------------------------------------------------------------------
// AxAboutDlg
//----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(AxAboutDlg,wxDialog)

END_EVENT_TABLE()

AxAboutDlg::AxAboutDlg( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style )
{
    wxColour normal( 0, 192, 0 );
    wxColour hover( 0, 125, 0 );  
    wxHyperlinkCtrl *link1 = new wxHyperlinkCtrl( this, ID0_WXWIDGETS, "WxWidgets GUI Library", "http://www.wxwidgets.org" );
    link1->SetNormalColour( normal );
    link1->SetHoverColour( hover );
    link1->SetVisitedColour( normal );
    wxHyperlinkCtrl *link2 = new wxHyperlinkCtrl( this, ID0_TORCH, "Torch Machine-Learning Library", "http://www.torch.ch" );
    link2->SetNormalColour( normal );
    link2->SetHoverColour( hover );
    link2->SetVisitedColour( normal );
    wxHyperlinkCtrl *link3 = new wxHyperlinkCtrl( this, ID0_IM, "IM Digital Imaging Library", "http://www.tecgraf.puc-rio.br/im" );
    link3->SetNormalColour( normal );
    link3->SetHoverColour( hover );
    link3->SetVisitedColour( normal );
    wxHyperlinkCtrl *link4 = new wxHyperlinkCtrl( this, ID0_WG, "Wolfgang Music Notation Software", "http://www.winwg.com" );
    link4->SetNormalColour( normal );
    link4->SetHoverColour( hover );
    link4->SetVisitedColour( normal );
    wxHyperlinkCtrl *link5 = new wxHyperlinkCtrl( this, ID0_LIBMEI, "Libmei C++ library for MEI", "http://ddmal.music.mcgill.ca/libmei" );
    link5->SetNormalColour( normal );
    link5->SetHoverColour( hover );
    link5->SetVisitedColour( normal );

    AboutDlgFunc( this, true );
    wxString str;
    this->GetTxAppVersion()->SetLabel( AxApp::s_version );
    str = wxString::Format( "%s - %s", AxApp::s_build_date.c_str() , AxApp::s_build_time.c_str() );
    this->GetTxAppBuild()->SetLabel( str );
    this->GetTxAppCopyright()->SetLabel( COPYRIGHT );
    this->GetTxAppLicense()->SetLabel( LICENSE );
    
    wxStringTokenizer tkz( CONTRIBUTORS, ";" );
    while ( tkz.HasMoreTokens() )
    {
        this->GetTxAppContributors()->AppendText( tkz.GetNextToken() );
        if ( tkz.HasMoreTokens() ) {
            this->GetTxAppContributors()->AppendText( "\n" );
        }
    }
    // trick for updating the scroll
    this->GetTxAppContributors()->SetInsertionPoint( 0 );
    this->GetTxAppContributors()->WriteText( "" );
    

#if defined(__WXMSW__)
    this->GetLogo()->SetBitmap( wxBitmap( wxGetApp().m_resourcesPath + "/logo.win.png" , wxBITMAP_TYPE_PNG ) );
#else // OS X
	this->GetLogo()->SetBitmap( wxBitmap( wxGetApp().m_resourcesPath + "/logo.png" , wxBITMAP_TYPE_PNG ) );
#endif	
	
}


//----------------------------------------------------------------------------
// AxUpdateDlg
//----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(AxUpdateDlg,wxDialog)

END_EVENT_TABLE()

AxUpdateDlg::AxUpdateDlg( wxWindow *parent, wxWindowID id, const wxString &title,
                       const wxPoint &position, const wxSize& size, long style ) :
wxDialog( parent, id, title, position, size, style )
{

}

bool AxUpdateDlg::CheckForUpdate()
{
	wxURL url( UPDATE_URL );
	wxInputStream *input = url.GetInputStream();
	if (!input || !input->IsOk()) { 
        return false;
    }
    wxString changes;
    int vmaj, vmin, vrev;
    wxTextInputStream text( *input );
    
    changes = text.ReadLine();
    wxString version = changes;
    if ( !version.Replace( "Version ", "" ) ) {
        return false;
    }
    
    wxStringTokenizer tkz( version, "." );
    if ( !tkz.HasMoreTokens() ) {
        return false;
    }
    vmaj = atoi( tkz.GetNextToken().c_str() );
    if ( !tkz.HasMoreTokens() ) {
        return false;
    }
    vmin = atoi( tkz.GetNextToken().c_str() );
    if ( !tkz.HasMoreTokens() ) {
        return false;
    }
    vrev = atoi( tkz.GetNextToken().c_str() );
    
	if ( AxFile::FormatVersion( vmaj, vmin, vrev ) <= AxFile::FormatVersion( 
            AxApp::s_version_major, AxApp::s_version_minor, AxApp::s_version_revision ) ) {
        return false;
    }

    while(!input->Eof() ) {
        changes += "\n";
        changes += text.ReadLine();
    }
    wxString linkString = wxString::Format( UPDATE_FILE, vmaj, vmin, vrev );
    
    
    wxColour normal( 0, 192, 0 );
    wxColour hover( 0, 125, 0 );  
    wxHyperlinkCtrl *link1 = new wxHyperlinkCtrl( this, ID0_UPDATE_LINK, linkString, linkString );
    link1->SetNormalColour( normal );
    link1->SetHoverColour( hover );
    link1->SetVisitedColour( normal );
    
    UpdateDlgFunc( this, true );
    
    this->GetTxAppFeatures()->SetValue( changes );
    // trick for updating the scroll
    this->GetTxAppFeatures()->SetInsertionPoint( 0 );
    this->GetTxAppFeatures()->WriteText( "" );    
    
    return true;
}



//----------------------------------------------------------------------------
//! IPC server for application APP_VENDOR-APP_NAME.
//----------------------------------------------------------------------------

wxConnectionBase *AxIPCServer::OnAcceptConnection (const wxString& topic)
{
    if (topic != IPC_START) 
        return NULL;
    return new AxIPCConnection;
}


//----------------------------------------------------------------------------
// AxDirTraverser
//----------------------------------------------------------------------------

AxDirTraverser::AxDirTraverser( wxString directory ) // clean directory
{
    int i;
    // delete files
    wxDir::GetAllFiles( directory, &m_names, wxEmptyString, wxDIR_DEFAULT );
    for( i = m_names.GetCount() - 1; i >=0; i-- )
        wxRemoveFile( m_names[i] );

    // delete directories
    m_names.Clear();
    wxDir dir( directory );
    dir.Traverse( *this, wxEmptyString, wxDIR_DIRS | wxDIR_HIDDEN  );
    for( i = m_names.GetCount() - 1; i >=0; i-- )
        wxRmdir( m_names[i] );
}


AxDirTraverser::AxDirTraverser( wxString from, wxString to ) // copy directory
{
    int i;
    // create directories
    wxDir dir( from );
    dir.Traverse( *this, wxEmptyString, wxDIR_DIRS | wxDIR_HIDDEN );
    for( i = 0; i < (int)m_names.GetCount(); i++ )
	{
		m_names[i].Replace( from, to, false );
		if ( !wxDirExists( m_names[i] ) )
				wxMkdir( m_names[i], 0755 );   
	}
	
    // copy files
	m_names.Clear();
    wxDir::GetAllFiles( from, &m_names, wxEmptyString, wxDIR_DEFAULT );
    for( i = 0; i < (int)m_names.GetCount(); i++ )
	{
		wxString file = m_names[i];
		m_names[i].Replace( from, to, false );
		wxCopyFile( file, m_names[i], true );
     
	}
}

wxDirTraverseResult AxDirTraverser::OnFile(const wxString& filename)
{
    //wxRemoveFile( filename );
    //wxLogMessage( filename );
    return wxDIR_CONTINUE;
}

wxDirTraverseResult AxDirTraverser::OnDir(const wxString& dirname)
{
    m_names.Add( dirname );
    return wxDIR_CONTINUE;
}

// ----------------------------------------------------------------------------
// AxSetupPage1
// ----------------------------------------------------------------------------

class AxSetupPage1 : public wxWizardPageSimple
{
public:
    AxSetupPage1(wxWizard *parent, int *language ) : wxWizardPageSimple(parent)
    {
        m_languagePtr = language;
		
		wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
        mainSizer->Add(
            new wxStaticText(this, wxID_ANY,
                             _T("Select the language to be used\n")
                             _T("by default in Aruspix\n")),
            0, wxALL, 5 );

		
        wxString langs[] = {"English", "French"};
		
		m_language = new wxRadioBox( this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 2, langs, 1, wxRA_SPECIFY_COLS );
		mainSizer->Add( m_language, 0, wxALL, 5 );
		
        SetSizer(mainSizer);
        mainSizer->Fit(this);
    }

    virtual bool TransferDataFromWindow()
    {
        *m_languagePtr = m_language->GetSelection();
        return true;
    }

private:
	wxRadioBox *m_language;
	int *m_languagePtr;
};

// ----------------------------------------------------------------------------
// AxSetupPage2
// ----------------------------------------------------------------------------

class AxSetupPage2 : public wxWizardPageSimple
{
public:
    AxSetupPage2(wxWizard *parent) : wxWizardPageSimple(parent)
    {
        m_checkbox = new wxCheckBox(this, wxID_ANY, _T("&Create folders"));
		m_checkbox->SetValue( true );

        wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
        mainSizer->Add(
            new wxStaticText(this, wxID_ANY,
                             _T("Do you want Aruspix to create default\n")
                             _T("folders in your home directory?\n")),
            0, wxALL, 5 );

        mainSizer->Add( m_checkbox, 0, wxALL, 5 );
		
        SetSizer(mainSizer);
        mainSizer->Fit(this);
    }

    virtual bool TransferDataFromWindow()
    {
        if ( m_checkbox->GetValue() )
        {
			wxString from = wxGetApp().m_resourcesPath + "/Aruspix.localized";
			wxString to = wxStandardPaths::Get().GetDocumentsDir() + "/Aruspix.localized";

			if ( !wxDirExists(to) )
				wxMkdir( to, 0755);

			AxDirTraverser( from, to );		
		
			//wxLogDebug( documents );
			//wxCopyFile( wxGetApp().m_resourcesPath + "/Aruspix.localized"  , documents + "/Aruspix.localized",  false );
        }

        return true;
    }

private:
    wxCheckBox *m_checkbox;
};


// ----------------------------------------------------------------------------
// AxSetup
// ----------------------------------------------------------------------------

AxSetup::AxSetup( wxBitmap logo )
        : wxWizard( NULL, wxID_ANY, _("Aruspix setup"),
                   logo, wxDefaultPosition,
                   wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    // a wizard page may be either an object of predefined class
	m_language = -1;
    m_page1 = new AxSetupPage1(this, &m_language);


#if defined(__WXMSW__) // to tired to fix this on Windows... language only
    //wxWizardPageSimple::Chain(m_page1);
#else // OS X
    AxSetupPage2 *page2 = new AxSetupPage2(this);
    wxWizardPageSimple::Chain(m_page1, page2);;
#endif


}

