/////////////////////////////////////////////////////////////////////////////
// Name:        sup.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_SUPERIMPOSITION

#ifdef __GNUG__
    #pragma implementation "sup.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "sup.h"
#include "supfile.h"
//#include "supctrl.h"

#include "app/axapp.h"
#include "app/axprogressdlg.h"
#include "app/axframe.h"

// statics
int SupEnv::s_segmentSize = 1200;
int SupEnv::s_split_x = 4;
int SupEnv::s_split_y = 8;
int SupEnv::s_corr_x = 30;
int SupEnv::s_corr_y = 50;
int SupEnv::s_interpolation = 0;
bool SupEnv::s_filter1 = true;
bool SupEnv::s_filter2 = true;

int SupEnv::s_book_sash = 200; // batch processing
int SupEnv::s_view_sash = 0;

bool SupEnv::s_expand_root = true;
bool SupEnv::s_expand_book = true;
bool SupEnv::s_expand_cmp = true;  

// WDR: class implementations

//----------------------------------------------------------------------------
// SupImController
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(SupImController,AxImageController)

// WDR: event table for SupImController

BEGIN_EVENT_TABLE(SupImController,AxImageController)
END_EVENT_TABLE()

SupImController::SupImController( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style , int flags) :
    AxImageController( parent, id, position, size, style, flags )
{
    m_selectCounter = 0;
}

SupImController::SupImController()
{
}

SupImController::~SupImController()
{
}

void SupImController::CloseDraggingSelection(wxPoint start, wxPoint end)
{
    m_points[m_selectCounter] = end;

    if (m_selectCounter == 0)
    {
        m_selectCounter++;
		//wxLogDebug("point 1");
    }
    else if (m_selectCounter == 1)
    {
        m_selectCounter++;
		//wxLogDebug("point 2");
    }
    else if (m_selectCounter == 2)
    {
        m_selectCounter++;
		//wxLogDebug("point 3");
    }
    else if (m_selectCounter == 3)
    {
        m_selectCounter = 0;
		//wxLogDebug("point 4");
        this->m_viewPtr->EndSelection();
    }
}


// WDR: handler implementations for SupImController


//----------------------------------------------------------------------------
// SupPanel
//----------------------------------------------------------------------------

// WDR: event table for SupPanel

BEGIN_EVENT_TABLE(SupPanel,wxPanel)
END_EVENT_TABLE()

SupPanel::SupPanel( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
    WindowFunc2( this, TRUE ); 
}

// WDR: handler implementations for SupPanel


//----------------------------------------------------------------------------
// SupEnv
//----------------------------------------------------------------------------

// WDR: event table for SupEnv

BEGIN_EVENT_TABLE(SupEnv,AxEnv)
	// copy paste 
    EVT_MENU( ID_PASTE, SupEnv::OnPaste )
    EVT_MENU( ID_COPY, SupEnv::OnCopy )
    EVT_MENU( ID_CUT, SupEnv::OnCut )
	EVT_UPDATE_UI( ID_PASTE , SupEnv::OnUpdateUI )
	EVT_UPDATE_UI( ID_COPY , SupEnv::OnUpdateUI )
	EVT_UPDATE_UI( ID_CUT , SupEnv::OnUpdateUI )
	// other
    EVT_MENU( ID2_NEXT, SupEnv::OnNextBoth )
    EVT_MENU( ID2_PREVIOUS, SupEnv::OnPreviousBoth )
    EVT_MENU( ID2_OPEN1, SupEnv::OnOpen )
    EVT_MENU( ID2_OPEN2, SupEnv::OnOpen )
    EVT_MENU( ID2_POINTS, SupEnv::OnPutPoints )
    EVT_MENU( ID2_SUPERIMPOSE, SupEnv::OnSuperimpose )
    EVT_UPDATE_UI_RANGE( wxID_LOWEST, ENV_IDS_MAX, SupEnv::OnUpdateUI )
    EVT_MENU( ID_CLOSE, SupEnv::OnClose )
    EVT_MENU( ID2_GOTO1, SupEnv::OnGoto )
    EVT_MENU( ID2_GOTO2, SupEnv::OnGoto )
    EVT_MENU( ID2_PREVIOUS1, SupEnv::OnPrevious )
    EVT_MENU( ID2_PREVIOUS2, SupEnv::OnPrevious )
    EVT_MENU( ID2_NEXT1, SupEnv::OnNext )
    EVT_MENU( ID2_NEXT2, SupEnv::OnNext )
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(SupEnv,AxEnv)

