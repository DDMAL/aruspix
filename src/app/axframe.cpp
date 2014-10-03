/////////////////////////////////////////////////////////////////////////////
// Name:        axframe.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
//#include "wx/wxprec.h"
#include "wx/wx.h"

#include "wx/config.h"
#include "wx/stdpaths.h"
#include "wx/gdicmn.h"
#include "wx/utils.h"

#include "axapp.h"
#include "axfile.h"
#include "axframe.h"
#include "resource.h"
#include "axenv.h"
#include "axprogressdlg.h"
#include "axoptionsdlg.h"
#include "aximage.h"

#include "im/impage.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(AxEnvArray);

#ifdef AX_RECOGNITION
    #include "recognition/rec.h"
#endif

#ifdef AX_EDITION
    #include "edition/edt.h"
#endif

#ifdef AX_SUPERIMPOSITION
    #include "superimposition/sup.h"
#endif

#ifdef AX_COMPARISON
    #include "comparison/cmp.h"
#endif


// MIDI
DEFINE_EVENT_TYPE( AX_EVT_MIDI )

#define MIDI_ON 144
#define MIDI_OFF 128

// MIDI CALLBACK
/*
void midi_callback( double deltatime, std::vector< unsigned char > *message, void *userData )
{
  unsigned int nBytes = message->size();
  for ( unsigned int i=0; i<nBytes; i++ )
    std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
  if ( nBytes > 0 )
    std::cout << "stamp = " << deltatime << std::endl;
  
  
  // return if no top window to send the message
  if (!wxGetApp().GetTopWindow()) {
    return;
  }

  // we just want the pitch - with is in byte 1 - and only at the beginning of the note
  if ((message->size() < 1) || ((int)message->at(0) != MIDI_ON)) {
    return;
  }
  
  // because I did not want to create a new type of event at this stage (maybe later), I just
  // use a wxCommandEvent an store the midi pitch with SetInt() 
  wxCommandEvent event( AX_EVT_MIDI, ID_MIDI_INPUT );
  event.SetInt( (int)message->at(1) );
  wxPostEvent( wxGetApp().GetTopWindow(), event );
}
*/


//----------------------------------------------------------------------------
// AxFrame
//----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(AxFrame,wxFrame)




BEGIN_EVENT_TABLE(AxFrame,wxFrame)
    EVT_CLOSE( AxFrame::OnClose)
    EVT_SIZE( AxFrame::OnSize )
    // UPDATE UI
    EVT_UPDATE_UI_RANGE( ENV_BASE_ID, ENV_BASE_ID + ENV_MAX , AxFrame::OnUpdateUIEnvs )
    EVT_UPDATE_UI_RANGE( ENV_IDS_MIN, ENV_IDS_MAX , AxFrame::OnUpdateUIEnvironmentMenu )
    EVT_UPDATE_UI( ID_UNDO, AxFrame::OnUpdateUIEnvironmentMenu )
    EVT_UPDATE_UI( ID_REDO, AxFrame::OnUpdateUIEnvironmentMenu )
    EVT_UPDATE_UI( ID_PASTE, AxFrame::OnUpdateUIEnvironmentMenu )
    EVT_UPDATE_UI( ID_COPY, AxFrame::OnUpdateUIEnvironmentMenu )
    EVT_UPDATE_UI( ID_CUT, AxFrame::OnUpdateUIEnvironmentMenu )
    EVT_UPDATE_UI( ID_NEW, AxFrame::OnUpdateUIEnvironmentMenu )
    EVT_UPDATE_UI( ID_OPEN, AxFrame::OnUpdateUIEnvironmentMenu )
    EVT_UPDATE_UI( ID_SAVE, AxFrame::OnUpdateUIEnvironmentMenu )
    EVT_UPDATE_UI( ID_SAVE_AS, AxFrame::OnUpdateUIEnvironmentMenu )
    EVT_UPDATE_UI( ID_CLOSE, AxFrame::OnUpdateUIEnvironmentMenu )
    // MENU
    EVT_MENU( ID_MENU_OPTIONS, AxFrame::OnOptions )
    EVT_MENU( ID_ABOUT, AxFrame::OnAbout )
    EVT_MENU(wxID_EXIT, AxFrame::OnQuit )
    EVT_MENU_RANGE( ENV_BASE_ID, ENV_BASE_ID + ENV_MAX , AxFrame::OnEnvironments )
    EVT_MENU_RANGE(ENV_IDS_MIN ,ENV_IDS_MAX, AxFrame::OnEnvironmentMenu )
    // redirection de la mise � jour du changement de sash
    EVT_CUSTOM_RANGE( wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED, ENV_IDS_MIN ,ENV_IDS_MAX, AxFrame::OnCustomMenu )
    // redirections des sliders de barre de menu
