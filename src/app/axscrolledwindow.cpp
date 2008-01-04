/////////////////////////////////////////////////////////////////////////////
// Name:        axscrolledwindow.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "axscrolledwindow.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "axapp.h"
#include "axscrolledwindow.h"
#include "aximagecontroller.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// AxScrolledWindow
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(AxScrolledWindow,wxScrolledWindow)

// WDR: event table for AxScrolledWindow

BEGIN_EVENT_TABLE(AxScrolledWindow,wxScrolledWindow)
    EVT_PAINT( AxScrolledWindow::OnPaint )
    EVT_SIZE( AxScrolledWindow::OnSize )
    EVT_RIGHT_DOWN( AxScrolledWindow::OnRightDown )
    EVT_MOUSE_EVENTS( AxScrolledWindow::OnMouseEvent )
    EVT_KEY_DOWN( AxScrolledWindow::OnKeyDown )
    EVT_KEY_UP( AxScrolledWindow::OnKeyUp )
END_EVENT_TABLE()

AxScrolledWindow::AxScrolledWindow( AxImageController *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxScrolledWindow( parent, id, position, size, style )
{
    //wxASSERT_MSG(parent->IsKindOf(CLASSINFO(AxImageController)),"Parent must be an image controller");

    m_bitmap = NULL;
    m_bufferBitmap = NULL;
    m_imControlPtr = parent;
    // dragging
    m_draggingShape = SHAPE_NONE;
    m_isDragging = false;
    m_shiftDown = false;
    m_selection = false;
    m_pen = *wxBLACK_PEN;
    m_brush = *wxTRANSPARENT_BRUSH;
    // zoom
    m_scale = 1.0; // actual scale factor (adjusted or zoomed)
    m_zoom = 1.0;
    m_zoomMax = 1.0;
    m_zoomMin = 1.0;
    m_adjustMode = ADJUST_NONE;
    m_updating = false;
	m_orgx = 0;
	m_orgy = 0;

    if ( wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE) == *wxWHITE )
        this->SetBackgroundColour( *wxLIGHT_GREY );
    else
        this->SetBackgroundColour( wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE) );

	m_popupMenu.Append( ID_CUT,_("Cut") );
    m_popupMenu.Append( ID_COPY,_("Copy") );
    m_popupMenu.Append( ID_PASTE,_("Paste") );
	m_popupMenu.Append( ID_POPUP_MENU_GOTO,_("Go to page ...") );

	if ( parent->HasNavigation( ) )
	{
		m_popupMenu.AppendSeparator( );
		wxMenu* navigation = new wxMenu();
		navigation->AppendCheckItem( ID_POPUP_MENU_BY_PAGES, _("by pages in file") );
		navigation->AppendCheckItem( ID_POPUP_MENU_BY_FILES, _("by files in directory") );
		m_popupMenu.Append( ID_POPUP_MENU_NAVIGATION, _("Navigation"), navigation );
	}
}

AxScrolledWindow::AxScrolledWindow()
{
}

AxScrolledWindow::~AxScrolledWindow()
{
    if (m_bitmap) delete m_bitmap;
    if (m_bufferBitmap) delete m_bufferBitmap;
}


// do not check scroll bars and so one assuming that size is the same
void AxScrolledWindow::UpdateViewFast( )
{
    if ( m_bitmap )
    {
        delete m_bitmap;
        m_bitmap = NULL;
    }
    if ( m_bufferBitmap )
    {
        delete m_bufferBitmap;
        m_bufferBitmap = NULL;
    }
	
    wxSize imsize;
    imsize.SetHeight(m_imControlPtr->GetHeight());
    imsize.SetWidth(m_imControlPtr->GetWidth());

    m_bitmap = new wxBitmap(
        m_imControlPtr->GetAxBitmap((int)(imsize.GetWidth() * m_scale),
            (int)(imsize.GetHeight() * m_scale)));
    wxMemoryDC memDC;
    m_bufferBitmap = new wxBitmap(m_bitmap->GetWidth(), m_bitmap->GetHeight()); // def depth
    memDC.SelectObject(*m_bufferBitmap);
    memDC.DrawBitmap(*m_bitmap, 0, 0);
    //memDC.SelectObject(NULL);

    this->Refresh();
}

