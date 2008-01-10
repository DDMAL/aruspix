/////////////////////////////////////////////////////////////////////////////
// Name:        aximagecontroller.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "aximagecontroller.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/clipbrd.h"
#include "wx/valgen.h"
#include "wx/filename.h"
#include "wx/dir.h"

#include "aximagecontroller.h"
#include "axscrolledwindow.h"
#include "axgotodlg.h"
#include "axapp.h"
#include "axenv.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// AxImageController
//----------------------------------------------------------------------------


//IMPLEMENT_CLASS2(AxImageController , AxImage, wxPanel )
IMPLEMENT_CLASS(AxImageController, AxImage )

// WDR: event table for AxImageController

BEGIN_EVENT_TABLE(AxImageController,wxPanel)
    EVT_TOOL_RANGE( CONTROLLER_IDS_MIN, CONTROLLER_IDS_MAX, AxImageController::OnCommand )
    EVT_UPDATE_UI_RANGE( CONTROLLER_IDS_MIN, CONTROLLER_IDS_MAX,  AxImageController::OnUpdateUI )
END_EVENT_TABLE()

AxImageController::AxImageController( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style , int flags) :
    wxPanel( parent, id, position, size, style ), AxImage()
{
	m_envPtr = NULL;
    m_viewPtr = NULL;
    m_sizerPtr = NULL;
	m_toolBarPtr = NULL;

    m_nbOfPages = 0;
    m_currentPage = 0;
	m_nbOfFiles = 0;
	m_currentFile = 0;

	m_flags = flags;
	m_multipage = true;
	
	// temporary, disable toolbar because it doesn't work on mac
	m_flags = CONTROLLER_NO_TOOLBAR;	

    m_sizerPtr = new wxFlexGridSizer( 1, 0, 0 );
    m_sizerPtr->AddGrowableCol( 0 );
    int viewRow = 1;
    if (m_flags & CONTROLLER_NO_TOOLBAR) 
		viewRow = 0;
    m_sizerPtr->AddGrowableRow( viewRow );

	
    if (!(m_flags & CONTROLLER_NO_TOOLBAR))
    {
        m_toolBarPtr = new wxToolBar( this, -1 , wxDefaultPosition,
            wxDefaultSize, wxTB_HORIZONTAL | wxTB_FLAT );
        ViewToolBarFunc( m_toolBarPtr );
        // m_flags - toujours dans l'ordre inverse à cause du DeleteToolByPos pour le separateur
		if (m_flags & CONTROLLER_NO_ADJUST)
        {
            m_toolBarPtr->DeleteTool(ID_CONTROLLER_ADJUST_VH);
            m_toolBarPtr->DeleteTool(ID_CONTROLLER_ADJUST_V);
            m_toolBarPtr->DeleteTool(ID_CONTROLLER_ADJUST_H);
        }
        if (m_flags & CONTROLLER_NO_ZOOM)
        {
            m_toolBarPtr->DeleteToolByPos(5);
            m_toolBarPtr->DeleteTool(ID_CONTROLLER_ZOOM_IN);
            m_toolBarPtr->DeleteTool(ID_CONTROLLER_ZOOM_OUT);
        }
        if (m_flags & CONTROLLER_NO_NAVIGATION)
        {
            m_toolBarPtr->DeleteToolByPos(2);
            m_toolBarPtr->DeleteTool(ID_CONTROLLER_PREVIOUS);
            m_toolBarPtr->DeleteTool(ID_CONTROLLER_NEXT);
        }
		else if (m_flags & CONTROLLER_NO_MULTIPAGE)
		{
			this->SetMultipage( false );	
		}

        m_sizerPtr->Add( m_toolBarPtr, 0, wxGROW|wxALL, 0 );
    }
	

    this->SetAutoLayout( TRUE );
    this->SetSizer( m_sizerPtr );

    m_sizerPtr->Fit( this );
    m_sizerPtr->SetSizeHints( this );
}

AxImageController::AxImageController()
{

}

AxImageController::~AxImageController()
{

}