#if defined(__WXMSW__) 
    EVT_CUSTOM_RANGE( wxEVT_SCROLL_ENDSCROLL, ENV_IDS_MIN ,ENV_IDS_MAX, AxFrame::OnCustomMenu )
    //EVT_CUSTOM_RANGE( wxEVT_COMMAND_SLIDER_UPDATED, ENV_IDS_MIN ,ENV_IDS_MAX, AxFrame::OnCustomMenu )
#else
    EVT_CUSTOM_RANGE( wxEVT_COMMAND_SLIDER_UPDATED, ENV_IDS_MIN ,ENV_IDS_MAX, AxFrame::OnCustomMenu )
#endif
    EVT_MENU( ID_UNDO, AxFrame::OnEnvironmentMenu )
    EVT_MENU( ID_REDO, AxFrame::OnEnvironmentMenu )
    EVT_MENU( ID_PASTE, AxFrame::OnEnvironmentMenu )
    EVT_MENU( ID_COPY, AxFrame::OnEnvironmentMenu )
    EVT_MENU( ID_CUT, AxFrame::OnEnvironmentMenu )
    EVT_MENU( ID_NEW, AxFrame::OnEnvironmentMenu )
	EVT_MENU( ID_OPEN, AxFrame::OnEnvironmentMenu )
    EVT_MENU( ID_SAVE, AxFrame::OnEnvironmentMenu )
    EVT_MENU( ID_SAVE_AS, AxFrame::OnEnvironmentMenu )
    EVT_MENU( ID_CLOSE, AxFrame::OnEnvironmentMenu )
    EVT_MENU( ID_HELP, AxFrame::OnHelp )
    // MIDI
    EVT_COMMAND( ID_MIDI_INPUT, AX_EVT_MIDI, AxFrame::OnEnvironmentMenu )
END_EVENT_TABLE()