SupEnv::SupEnv():
    AxEnv()
{
    m_imControl1Ptr = NULL;
    m_imControl2Ptr = NULL;
    m_view1Ptr = NULL;
    m_view2Ptr = NULL;

    this->m_envMenuBarFuncPtr = MenuBarFunc2;
}

SupEnv::~SupEnv()
{
    if (m_envWindowPtr) m_envWindowPtr->Destroy();
    m_envWindowPtr = NULL;
}

void SupEnv::LoadWindow()
{
    this->m_envWindowPtr = new SupPanel(m_framePtr,-1);
    if (!m_envWindowPtr)
        return;

    SupPanel *panel = ((SupPanel*)m_envWindowPtr);
    if (!panel)
        return;

    wxSplitterWindow *splitter = panel->GetSplitter1();
    if (!splitter)
        return;

    splitter->SetWindowStyleFlag( wxSP_FULLSASH );
    splitter->SetMinimumPaneSize( 100 );

    m_imControl1Ptr = new SupImController( splitter, ID2_CONTROLLER1  );
    m_view1Ptr = new AxScrolledWindow( m_imControl1Ptr, ID2_VIEW1 , wxDefaultPosition, 
        wxDefaultSize, wxHSCROLL| wxVSCROLL | wxSUNKEN_BORDER);
    m_view1Ptr->m_popupMenu.AppendSeparator();
    m_view1Ptr->m_popupMenu.Append(ID2_POINTS,_("Put points"));
    m_imControl1Ptr->Init( this, m_view1Ptr );


    m_imControl2Ptr = new SupImController( splitter, ID2_CONTROLLER2 );
    m_view2Ptr = new AxScrolledWindow( m_imControl2Ptr, ID2_VIEW2 , wxDefaultPosition, 
        wxDefaultSize, wxHSCROLL| wxVSCROLL | wxSUNKEN_BORDER);
    m_view2Ptr->m_popupMenu.AppendSeparator();
    m_view2Ptr->m_popupMenu.Append(ID2_POINTS,_("Put points"));
    m_imControl2Ptr->Init( this, m_view2Ptr );

    splitter->SplitVertically(m_imControl1Ptr,m_imControl2Ptr);
}

void SupEnv::LoadConfig()
{
    wxConfigBase *pConfig = wxConfigBase::Get();
    wxASSERT_MSG( pConfig, wxT("pConfig cannot be NULL") );
    pConfig->SetPath("/Superimposition");

    // superimposition
    SupEnv::s_segmentSize = pConfig->Read("SegmentSize", 1200);
    SupEnv::s_interpolation = pConfig->Read("Interpolation", 0L);
    SupEnv::s_split_x = pConfig->Read("SplitX", 4);
    SupEnv::s_split_y = pConfig->Read("SplitY", 8);
    SupEnv::s_corr_x = pConfig->Read("CorrX", 30);
    SupEnv::s_corr_y = pConfig->Read("CorrY", 50);
    SupEnv::s_filter1 = (pConfig->Read("Filter1",1)==1);
    SupEnv::s_filter2 = (pConfig->Read("Filter2",1)==1);

    pConfig->SetPath("/");
}



void SupEnv::SaveConfig()
{
    wxConfigBase *pConfig = wxConfigBase::Get();
    wxASSERT_MSG( pConfig, wxT("pConfig cannot be NULL") );
    pConfig->SetPath("/Superimposition");

    // superimposition
    pConfig->Write("SegmentSize", SupEnv::s_segmentSize);
    pConfig->Write("Interpolation", SupEnv::s_interpolation);
    pConfig->Write("SplitX", SupEnv::s_split_x);
    pConfig->Write("SplitY", SupEnv::s_split_y);
    pConfig->Write("CorrX", SupEnv::s_corr_x);
    pConfig->Write("CorrY", SupEnv::s_corr_y);
    pConfig->Write("Filter1", SupEnv::s_filter1);
    pConfig->Write("Filter2", SupEnv::s_filter2);

    pConfig->SetPath("/");
}

void SupEnv::RealizeToolbar( )
{
	ToolBarFunc2( m_framePtr->GetToolBar() ); // function generated by wxDesigner
}

void SupEnv::ParseCmd( wxCmdLineParser *parser )
{
    wxASSERT_MSG( parser, wxT("Parser cannot be NULL") );
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if ( parser->Parse( false ) == 0) 
    {
        if ( parser->GetParamCount() > 1 )
            m_imControl1Ptr->Open( parser->GetParam (1) );
        if ( parser->GetParamCount() > 2 )
            m_imControl2Ptr->Open( parser->GetParam (2) );
    }
}