void AxImageController::Init( AxEnv *env, AxScrolledWindow *window )
{
	// environement
    wxASSERT_MSG(env,"Environment cannot be NULL");
    m_envPtr = env;

	// view
    wxASSERT_MSG(m_sizerPtr,"Sizer cannot be NULL");
    wxASSERT_MSG(window,"View cannot be NULL");
    m_viewPtr = window;
    m_sizerPtr->Add( m_viewPtr, 0, wxGROW|wxALL, 0 );
    m_sizerPtr->Fit( this );
}

void AxImageController::SetMultipage( bool multi )
{
	if ( !HasNavigation() || !m_toolBarPtr )
		return;

	m_toolBarPtr->DeleteTool( ID_CONTROLLER_PREVIOUS );
	m_toolBarPtr->DeleteTool( ID_CONTROLLER_NEXT );

	m_multipage = multi;

	if ( multi )
	{
		m_toolBarPtr->InsertTool( 0, ID_CONTROLLER_PREVIOUS, _("Previous page"),
                   ViewBitmapsFunc( 0 ), wxNullBitmap,
                   wxITEM_NORMAL,
                   _("Previous page"),
                   _("Go to the previous page in the file"));
		m_toolBarPtr->InsertTool( 1, ID_CONTROLLER_NEXT, _("Next page"),
                   ViewBitmapsFunc( 2 ), wxNullBitmap,
                   wxITEM_NORMAL,
                   _("Next page"),
                   _("Go to the next page in the file"));
	}
	else
	{
		m_toolBarPtr->InsertTool( 0, ID_CONTROLLER_PREVIOUS, _("Previous file"),
                   ViewBitmapsFunc( 1 ), wxNullBitmap,
                   wxITEM_NORMAL,
                   _("Previous file"),
                   _("Go to the previous file in the directory"));
		m_toolBarPtr->InsertTool( 1, ID_CONTROLLER_NEXT, _("Next file"),
                   ViewBitmapsFunc( 3 ), wxNullBitmap,
                   wxITEM_NORMAL,
                   _("Next file"),
                   _("Go to the next file in the directory"));	}
	m_toolBarPtr->Realize( );
}


// methodes d'etat

bool AxImageController::HasNavigation()
{
    return !(m_flags & CONTROLLER_NO_NAVIGATION);
}

bool AxImageController::CanPaste()
{
    return true;
}

bool AxImageController::CanCopy()
{
    return ( this->Ok() );
}

bool AxImageController::CanCut()
{
    return ( this->Ok() );
}

bool AxImageController::HasNext()
{
    if ( m_multipage )
		return ( m_currentPage < m_nbOfPages -1 );
	else
		return ( m_currentFile < m_nbOfFiles -1 );
}

bool AxImageController::HasPrevious()
{
    if ( m_multipage )
		return (m_currentPage != 0);
	else
		return (m_currentFile != 0);
}

bool AxImageController::CanGoto()
{
    return ( m_nbOfPages > 1 );
}

// methodes d'action

void AxImageController::UpdateTitleMsg( )
{
	// eventuellemnt diviser en deux methodes pour customizer le message dans les classe derivees
	// virtual SetTitleMsg()
	// UpdateTitleMsg() qui appel la methode virtual SetTitleMsg

	if ( !m_envPtr || !m_viewPtr )
		return;
	
	if ( !m_shortname.IsEmpty() )
		m_titleMsg = m_shortname;
	else
		m_titleMsg = _("[untitled]");
	
	// index
	if ( m_nbOfPages > 1 )
		m_titleMsg += wxString::Format(_(" page %d/%d"), m_currentPage + 1, m_nbOfPages );

	// zoom
	m_titleMsg += wxString::Format(" (%d%%)", m_viewPtr->GetZoomPercent() );

    m_envPtr->UpdateTitle( );
}

void AxImageController::LoadDirectoryFilenames( )
{
	wxFileName fn( m_filename );
	wxString path = fn.GetPath( wxPATH_GET_VOLUME );

	wxDir::GetAllFiles( path, &m_files, "*.tif", wxDIR_FILES );
	m_nbOfFiles = m_files.GetCount();
	if ( m_nbOfFiles != 0 )
		m_currentFile = m_files.Index( m_filename );
}