void AxScrolledWindow::UpdateView( )
{
    wxASSERT_MSG(m_imControlPtr,"Image cannot be NULL");
    
    if ( m_bitmap )
    {
        delete m_bitmap;
        m_bitmap = NULL;
    }
    if ( m_bufferBitmap )
    {
        delete m_bufferBitmap;
        m_bufferBitmap = NULL;
    }

    if ( !m_imControlPtr->Ok() )
    {
        this->SetScrollbars( 0,0,0,0 );
        this->Refresh();
        return;
    }

    m_updating = true; // avoid onsize to recall this method

	m_orgx = 0;
	m_orgy = 0;

    wxScrollBar bar(this,-1);
    wxSize barSize = bar.GetSize();

    this->SetScrollbars(0, 0, 0, 0); // Sent Size event -> client size recalculated
    wxSize client = this->GetClientSize();
    wxSize clientSm;
    clientSm.x = client.x - barSize.y - 3;
    clientSm.y = client.y - barSize.y - 3;

    wxSize imsize;
    imsize.SetHeight(m_imControlPtr->GetHeight());
    imsize.SetWidth(m_imControlPtr->GetWidth());

    //wxLogDebug("UpdateView client size %d %d",client.x,client.y);
    //wxLogDebug("UpdateView clientSm size %d %d",clientSm.x,clientSm.y);

    if (m_adjustMode != ADJUST_NONE)
    {
        double scaleH = min(1.0,((double)client.x/(double)imsize.GetWidth()));
        double scaleV = min(1.0,((double)client.y/(double)imsize.GetHeight()));

        if (m_adjustMode == ADJUST_HORIZONTAL)
        {
            if (scaleH > scaleV)
                m_scale = min(1.0,((double)clientSm.x/(double)imsize.GetWidth()));
            else
                m_scale = scaleH;     
        
        }
        else if (m_adjustMode == ADJUST_VERTICAL)
        {
            if (scaleV > scaleH)
                m_scale = min(1.0,((double)clientSm.y/(double)imsize.GetHeight()));
            else
                m_scale = scaleV;     
        }
        else if (m_adjustMode == ADJUST_BOTH)
            m_scale = min(scaleH,scaleV);
    }
    else
        m_scale = m_zoom;

    //wxImage image = m_image->Scale((int)(imsize.GetWidth() * m_scale),(int)(imsize.GetHeight() * m_scale));
    //m_bitmap = new wxBitmap(image);
    //m_bufferBitmap = new wxBitmap(image);
    m_bitmap = new wxBitmap(
        m_imControlPtr->GetAxBitmap((int)(imsize.GetWidth() * m_scale),
            (int)(imsize.GetHeight() * m_scale)));
    wxMemoryDC memDC;
    m_bufferBitmap = new wxBitmap(m_bitmap->GetWidth(), m_bitmap->GetHeight()); // def depth
    memDC.SelectObject(*m_bufferBitmap);
    memDC.DrawBitmap(*m_bitmap, 0, 0);
    //memDC.SelectObject(NULL);

    if ( m_bitmap )
    {
        //wxLogDebug("bitmap size %d %d", m_bitmap->GetWidth() / 20, m_bitmap->GetHeight() / 20);
        if (m_adjustMode == ADJUST_HORIZONTAL)
            this->SetScrollbars( 0, 20, 0, m_bitmap->GetHeight() / 20);
        else if (m_adjustMode == ADJUST_VERTICAL)
            this->SetScrollbars( 20, 0, m_bitmap->GetWidth() / 20, 0);
        else
            this->SetScrollbars( 20, 20, m_bitmap->GetWidth() / 20, m_bitmap->GetHeight() / 20);

		if ( client.x > m_bitmap->GetWidth() )
			m_orgx = (client.x - m_bitmap->GetWidth()) / 2;
		if ( client.y > m_bitmap->GetHeight() )
			m_orgy = (client.y - m_bitmap->GetHeight()) / 2;
            
    }
    this->Refresh();
	
	m_imControlPtr->UpdateTitleMsg();
	
    m_updating = false;
}

