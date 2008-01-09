/////////////////////////////////////////////////////////////////////////////
// Name:        display.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_DISPLAY

#ifdef __GNUG__
    #pragma implementation "display.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/config.h"
#include "wx/valgen.h"

#include "display.h"

// IMLIB
#include <im.h>
#include <im_image.h>
#include <im_convert.h>
#include <im_process.h>
#include <im_util.h>
#include <im_binfile.h>
#include <im_math_op.h>

#include "app/axapp.h"
#include "app/axframe.h"

// statics
int DspEnv::s_sashPosition = 800;

// WDR: class implementations

//----------------------------------------------------------------------------
// DspSrcWindow
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(DspSrcWindow, AxScrolledWindow)

// WDR: event table for DspSrcWindow

BEGIN_EVENT_TABLE(DspSrcWindow,AxScrolledWindow)
END_EVENT_TABLE()

DspSrcWindow::DspSrcWindow( AxImageController *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    AxScrolledWindow( parent, id, position, size, style )
{
    m_circleCenter = wxPoint(0,0);
}

DspSrcWindow::DspSrcWindow()
{
}

DspSrcWindow::~DspSrcWindow()
{
}

void DspSrcWindow::SetCirclePen( const wxPen *pen, int width )
{
    m_pen = *pen;
    m_pen.SetWidth( width );
}

void DspSrcWindow::DrawCircle( )
{
    wxASSERT_MSG(m_bitmap,"Bitmap cannot be NULL");
    wxASSERT_MSG(m_bufferBitmap,"Buffer bitmap cannot be NULL");

    // calculate scroll position
    int scrollX, scrollY;
    this->GetViewStart(&scrollX,&scrollY);
    int unitX, unitY;
    this->GetScrollPixelsPerUnit(&unitX,&unitY);
    scrollX *= unitX;
    scrollY *= unitY;

    wxMemoryDC memDC;
    memDC.SelectObject(*m_bufferBitmap);
    
    wxMemoryDC bufDC;
    bufDC.SelectObject(*m_bitmap);
    bufDC.SetBrush( *wxTRANSPARENT_BRUSH );
    bufDC.SetPen( m_pen );

    int clientX, clientY;
    this->GetClientSize( &clientX, &clientY );

    bufDC.Blit(scrollX, scrollY, clientX, clientY,
        &memDC, scrollX, scrollY );

    memDC.SelectObject( wxNullBitmap );
    
    int radius = 40; 
    bufDC.DrawCircle( m_circleCenter.x, m_circleCenter.y, radius);

    wxClientDC dc(this);
    dc.Blit(0, 0, clientX, clientY,
        &bufDC, scrollX, scrollY);

    bufDC.SelectObject( wxNullBitmap );
}

void DspSrcWindow::ScrollSource( double x, double y )
{
    // calculate scroll position
    int scrollX = (int)(x * m_scale);
    int scrollY = (int)(y * m_scale);

    int clientX, clientY;
    this->GetClientSize( &clientX, &clientY );

    m_circleCenter.x = scrollX;
    m_circleCenter.y = scrollY;

    scrollX -= clientX / 2;
    scrollY -= clientY / 2;
    
    int unitX, unitY;
    this->GetScrollPixelsPerUnit( &unitX,&unitY );

    if ( unitX )
        scrollX /= unitX;
    if ( unitY )
        scrollY /= unitY;

    scrollX = ( scrollX < 0 ) ? 0 : scrollX;
    scrollY = ( scrollY < 0 ) ? 0 : scrollY;

    this->Scroll( scrollX, scrollY );
}

// WDR: handler implementations for DspSrcWindow


//----------------------------------------------------------------------------
// DspResWindow
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(DspResWindow, AxScrolledWindow)

// WDR: event table for DspResWindow

BEGIN_EVENT_TABLE(DspResWindow,AxScrolledWindow)
    EVT_MOUSE_EVENTS( DspResWindow::OnMouse )
END_EVENT_TABLE()

DspResWindow::DspResWindow( AxImageController *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    AxScrolledWindow( parent, id, position, size, style )
{
}

DspResWindow::DspResWindow()
{
}

DspResWindow::~DspResWindow()
{
}

void DspResWindow::SynchronizeScroll( int x, int y )
{

    wxASSERT_MSG( m_imControlPtr,"Image controller cannot be NULL" );
    wxClassInfo *info;
    info = m_imControlPtr->GetClassInfo();
    wxASSERT_MSG( info,"Class info cannot be NULL" );
    wxASSERT_MSG( info->IsKindOf( CLASSINFO( DspImController ) ),
        "Controller must be a DspImController");

    DspImController *controller = (DspImController*)m_imControlPtr;

    // calculate scroll position
    int scrollX, scrollY;
    this->GetViewStart(&scrollX,&scrollY);
    int unitX, unitY;
    this->GetScrollPixelsPerUnit(&unitX,&unitY);
    scrollX *= unitX;
    scrollY *= unitY;

    double absX = (double)(x + scrollX) / m_scale;
    double absY = (double)(y + scrollY) / m_scale;
        
    controller->ScrollSources( absX, absY );
    this->SetFocus();
}

// WDR: handler implementations for DspResWindow

void DspResWindow::OnMouse(wxMouseEvent &event)
{

    wxASSERT_MSG( m_imControlPtr,"Image controller cannot be NULL" );
    wxClassInfo *info;
    info = m_imControlPtr->GetClassInfo();
    wxASSERT_MSG( info,"Class info cannot be NULL" );
    wxASSERT_MSG( info->IsKindOf( CLASSINFO( DspImController ) ),
        "Controller must be a DspImController");

    DspImController *controller = (DspImController*)m_imControlPtr;


    // enter and leaving window
    if (event.GetEventType() == wxEVT_LEAVE_WINDOW)
    {
        this->SetCursor(*wxSTANDARD_CURSOR);
        event.Skip();
        return;
    }

    if (event.GetEventType() == wxEVT_ENTER_WINDOW)
    {
        if (event.m_leftDown)
            this->SetCursor( wxCURSOR_HAND );
    }

    // checking bitmap and position
    if (!m_bitmap || ( event.m_x > m_bitmap->GetWidth() ) || (event.m_y > m_bitmap->GetHeight() ) )
    {
        event.Skip();
        return;
    }


    if (event.m_middleDown)
    {
        if (!event.Dragging() && !event.m_wheelRotation)
        {
            this->SynchronizeScroll( event.m_x, event.m_y );
            wxGetApp().Yield();
            controller->DrawCircles( );
            this->SetFocus();
            event.Skip();
        }
        return;
    }
    if (event.GetEventType() == wxEVT_MIDDLE_UP)
    {        
        controller->DrawCircles( true );
        this->SetFocus();
    }

    // left up and down
    if (event.GetEventType() == wxEVT_LEFT_UP)
    {
        this->SetCursor( *wxSTANDARD_CURSOR );
    }

    if (event.GetEventType() == wxEVT_LEFT_DOWN)
    {
        this->SetCursor( wxCURSOR_HAND );
        this->SynchronizeScroll( event.m_x, event.m_y );
    }

    // dragging with left down
    if ( event.Dragging() && event.m_leftDown)
    {
        this->SynchronizeScroll( event.m_x, event.m_y );
    }
    event.Skip();
}



//----------------------------------------------------------------------------
// DspImController
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(DspImController,AxImageController)

// WDR: event table for DspImController

BEGIN_EVENT_TABLE(DspImController,AxImageController)
END_EVENT_TABLE()

DspImController::DspImController( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style , int flags) :
    AxImageController( parent, id, position, size, style, flags )
{
    m_redIm = NULL;
    m_greenIm = NULL;
    m_imControl1Ptr = NULL;
    m_imControl2Ptr = NULL;
    m_viewSrc1Ptr = NULL;
    m_viewSrc2Ptr = NULL;

    m_red = 0;
    m_green = 0;
}

DspImController::DspImController()
{
}

DspImController::~DspImController()
{
    if ( m_redIm ) imImageDestroy( m_redIm );
    if ( m_greenIm ) imImageDestroy( m_greenIm );
}

void DspImController::SetControllers( AxImageController *controller1, AxImageController *controller2 )
{
    wxASSERT_MSG( controller1,"Controller 1 cannot be NULL");
    wxASSERT_MSG( controller2,"Controller 2 cannot be NULL");

    m_imControl1Ptr = controller1;
    m_imControl2Ptr = controller2;
}

void DspImController::SetViews( DspSrcWindow *view1, DspSrcWindow *view2 )
{
    wxASSERT_MSG( view1,"View 1 cannot be NULL");
    wxASSERT_MSG( view2,"View 2 cannot be NULL");

    m_viewSrc1Ptr = view1;
    m_viewSrc2Ptr = view2;
}

void DspImController::OpenPage( bool yield )
{
    AxImageController::OpenPage( false );

    wxGetApp().AxBeginBusyCursor();

    imImage *im1 = GetImImage( this, ( IM_RGB ) );

    if ( m_redIm ) imImageDestroy( m_redIm );
    m_redIm = NULL;
    if ( m_greenIm ) imImageDestroy( m_greenIm );
    m_greenIm = NULL;

    // memorisation des 2 buffers
    m_redIm = imImageCreate( im1->width, im1->height, IM_GRAY, IM_BYTE );
    memcpy( m_redIm->data[0], im1->data[0], m_redIm->size );
    m_greenIm = imImageCreate( im1->width, im1->height, IM_GRAY, IM_BYTE );
    memcpy( m_greenIm->data[0], im1->data[1], m_greenIm->size );

    if ( ( m_red != 0 ) || ( m_green != 0 ))
    {
        this->UpdateBrightness( );

    }
    wxGetApp().Yield( );

    // image sources
    wxASSERT_MSG( m_imControl1Ptr, "Controller 1 cannot be NULL");
    wxASSERT_MSG( m_imControl2Ptr, "Controller 2 cannot be NULL");
    AxImage img;
    SetImImage( m_redIm, &img );
    m_imControl1Ptr->SetImage( img );
    wxGetApp().Yield( );

    if ( this->m_nbOfPages > 1 )
    {
        m_imControl2Ptr->Open( m_filename, 1 );
    }
    else
    {
        SetImImage( m_greenIm, &img );
        m_imControl2Ptr->SetImage( img );
    }
    
    imImageDestroy( im1 );

    wxGetApp().AxEndBusyCursor();
}

void DspImController::UpdateBrightness( )
{
    if (! m_redIm || ! m_greenIm ) 
        return;

    wxASSERT_MSG(m_viewPtr,"View cannot be NULL");

    wxGetApp().AxBeginBusyCursor();
    
    int w = m_redIm->width;
    int h = m_redIm->height;
    // ajustement de brightness
    imImage *im1 = imImageCreate( w, h, IM_RGB, IM_BYTE );
    imImage *r_buf = imImageDuplicate( m_redIm );
    imImage *g_buf = imImageDuplicate( m_greenIm );
    imImage *imTmp = imImageCreate( w, h, IM_GRAY, IM_BYTE );

    float param[2] = { 0, 0 }; // %

    if ( m_green != 0 )
    {
        param[0] = 10.0 * (float)m_green;
        imProcessToneGamut( r_buf , imTmp, IM_GAMUT_BRIGHTCONT, param);
        imProcessBitwiseOp( r_buf, g_buf, r_buf, IM_BIT_OR ); // valeurs communes doivent rester à 100%
        imProcessBitwiseOp( imTmp, r_buf, r_buf, IM_BIT_AND ); // AND entre valeurs communes et brightness ajuste
    }
    if ( m_red != 0 )
    {
        param[0] = 10.0 * (float)m_red;
        imProcessToneGamut( g_buf , imTmp, IM_GAMUT_BRIGHTCONT, param);
        imProcessBitwiseOp( g_buf, r_buf, g_buf, IM_BIT_OR ); // valeurs communes doivent rester à 100%
        imProcessBitwiseOp( imTmp, g_buf, g_buf, IM_BIT_AND ); // AND entre valeurs communes et brightness ajuste
    }
    imProcessBitwiseOp( r_buf, g_buf, imTmp, IM_BIT_AND );

    memcpy( im1->data[0], r_buf->data[0], r_buf->size );
    memcpy( im1->data[1], g_buf->data[0], g_buf->size );
    memcpy( im1->data[2], imTmp->data[0], imTmp->size );

    imImageDestroy( r_buf );
    imImageDestroy( g_buf );
    imImageDestroy( imTmp );


    SetImImage( im1, this );

    imImageDestroy( im1 );
    m_viewPtr->UpdateView();
    wxGetApp().AxEndBusyCursor();
}


void DspImController::CloseDraggingSelection(wxPoint start, wxPoint end)
{

}


void DspImController::ScrollSources( double x, double y )
{
    wxASSERT_MSG( m_viewSrc1Ptr, "View 1 cannot be NULL");
    wxASSERT_MSG( m_viewSrc2Ptr, "View 2 cannot be NULL");

    m_viewSrc1Ptr->ScrollSource( x, y );
    m_viewSrc2Ptr->ScrollSource( x, y );
}

void DspImController::DrawCircles( bool clear )
{
    wxASSERT_MSG( m_viewSrc1Ptr, "View 1 cannot be NULL");
    wxASSERT_MSG( m_viewSrc2Ptr, "View 2 cannot be NULL");

    if ( !clear )
    {
        m_viewSrc1Ptr->DrawCircle( );
        m_viewSrc2Ptr->DrawCircle( );
    }
    else
    {
        m_viewSrc1Ptr->RedrawBuffer( );
        m_viewSrc2Ptr->RedrawBuffer( );
    }
}


// WDR: handler implementations for DspImController


//----------------------------------------------------------------------------
// DspPanel
//----------------------------------------------------------------------------

// WDR: event table for DspPanel

BEGIN_EVENT_TABLE(DspPanel,wxPanel)
    EVT_SPLITTER_SASH_POS_CHANGED( ID3_SPLITTER1, DspPanel::OnSashPosition )
END_EVENT_TABLE()

DspPanel::DspPanel( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
    WindowFunc3( this, TRUE ); 
}

// WDR: handler implementations for DspPanel

void DspPanel::OnSashPosition( wxSplitterEvent &event )
{
    if ( GetParent() && GetParent()->GetEventHandler() )
        GetParent()->GetEventHandler()->ProcessEvent( event );
}

//----------------------------------------------------------------------------
// ToolBar Function with sliders
//----------------------------------------------------------------------------

// fonction avec ajout de controles speciaux (sliders, ...)
// appel la fonction ToolBarFunc3
void ToolBarFuncAddOns3( wxToolBar *parent )
{
    ToolBarFunc3( parent );

#if (wxUSE_TOOLBAR_NATIVE && !USE_GENERIC_TBAR) && !defined(__WXMOTIF__) && !defined(__WXX11__) && !defined(__WXMAC__)

    wxStaticText *rouge = new wxStaticText( parent, -1, _("Red ") );
    wxSlider *slider1 = new wxSlider(parent, ID3_SLIDER_R, 0, 0, 10, wxDefaultPosition, wxSize(80,-1) );
    wxStaticText *vert = new wxStaticText( parent, -1, _("Green ") );
    wxSlider *slider2 = new wxSlider(parent, ID3_SLIDER_G, 0, 0, 10, wxDefaultPosition, wxSize(80,-1) );
    parent->AddSeparator();
    parent->AddControl(rouge);
    parent->AddControl(slider1);
    parent->AddSeparator();
    parent->AddControl(vert);
    parent->AddControl(slider2);

#endif // toolbars which don't support controls

}

//----------------------------------------------------------------------------
// DspEnv
//----------------------------------------------------------------------------


// WDR: event table for DspEnv

BEGIN_EVENT_TABLE(DspEnv,AxEnv)
	// copy paste 
    EVT_MENU( ID_PASTE, DspEnv::OnPaste )
    EVT_MENU( ID_COPY, DspEnv::OnCopy )
    EVT_MENU( ID_CUT, DspEnv::OnCut )
	EVT_UPDATE_UI( ID_PASTE , DspEnv::OnUpdateUI )
	EVT_UPDATE_UI( ID_COPY , DspEnv::OnUpdateUI )
	EVT_UPDATE_UI( ID_CUT , DspEnv::OnUpdateUI )
	// other
    EVT_MENU( ID_OPEN, DspEnv::OnOpen )
    EVT_MENU( ID3_OPEN1, DspEnv::OnOpen )
    EVT_MENU( ID3_OPEN2, DspEnv::OnOpen )
    // redirections des sliders de barre de menu    
#if defined(__WXMSW__) 
    EVT_SCROLL_ENDSCROLL( DspEnv::OnRedGreen )
#else
    EVT_SCROLL_ENDSCROLL( DspEnv::OnRedGreen )
    //EVT_SLIDER( ID3_SLIDER_R, DspEnv::OnRedGreen )
    //EVT_SLIDER( ID3_SLIDER_G, DspEnv::OnRedGreen )
#endif
    EVT_UPDATE_UI_RANGE( wxID_LOWEST, ENV_IDS_MAX , DspEnv::OnUpdateUI )
    EVT_SPLITTER_SASH_POS_CHANGED( ID3_SPLITTER1, DspEnv::OnSashPosition )
    EVT_MENU( ID_CLOSE, DspEnv::OnClose )
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(DspEnv,AxEnv)

DspEnv::DspEnv():
    AxEnv()
{
    m_imControlPtr = NULL;
    m_imControl1Ptr = NULL;
    m_imControl2Ptr = NULL;
    m_viewPtr = NULL;
    m_view1Ptr = NULL;
    m_view2Ptr = NULL;

    this->m_envMenuBarFuncPtr = MenuBarFunc3;
}

DspEnv::~DspEnv()
{
    if (m_envWindowPtr) m_envWindowPtr->Destroy();
    m_envWindowPtr = NULL;
}

void DspEnv::LoadWindow()
{
    this->m_envWindowPtr = new DspPanel((wxFrame*)m_framePtr,-1);
    if (!m_envWindowPtr)
        return;

    DspPanel *panel = ((DspPanel*)m_envWindowPtr);
    if (!panel)
        return;

    wxSplitterWindow *splitter = panel->GetSplitter1();
    if (!splitter)
        return;

    wxSplitterWindow *splitter2 = new wxSplitterWindow( splitter, -1, wxDefaultPosition, wxDefaultSize, 0 );
    if (!splitter2)
        return;

    splitter->SetWindowStyleFlag(wxSP_FULLSASH);
    splitter->SetMinimumPaneSize(100);

    splitter2->SetWindowStyleFlag(wxSP_FULLSASH);
    splitter2->SetMinimumPaneSize(100);


    m_imControl1Ptr = new AxImageController( splitter2, ID3_CONTROLLER1, wxDefaultPosition, wxDefaultSize, 0,
        CONTROLLER_NO_ADJUST | CONTROLLER_NO_NAVIGATION );
    m_view1Ptr = new DspSrcWindow( m_imControl1Ptr, ID3_VIEW1 , wxDefaultPosition, 
        wxDefaultSize, wxHSCROLL| wxVSCROLL | wxSUNKEN_BORDER);
    ((DspSrcWindow*)m_view1Ptr)->SetCirclePen( wxGREEN_PEN, 2 );
    m_imControl1Ptr->Init( this, m_view1Ptr );


    m_imControl2Ptr = new AxImageController( splitter2, ID3_CONTROLLER2, wxDefaultPosition, wxDefaultSize, 0,
        CONTROLLER_NO_ADJUST | CONTROLLER_NO_NAVIGATION);
    m_view2Ptr = new DspSrcWindow( m_imControl2Ptr, ID3_VIEW2 , wxDefaultPosition, 
        wxDefaultSize, wxHSCROLL| wxVSCROLL | wxSUNKEN_BORDER);
    ((DspSrcWindow*)m_view2Ptr)->SetCirclePen( wxRED_PEN, 2 );
    m_imControl2Ptr->Init( this, m_view2Ptr );


    m_imControlPtr = new DspImController( splitter, ID3_CONTROLLER, wxDefaultPosition, wxDefaultSize, 0,
        CONTROLLER_NO_MULTIPAGE  );
    m_imControlPtr->SetControllers( m_imControl1Ptr, m_imControl2Ptr );
    m_imControlPtr->SetViews( (DspSrcWindow*)m_view1Ptr, (DspSrcWindow*)m_view2Ptr );
    m_viewPtr = new DspResWindow( m_imControlPtr, ID3_VIEW , wxDefaultPosition, 
        wxDefaultSize, wxHSCROLL| wxVSCROLL | wxSUNKEN_BORDER);
    //m_viewPtr->m_popupMenu.AppendSeparator();
    //m_viewPtr->m_popupMenu.Append(ID3_POINTS,_("Placer les reperes"));
    m_imControlPtr->Init( this, m_viewPtr);

    splitter2->SplitHorizontally(m_imControl1Ptr,m_imControl2Ptr);
    splitter->SplitVertically(m_imControlPtr, splitter2, s_sashPosition );
}

void DspEnv::LoadConfig()
{
    wxConfigBase *pConfig = wxConfigBase::Get();
    wxASSERT_MSG( pConfig, wxT("pConfig cannot be NULL") );
    pConfig->SetPath("/Display");

    // display
    DspEnv::s_sashPosition = pConfig->Read("SashPosition", 800);

    pConfig->SetPath("/");
}



void DspEnv::SaveConfig()
{
    wxConfigBase *pConfig = wxConfigBase::Get();
    wxASSERT_MSG( pConfig, wxT("pConfig cannot be NULL") );
    pConfig->SetPath("/Display");

    pConfig->Write("SashPosition", DspEnv::s_sashPosition);

    pConfig->SetPath("/");
}

void DspEnv::ParseCmd( wxCmdLineParser *parser )
{
    wxASSERT_MSG( parser, wxT("Parser cannot be NULL") );
    wxASSERT_MSG( m_imControlPtr, wxT("Image controller  cannot be NULL") );
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if ( parser->Parse( false ) == 0) 
    {
        if ( parser->GetParamCount() > 1 )
            m_imControlPtr->Open( parser->GetParam (1) );
    }
}

void DspEnv::RestoreToolBar()
{
    if (!m_framePtr)
        return;

    if (this->GetRedSlider())
        this->GetRedSlider()->SetValue( this->m_imControlPtr->m_red );
    if (this->GetGreenSlider())
        this->GetGreenSlider()->SetValue( this->m_imControlPtr->m_green );
}

void DspEnv::RealizeToolbar( )
{
	ToolBarFuncAddOns3( m_framePtr->GetToolBar() ); // function generated by wxDesigner with addons
}

void DspEnv::UpdateTitle( )
{
    wxASSERT_MSG( m_imControlPtr, wxT("Image controller  cannot be NULL") );
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if ( !m_imControlPtr->Ok() )
    {
        SetTitle( "" );
        return;
    }

    SetTitle( "%s", m_imControlPtr->GetTitleMsg().c_str() );
}

wxSlider* DspEnv::GetRedSlider()
{
	return (wxSlider*) m_framePtr->FindWindow( ID3_SLIDER_R ); 
}

wxSlider* DspEnv::GetGreenSlider()
{
	return (wxSlider*) m_framePtr->FindWindow( ID3_SLIDER_G ); 
}


// WDR: handler implementations for DspEnv

void DspEnv::OnClose( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControlPtr, wxT("Image controller cannot be NULL") );
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if ( m_imControlPtr->Ok() )
        m_imControlPtr->Close();
    if ( m_imControl1Ptr->Ok() )
        m_imControl1Ptr->Close();
    if ( m_imControl2Ptr->Ok() )
        m_imControl2Ptr->Close();      
}

void DspEnv::OnSashPosition( wxSplitterEvent &event )
{
    this->s_sashPosition = event.GetSashPosition();
}

void DspEnv::OnRedGreen( wxScrollEvent &event )
//void DspEnv::OnRedGreen( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControlPtr, wxT("Image controller cannot be NULL") );

    if ( event.GetId() == ID3_SLIDER_R )
    {
        if ( m_imControlPtr->m_red != event.GetPosition() )
        {
            m_imControlPtr->m_red = event.GetPosition();
            m_imControlPtr->UpdateBrightness( );
        }
        
    }
    else if ( event.GetId() == ID3_SLIDER_G )
    {
        if ( m_imControlPtr->m_green != event.GetPosition() )
        {
            m_imControlPtr->m_green = event.GetPosition();
            m_imControlPtr->UpdateBrightness( );
        }
    }
}