void SupEnv::UpdateTitle( )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    wxString msg;
    if ( m_imControl1Ptr->Ok() )
    {
        msg += m_imControl1Ptr->GetTitleMsg().c_str();
    }

    if ( m_imControl2Ptr->Ok() )
    {
        msg += ( msg.IsEmpty() ) ? "" : " - ";
        msg += m_imControl2Ptr->GetTitleMsg().c_str();
    }

    SetTitle( "%s", msg.c_str() );
}


// WDR: handler implementations for SupEnv

void SupEnv::OnPrevious( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if ( (event.GetId() == ID2_PREVIOUS1) && m_imControl1Ptr->HasPrevious() )
        m_imControl1Ptr->Previous();
    else if ((event.GetId() == ID2_PREVIOUS2) && m_imControl2Ptr->HasPrevious() )
        m_imControl2Ptr->Previous();      
}

void SupEnv::OnNext( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if ( (event.GetId() == ID2_NEXT1) && m_imControl1Ptr->HasNext() )
        m_imControl1Ptr->Next();
    else if ((event.GetId() == ID2_NEXT2) && m_imControl2Ptr->HasNext() )
        m_imControl2Ptr->Next();    
}

void SupEnv::OnGoto( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if ( (event.GetId() == ID2_GOTO1) && m_imControl1Ptr->CanGoto() )
        m_imControl1Ptr->Goto();
    else if ((event.GetId() == ID2_GOTO2) && m_imControl2Ptr->CanGoto() )
        m_imControl2Ptr->Goto();      
}

void SupEnv::OnClose( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if ( m_imControl1Ptr->Ok() )
        m_imControl1Ptr->Close();
    if ( m_imControl2Ptr->Ok() )
        m_imControl2Ptr->Close();   
}

void SupEnv::OnNextBoth( wxCommandEvent &event )
{ 
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if (!m_imControl1Ptr->HasNext() || !m_imControl2Ptr->HasNext())
        return;

    m_imControl1Ptr->Next();
    m_imControl2Ptr->Next();
}

void SupEnv::OnPreviousBoth( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if (!m_imControl1Ptr->HasPrevious() || !m_imControl2Ptr->HasPrevious())
        return;

    m_imControl1Ptr->Previous();
    m_imControl2Ptr->Previous();
}

void SupEnv::OnPutPoints( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if ( m_imControl1Ptr->Ok() )
    {
        m_view1Ptr->BeginSelection( SHAPE_POINT );
    }
    if ( m_imControl2Ptr->Ok() )
    {
        m_view2Ptr->BeginSelection( SHAPE_POINT );
    }
}

void SupEnv::OnSuperimpose( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if ( !m_imControl1Ptr->Ok() || !m_imControl2Ptr->Ok() )
        return;

    wxString filename = wxFileSelector( _("Save"), wxGetApp().m_lastDirTIFF_out , _T(""), NULL, IMAGE_FILES, wxSAVE );
    if (filename.IsEmpty())
        return;
	wxGetApp().m_lastDirTIFF_out = wxPathOnly( filename );

    // debug
    /*m_imControl1Ptr->m_points[0] = wxPoint(26,40);
    m_imControl1Ptr->m_points[1] = wxPoint(26,840);
    m_imControl1Ptr->m_points[2] = wxPoint(580,840);
    m_imControl2Ptr->m_points[0] = wxPoint(26,40);
    m_imControl2Ptr->m_points[1] = wxPoint(26,840);
    m_imControl2Ptr->m_points[2] = wxPoint(580,840);*/


    AxProgressDlg *dlg = new AxProgressDlg( m_framePtr, -1, _("Superposition") );
    dlg->Center( wxBOTH );
    dlg->Show();
    dlg->SetMaxBatchBar( 1 );
//    dlg->SetBatchBar( "Superposition (1/1)", 0 );
    wxYield();

    SupOldFile sp( "", "", dlg );
    if ( sp.Superimpose( m_imControl1Ptr, m_imControl2Ptr, filename  ))

    imCounterEnd( dlg->GetCounter() );
    dlg->GetGaugeBatch()->SetValue( 100 );
#ifdef __WXMAC__
	dlg->Show(false);  // otherwise doesn't stop	
#endif	
    dlg->ShowModal( ); // stop process
    dlg->Destroy();
    
    if ( sp.GetError()!= ERR_NONE ) // operation annulee ou failed
        return;

    int res = wxMessageBox("Visualiser le resultat de la superposition?", wxGetApp().GetAppName() ,
                            wxYES | wxNO | wxICON_QUESTION );
    if (res != wxYES)
        return;

	// TODO, changed as Dsp doesn't exist anymore...
	/*
    static const wxCmdLineEntryDesc cmdLineDesc[] = {
        {wxCMD_LINE_PARAM,  NULL, NULL, _("input files"),
         wxCMD_LINE_VAL_STRING,
         wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE},
        {wxCMD_LINE_NONE}
    };

    wxCmdLineParser parser( cmdLineDesc );
    parser.SetCmdLine("cmd \"" + filename + "\"");
    ((AxFrame*)m_framePtr)->SetEnvironment( "DspEnv" );
    ((AxFrame*)m_framePtr)->ParseCmd( &parser );
	*/
}

