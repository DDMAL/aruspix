/////////////////////////////////////////////////////////////////////////////
// Name:        edt.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_EDT

#ifdef __GNUG__
    #pragma implementation "edt.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/dirctrl.h"
#include "wx/calctrl.h"

#include "edt.h"
#include "edtfile.h"

#include "app/axframe.h"
#include "app/axapp.h"

#include "mus/muswindow.h"
#include "mus/musfile.h"
#include "mus/mustoolpanel.h"
#include "mus/musiowwg.h"
#include "mus/musiomlf.h"
#include "mus/musiomei.h"


// WDR: class implementations

//----------------------------------------------------------------------------
// EdtPanel
//----------------------------------------------------------------------------

// WDR: event table for EdtPanel

BEGIN_EVENT_TABLE(EdtPanel,wxPanel)
    //EVT_TREE_SEL_CHANGED( ID0_DIRECTORIES, EdtPanel::OnTree )
    EVT_SIZE( EdtPanel::OnSize )
END_EVENT_TABLE()

EdtPanel::EdtPanel( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{

}

// WDR: handler implementations for EdtPanel

void EdtPanel::OnTree( wxTreeEvent &event )
{
    wxMessageBox("TREE");
}

void EdtPanel::OnSize( wxSizeEvent &event )
{
	this->Layout();
    wxWindow *w = FindWindowById( ID5_MUSWINDOW );
    if ( w && w->IsKindOf( CLASSINFO( MusWindow ) ))
        ((MusWindow*)w)->Resize( );

    event.Skip();
}

//----------------------------------------------------------------------------
// EdtEnv
//----------------------------------------------------------------------------

// WDR: event table for EdtEnv

BEGIN_EVENT_TABLE(EdtEnv,AxEnv)
    // file
    EVT_MENU( ID_CLOSE, EdtEnv::OnClose )
    EVT_MENU( ID_SAVE_AS, EdtEnv::OnSaveAs )
    EVT_MENU( ID_SAVE, EdtEnv::OnSave )
    //EVT_MENU( ID_OPEN, EdtEnv::OnOpen )
    EVT_MENU( ID_NEW, EdtEnv::OnNew ) 
    //
    EVT_UPDATE_UI_RANGE( wxID_LOWEST, ENV_IDS_MAX, EdtEnv::OnUpdateUI )
	EVT_MENU( ID_UNDO, EdtEnv::OnUndo )
	EVT_MENU( ID_REDO, EdtEnv::OnRedo )
    EVT_MENU( ID5_NEXT, EdtEnv::OnNext )
    EVT_MENU( ID5_PREVIOUS, EdtEnv::OnPrevious )
    EVT_MENU( ID5_GOTO, EdtEnv::OnGoto )
    EVT_MENU( ID5_ZOOMOUT, EdtEnv::OnZoomOut )
    EVT_MENU( ID5_ZOOMIN, EdtEnv::OnZoomIn )
	EVT_MENU( ID5_SAVE_MODEL, EdtEnv::OnSaveModel )
    EVT_MENU( ID5_SAVE_MLF, EdtEnv::OnSaveMLF )
    EVT_MENU( ID5_OPEN_MLF, EdtEnv::OnOpenMLF )
    EVT_MENU( ID5_SAVE_WWG, EdtEnv::OnSaveWWG )
    EVT_MENU( ID5_OPEN_WWG, EdtEnv::OnOpenWWG )
    EVT_MENU( ID5_SAVE_SVG, EdtEnv::OnSaveSVG )
    EVT_MENU( ID5_SAVE_MEI, EdtEnv::OnSaveMEI )
    EVT_MENU( ID5_OPEN_MEI, EdtEnv::OnOpenMEI )
    EVT_MENU( ID5_VOICES, EdtEnv::OnValues )
    EVT_MENU( ID5_INDENT, EdtEnv::OnValues )
    EVT_MENU_RANGE( ID5_INSERT_MODE, ID5_SYMBOLES, EdtEnv::OnTools )
    EVT_COMMAND( ID_MIDI_INPUT, AX_EVT_MIDI, EdtEnv::OnMidiInput )
	//EVT_SIZE( EdtEnv::OnSize )
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(EdtEnv,AxEnv)

EdtEnv::EdtEnv():
    AxEnv()
{
    m_panelPtr = NULL;
    m_edtFilePtr = new EdtFile( "edt_env_file", this ); 
    m_musViewPtr = NULL;   

    this->m_envMenuBarFuncPtr = MenuBarFunc5;
}

EdtEnv::~EdtEnv()
{
    if (m_envWindowPtr) m_envWindowPtr->Destroy();
    m_envWindowPtr = NULL;

    if ( m_edtFilePtr )
        delete m_edtFilePtr;
}

void EdtEnv::LoadWindow()
{
    this->m_envWindowPtr = new wxPanel(m_framePtr,-1);

    //m_panelPtr = ((EdtPanel*)m_envWindowPtr)->GetMusPanel();
	//
	WindowFunc5( m_envWindowPtr, true );
	m_panelPtr = (EdtPanel*)m_envWindowPtr->FindWindowById( ID5_MUSPANEL );
    wxASSERT_MSG( m_panelPtr, "WG Panel cannot be NULL ");
    
    if ( wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE) == *wxWHITE )
        m_panelPtr->SetBackgroundColour( *wxLIGHT_GREY );
    else
        m_panelPtr->SetBackgroundColour( wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE) );
        
    m_musViewPtr = new MusWindow( m_panelPtr, ID5_MUSWINDOW, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL|wxSIMPLE_BORDER );
}


