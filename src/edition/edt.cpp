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
    //EVT_MENU( ID_OPEN, EdtEnv::OnOpen )
    EVT_MENU( ID_SAVE, EdtEnv::OnSave )
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
	//EVT_SIZE( EdtEnv::OnSize )
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(EdtEnv,AxEnv)

EdtEnv::EdtEnv():
    AxEnv()
{
    m_panelPtr = NULL;
    m_filePtr = NULL;
    m_musViewPtr = NULL;

    this->m_envMenuBarFuncPtr = MenuBarFunc5;
}

EdtEnv::~EdtEnv()
{
    if (m_envWindowPtr) m_envWindowPtr->Destroy();
    m_envWindowPtr = NULL;

    if ( m_filePtr )
        delete m_filePtr;
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

void EdtEnv::OnSaveModel( wxCommandEvent &event )
{
	wxLogMessage(" doesn't exists anymore.... pfff");
}

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


void EdtEnv::OnSave( wxCommandEvent &event )
{
    if (  !m_panelPtr || !m_filePtr )
        return;

    wxString wwg = wxFileSelector( _("Save"), wxGetApp().m_lastDir, _T(""), NULL, "WWG|*.wwg", wxSAVE);
    if ( wwg.IsEmpty() )
        return;
    //wxGetApp().m_lastDirWWG_out = wxPathOnly( wwg );   
    
    m_filePtr->m_fname = wwg;
    MusWWGOutput *wwgoutput = new MusWWGOutput( m_filePtr, wwg );
    wwgoutput->ExportFile();
    delete wwgoutput;
}

void EdtEnv::OnOpenMLF( wxCommandEvent &event )
{
    wxString mlf = wxFileSelector( _("Open"), wxGetApp().m_lastDir, _T(""), NULL, "MLF|*.mlf", wxOPEN);
    if ( mlf.IsEmpty() )
        return;
    //wxGetApp().m_lastDirMLF_in = wxPathOnly( mlf );

    if ( m_musViewPtr )
    {
        m_musViewPtr->Destroy();
        m_musViewPtr = NULL;
    }

    if ( m_filePtr )
    {
        delete m_filePtr;
        m_filePtr = NULL;
    }
    
    m_filePtr = new MusFile();
	//MusPage *page = new MusPage();
	//MusStaff *staff = new MusStaff();
	//page->m_staves.Add( staff );
	//m_filePtr->m_pages.Add( page );
    MusMLFInput *mlfinput = new MusMLFInput( m_filePtr, mlf );
    bool import = mlfinput->ImportFile( 12 );
    delete mlfinput;

    if ( !import )
    {   
        wxLogError( _("Importation of file '%s' failed") , mlf.c_str() );
        return;
    }

    m_musViewPtr = new MusWindow( m_panelPtr, ID5_MUSWINDOW, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL|wxSIMPLE_BORDER );
    m_musViewPtr->SetFile( m_filePtr );
    m_musViewPtr->Resize( );
}

void EdtEnv::OnSaveMLF( wxCommandEvent &event )
{
    wxASSERT_MSG( m_panelPtr, "Panel cannot be NULL ");

	wxFileName fn( m_filePtr->m_fname );
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

    MusMLFOutput *mlfoutput = new MusMLFOutput( m_filePtr, filename, NULL, "MusMLFSymbol" );
	//mlfoutput->LoadTypes( dic );
	mlfoutput->m_pagePosition = positions;
    mlfoutput->m_addPageNo = true;
    
    mlfoutput->ExportFile();
    delete mlfoutput; 
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
    else if ( event.GetId() == ID5_SAVE_MLF )
        event.Enable( m_musViewPtr != NULL );
    else if ( event.GetId() == ID5_SAVE_SVG )
        event.Enable( m_musViewPtr != NULL );
    else if ( event.GetId() == ID5_SAVE_MEI )
        event.Enable( m_musViewPtr != NULL );
    else if ( event.GetId() == ID5_VOICES )
        event.Enable( m_musViewPtr != NULL );
	else if ( event.GetId() == ID5_INSERT_MODE )
	{
		event.Enable( true );
		event.Check( m_musViewPtr && !m_musViewPtr->m_editElement );
	}
    else
        event.Enable(true);
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

void EdtEnv::OnSaveWWG( wxCommandEvent &event )
{   
    if (!m_filePtr) {
        return;
    };

    wxString filename;
    filename = wxFileSelector( _("Save"), wxGetApp().m_lastDirAX0_out, m_filePtr->m_fheader.name + ".wwg", "wwg", "Wolfgang WWG|*.wwg", wxSAVE);
    if (filename.IsEmpty())
        return;
        
    wxGetApp().m_lastDirAX0_out = wxPathOnly( filename );
    
    // save
    MusWWGOutput *wwg_output = new MusWWGOutput( m_filePtr, filename );
    wwg_output->ExportFile();
    delete wwg_output;
}


void EdtEnv::OnSaveMEI( wxCommandEvent &event )
{

    if (  !m_panelPtr || !m_filePtr )
        return;

    wxString filename = wxFileSelector( _("Save"), wxGetApp().m_lastDir, _T(""), NULL, "MEI XML|*.xml", wxSAVE);
    if ( filename.IsEmpty() )
        return;     
    wxGetApp().m_lastDirAX0_out = wxPathOnly( filename );
    
    // save
    MusMeiOutput *mei_output = new MusMeiOutput( m_filePtr, filename );
    mei_output->ExportFile();
    delete mei_output;
}

void EdtEnv::OnOpenWWG( wxCommandEvent &event )
{
    wxASSERT_MSG( m_panelPtr, "Panel cannot be NULL ");


    wxString filename = wxFileSelector( _("Open"), wxGetApp().m_lastDir, _T(""), NULL, "WWG|*.wwg", wxOPEN);
    if ( filename.IsEmpty() )
        return;
    wxGetApp().m_lastDir = wxPathOnly( filename );
    
    if ( m_filePtr )
    {
        delete m_filePtr;
        m_filePtr = NULL;
    }
    m_filePtr = new MusFile();
    m_filePtr->m_fname = filename;
    MusWWGInput wwginput( m_filePtr, filename );
	if ( !wwginput.ImportFile() )
		return;

    /*
    MusWWGOutput *wwgoutput = new MusWWGOutput( m_filePtr, filename );
    wwgoutput->ExportFile();
    delete wwgoutput;*/
    

    if ( m_musViewPtr )
    {
        m_musViewPtr->Destroy();
        m_musViewPtr = NULL;
    }
    m_musViewPtr = new MusWindow( m_panelPtr, ID5_MUSWINDOW, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL|wxSIMPLE_BORDER );
    m_musViewPtr->SetToolPanel( ((EdtPanel*)m_envWindowPtr)->GetMusToolPanel() );
    m_musViewPtr->SetFile( m_filePtr );
    m_musViewPtr->Resize( );
}

void EdtEnv::OnOpenMEI( wxCommandEvent &event )
{
    wxASSERT_MSG( m_panelPtr, "Panel cannot be NULL ");


    wxString filename = wxFileSelector( _("Open"), wxGetApp().m_lastDir, _T(""), NULL, "XML|*.xml", wxOPEN);
    if ( filename.IsEmpty() )
        return;
    wxGetApp().m_lastDir = wxPathOnly( filename );
    
    if ( m_filePtr )
    {
        delete m_filePtr;
        m_filePtr = NULL;
    }
    m_filePtr = new MusFile();
    m_filePtr->m_fname = filename;
    MusMeiInput meiinput( m_filePtr, filename );
	if ( !meiinput.ImportFile() )
		return;

    /*
    MusWWGOutput *wwgoutput = new MusWWGOutput( m_filePtr, filename );
    wwgoutput->ExportFile();
    delete wwgoutput;*/
    

    if ( m_musViewPtr )
    {
        m_musViewPtr->Destroy();
        m_musViewPtr = NULL;
    }
    m_musViewPtr = new MusWindow( m_panelPtr, ID5_MUSWINDOW, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL|wxSIMPLE_BORDER );
    m_musViewPtr->SetToolPanel( ((EdtPanel*)m_envWindowPtr)->GetMusToolPanel() );
    m_musViewPtr->SetFile( m_filePtr );
    m_musViewPtr->Resize( );
}

#endif // AX_EDT