void SupEnv::OnPaste( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
        return;

    if (win->GetId() == ID2_VIEW1)
        m_imControl1Ptr->Paste();
    else if (win->GetId() == ID2_VIEW2)
        m_imControl2Ptr->Paste();
}

void SupEnv::OnCopy( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
        return;

    if (win->GetId() == ID2_VIEW1)
        m_imControl1Ptr->Copy();
    else if (win->GetId() == ID2_VIEW2)
        m_imControl2Ptr->Copy();
}

void SupEnv::OnCut( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
        return;

    if (win->GetId() == ID2_VIEW1)
        m_imControl1Ptr->Cut();
    else if (win->GetId() == ID2_VIEW2)
        m_imControl2Ptr->Cut();
}



void SupEnv::OnOpen( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if (event.GetId() == ID2_OPEN1)
        m_imControl1Ptr->Open();
    else if (event.GetId() == ID2_OPEN2)
        m_imControl2Ptr->Open();
}

void SupEnv::OnUpdateUI( wxUpdateUIEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
        return;

    int id = event.GetId();

    if (id == ID_CUT)
    {
        if (win->GetId() == ID2_VIEW1)
            event.Enable( m_imControl1Ptr->CanCut() );
        else if (win->GetId() == ID2_VIEW2)
            event.Enable( m_imControl2Ptr->CanCut() );
        else
            event.Enable(false);
    }
    else if (id == ID_COPY)
    {
        if (win->GetId() == ID2_VIEW1)
            event.Enable( m_imControl1Ptr->CanCopy() );
        else if (win->GetId() == ID2_VIEW2)
            event.Enable( m_imControl2Ptr->CanCopy() );
        else
            event.Enable(false);
    }
    else if (id == ID_PASTE)
    {
        if (win->GetId() == ID2_VIEW1)
            event.Enable( m_imControl1Ptr->CanPaste() );
        else if (win->GetId() == ID2_VIEW2)
            event.Enable( m_imControl2Ptr->CanPaste() );
        else
            event.Enable(false);
    }
    else if (id == ID2_POINTS)
    {
        event.Enable( m_imControl1Ptr->Ok() && m_imControl2Ptr->Ok() );
    }
    else if (id == ID2_SUPERIMPOSE)
    {
        event.Enable( m_imControl1Ptr->Ok() && m_imControl2Ptr->Ok() );
    }
    else if (id == ID2_NEXT)
    {
        event.Enable( m_imControl1Ptr->HasNext() && m_imControl2Ptr->HasNext() );
    }
    else if (id == ID2_NEXT1)
    {
        event.Enable( m_imControl1Ptr->HasNext());
    }
    else if (id == ID2_NEXT2)
    {
        event.Enable( m_imControl2Ptr->HasNext());
    }
    else if (id == ID2_PREVIOUS)
    {
        event.Enable( m_imControl1Ptr->HasPrevious() && m_imControl2Ptr->HasPrevious() );
    }
    else if (id == ID2_PREVIOUS1)
    {
        event.Enable( m_imControl1Ptr->HasPrevious() );
    }
    else if (id == ID2_PREVIOUS2)
    {
        event.Enable( m_imControl2Ptr->HasPrevious());
    }
    else if (id == ID2_GOTO1)
    {
        event.Enable( m_imControl1Ptr->CanGoto() );
    }
    else if (id == ID2_GOTO2)
    {
        event.Enable( m_imControl2Ptr->CanGoto());
    }
    else
        event.Enable(true);
}

#endif // AX_SUPERIMPOSITION

