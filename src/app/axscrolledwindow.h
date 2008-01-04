/////////////////////////////////////////////////////////////////////////////
// Name:        axscrolledwindow.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __axscrolledwindow_H__
#define __axscrolledwindow_H__

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "axscrolledwindow.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "aximcontrol_wdr.h"

class AxImageController;
class AxEnv;

#define ADJUST_NONE 0
#define ADJUST_BOTH 1
#define ADJUST_VERTICAL 2
#define ADJUST_HORIZONTAL 3

#define SHAPE_NONE 0
#define SHAPE_POINT 1
#define SHAPE_LINE 2
#define SHAPE_RECT 3



// WDR: class declarations

//----------------------------------------------------------------------------
// AxScrolledWindow
//----------------------------------------------------------------------------

class AxScrolledWindow: public wxScrolledWindow
{
public:
    // constructors and destructors
    AxScrolledWindow();
    AxScrolledWindow( AxImageController *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxScrolledWindowStyle );
    virtual ~AxScrolledWindow();
    
    // WDR: method declarations for AxScrolledWindow
    void BeginSelection(int draggingShape = SHAPE_RECT);
	void RedrawBuffer( );
    void EndSelection();
    void SetPen(wxPen pen);
    void SetBrush(wxBrush brush);
    void UpdateZoomLimits( );
    void UpdateView( );
	void UpdateViewFast( );
    int GetAdjustMode();
	int GetZoomPercent( ) { return int(m_scale * 100.0);}
	int GetZoomPermil( ) { return int(m_scale * 1000.0);}
    void SetAdjustMode(int mode);
    void ZoomIn( );
    bool CanZoomIn( );
    void ZoomOut( );
    bool CanZoomOut( );

private:
    void RedrawDraggingArea(wxPoint newPosition, bool drawShape);
    void GetSelectionPoints(wxPoint *start, wxPoint *end);

private:
    // WDR: member variable declarations for AxScrolledWindow
    int m_adjustMode;
	int m_orgx, m_orgy;
    double m_zoom;
    double m_zoomMin;
    double m_zoomMax;
    bool m_updating;
    wxPoint m_startPosition,m_endPosition,m_lastPosition;
    wxPen m_pen;
    wxBrush m_brush;
    int m_draggingShape;

protected:
    AxImageController *m_imControlPtr;
    wxBitmap *m_bitmap;
    wxBitmap *m_bufferBitmap;
    double m_scale;
    bool m_shiftDown;
    bool m_selection;
    bool m_isDragging;

public:
    wxMenu m_popupMenu;
    
private:
    // WDR: handler declarations for AxScrolledWindow
    void OnKeyUp( wxKeyEvent &event );
    void OnKeyDown( wxKeyEvent &event );
    void OnMouseEvent( wxMouseEvent &event );
    void OnRightDown( wxMouseEvent &event );
    void OnPaint( wxPaintEvent &event );
    void OnSize( wxSizeEvent &event );

private:
    DECLARE_CLASS(AxScrolledWindow)
    DECLARE_EVENT_TABLE()
};


#endif