void AxImageController::Paste()
{
    if (!CanPaste())
        return;

    wxGetApp().AxBeginBusyCursor();
    
    if ( !wxTheClipboard->Open() )
    {
        wxGetApp().AxEndBusyCursor();
        wxLogError(_("Cannot open clipboard"));
        return;
    }

    if ( !wxTheClipboard->IsSupported(wxDF_BITMAP) )
    {
        wxGetApp().AxEndBusyCursor();
        wxLogError(_("Unsupported format"));
        wxTheClipboard->Close();
        return;
    }

    wxBitmapDataObject object;
    
    if ( !wxTheClipboard->GetData( object) )
    {
        wxGetApp().AxEndBusyCursor();
        wxLogError(_("Cannot read image from the clipboard"));
        wxTheClipboard->Close();
        return;
    }

    wxBitmap bitmap = object.GetBitmap();
    SetImage ( bitmap.ConvertToImage() );


    wxTheClipboard->Flush();
    wxTheClipboard->Close();
    wxGetApp().AxEndBusyCursor();
}

void AxImageController::SetImage( AxImage image )
{
    AxImage::Destroy();
    m_filename = "";
	m_shortname = "";
    m_currentPage = 0;
    m_nbOfPages = 1;
    AxImage::Create( image.GetWidth(), image.GetHeight());
    memcpy( this->GetData(), image.GetData(), image.GetWidth() * image.GetHeight() * 3 );
    m_viewPtr->UpdateZoomLimits();
    //m_viewPtr->UpdateViewFast();
	m_viewPtr->UpdateView();
}

void AxImageController::ResetImage( AxImage image )
{
    AxImage::Destroy();
    AxImage::Create( image.GetWidth(), image.GetHeight());
    memcpy( this->GetData(), image.GetData(), image.GetWidth() * image.GetHeight() * 3 );
    m_viewPtr->UpdateZoomLimits();
	m_viewPtr->UpdateView();
}

void AxImageController::Copy()
{
    if (!CanCopy())
        return;

    wxGetApp().AxBeginBusyCursor();
    wxBitmap bitmap( this->GetAxBitmap() );

    if ( !wxTheClipboard->Open() )
    {
        wxGetApp().AxEndBusyCursor();
        wxLogError(_("Cannot open clipboard"));
        return;
    }
    
    if ( !wxTheClipboard->AddData(new wxBitmapDataObject(bitmap)) )
    {
        wxLogError(_("Cannot insert the image into the clipboard"));
    }

    wxTheClipboard->Flush();
    wxTheClipboard->Close();
    wxGetApp().AxEndBusyCursor();
}

void AxImageController::Cut()
{
	this->Copy( );
	this->Close( );
}

void AxImageController::Open( const wxString fname , const int index)
{
    wxString filename;
    if ( fname.IsEmpty() )
	{
		filename = wxFileSelector( _("Open"), wxGetApp().m_lastDirTIFF_in, _T(""), NULL, IMAGE_FILES, wxOPEN);
		if (filename.IsEmpty())
			return;
		wxGetApp().m_lastDirTIFF_in = wxPathOnly( filename );
	}
    else
        filename = fname;

	this->Close();
	m_filename = filename;
	this->LoadDirectoryFilenames( );

	wxString ext;
	wxFileName::SplitPath( m_filename, NULL, &m_shortname, &ext );
	m_shortname += "." + ext;
    m_currentPage = index;
    m_nbOfPages = wxImage::GetImageCount( m_filename );
    if ( m_nbOfPages <= m_currentPage )
    {
        wxLogWarning(_("Image %d not found in file '%s'"), index, filename.c_str() );
        this->Close();
        return;
    }
    OpenPage();
}

void AxImageController::Close( bool yield )
{
    if ( this->Ok() )
        AxImage::Destroy();

    m_filename = "";
	m_shortname = "";
    m_currentPage = 0;
    m_nbOfPages = 0;
	m_currentFile = 0;
	m_nbOfFiles = 0;
	m_files.Clear( );
    m_viewPtr->UpdateZoomLimits();
    m_viewPtr->UpdateView();

    if ( yield )
		wxGetApp().Yield( );
}

void AxImageController::OpenPage( bool yield )
{
    wxASSERT_MSG(m_viewPtr,"View cannot be NULL");

    wxGetApp().AxBeginBusyCursor();
    this->LoadFile( m_filename, wxBITMAP_TYPE_ANY,  m_currentPage);
    m_viewPtr->UpdateZoomLimits();
    m_viewPtr->UpdateView();

    if ( yield )
		wxGetApp().Yield( );
    wxGetApp().AxEndBusyCursor();
}