AxFrame::AxFrame( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxFrame( parent, id, title, position, size, style ) 
{

#ifdef __WXMAC__
	wxApp::s_macAboutMenuItemId = ID_ABOUT;
	wxApp::s_macPreferencesMenuItemId = ID_MENU_OPTIONS;
	wxApp::s_macHelpMenuTitleName = _("&Help");
#else
    // set the frame icon
    SetIcon(wxICON(ax));
#endif

    m_env = NULL;
    m_envRow = NULL;
        
    m_menuBarPtr = NULL;
    m_toolBarPtr = NULL;

    m_midiIn = NULL;

    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar();

    // create menubar
    m_menuBarPtr = MainMenuBarFunc();
    SetMenuBar( m_menuBarPtr );
    
    wxASSERT_MSG(m_menuBarPtr,"Menubar cannot be NULL");


    //m_toolBarPtr->AddSeparator();
    int menuId = ENV_BASE_ID;
    int menuPos = 0;
    AxEnvRow *envRow = NULL;
    wxMenu *menuEnv = m_menuBarPtr->GetMenu(0)->FindItem(ID_MENU_ENV)->GetSubMenu();

    wxASSERT_MSG(menuEnv,"Environment menu cannot be NULL");


#ifdef AX_SUPERIMPOSITION
    envRow = new AxEnvRow( "SupEnv", _("Superimposition"), menuId++, _("Preparation and superimpostition workspace") );
    menuEnv->Insert( menuPos++, envRow->m_menuId, envRow->m_menuItem, envRow->m_helpStr, wxITEM_CHECK );
    m_envArray.Add( envRow );
	SupEnv::LoadConfig();
#endif //AX_SUPERIMPOSITION

#ifdef AX_RECOGNITION
    envRow = new AxEnvRow( "RecEnv", _("Recognition"), menuId++, _("Recognition workspace") );
    menuEnv->Insert( menuPos++, envRow->m_menuId, envRow->m_menuItem, envRow->m_helpStr, wxITEM_CHECK );
    m_envArray.Add( envRow );
	RecEnv::LoadConfig();
#endif //AX_RECOGNITION

#ifdef AX_COMPARISON
    envRow = new AxEnvRow( "CmpEnv", _("Collation"), menuId++, _("Sources collation workspace") );
    menuEnv->Insert( menuPos++, envRow->m_menuId, envRow->m_menuItem, envRow->m_helpStr, wxITEM_CHECK );
    m_envArray.Add( envRow );
	CmpEnv::LoadConfig();
#endif

#ifdef AX_EDITION
    envRow = new AxEnvRow( "EdtEnv", _("Edition"), menuId++, _("Edition workspace") );
    menuEnv->Insert( menuPos++, envRow->m_menuId, envRow->m_menuItem, envRow->m_helpStr, wxITEM_CHECK );
    m_envArray.Add( envRow );
	EdtEnv::LoadConfig();
#endif //AX_EDITION


    RealizeToolbar();
    
    // load config
    this->LoadConfig();

    // counter callback
    imCounterSetCallback(NULL, NULL);
}

AxFrame::~AxFrame()
{
#ifdef AX_RECOGNITION
	RecEnv::SaveConfig();
#endif //AX_RECOGNITION

#ifdef AX_SUPERIMPOSITION
	SupEnv::SaveConfig();
#endif //AX_SUPERIMPOSITION

#ifdef AX_EDITION
	EdtEnv::SaveConfig();
#endif //AX_EDITION

#ifdef AX_COMPARISON
	CmpEnv::SaveConfig();
#endif //AX_COMPARISON
}

void AxFrame::RealizeToolbar()
{
	if ( m_toolBarPtr )
		m_toolBarPtr->ClearTools();
	else
	{
		m_toolBarPtr = this->CreateToolBar( wxTB_HORIZONTAL | wxNO_BORDER  | wxTB_TEXT /*| wxTB_FLAT */, -1);
		int size = wxGetApp().m_toolbar_toolsize;
#ifdef __WXMAC__
		if (wxGetApp().m_toolbar_toolsize == 16)  // 16 icons are enlarged, don't know why...
			size = 24;
#endif
		m_toolBarPtr->SetToolBitmapSize( wxSize( size, size ) );
	}
    
    m_toolBarPtr->AddTool( ID_NEW, _("New"), GetToolbarBitmap( "filenew.png" ) , wxNullBitmap, wxITEM_NORMAL, _("New"), _("Create a new file") );
    m_toolBarPtr->AddTool( ID_OPEN, _("Open"), GetToolbarBitmap( "folder_yellow.png" ) , wxNullBitmap, wxITEM_NORMAL, _("Open"), _("Open a file") );
    m_toolBarPtr->AddSeparator();
    m_toolBarPtr->AddTool( ID_SAVE, _("Save"), GetToolbarBitmap( "filesave.png" ) , wxNullBitmap, wxITEM_NORMAL, _("Save"), _("Save the file") );
    m_toolBarPtr->AddSeparator();
    m_toolBarPtr->AddTool( ID_CUT, _("Cut"), GetToolbarBitmap( "editcut.png" ) , wxNullBitmap, wxITEM_NORMAL, _("Cut"), _("Cut the image and place it in the clipboard") );
    m_toolBarPtr->AddTool( ID_COPY, _("Copy"), GetToolbarBitmap( "editcopy.png" ) , wxNullBitmap, wxITEM_NORMAL, _("Copy"), _("Copy the image in the clipboard") );
    m_toolBarPtr->AddTool( ID_PASTE, _("Paste"), GetToolbarBitmap( "editpaste.png" ) , wxNullBitmap, wxITEM_NORMAL, _("Paste"), _("Paste the image from clipboard") );
    m_toolBarPtr->AddSeparator();
    m_toolBarPtr->AddTool( ID_UNDO, _("Undo"), GetToolbarBitmap( "undo.png" ) , wxNullBitmap, wxITEM_NORMAL, _("Undo"), _("Undo last operation") );
    m_toolBarPtr->AddTool( ID_REDO, _("Redo"), GetToolbarBitmap( "redo.png" ) , wxNullBitmap, wxITEM_NORMAL, _("Redo"), _("Redo last operation") );
	
	/*
	
	AxEnvRow *envRow;
	int pos;
	
#ifdef AX_RECOGNITION
	GetEnvironmentMenuId( "RecEnv", &pos );
	if ( pos != -1 )
	{
		envRow = &m_envArray[pos];
		m_toolBarPtr->AddTool( envRow->m_menuId, EnvBitmapFunc(3), wxNullBitmap, true, NULL, envRow->m_menuItem, envRow->m_helpStr );
    }
#endif //AX_RECOGNITION

#ifdef AX_SUPERIMPOSITION
    GetEnvironmentMenuId( "SupEnv", &pos );
	if ( pos != -1 )
	{
		envRow = &m_envArray[pos];
		m_toolBarPtr->AddTool( envRow->m_menuId, EnvBitmapFunc(1), wxNullBitmap, true, NULL, envRow->m_menuItem, envRow->m_helpStr );
    }
#endif //AX_SUPERIMPOSITION

#ifdef AX_EDITION
	GetEnvironmentMenuId( "EdtEnv", &pos );
	if ( pos != -1 )
	{
		envRow = &m_envArray[pos];
		m_toolBarPtr->AddTool( envRow->m_menuId, EnvBitmapFunc(4), wxNullBitmap, true, NULL, envRow->m_menuItem, envRow->m_helpStr );
	}
#endif //AX_EDITION

	*/
    
    //m_toolBarPtr->Realize();
}

wxBitmap AxFrame::GetToolbarBitmap( wxString name )
{
	int size = wxGetApp().m_toolbar_toolsize;
#ifdef __WXMAC__
	if (wxGetApp().m_toolbar_toolsize == 16)  // 16 icons are enlarged, don't know why...
		size = 32;	// use 32 ones, and they will be reduced to 24 (better than enlarged 16)
#endif
	wxString subdir = wxString::Format("/icons/%dx%d/", size, size );
	wxBitmap bitmap ( wxGetApp().m_resourcesPath + subdir + name , wxBITMAP_TYPE_PNG );
	return bitmap;
}


void AxFrame::InitMidi()
{
/*
    // midi input
    m_midiIn = new RtMidiIn();
	wxString portName;
	
    // Check available ports.
    unsigned int nPorts = m_midiIn->getPortCount();
	m_midiIn->openVirtualPort("test");
	
	for ( unsigned int i=0; i<nPorts; i++ ) {
		try {
			portName = m_midiIn->getPortName(i);
		}
		catch ( RtError &error ) {
			error.printMessage();
		}
		std::cout << "  Input Port #" << i+1 << ": " << portName << '\n';
	}
	

    if ( nPorts == 0 ) {
        wxLogDebug("Midi init failed, nPorts == 0");
        delete m_midiIn;
        m_midiIn = NULL;
    } else {
        m_midiIn->openPort( 0 );
        // Set our callback function
        m_midiIn->setCallback( &midi_callback );
        // Don't ignore sysex, timing, or active sensing messages.
        m_midiIn->ignoreTypes( false, false, false );        
    }
*/
}


void AxFrame::CheckOptions( )
{
	if ( !AxApp::CheckDir( wxGetApp().m_docDir, 0755 ) )
		wxLogWarning( _("Unable to create or write in the documents directory '%s'. Please verify the access permissions or change it in the preferences."), 
			wxGetApp().m_docDir.c_str() );
	if ( !AxApp::CheckDir( wxGetApp().m_workingDir, 0755 ) )
		wxLogWarning( _("Unable to create or write in the working directory '%s'. Please verify the access permissions or change it in the preferences."), 
			wxGetApp().m_workingDir.c_str() );
	if ( !AxApp::CheckDir( wxGetApp().m_logDir, 0755 ) )
		wxLogDebug( _("Unable to create or write in the log directory '%s'. Please verify the access permissions."), wxGetApp().m_logDir.c_str() );
	else
		AxDirTraverser clean( wxGetApp().m_logDir );
}


void AxFrame::LoadConfig()
{
    wxConfigBase *pConfig = wxConfigBase::Get();
    wxASSERT_MSG( pConfig, wxT("pConfig cannot be NULL") );
    pConfig->SetPath("/");
    // frame size and position
	int width, height;
	int val1,val2;
	wxDisplaySize(&width, &height);
	pConfig->Read( "Width", &val1, width - 150);
    pConfig->Read( "Height", &val2, height - 150);
    this->SetSize(val1,val2);
    pConfig->Read("PX",&val1,10);
    pConfig->Read("PY",&val2,30);
    this->Move(val1,val2);
    if (1 == pConfig->Read("Maximize",0L))
        this->Maximize();
    
//	int val1,val2;
//    pConfig->Read("Width",&val1,800);
//    pConfig->Read("Height",&val2,650);
//   this->SetSize(val1,val2);
//    pConfig->Read("PX",&val1,50);
//    pConfig->Read("PY",&val2,50);
//    this->Move(val1,val2);
//    if (1 == pConfig->Read("Maximize",0L))
//        this->Maximize();

    //images
    AxImage::s_zoomInterpolation = (pConfig->Read("Gray",0L)==1);
    AxImage::s_reduceBigImages = (pConfig->Read("ReduceBigImages",0L)==1);
    AxImage::s_imageSizeToReduce = pConfig->Read("ImageSizeToReduce",3000);
	AxImage::s_checkIfNegative = (pConfig->Read("ImageCheckIfNegative",0L)==1);
	
	// binarization parameters
	ImOperator::s_pre_image_binarization_method = pConfig->Read( "Image binarization method", IM_BINARIZATION_BRINK );
    ImPage::s_pre_page_binarization_method = pConfig->Read( "Page binarization method", PRE_BINARIZATION_BRINK );
    ImPage::s_pre_page_binarization_method_size = pConfig->Read( "Binarization region size", 15 );
	ImPage::s_pre_page_binarization_select = ( pConfig->Read( "Binarization selector dialogue", 1L ) == 1 );
	
	// Number of lines per staff
	ImPage::s_num_staff_lines = pConfig->Read("Lines per staff", 5);

#if defined(__WXMSW__)
	wxString default_workingDir = wxStandardPaths::Get().GetTempDir();		
	wxString default_docDir = wxStandardPaths::Get().GetDocumentsDir();
	wxString default_images = default_docDir + "/Images";
	wxString default_pages = default_docDir + "/Pages";
	wxString default_models = default_docDir + "/Models";
#else // OS X / Linux
	#if defined(__linux__)
	    // wx still doesn't support XDG directories:
	    // http://trac.wxwidgets.org/ticket/9300
		wxString xdg_data;
		wxString default_workingDir;
		if (wxGetEnv("XDG_DATA_HOME", &xdg_data)) {
			default_workingDir = xdg_data + "/aruspix";
		} else {
			wxString home_dir;
			wxGetEnv("HOME", &home_dir);
			default_workingDir = home_dir + "/.local/share/aruspix";
		}
	#else // OSX
		wxString default_workingDir = wxGetHomeDir() + "/.aruspix"; // on OS X
	#endif
	wxString default_docDir = wxGetHomeDir() + "/Documents/Aruspix.localized"; // on OS X
	wxString default_images = default_docDir + "/Images.localized";
	wxString default_pages = default_docDir + "/Pages.localized";
	wxString default_models = default_docDir + "/Models.localized";
#endif
	
	//wxGetApp().m_workingDir = pConfig->Read("WorkingDir", wxGetApp().m_path );
	wxGetApp().m_workingDir = pConfig->Read("WorkingDir", default_workingDir );
	wxGetApp().m_docDir = pConfig->Read("DocumentsDir", default_docDir );
	
	wxGetApp().m_lastDir = pConfig->Read("LastDir", default_docDir );
	wxGetApp().m_lastDirAX0_in = pConfig->Read("LastDirAX0_in", wxGetApp().m_lastDir );
	wxGetApp().m_lastDirAX0_out = pConfig->Read("LastDirAX0_out", wxGetApp().m_lastDir );
	wxGetApp().m_lastDirAX1_in = pConfig->Read("LastDirAX1_in", wxGetApp().m_lastDir );
	wxGetApp().m_lastDirAX1_out = pConfig->Read("LastDirAX1_out", wxGetApp().m_lastDir );
	wxGetApp().m_lastDirAX2_in = pConfig->Read("LastDirAX2_in", wxGetApp().m_lastDir );
	wxGetApp().m_lastDirAX2_out = pConfig->Read("LastDirAX2_out", wxGetApp().m_lastDir );
	wxGetApp().m_lastDirAX3_in = pConfig->Read("LastDirAX3_in", wxGetApp().m_lastDir );
	wxGetApp().m_lastDirAX3_out = pConfig->Read("LastDirAX3_out", wxGetApp().m_lastDir );
	wxGetApp().m_lastDirAX4_in = pConfig->Read("LastDirAX4_in", wxGetApp().m_lastDir );
	wxGetApp().m_lastDirAX4_out = pConfig->Read("LastDirAX4_out", wxGetApp().m_lastDir );
	wxGetApp().m_lastDirTIFF_in = pConfig->Read("LastDirTIFF_in", default_images );
	wxGetApp().m_lastDirTIFF_out = pConfig->Read("LastDirTIFF_out", default_pages );
	wxGetApp().m_lastDirBatch_in = pConfig->Read("LastDirBatch_in", default_images );
	wxGetApp().m_lastDirBatch_out = pConfig->Read("LastDirBatch_out", default_pages );
	wxGetApp().m_lastDirMusModel_in = pConfig->Read("LastDirModel_in", default_models );
	wxGetApp().m_lastDirTypModel_in = pConfig->Read("LastDirTypModel_in", default_models );

#if defined(__WXMSW__)
	wxGetApp().m_musicFontDesc = pConfig->Read("MusicFontDesc", "0;-13;0;0;0;400;0;0;0;2;3;2;1;2;Leipzig 4.9" );
	wxGetApp().m_lyricFontDesc = pConfig->Read("LyricFontDesc", "0;-13;0;0;0;400;0;0;0;2;3;2;1;2;Leipzig 4.3" ); // To be verified
#else // OS X
    // From version 2.0.0, with windows encoding (33, should be wxFONTENCODING_CP1252, WinLatin1)
    // From version 2.0.0, get version 4.9
	wxGetApp().m_musicFontDesc = pConfig->Read("MusicFontDesc-2.0.0", "0;13;70;90;90;0;Leipzig 4.9;33" ); // on OS X
	wxGetApp().m_neumeFontDesc = pConfig->Read("NeumeFontDesc", "0;53;70;90;90;0;Festa Dies A;0" );
	wxGetApp().m_lyricFontDesc = pConfig->Read("LyricFontDesc", "0;12;70;93;90;0;Garamond;0" );
#endif
    pConfig->Read("FontSizeCorrection",&wxGetApp().m_fontSizeCorrection,100);
    pConfig->Read("FontPosCorrection",&wxGetApp().m_fontPosCorrection,0);

	// check directories
	this->CheckOptions();
	
    // last environment
    pConfig->Read("EnvID", &val1, ENV_BASE_ID);
    this->SetEnvironment(val1);

	// load last execution time
	AxProgressDlg::LoadValues( pConfig );

    pConfig->SetPath("/");
}
	

void AxFrame::SaveConfig(int lastEnvId)
{
    /*pConfig->SetPath("/Email");
    pConfig->Write("Smtp",GB::s_mailSmtp);
    pConfig->Write("DefaultSender",GB::s_mailDefaultSender);
    pConfig->Write("DefaultBcc",GB::s_mailDefaultBcc);
    pConfig->Write("Signature",GB::s_mailSignature);
    pConfig->Write("Port",atoi(GB::s_mailPortStr));
    pConfig->Write("Split",atoi(GB::s_mailSplitStr));
    pConfig->Write("IncludeNoPub",GB::s_mailIncludeNoPub);

    pConfig->SetPath("/Images");
    pConfig->Write("Enable",GB::s_imagesEnable);
    pConfig->Write("Server",GB::s_imagesServer);
    pConfig->Write("Type",GB::s_imagesType);

    pConfig->SetPath("/Encode");
    pConfig->Write("Value1",GB::s_key1);
    pConfig->Write("Value2",GB::s_key2);*/
    // save config
    wxConfigBase *pConfig = wxConfigBase::Get();
    wxASSERT_MSG( pConfig, wxT("pConfig cannot be NULL") );
    pConfig->SetPath("/");
    // frame size and position
    bool max= (this->IsMaximized());
    if (max)
        this->Restore();
    pConfig->Write("Maximize",max);
    wxSize fsize = this->GetSize();
	int width, height;
	wxDisplaySize( &width, &height );
	if ( ( fsize.GetWidth() < width - 50 ) && ( fsize.GetHeight() < height - 50 ) ) {
		pConfig->Write("Width",fsize.GetWidth());
		pConfig->Write("Height",fsize.GetHeight());
    }
	wxPoint fpos = this->GetPosition();
    if ( ( fpos.x < width - 50 ) && ( fpos.y < height - 50 ) ){
		pConfig->Write("PX",fpos.x);
		pConfig->Write("PY",fpos.y);
    }
	// last environment
    pConfig->Write("EnvID",lastEnvId);

    // images
    pConfig->Write("Gray", AxImage::s_zoomInterpolation);
    pConfig->Write("ReduceBigImages", AxImage::s_reduceBigImages);
    pConfig->Write("ImageSizeToReduce", AxImage::s_imageSizeToReduce);
	pConfig->Write("ImageCheckIfNegative", AxImage::s_checkIfNegative);

	// Number of lines per staff
	pConfig->Write("Lines per staff", ImPage::s_num_staff_lines);
	
	// binarization parameters
	pConfig->Write("Image binarization method", ImOperator::s_pre_image_binarization_method );
    pConfig->Write("Page binarization method", ImPage::s_pre_page_binarization_method );
    pConfig->Write("Binarization region size", ImPage::s_pre_page_binarization_method_size );	
	pConfig->Write("Binarization selector dialogue", ImPage::s_pre_page_binarization_select );		
	
	// system
	pConfig->Write("DocumentsDir", wxGetApp().m_docDir );
	pConfig->Write("WorkingDir", wxGetApp().m_workingDir );
	pConfig->Write("LastDir", wxGetApp().m_lastDir );

	pConfig->Write("LastDirAX0_in", wxGetApp().m_lastDirAX0_in );
	pConfig->Write("LastDirAX0_out", wxGetApp().m_lastDirAX0_out );
	pConfig->Write("LastDirAX1_in", wxGetApp().m_lastDirAX1_in );
	pConfig->Write("LastDirAX1_out", wxGetApp().m_lastDirAX1_out );
	pConfig->Write("LastDirAX2_in", wxGetApp().m_lastDirAX2_in );
	pConfig->Write("LastDirAX2_out", wxGetApp().m_lastDirAX2_out );
	pConfig->Write("LastDirAX3_in", wxGetApp().m_lastDirAX3_in );
	pConfig->Write("LastDirAX3_out", wxGetApp().m_lastDirAX3_out );
	pConfig->Write("LastDirAX4_in", wxGetApp().m_lastDirAX4_in );
	pConfig->Write("LastDirAX4_out", wxGetApp().m_lastDirAX4_out );
	pConfig->Write("LastDirTIFF_in", wxGetApp().m_lastDirTIFF_in );
	pConfig->Write("LastDirTIFF_out", wxGetApp().m_lastDirTIFF_out );
	pConfig->Write("LastDirBatch_in", wxGetApp().m_lastDirBatch_in );
	pConfig->Write("LastDirBatch_out", wxGetApp().m_lastDirBatch_out );
	pConfig->Write("LastDirMusModel_in", wxGetApp().m_lastDirMusModel_in );
	pConfig->Write("LastDirTypModel_in", wxGetApp().m_lastDirTypModel_in );

	pConfig->Write("MusicFontDesc-2.0.0", wxGetApp().m_musicFontDesc );
	pConfig->Write("LyricFontDesc", wxGetApp().m_lyricFontDesc );
    pConfig->Write("FontSizeCorrection", wxGetApp().m_fontSizeCorrection );
    pConfig->Write("FontPosCorrection", wxGetApp().m_fontPosCorrection );

	// load last execution time
	AxProgressDlg::SaveValues( pConfig );

    // toolbar tool size and language - save only, loaded by AxApp before creating frame
	pConfig->Write("ToolbarToolsize", wxGetApp().m_toolbar_toolsize );
    pConfig->Write("Language", wxGetApp().m_language );

    pConfig->SetPath("/");
}


void AxFrame::ParseCmd( wxCmdLineParser *parser )
{
    wxString env;
    if ( parser->Found("e",&env) )
	{
        this->SetEnvironment( env + "Env" );
		if (m_env)
		{
			m_env->ParseCmd( parser );
		}
	}
	else if ( parser->GetParamCount() == 1 )
		this->Open( parser->GetParam( 0 ) );
}

void AxFrame::Open( wxString filename )
{
	int type, envtype;
	if ( !AxFile::Check( filename, &type, &envtype ))
		return;

	wxString className = AxFile::GetEnvName( envtype );
	int pos;
	int menuId = this->GetEnvironmentMenuId( className + "Env" , &pos );
	if ( menuId != -1 )
	{
        this->SetEnvironment( menuId );
		if ( m_env->CloseAll( ) )
			m_env->Open( filename, type );
	}
	else
		wxLogMessage( _("The enviromnent needed to open this file doesn't exists in this version of Aruspix") );

}

wxString AxFrame::GetEnvironmentName( )
{
    if (!m_envRow)
        return "";
    else
        return m_envRow->m_menuItem;
}

int AxFrame::GetEnvironmentMenuId( const wxString className, int *pos )
{
    for (int i=0; i < (int)m_envArray.GetCount(); i++)
    {
        AxEnvRow *envRow = &m_envArray[i];
        //wxLogDebug("%s, %s", className.c_str(), envRow->m_className.c_str());
        if ( className.CmpNoCase( envRow->m_className ) == 0 )
		{
			*pos = i;
			return envRow->m_menuId;
		}
    }
	*pos = -1;
	return -1;
}

void AxFrame::SetEnvironment( const wxString className )
{
	int pos;
	int menuId = this->GetEnvironmentMenuId( className, &pos );
	if ( menuId != -1 )
        this->SetEnvironment( menuId );
}

void AxFrame::SetEnvironment( int menuId )
{
    if (m_envRow && (m_envRow->m_menuId == menuId))
        return; // already selected;

    for (int i=0; i < (int)m_envArray.GetCount(); i++)
    {
        AxEnvRow *envRow = &m_envArray[i];
        if (envRow->m_menuId != menuId)
            continue; // not the right id

        // hide previous environment
        if (m_env && m_env->m_isShown) m_env->Hide();

        // create new environemnt if first time
        if (!envRow->m_envPtr)
            envRow->m_envPtr = (AxEnv*)wxCreateDynamicObject(envRow->m_className);

        if (!envRow->m_envPtr)
        {
            m_env = NULL;
            m_envRow = NULL;
            wxLogError(_("Error loading environment '%s'"),envRow->m_className.c_str());
            return;
        }

        m_env = envRow->m_envPtr;
        m_envRow = envRow;
        if (!m_env->m_isLoaded) m_env->Load(this);	
        if (!m_env->m_isShown) m_env->Show();
	}
}


void AxFrame::OnHelp( wxCommandEvent &event )
{
    wxASSERT_MSG( wxGetApp().m_help , "Help controller cannot be NULL ");

    wxString help = "Aruspix";
    if ( m_envRow )
        help = m_envRow->m_menuItem.c_str();

    wxGetApp().m_help->Display( help );  
}

void AxFrame::OnOptions( wxCommandEvent &event )
{
    AxOptionsDlg *dlg = new AxOptionsDlg(this, -1, _("Options"));
    dlg->Center(wxBOTH);
    dlg->ShowModal();
    dlg->Destroy();
	
	CheckOptions( ); 
}

void AxFrame::OnSize(wxSizeEvent& event)
{
    if (m_env && m_env->m_isShown)
        m_env->Resize();
    else
        event.Skip();
}

void AxFrame::OnCustomMenu( wxEvent &event )
{
    if (!m_env)
        return;
    else
        m_env->ProcessEvent(event);
}

void AxFrame::OnEnvironmentMenu( wxCommandEvent &event )
{
	if ( event.GetId() == ID_OPEN )
	{
		wxString filename = AxFile::Open( AX_FILE_DEFAULT );
		Open( filename );
	}
    else if (!m_env)
        return;
    else
        m_env->ProcessEvent(event);
}

void AxFrame::OnEnvironments( wxCommandEvent &event )
{
    this->SetEnvironment( event.GetId());
}

void AxFrame::OnUpdateUIEnvironmentMenu( wxUpdateUIEvent &event )
{
    if (!m_env)
        event.Enable( false );
    else
        m_env->ProcessEvent(event);
	//event.Skip();
}

void AxFrame::OnUpdateUIEnvs( wxUpdateUIEvent &event )
{
    if (m_envRow && (m_envRow->m_menuId==event.GetId()))
        event.Check(true);
    else
        event.Check(false);     
}

void AxFrame::OnQuit( wxCommandEvent &event )
{
    this->Close();
}

void AxFrame::OnClose( wxCloseEvent &event )
{
    int lastEnvId = 0, i;
    if (m_env && m_env->m_isShown)
    {
		if ( !m_env->CloseAll( ) ) 
			return;
        m_env->Hide();
        lastEnvId = m_envRow->m_menuId;
    }
	
	// check if all allow to close
	// doesn't work if a file is open in a hidden environment...
    for (i=0; i < (int)m_envArray.GetCount(); i++)
    {
        AxEnv *env = (AxEnv*)(m_envArray[i]).m_envPtr;
        if (env && !env->CloseAll( ))
			return;
    }

    for (i=0; i < (int)m_envArray.GetCount(); i++)
    {
        AxEnv *env = (AxEnv*)(m_envArray[i]).m_envPtr;
        if (env && env->m_isLoaded)
			env->Unload();
    }
    m_envArray.Clear();
    m_env = NULL;
    m_envRow = NULL;

    this->SaveConfig(lastEnvId);
    event.Skip();
}

void AxFrame::OnAbout( wxCommandEvent &event )
{
    AxAboutDlg *dlg = new AxAboutDlg(this,-1,_("About ..."));
    dlg->Center(wxBOTH);
    dlg->ShowModal();
    dlg->Destroy();
}





