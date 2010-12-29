/////////////////////////////////////////////////////////////////////////////
// Name:        axcairodc.h 
// Author:      Laurent Pugin
// Created:     2010
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __AX_CAIRO_DC_H__
#define __AX_CAIRO_DC_H__

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "axcairodc.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "axdc.h"

#include "cairo-quartz.h"

/*
    This class is a wrapper to Cairo drawing context
    All functionnalities are not implemented
*/
class AxCairoDC: public AxDC
{
public:

    AxCairoDC ( cairo_t *cr );
    virtual ~AxCairoDC();
    
    // Setters
    
    virtual void SetBrush( int colour, int style = wxSOLID );
    
    virtual void SetBackground( int colour, int style = wxSOLID );
    
    virtual void SetBackgroundMode( int mode );
    
    virtual void SetPen( int colour, int width = 1, int style = wxSOLID );
    
    virtual void SetFont( AxFontInfo *font_info );
        
    virtual void SetTextForeground( int colour );
    
    virtual void SetTextBackground( int colour );
    
    virtual void ResetBrush( );
    
    virtual void ResetPen( );
    
    // Getters
    
    virtual void GetTextExtent( wxString& string, int *w, int *h );

    // Drawing methods
    
    virtual void DrawCircle(int x, int y, int radius);
    
    virtual void DrawEllipticArc(int x, int y, int width, int height, double start, double end);
    
    virtual void DrawLine(int x1, int y1, int x2, int y2);
    
    virtual void DrawPolygon(int n, AxPoint points[], int xoffset, int yoffset, int fill_style = wxODDEVEN_RULE);
    
    virtual void DrawRectangle(int x, int y, int width, int height);
    
    virtual void DrawText(const wxString& text, int x, int y);
    
    virtual void DrawSpline(int n, AxPoint points[]);
    
private:
    cairo_t *m_cr;
    
    void SetColour( int colour );
    void GetColour( int colour, double *red, double *green, double *blue );
        
        
};

/** sample code used in OnPaint in MusWindow **/
/*

   
    // Experiment with Cairo on OSX using the native Context
    drawRect.height = -drawRect.height;
    CGContextRef context = (CGContextRef) dc.GetGraphicsContext()->GetNativeContext();
    if(context == 0)
        return;
    cairo_surface_t* cairo_surface = cairo_quartz_surface_create_for_cg_context(context, drawRect.width, drawRect.height);
    cairo_t *cr = cairo_create(cairo_surface);
    AxCairoDC ax_dc( cr );
    ax_dc.SetBrush( AxWHITE, wxSOLID );
    m_page->DrawPage( &ax_dc );
    cairo_surface_flush(cairo_surface);
    CGContextFlush( context );
    cairo_surface_destroy(cairo_surface);
    cairo_destroy(cr);
    
*/

#endif // __AX_CAIRO_DC_H__