void AxImageController::Next()
{
    if ( !HasNext() )
        return;

    if ( m_multipage )
	{
		m_currentPage += 1;
		OpenPage(); 
	}
	else
		Open( m_files[++m_currentFile] );
}



void AxImageController::Previous()
{
    if ( !HasPrevious() )
        return;

	if ( m_multipage )
	{
		m_currentPage -= 1;
		OpenPage(); 
	}
	else
		Open( m_files[--m_currentFile] );
}

void AxImageController::Goto()
{
    AxGotoDlg *dlg = new AxGotoDlg(this, -1, _("Go to page ..."), m_nbOfPages, m_currentPage );
    dlg->Center(wxBOTH);
    if ( dlg->ShowModal() == wxID_OK )
	{
		m_currentPage = dlg->GetPage();
		OpenPage();
    }
	dlg->Destroy();
}

// WDR: handler implementations for AxImageController

void AxImageController::OnUpdateUI( wxUpdateUIEvent &event )
{
    wxASSERT_MSG(m_viewPtr,"View cannot be NULL");

    // adjust
    if (event.GetId() == ID_CONTROLLER_ADJUST_VH)
        event.Check( m_viewPtr->GetAdjustMode() == ADJUST_BOTH );
    else if (event.GetId() == ID_CONTROLLER_ADJUST_V)
        event.Check( m_viewPtr->GetAdjustMode() == ADJUST_VERTICAL );
    else if (event.GetId() == ID_CONTROLLER_ADJUST_H)
        event.Check( m_viewPtr->GetAdjustMode() == ADJUST_HORIZONTAL );  
    // previous - next
    else if (event.GetId() == ID_CONTROLLER_PREVIOUS)
        event.Enable( this->HasPrevious() );
    else if (event.GetId() == ID_CONTROLLER_NEXT)
        event.Enable( this->HasNext() );  
    // zoom
    else if (event.GetId() == ID_CONTROLLER_ZOOM_IN)
        event.Enable( m_viewPtr->CanZoomIn() );
    else if (event.GetId() == ID_CONTROLLER_ZOOM_OUT)
        event.Enable( m_viewPtr->CanZoomOut() );
    else if (event.GetId() == ID_POPUP_MENU_GOTO)
        event.Enable( this->CanGoto() );
    else if (event.GetId() == ID_POPUP_MENU_BY_PAGES)
        event.Check( this->IsMultipage() );
    else if (event.GetId() == ID_POPUP_MENU_BY_FILES)
        event.Check( !this->IsMultipage() );
}

void AxImageController::OnCommand( wxCommandEvent &event )
{
    wxASSERT_MSG(m_viewPtr,"View cannot be NULL");

    if (event.GetId() == ID_CONTROLLER_PREVIOUS)
	{
        this->Previous();
	}
    else if (event.GetId() == ID_CONTROLLER_NEXT)
	{
        this->Next();
	}
    else if (event.GetId() == ID_CONTROLLER_ZOOM_OUT)
	{
        m_viewPtr->ZoomOut();
	}
    else if (event.GetId() == ID_CONTROLLER_ZOOM_IN)
	{
        m_viewPtr->ZoomIn();
	}
    else if (event.GetId() == ID_CONTROLLER_ADJUST_VH)
	{
        m_viewPtr->SetAdjustMode(ADJUST_BOTH);
	}
    else if (event.GetId() == ID_CONTROLLER_ADJUST_V)
	{
        m_viewPtr->SetAdjustMode(ADJUST_VERTICAL);
	}
    else if (event.GetId() == ID_CONTROLLER_ADJUST_H)
	{
        m_viewPtr->SetAdjustMode(ADJUST_HORIZONTAL);
	}
    else if (event.GetId() == ID_POPUP_MENU_GOTO)
	{
        this->Goto();
	}
    else if (event.GetId() == ID_POPUP_MENU_BY_PAGES)
	{
        this->SetMultipage( true );
	}
    else if (event.GetId() == ID_POPUP_MENU_BY_FILES)
	{
        this->SetMultipage( false );
	}
}