void AxScrolledWindow::UpdateZoomLimits( )
{
    m_zoomMax = 5.0;
    m_zoomMin = 0.2;

    if (!m_imControlPtr || !m_imControlPtr->Ok())
        return;

    int h,w;
    h = m_imControlPtr->GetHeight();
    w = m_imControlPtr->GetWidth();
    double wMax = 2500.0 / (double)w;
    double hMax = 2500.0 / (double)h;

    m_zoomMax = min(m_zoomMax,min(wMax,hMax));
    m_zoom = min(m_zoom,m_zoomMax);
}

void AxScrolledWindow::ZoomIn()
{
    if (!CanZoomIn())
        return;

    wxGetApp().AxBeginBusyCursor();
    m_adjustMode = ADJUST_NONE;
    m_zoom *= 1.5;

    //wxLogDebug("Zoom %f; Max %f",m_zoom, m_zoomMax);
    this->UpdateView();
    wxGetApp().AxEndBusyCursor();
}

bool AxScrolledWindow::CanZoomIn()
{
    if (!m_imControlPtr || !m_imControlPtr->Ok())
        return false;
    if (m_zoom >= m_zoomMax)
        return false;

    return true;
}

void AxScrolledWindow::ZoomOut()
{
    if (!CanZoomOut())
        return;

    wxGetApp().AxBeginBusyCursor();
    m_adjustMode = ADJUST_NONE;
    m_zoom /= 1.5;

    //wxLogDebug("Zoom %f; Min %f",m_zoom, m_zoomMin);
    this->UpdateView();
    wxGetApp().AxEndBusyCursor();
}

bool AxScrolledWindow::CanZoomOut()
{
    if (!m_imControlPtr || !m_imControlPtr->Ok())
        return false;
    if (m_zoom <= m_zoomMin)
        return false;

    return true;
}

void AxScrolledWindow::SetAdjustMode(int mode)
{
    if (m_adjustMode == mode)
        m_adjustMode = ADJUST_NONE;
    else
        m_adjustMode = mode;
    this->UpdateView();
}

int AxScrolledWindow::GetAdjustMode()
{
    return m_adjustMode;    
}

void AxScrolledWindow::BeginSelection(int draggingShape)
{
    m_draggingShape = draggingShape;
    m_selection = true;
}

void AxScrolledWindow::EndSelection()
{
    m_draggingShape = SHAPE_NONE;
    m_pen = *wxBLACK_PEN;
    m_brush = *wxTRANSPARENT_BRUSH;
    m_selection = false;
    RedrawDraggingArea(m_lastPosition,false);
}

void AxScrolledWindow::SetPen(wxPen pen)
{
    m_pen = pen;
}

void AxScrolledWindow::SetBrush(wxBrush brush)
{
    m_brush = brush;
}

void AxScrolledWindow::RedrawDraggingArea(wxPoint newPosition, bool drawShape)
{
    wxASSERT_MSG(m_bitmap,"Bitmap cannot be NULL");
    wxASSERT_MSG(m_bufferBitmap,"Buffer bitmap cannot be NULL");

    wxPoint redrawPosition;
    redrawPosition.x = min(m_startPosition.x, min(m_lastPosition.x , newPosition.x));
    int x = max(m_startPosition.x, max(m_lastPosition.x , newPosition.x));
    redrawPosition.y = min(m_startPosition.y, min(m_lastPosition.y , newPosition.y));
    int y = max(m_startPosition.y, max(m_lastPosition.y , newPosition.y));
    int width = x -  redrawPosition.x + 1;
    int height = y -  redrawPosition.y + 1;

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
    bufDC.SetBrush(m_brush);
    bufDC.SetPen(m_pen);

    bufDC.Blit(redrawPosition.x  + scrollX, redrawPosition.y  + scrollY, width, height,
        &memDC, redrawPosition.x + scrollX, redrawPosition.y + scrollY);

    memDC.SelectObject( wxNullBitmap );

    m_lastPosition = newPosition;
    
    if (drawShape && (m_draggingShape == SHAPE_RECT))
    {
        int rheight = m_lastPosition.y - m_startPosition.y; 
        int rwidth = m_lastPosition.x - m_startPosition.x; 
        bufDC.DrawRectangle(m_startPosition.x + scrollX, m_startPosition.y +  scrollY,rwidth,rheight);
    }
    else if (drawShape && (m_draggingShape == SHAPE_LINE))
    {
        bufDC.DrawLine(m_startPosition.x  +  scrollX ,m_startPosition.y  +  scrollY,
            m_lastPosition.x  +  scrollX ,m_lastPosition.y +  scrollY);
    }

    wxClientDC dc(this);
    dc.Blit(redrawPosition.x,redrawPosition.y, width, height,
        &bufDC,redrawPosition.x + scrollX, redrawPosition.y + scrollY);

    bufDC.SelectObject( wxNullBitmap );
}