void EdtEnv::RealizeToolbar( )
{
    wxToolBar *toolbar =  m_framePtr->GetToolBar();
    
    toolbar->AddSeparator();
    toolbar->AddTool( ID5_ZOOMOUT, _T("Zoom out"), m_framePtr->GetToolbarBitmap( "viewmag-.png" ), wxNullBitmap, wxITEM_NORMAL, _("Zoom out"), _("Zoom out") );
    toolbar->AddTool( ID5_ZOOMIN, _T("Zoom in"), m_framePtr->GetToolbarBitmap( "viewmag+.png" ), wxNullBitmap, wxITEM_NORMAL, _("Zoom in"), _("Zoom in") );
    toolbar->AddSeparator();
    toolbar->AddTool( ID5_PREVIOUS, _T("Previous"), m_framePtr->GetToolbarBitmap( "previous.png" ), wxNullBitmap, wxITEM_NORMAL, _("Previous page"), _("Go to the previous page") );
    toolbar->AddTool( ID5_NEXT, _T("Next"), m_framePtr->GetToolbarBitmap( "next.png" ), wxNullBitmap, wxITEM_NORMAL, _("Next page"), _("Go to the next page") );
    
    toolbar->Realize();
}


// WDR: handler implementations for EdtEnv

void EdtEnv::OnSize( wxSizeEvent &event )
{

    event.Skip();
}

void EdtEnv::OnTools( wxCommandEvent &event )
{
	if ( !m_musViewPtr )
		return;

	if ( event.GetId() == ID5_INSERT_MODE )
		m_musViewPtr->SetInsertMode( event.IsChecked() );
 	else if ( event.GetId() == ID5_NOTES )
		m_musViewPtr->SetToolType( MUS_TOOLS_NOTES );
	else if ( event.GetId() == ID5_KEYS)
		m_musViewPtr->SetToolType( MUS_TOOLS_CLEFS );
	else if ( event.GetId() == ID5_SIGNS )
		m_musViewPtr->SetToolType( MUS_TOOLS_SIGNS );
	else if ( event.GetId() == ID5_SYMBOLES )
		m_musViewPtr->SetToolType( MUS_TOOLS_OTHER );
}


bool EdtEnv::ResetFile()
{
    wxASSERT_MSG( m_musViewPtr, "WG Window cannot be NULL ");
    
    if ( !m_edtFilePtr->Close( true ) )
        return false;

    m_musViewPtr->Show( false );
    m_musViewPtr->SetFile( NULL );
    
    ((EdtPanel*)m_envWindowPtr)->GetMusToolPanel()->SetMusWindow( NULL );
    //m_toolpanel->SetMusWindow( NULL ); 
    UpdateTitle( );
    
    return true;
}

void EdtEnv::UpdateTitle( )
{
    wxString msg = wxString::Format("%s", m_edtFilePtr->m_shortname.c_str() );

    SetTitle( "%s", msg.c_str() );
}

void EdtEnv::UpdateViews( int flags )
{
    m_musViewPtr->SetFile( m_edtFilePtr->m_musFilePtr );
    m_musViewPtr->SetToolPanel( ((EdtPanel*)m_envWindowPtr)->GetMusToolPanel()  );
    m_musViewPtr->Resize( );
    //m_musViewPtr->Goto( 1 );
    //m_musControlPtr->SyncZoom();  
    
    /*
    if ( m_musViewPtr )
    {
        m_musViewPtr->Destroy();
        m_musViewPtr = NULL;
    }
    m_musViewPtr = new MusWindow( m_panelPtr, ID5_MUSWINDOW, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL|wxSIMPLE_BORDER );
    m_musViewPtr->SetToolPanel( ((EdtPanel*)m_envWindowPtr)->GetMusToolPanel() );
    m_musViewPtr->SetFile( m_edtFilePtr );
    m_musViewPtr->Resize( );
    */
}