void DspEnv::OnPaste( wxCommandEvent &event )
{
    wxLogDebug("Paste");
    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
        return;

    if (win->GetId() == ID3_VIEW)
        m_imControlPtr->Paste();
    else if (win->GetId() == ID3_VIEW1)
        m_imControl1Ptr->Paste();
    else if (win->GetId() == ID3_VIEW2)
        m_imControl2Ptr->Paste();
}

void DspEnv::OnCopy( wxCommandEvent &event )
{
    wxLogDebug("Copy");
    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
        return;

    if (win->GetId() == ID3_VIEW)
        m_imControlPtr->Copy();
    else if (win->GetId() == ID3_VIEW1)
        m_imControl1Ptr->Copy();
    else if (win->GetId() == ID3_VIEW2)
        m_imControl2Ptr->Copy();
}

void DspEnv::OnCut( wxCommandEvent &event )
{
    wxLogDebug("Cut");
    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
        return;

    if (win->GetId() == ID3_VIEW)
        m_imControlPtr->Cut();
    else if (win->GetId() == ID3_VIEW1)
        m_imControl1Ptr->Cut();
    else if (win->GetId() == ID3_VIEW2)
        m_imControl2Ptr->Cut();
}

void DspEnv::OnOpen( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControlPtr, wxT("Image controller cannot be NULL") );
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if (event.GetId() == ID_OPEN)
    {
        /*if ( m_imControl1Ptr->Ok() )
            m_imControl1Ptr->Close();
        if ( m_imControl2Ptr->Ok() )
            m_imControl2Ptr->Close(); */

        m_imControlPtr->Open();

    }
    else if (event.GetId() == ID3_OPEN1)
        m_imControl1Ptr->Open();
    else if (event.GetId() == ID3_OPEN2)
        m_imControl2Ptr->Open();
}