void AxScrolledWindow::RedrawBuffer( )
{
    wxASSERT_MSG(m_bitmap,"Bitmap cannot be NULL");
    wxASSERT_MSG(m_bufferBitmap,"Buffer bitmap cannot be NULL");

	int scrollX, scrollY;
	this->GetViewStart(&scrollX,&scrollY);
	int unitX, unitY;
	this->GetScrollPixelsPerUnit(&unitX,&unitY);
	scrollX *= unitX;
	scrollY *= unitY;

	int clientX, clientY;
	this->GetClientSize( &clientX, &clientY );

	wxMemoryDC memDC;
	memDC.SelectObject(*m_bufferBitmap);
    
	wxMemoryDC bufDC;
	bufDC.SelectObject(*m_bitmap);

    bufDC.Blit(scrollX, scrollY, clientX, clientY,
        &memDC, scrollX, scrollY );
	memDC.SelectObject( wxNullBitmap );

	wxClientDC dc(this);
    dc.Blit(0, 0, clientX, clientY,
        &bufDC, scrollX, scrollY);

	bufDC.SelectObject( wxNullBitmap );
}

void AxScrolledWindow::GetSelectionPoints(wxPoint *start, wxPoint *end)
{
    wxASSERT_MSG(start,"Start point cannot be NULL");
    wxASSERT_MSG(end,"End point bitmap cannot be NULL");

    int scrollX, scrollY;
    this->GetViewStart(&scrollX,&scrollY);
    int unitX, unitY;
    this->GetScrollPixelsPerUnit(&unitX,&unitY);
    scrollX *= unitX;
    scrollY *= unitY;

    start->x = (int)((double)(m_startPosition.x  +  scrollX) * (1.0 / m_scale));
	start->x = max( 0, start->x );
	start->x = min( start->x, m_imControlPtr->GetWidth() );
    start->y = (int)((double)(m_startPosition.y  +  scrollY) * (1.0 / m_scale));
	start->y = max( 0, start->y );
	start->y = min( start->y, m_imControlPtr->GetHeight() );
    end->x = (int)((double)(m_lastPosition.x  +  scrollX) * (1.0 / m_scale));
	end->x = max( 0, end->x );
	end->x = min( end->x, m_imControlPtr->GetWidth() );
    end->y = (int)((double)(m_lastPosition.y  +  scrollY) * (1.0 / m_scale));
	end->y = max( 0, end->y );
	end->y = min( end->y, m_imControlPtr->GetHeight() );
}


// WDR: handler implementations for AxScrolledWindow

void AxScrolledWindow::OnKeyUp( wxKeyEvent &event )
{
    if (event.GetKeyCode() == WXK_SHIFT) m_shiftDown = FALSE;    
}

void AxScrolledWindow::OnKeyDown( wxKeyEvent &event )
{
    if (event.GetKeyCode() == WXK_SHIFT) m_shiftDown = TRUE;
}