void EdtEnv::OnUndo( wxCommandEvent &event )
{
    if ( !m_musViewPtr || !m_musViewPtr->CanUndo() )
		return;
		
	m_musViewPtr->Undo();
}

void EdtEnv::OnRedo( wxCommandEvent &event )
{
    if ( !m_musViewPtr || !m_musViewPtr->CanRedo() )
		return;
		
	m_musViewPtr->Redo();
}


void EdtEnv::Open( wxString filename, int type )
{
    if ( !this->ResetFile( ) )
        return;

    if ( (type == AX_FILE_DEFAULT) && this->m_edtFilePtr->Open( filename ) )
    {
        wxGetApp().AxBeginBusyCursor( );
        UpdateViews( 0 );
        wxGetApp().AxEndBusyCursor();
    }
}

void EdtEnv::OnNew( wxCommandEvent &event )
{
    Open( "" , -1 );
}

void EdtEnv::OnSave( wxCommandEvent &event )
{
    wxASSERT_MSG( m_edtFilePtr, wxT("EdtFile cannot be NULL") );

    m_edtFilePtr->Save();
}

void EdtEnv::OnSaveAs( wxCommandEvent &event )
{
    wxASSERT_MSG( m_edtFilePtr, wxT("EdtFile cannot be NULL") );

    m_edtFilePtr->SaveAs();
    UpdateTitle();
}

void EdtEnv::OnClose( wxCommandEvent &event )
{
    ResetFile();
}

void EdtEnv::OnValues( wxCommandEvent &event )
{
    if ( !m_musViewPtr )
        return;
    
    if ( event.GetId() == ID5_VOICES ) {
        m_musViewPtr->m_page->SetValues( PAGE_VALUES_VOICES );   
    } else if ( event.GetId() == ID5_INDENT ) {
        m_musViewPtr->m_page->SetValues( PAGE_VALUES_INDENT );  
    }
}

void EdtEnv::OnZoomOut( wxCommandEvent &event )
{
    if ( !m_musViewPtr )
        return;
    if ( m_musViewPtr->CanZoom( false ) )
        m_musViewPtr->Zoom( false );
}

void EdtEnv::OnZoomIn( wxCommandEvent &event )
{
    if ( !m_musViewPtr )
        return;
    if (m_musViewPtr->CanZoom( true ) )
        m_musViewPtr->Zoom( true );    
}

void EdtEnv::OnGoto( wxCommandEvent &event )
{
    if ( !m_musViewPtr )
        return;
    if (m_musViewPtr->CanGoto(  ) )
        m_musViewPtr->Goto(  );    
}

void EdtEnv::OnPrevious( wxCommandEvent &event )
{
    if ( !m_musViewPtr )
        return;
    if (m_musViewPtr->HasNext( false ) )
        m_musViewPtr->Next( false );    
}

void EdtEnv::OnNext( wxCommandEvent &event )
{
    if ( !m_musViewPtr )
        return;
    if (m_musViewPtr->HasNext( true ) )
        m_musViewPtr->Next( true );    
}

