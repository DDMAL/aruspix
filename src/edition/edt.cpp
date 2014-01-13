    /////////////////////////////////////////////////////////////////////////////
// Name:        edt.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_EDITION

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "wx/dirctrl.h"
#include "wx/calctrl.h"

#include "edt.h"
#include "edtfile.h"

#include "app/axframe.h"
#include "app/axapp.h"


#include "mus/musdoc.h"
#include "mus/muspage.h"

#include "musapp/muswindow.h"
#include "musapp/mustoolpanel.h"
#include "musapp/musiowwg.h"
#include "musapp/musiomlf.h"

// experimental
#include "mus/musiopae.h"
#include "mus/musiomei.h"
#include "mus/mussvgdc.h"
#include "mus/musiodarms.h"

//----------------------------------------------------------------------------
// EdtPanel
//----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(EdtPanel,wxPanel)
    //EVT_TREE_SEL_CHANGED( ID0_DIRECTORIES, EdtPanel::OnTree )
    EVT_SIZE( EdtPanel::OnSize )
END_EVENT_TABLE()

EdtPanel::EdtPanel( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{

}


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
    EVT_MENU( ID5_OPEN_DARMS, EdtEnv::OnOpenDARMS )
    EVT_MENU( ID5_OPEN_PAE, EdtEnv::OnOpenPAE )
    EVT_MENU( ID5_OPEN_WWG, EdtEnv::OnOpenWWG )
    EVT_MENU( ID5_OPEN_MEI, EdtEnv::OnOpenMEI )
    EVT_MENU( ID5_SAVE_SVG, EdtEnv::OnSaveSVG )
    EVT_MENU( ID5_SAVE_MEI, EdtEnv::OnSaveMEI )
    EVT_MENU( ID5_SAVE_MODEL, EdtEnv::OnSaveModel )
    EVT_MENU( ID5_SAVE_MLF, EdtEnv::OnSaveMLF )
    EVT_MENU( ID5_SAVE_WWG, EdtEnv::OnSaveWWG )
    EVT_MENU( ID5_VOICES, EdtEnv::OnValues )
    EVT_MENU( ID5_INDENT, EdtEnv::OnValues )
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



void EdtEnv::OnSize( wxSizeEvent &event )
{

    event.Skip();
}


bool EdtEnv::ResetFile()
{
    wxASSERT_MSG( m_musViewPtr, "WG Window cannot be NULL ");
    
    if ( !m_edtFilePtr->Close( true ) )
        return false;

    m_musViewPtr->Show( false );
    m_musViewPtr->SetDoc( NULL );
    
    // XXX: What's this doing?
    //((EdtPanel*)m_envWindowPtr)->GetMusToolPanel()->SetMusWindow( NULL );
    //m_toolpanel->SetMusWindow( NULL ); 
    UpdateTitle( );
    
    return true;
}

void EdtEnv::ParseCmd( wxCmdLineParser *parser )
{
    wxASSERT_MSG( parser, wxT("Parser cannot be NULL") );

    if ( parser->Found("q") &&  (parser->GetParamCount() > 0) )
    {
        // it might be useful to disable logging
        // uhmm - it is actually necessary because any call a log function (execpt wxLogDebug) will
        // stop the program (i.e., it will wait for the user to click OK...)
        wxLogNull logNo;;
        
        // first parameter is input file
        wxString file = parser->GetParam( 0 );
        wxLogDebug( file.c_str() );
        
        // experiments
        // convert mei file to svg
        if ( parser->Found("m") && (parser->GetParamCount() == 2) )
        {
        
            
            // with -m, the second parameter is the output filename
            wxString outfile = parser->GetParam( 1 );
            wxLogDebug( outfile.c_str() );
            
            /*
            MusDoc *mfile = new MusDoc();
            //MusMeiInput meiinput( mfile, file );
            if ( !meiinput.ImportFile() )
                return;
            
            // draw it
            Page *page = &mfile->m_pages[0];
            m_musViewPtr->SetDoc( mfile );
            page->Init( m_musViewPtr );
            m_musViewPtr->SetZoom( 50 ); // this should probably be a parameter...
            m_musViewPtr->UpdatePageValues();
            //m_musViewPtr->m_pageMaxX = page->lrg_lign*10; // !fix it!! in the GUI initialized in OnPaint // ax2
            m_musViewPtr->m_currentElement = NULL; // !fix it!! deselect elemnts

            //MusSVGFileDC svgDC (outfile, mfile->m_parameters.param.m_paperWidth, mfile->m_parameters.param.m_paperHeight );
            MusSVGFileDC svgDC (outfile, m_musViewPtr->ToRendererX( m_musViewPtr->m_pageWidth )  ,
                m_musViewPtr->ToRendererX( m_musViewPtr->m_paperHeight )) ;
            
            svgDC.SetUserScale( 1, 1 );
            svgDC.SetLogicalScale( 1.0, 1.0 );  
            
            svgDC.SetLogicalOrigin( -m_musViewPtr->m_leftMargin, 0 );
    
            svgDC.SetTextForeground( *wxBLACK );
	
            svgDC.SetAxisOrientation( true, false );
            (&mfile->m_pages[0])->DrawPage( &svgDC, false );
            */ // ax2

        }
    }
}
    
void EdtEnv::UpdateTitle( )
{
    wxString msg = wxString::Format("%s", m_edtFilePtr->m_shortname.c_str() );
    SetTitle( _T("%s"), msg.c_str() );
}

void EdtEnv::UpdateViews( int flags )
{
    m_musViewPtr->SetDoc( m_edtFilePtr->m_musDocPtr );
    m_musViewPtr->SetToolPanel( ((EdtPanel*)m_envWindowPtr)->GetMusToolPanel()  );
    m_musViewPtr->Resize( );
    wxYield();
    //m_musViewPtr->Goto( 1 );
    //m_musControlPtr->SyncZoom();  
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

bool EdtEnv::CloseAll( )
{
    if ( !ResetFile() )
        return false;
        
    return true;
}


void EdtEnv::Open( wxString filename, int type )
{
    if ( !this->ResetFile( ) )
        return;

    if ( (type == AX_FILE_DEFAULT) && this->m_edtFilePtr->Open( filename ) )
    {
        wxGetApp().AxBeginBusyCursor( );
        UpdateViews( 0 );
        m_edtFilePtr->Modify(); // always flag it as modify. The correct way to do it 
                                // would be to put this in EndEdition but we would need a 
                                // child class to override the method
        wxGetApp().AxEndBusyCursor();
    }
}

void EdtEnv::OnNew( wxCommandEvent &event )
{
    //Open( "" , AX_FILE_DEFAULT );

    if ( !this->ResetFile( ) )
        return;

    if ( this->m_edtFilePtr->New( ) && m_edtFilePtr->Create( ) )
    {
        
        wxGetApp().AxBeginBusyCursor( );
        UpdateViews( 0 );
        m_edtFilePtr->Modify(); // always flag it as modify. The correct way to do it 
                                // would be to put this in EndEdition but we would need a 
                                // child class to override the method
        wxGetApp().AxEndBusyCursor();
    }    

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
    else if ( event.GetId() == ID5_OPEN_DARMS )
        event.Enable( true );
    else if ( event.GetId() == ID5_OPEN_PAE )
        event.Enable( true );
    else if ( event.GetId() == ID5_OPEN_WWG )
        event.Enable( true );
    else if ( event.GetId() == ID5_OPEN_MEI )
        event.Enable( true );

    else if ( !m_musViewPtr )
        event.Enable(false);
        
    else if ( event.GetId() == ID_UNDO )
        event.Enable( true ); // undo to be fixed
        //event.Enable( m_musViewPtr->CanUndo() );
    else if ( event.GetId() == ID_REDO )
        event.Enable( true ); // undo to be fixed
        //event.Enable( m_musViewPtr->CanRedo() );
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
        event.Enable( false ); 
        //event.Enable( m_edtFilePtr->IsOpened() );
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
        event.Enable( false );
        //event.Enable( m_edtFilePtr->IsOpened() );
    else if ( event.GetId() == ID5_INDENT )
        event.Enable( false );
    //event.Enable( m_edtFilePtr->IsOpened() );
    else if ( event.GetId() == ID_SAVE )
           event.Enable( m_edtFilePtr->IsOpened() && m_edtFilePtr->IsModified() );
    else if ( event.GetId() == ID_SAVE_AS )
        event.Enable( m_edtFilePtr->IsOpened() );
    else if ( event.GetId() == ID_NEW )
        event.Enable( false );
    else
        event.Enable(true);
}


// import / export


void EdtEnv::OnOpenPAE( wxCommandEvent &event )
{
    
    wxASSERT_MSG( m_panelPtr, "Panel cannot be NULL ");
    
    if ( !this->ResetFile( ) )
        return;
    
    
    wxString filename = wxFileSelector( _("Import PAE"), wxGetApp().m_lastDir, _T(""), _T(""), "PAE Files|*.pae|All Files|*.*", wxFD_OPEN);
    if ( filename.IsEmpty() )
        return;
    wxGetApp().m_lastDir = wxPathOnly( filename );
    
    m_edtFilePtr->New();

    MusPaeInput paeInput( m_edtFilePtr->m_musDocPtr, filename.mb_str() );
	if ( !paeInput.ImportFile() )
		return;

    m_edtFilePtr->m_musDocPtr->Layout();
    
	m_musViewPtr->SetEditorMode(MUS_EDITOR_EDIT);
    UpdateViews( 0 );
}


void EdtEnv::OnOpenDARMS( wxCommandEvent &event )
{
    
    wxASSERT_MSG( m_panelPtr, "Panel cannot be NULL ");
    
    if ( !this->ResetFile( ) )
        return;

    wxString filename = wxFileSelector( _("Import DARMS"), wxGetApp().m_lastDir, _T(""), _T(""), "DARMS Files|*.darms|All Files|*.*", wxFD_OPEN);
    if ( filename.IsEmpty() )
        return;
    wxGetApp().m_lastDir = wxPathOnly( filename );
    
    m_edtFilePtr->New();
    
    MusDarmsInput darmsInput( m_edtFilePtr->m_musDocPtr, filename.mb_str() );
	if ( !darmsInput.ImportFile() )
		return;
    
    m_edtFilePtr->m_musDocPtr->Layout();
    
	m_musViewPtr->SetEditorMode(MUS_EDITOR_EDIT);
    UpdateViews( 0 );
}

/*
void EdtEnv::OnOpenMLF( wxCommandEvent &event )
{

    if ( !this->ResetFile( ) )
        return;

    wxString mlf = wxFileSelector( _("Open"), wxGetApp().m_lastDir, _T(""), _T(""), "MLF|*.mlf", wxFD_OPEN);
    if ( mlf.IsEmpty() )
        return;
    //wxGetApp().m_lastDirMLF_in = wxPathOnly( mlf );
    
    m_edtFilePtr->New();
	//Page *page = new Page();
	//Staff *staff = new Staff();
	//page->m_staves.Add( staff );
	//m_edtFilePtr->m_pages.Add( page );
    MusMLFInput *mlfinput = new MusMLFInput( m_edtFilePtr->m_musDocPtr, mlf );
    bool import = mlfinput->ImportFile( 12 );
    delete mlfinput;

    if ( !import )
    {   
        wxLogError( _("Importation of file '%s' failed") , mlf.c_str() );
        return;
    }
    UpdateViews( 0 );
}
*/ // ax2

void EdtEnv::OnSaveMLF( wxCommandEvent &event )
{
    /*
    wxASSERT_MSG( m_panelPtr, "Panel cannot be NULL ");

	wxFileName fn( m_edtFilePtr->m_shortname );
	fn.SetExt( "mlf" );
	fn.SetPath( wxGetApp().m_lastDir );

    wxString filename = wxFileSelector( _("Save"), wxGetApp().m_lastDir, fn.GetFullPath() , _T(""), "MLF|*.mlf", wxFD_SAVE);
    if ( filename.IsEmpty() )
        return;
    //wxGetApp().m_lastDirMLF_out = wxPathOnly( filename );

    wxString dic = wxFileSelector( _("Open dictionnay"), wxGetApp().m_lastDir, _T(""), _T(""), "DIC|*.dic", wxFD_OPEN);
    if ( dic.IsEmpty() )
        return;
    //wxGetApp().m_lastDirDIC_in = wxPathOnly( dic );

    bool positions = ( wxMessageBox("Output symbols positions?", wxGetApp().GetAppName() ,
                            wxYES | wxNO | wxICON_QUESTION ) == wxYES );

    MusMLFOutput *mlfoutput = new MusMLFOutput( m_edtFilePtr->m_musDocPtr, filename, NULL, "MusMLFSymbol" );
	//mlfoutput->LoadTypes( dic );
	mlfoutput->m_pagePosition = positions;
    mlfoutput->m_addPageNo = true;
    
    mlfoutput->ExportFile();
    delete mlfoutput; 
    */
}


void EdtEnv::OnSaveSVG( wxCommandEvent &event )
{   

    wxString filename;
    filename = wxFileSelector( _("Save"), wxGetApp().m_lastDirTIFF_out, _T(""), _T(""), "SVG|*.svg", wxFD_SAVE);
    if (filename.IsEmpty())
        return;
        
    wxGetApp().m_lastDirTIFF_out = wxPathOnly( filename );
    
    int zoom = m_musViewPtr->GetZoom( );
    m_musViewPtr->SetZoom( 100 );
    
    
    MusSvgDC svgDC (m_musViewPtr->ToRendererX( m_musViewPtr->m_doc->m_rendPageWidth + 30 )  ,
        m_musViewPtr->ToRendererX( m_musViewPtr->m_doc->m_rendPageHeight + 10 )) ;
        
    //svgDC.SetUserScale( 1, 1 );
    //svgDC.SetLogicalScale( 1.0, 1.0 );  
    
	//if ( m_musViewPtr->m_center )
	//	svgDC.SetLogicalOrigin( (m_musViewPtr->margeMorteHor - m_musViewPtr->m_leftMargin), m_musViewPtr->margeMorteVer );
	//else 
    //svgDC.SetLogicalOrigin( -m_musViewPtr->m_leftMargin, 0 );
    
    // font data
    //svgDC.ConcatFile( wxGetApp().m_resourcesPath + "/svg/font.xml" );
	svgDC.SetTextForeground( AxBLACK );
    
    //svgDC.SetUserScale( 0.1, 0.2 );
	
    m_musViewPtr->m_currentElement = NULL;
	//svgDC.SetAxisOrientation( true, false );
    MusRC rc;
    rc.SetDoc(m_musViewPtr->m_doc );
    rc.DrawPage(  &svgDC, m_musViewPtr->m_page, false );
    
    wxString svg = wxString( svgDC.GetStringSVG().c_str() ); // ax3
    // we still need to write the SVG to the file...
    //filename.mb_str(),
    
    // reset the zoom
    m_musViewPtr->SetZoom( zoom );
    
}

void EdtEnv::OnOpenWWG( wxCommandEvent &event )
{
    wxASSERT_MSG( m_panelPtr, "Panel cannot be NULL ");

    if ( !this->ResetFile( ) )
        return;

    wxString filename = wxFileSelector( _("Open"), wxGetApp().m_lastDir, _T(""), _T(""), "WWG|*.wwg", wxFD_OPEN);
    if ( filename.IsEmpty() )
        return;
    wxGetApp().m_lastDir = wxPathOnly( filename );
    
    m_edtFilePtr->New();
    
    MusWWGInput wwginput( m_edtFilePtr->m_musDocPtr, filename.mb_str() );
	if ( !wwginput.ImportFile() )
		return;

    UpdateViews( 0 );
}

void EdtEnv::OnSaveWWG( wxCommandEvent &event )
{   
    if (!m_edtFilePtr) {
        return;
    };
    
    wxString wxdocname = m_edtFilePtr->m_shortname;
    wxString name, ext;
    wxFileName::SplitPath(wxdocname, NULL, &name, &ext);
    wxString savename = name + ".wwg";

    wxString filename;
    filename = wxFileSelector( _("Save"), wxGetApp().m_lastDirAX0_out, savename, "wwg", "Wolfgang WWG|*.wwg", wxFD_SAVE);
    if (filename.IsEmpty())
        return;
        
    wxGetApp().m_lastDirAX0_out = wxPathOnly( filename );
    
    // save
    MusWWGOutput *wwg_output = new MusWWGOutput( m_edtFilePtr->m_musDocPtr, filename.mb_str() );
    wwg_output->ExportFile();
    delete wwg_output;
}

void EdtEnv::OnOpenMEI( wxCommandEvent &event )
{

    wxASSERT_MSG( m_panelPtr, "Panel cannot be NULL ");

    if ( !this->ResetFile( ) )
        return;
    
    wxString filename = wxFileSelector( _("Import MEI"), wxGetApp().m_lastDir, _T(""), _T(""), "MEI Files|*.mei|XML Files|*.xml", wxFD_OPEN);
    if ( filename.IsEmpty() )
        return;
    wxGetApp().m_lastDir = wxPathOnly( filename );
    
    m_edtFilePtr->New();

    MusMeiInput meiInput( m_edtFilePtr->m_musDocPtr, filename.mb_str() );
	if ( !meiInput.ImportFile() )
		return;
    
    //OnSaveMEI( event );
    
    if ( m_edtFilePtr->m_musDocPtr->GetType() != Transcription ) {
        m_edtFilePtr->m_musDocPtr->Layout();
    }
    
	m_musViewPtr->SetEditorMode(MUS_EDITOR_EDIT);
    UpdateViews( 0 );
}

void EdtEnv::OnSaveMEI( wxCommandEvent &event )
{
    if (  !m_panelPtr || !m_edtFilePtr )
        return;

    wxString wxdocname = m_edtFilePtr->m_shortname;
    wxString name, ext;
    wxFileName::SplitPath(wxdocname, NULL, &name, &ext);
    wxString savename = name + "." + ext;

    wxString filename = wxFileSelector( _("Export MEI"), wxGetApp().m_lastDir, savename, _T(""), "MEI Files|*.mei|XML Files|*.xml", wxFD_SAVE);
    if ( filename.IsEmpty() )
        return;
    if (wxFileExists(filename)) {
        wxString message = _("File aldready exists. Overwrite?");
        wxMessageDialog dialog ( m_musViewPtr, message, _("File exits"), wxYES_NO|wxICON_QUESTION );
        if ( dialog.ShowModal () == wxID_YES ) {
            MusMeiOutput *mei_output = new MusMeiOutput( m_edtFilePtr->m_musDocPtr, filename.mb_str() );
            mei_output->ExportFile();
            delete mei_output;
        }
    } else {
        MusMeiOutput *mei_output = new MusMeiOutput( m_edtFilePtr->m_musDocPtr, filename.mb_str() );
        mei_output->ExportFile();
        delete mei_output;
	}
    wxGetApp().m_lastDirAX0_out = wxPathOnly( filename );
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

#endif // AX_EDITION