void AxScrolledWindow::OnMouseEvent( wxMouseEvent &event )
{
    if (event.Dragging())
    {
        if (!m_bitmap)
            return;
        if (event.m_x > m_bitmap->GetWidth())
            return;
        if (event.m_y > m_bitmap->GetHeight())
            return;
        if (m_selection)
        {
            wxPoint newPosition = event.GetPosition();
            RedrawDraggingArea(newPosition,true);
        }
    }
    if (event.GetEventType() == wxEVT_MOUSEWHEEL)
    {
        if (event.LeftIsDown()) 
            return;

        int x, y;
        this->GetViewStart( &x, &y );
        int rot = 0;
        if ( event.m_wheelDelta != 0)
            rot = event.m_wheelRotation / event.m_wheelDelta * 6;
        //if (!m_shiftDown)
		if (!event.ShiftDown())
            this->Scroll( x, max(0, y - rot) );
        else
            this->Scroll( max( 0, x - rot ), y );
    }
    if (event.GetEventType() == wxEVT_LEAVE_WINDOW)
    {
        if (!m_bitmap)
            return;
			
		wxASSERT_MSG(m_imControlPtr,"Image controller cannot be NULL");
        wxPoint start, end;
        GetSelectionPoints(&start, &end);
		//m_imControlPtr->CloseDraggingSelection(start, end); // WARNING : commented to have superimposition ractivated using points added by hand. 
																// Not checked for pre-processing selection in recognition
        //m_isDragging = false;
        //this->SetCursor(*wxSTANDARD_CURSOR);
        m_endPosition = event.GetPosition();
    }
    if (event.GetEventType() == wxEVT_ENTER_WINDOW)
    {
        if (!m_bitmap)
            return;
        if (m_isDragging && event.m_leftDown)
            this->SetCursor(*wxCROSS_CURSOR);
		else // button has been released outside the window
		{
			this->SetCursor(*wxSTANDARD_CURSOR);
			m_isDragging = false;
		}
    }
    if (event.GetEventType() == wxEVT_LEFT_DOWN)
    {
        if (!m_bitmap)
            return;
        if (event.m_x > m_bitmap->GetWidth())
            return;
        if (event.m_y > m_bitmap->GetHeight())
            return;
        if (m_selection)
        {
            m_isDragging = true;
            this->SetCursor(*wxCROSS_CURSOR);
            RedrawDraggingArea(m_lastPosition,false);
            m_startPosition = event.GetPosition();
            m_lastPosition = m_startPosition;
        }
    }
    if (event.GetEventType() == wxEVT_LEFT_UP)
    {

        if (!m_bitmap)
            return;

        if (m_selection)
        {
            wxASSERT_MSG(m_imControlPtr,"Image controller cannot be NULL");
            wxPoint start, end;
            GetSelectionPoints(&start, &end);
            m_imControlPtr->CloseDraggingSelection(start, end);
            m_isDragging = false;
            this->SetCursor(*wxSTANDARD_CURSOR);
            m_endPosition = event.GetPosition();
            RedrawDraggingArea(m_lastPosition,true);
        }
    }
	if (event.GetEventType() == wxEVT_LEFT_DCLICK )
	{
        if (!m_bitmap)
            return;
		if (event.m_x > m_bitmap->GetWidth())
            return;
        if (event.m_y > m_bitmap->GetHeight())
            return;

		wxASSERT_MSG(m_imControlPtr,"Image controller cannot be NULL");
		m_lastPosition = event.GetPosition();
        wxPoint start, end;
        GetSelectionPoints(&start, &end);
		m_imControlPtr->LeftDClick( end );	
	}
}  

void AxScrolledWindow::OnRightDown( wxMouseEvent &event )
{
    this->SetFocus();
	// TODO, not up to date
    //PopupMenu(&m_popupMenu,event.GetPosition());    
}

void AxScrolledWindow::OnPaint(wxPaintEvent &event)
{
    wxPaintDC dc( this );
    PrepareDC( dc );

    if (!m_bitmap) 
        return;
    //dc.DrawBitmap(*m_bitmap, m_orgx, m_orgy); 
	dc.DrawBitmap(*m_bitmap, 0, 0);
}

void AxScrolledWindow::OnSize(wxSizeEvent &event)
{
    //wxLogDebug("Size event");
    if (!m_updating && ((m_orgx != 0) || (m_orgy != 0) ))
	//if (!m_updating && (m_adjustMode != ADJUST_NONE))
        this->UpdateView( );
    event.Skip();
}