void EdtEnv::OnUpdateUI( wxUpdateUIEvent &event )
{
    if ( event.GetId() == ID_OPEN )
        event.Enable(true);
    else if ( event.GetId() == ID5_OPEN_MLF )
        event.Enable( true );
    else if ( event.GetId() == ID5_OPEN_WWG )
        event.Enable( true );
    else if ( event.GetId() == ID5_OPEN_MEI )
        event.Enable( true );

    else if ( !m_musViewPtr )
        event.Enable(false);
        
    else if ( event.GetId() == ID_UNDO )
        event.Enable( m_musViewPtr->CanUndo() );
    else if ( event.GetId() == ID_REDO )
        event.Enable( m_musViewPtr->CanRedo() );
    else if ( event.GetId() == ID5_GOTO )
        event.Enable( m_musViewPtr->CanGoto() );
    else if ( event.GetId() == ID5_PREVIOUS )
        event.Enable( m_musViewPtr->HasNext( false ) );
    else if ( event.GetId() == ID5_NEXT )
        event.Enable( m_musViewPtr->HasNext( true ) );
    else if ( event.GetId() == ID5_ZOOMOUT )
        event.Enable( m_musViewPtr->CanZoom( false ) );
    else if ( event.GetId() == ID5_ZOOMIN )
        event.Enable( m_musViewPtr->CanZoom( true ) );
    else if ( event.GetId() == ID5_SAVE_MODEL )
        event.Enable( m_edtFilePtr->IsOpened() );
    else if ( event.GetId() == ID5_SAVE_WWG )
        event.Enable( m_edtFilePtr->IsOpened() );
    else if ( event.GetId() == ID5_SAVE_MEI )
        event.Enable( m_edtFilePtr->IsOpened() );
    else if ( event.GetId() == ID5_SAVE_MLF )
        event.Enable( m_edtFilePtr->IsOpened() );
    else if ( event.GetId() == ID5_SAVE_SVG )
        event.Enable( m_edtFilePtr->IsOpened() );
    else if ( event.GetId() == ID5_SAVE_MEI )
        event.Enable( m_edtFilePtr->IsOpened() );
    else if ( event.GetId() == ID5_VOICES )
        event.Enable( m_edtFilePtr->IsOpened() );
    else if ( event.GetId() == ID_SAVE )
        event.Enable( m_edtFilePtr->IsOpened() && !m_edtFilePtr->IsNew() );
    else if ( event.GetId() == ID_SAVE_AS )
        event.Enable( m_edtFilePtr->IsOpened() );
	else if ( event.GetId() == ID5_INSERT_MODE )
	{
		event.Enable( true );
		event.Check( m_musViewPtr && !m_musViewPtr->m_editElement );
	}
    else
        event.Enable(true);
}


// import / export

void EdtEnv::OnOpenMLF( wxCommandEvent &event )
{
    if ( !this->ResetFile( ) )
        return;

    wxString mlf = wxFileSelector( _("Open"), wxGetApp().m_lastDir, _T(""), NULL, "MLF|*.mlf", wxOPEN);
    if ( mlf.IsEmpty() )
        return;
    //wxGetApp().m_lastDirMLF_in = wxPathOnly( mlf );
    
    m_edtFilePtr->New();
	//MusPage *page = new MusPage();
	//MusStaff *staff = new MusStaff();
	//page->m_staves.Add( staff );
	//m_edtFilePtr->m_pages.Add( page );
    MusMLFInput *mlfinput = new MusMLFInput( m_edtFilePtr->m_musFilePtr, mlf );
    bool import = mlfinput->ImportFile( 12 );
    delete mlfinput;

    if ( !import )
    {   
        wxLogError( _("Importation of file '%s' failed") , mlf.c_str() );
        return;
    }
    UpdateViews( 0 );

}

void EdtEnv::OnSaveMLF( wxCommandEvent &event )
{
    wxASSERT_MSG( m_panelPtr, "Panel cannot be NULL ");

	wxFileName fn( m_edtFilePtr->m_shortname );
	fn.SetExt( "mlf" );
	fn.SetPath( wxGetApp().m_lastDir );

    wxString filename = wxFileSelector( _("Save"), wxGetApp().m_lastDir, fn.GetFullPath() , NULL, "MLF|*.mlf", wxSAVE);
    if ( filename.IsEmpty() )
        return;
    //wxGetApp().m_lastDirMLF_out = wxPathOnly( filename );

    wxString dic = wxFileSelector( _("Open dictionnay"), wxGetApp().m_lastDir, _T(""), NULL, "DIC|*.dic", wxOPEN);
    if ( dic.IsEmpty() )
        return;
    //wxGetApp().m_lastDirDIC_in = wxPathOnly( dic );

    bool positions = ( wxMessageBox("Output symbols positions?", wxGetApp().GetAppName() ,
                            wxYES | wxNO | wxICON_QUESTION ) == wxYES );

    MusMLFOutput *mlfoutput = new MusMLFOutput( m_edtFilePtr->m_musFilePtr, filename, NULL, "MusMLFSymbol" );
	//mlfoutput->LoadTypes( dic );
	mlfoutput->m_pagePosition = positions;
    mlfoutput->m_addPageNo = true;
    
    mlfoutput->ExportFile();
    delete mlfoutput; 
}


/* 
    Methode de travail !!!!!!!!! Pas DU TOUT plombee !!!!
  */ 
  
#include "mus/mussvg.h"