void DspEnv::OnUpdateUI( wxUpdateUIEvent &event )
{
    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
        return;

    int id = event.GetId();

    if (id == ID_CUT)
    {
        if (win->GetId() == ID3_VIEW)
            event.Enable(m_imControlPtr->CanCut());
        else if (win->GetId() == ID3_VIEW1)
            event.Enable(m_imControl1Ptr->CanCut());
        else if (win->GetId() == ID3_VIEW2)
            event.Enable(m_imControl2Ptr->CanCut());
        else
            event.Enable(false);
    }
    else if (id == ID_COPY)
    {
        if (win->GetId() == ID3_VIEW)
            event.Enable(m_imControlPtr->CanCopy());
        else if (win->GetId() == ID3_VIEW1)
            event.Enable(m_imControl1Ptr->CanCopy());
        else if (win->GetId() == ID3_VIEW2)
            event.Enable(m_imControl2Ptr->CanCopy());
        else
            event.Enable(false);
    }
    else if (id == ID_PASTE)
    {
        if (win->GetId() == ID3_VIEW)
            event.Enable(m_imControlPtr->CanPaste());
        else if (win->GetId() == ID3_VIEW1)
            event.Enable(m_imControl1Ptr->CanPaste());
        else if (win->GetId() == ID3_VIEW2)
            event.Enable(m_imControl2Ptr->CanPaste());
        else
            event.Enable(false);
    }
    else
        event.Enable(true);
}


#endif // AX_DISPLAY