void EdtEnv::OnSaveSVG( wxCommandEvent &event )
{   

    wxString filename;
    filename = wxFileSelector( _("Save"), wxGetApp().m_lastDirTIFF_out, _T(""), NULL, "SVG|*.svg", wxSAVE);
    if (filename.IsEmpty())
        return;
        
    wxGetApp().m_lastDirTIFF_out = wxPathOnly( filename );
    
    int zoom = m_musViewPtr->GetZoom( );
    m_musViewPtr->SetZoom( 100 );
    
    MusSVGFileDC svgDC (filename, m_musViewPtr->ToZoom( m_musViewPtr->pageFormatHor + 30 )  ,
        m_musViewPtr->ToZoom( m_musViewPtr->pageFormatVer + 10 )) ;
        
    svgDC.SetUserScale( 1, 1 );
    svgDC.SetLogicalScale( 1.0, 1.0 );  
    
	/*if ( m_musViewPtr->m_center )
		svgDC.SetLogicalOrigin( (m_musViewPtr->margeMorteHor - m_musViewPtr->mrgG), m_musViewPtr->margeMorteVer );
	else */
    svgDC.SetLogicalOrigin( -m_musViewPtr->mrgG, 0 );
    
    // font data
    svgDC.ConcatFile( wxGetApp().m_resourcesPath + "/svg/font.xml" );
	svgDC.SetTextForeground( *wxBLACK );
	
	svgDC.SetAxisOrientation( true, false );
    m_musViewPtr->m_page->DrawPage( &svgDC, false );
    
    // reset the zoom
    m_musViewPtr->SetZoom( zoom );
}


void EdtEnv::OnOpenWWG( wxCommandEvent &event )
{
    wxASSERT_MSG( m_panelPtr, "Panel cannot be NULL ");

    if ( !this->ResetFile( ) )
        return;

    wxString filename = wxFileSelector( _("Open"), wxGetApp().m_lastDir, _T(""), NULL, "WWG|*.wwg", wxOPEN);
    if ( filename.IsEmpty() )
        return;
    wxGetApp().m_lastDir = wxPathOnly( filename );
    
    m_edtFilePtr->New();
    
    MusWWGInput wwginput( m_edtFilePtr->m_musFilePtr, filename );
	if ( !wwginput.ImportFile() )
		return;

    UpdateViews( 0 );
}

void EdtEnv::OnSaveWWG( wxCommandEvent &event )
{   
    if (!m_edtFilePtr) {
        return;
    };

    wxString filename;
    filename = wxFileSelector( _("Save"), wxGetApp().m_lastDirAX0_out, m_edtFilePtr->m_musFilePtr->m_fheader.name + ".wwg", "wwg", "Wolfgang WWG|*.wwg", wxSAVE);
    if (filename.IsEmpty())
        return;
        
    wxGetApp().m_lastDirAX0_out = wxPathOnly( filename );
    
    // save
    MusWWGOutput *wwg_output = new MusWWGOutput( m_edtFilePtr->m_musFilePtr, filename );
    wwg_output->ExportFile();
    delete wwg_output;
}


void EdtEnv::OnOpenMEI( wxCommandEvent &event )
{
    wxASSERT_MSG( m_panelPtr, "Panel cannot be NULL ");

    if ( !this->ResetFile( ) )
        return;

    wxString filename = wxFileSelector( _("Import MEI"), wxGetApp().m_lastDir, _T(""), 
									   NULL, "MEI Files|*.mei|XML Files|*.xml", wxOPEN);
    if ( filename.IsEmpty() )
        return;
    wxGetApp().m_lastDir = wxPathOnly( filename );
    
    m_edtFilePtr->New();
    
    MusMeiInput meiinput( m_edtFilePtr->m_musFilePtr, filename );
	if ( !meiinput.ImportFile() )
		return;

    UpdateViews( 0 );
}

void EdtEnv::OnSaveMEI( wxCommandEvent &event )
{

    if (  !m_panelPtr || !m_edtFilePtr )
        return;

    wxString filename = wxFileSelector( _("Export MEI"), wxGetApp().m_lastDir, _T(""),
									   NULL, "MEI Files|*.meiMEI|XML Files|*.xml", wxSAVE);
    if ( filename.IsEmpty() )
        return;     
    wxGetApp().m_lastDirAX0_out = wxPathOnly( filename );
    
    // save
    MusMeiOutput *mei_output = new MusMeiOutput( m_edtFilePtr->m_musFilePtr, filename );
    mei_output->ExportFile();
    delete mei_output;
}

void EdtEnv::OnSaveModel( wxCommandEvent &event )
{
	//wxLogMessage(" doesn't exists anymore.... pfff");
    //wxLogDebug("Midi %d", event.GetInt() ) ;
    event.Skip();
}


void EdtEnv::OnMidiInput( wxCommandEvent &event )
{
	if ( m_edtFilePtr->IsOpened() ) {
        m_musViewPtr->ProcessEvent( event );
    }
}

#endif // AX_EDT

