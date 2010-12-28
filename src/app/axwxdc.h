/////////////////////////////////////////////////////////////////////////////
// Name:        axwxdc.h 
// Author:      Laurent Pugin
// Created:     2010
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __AX_WXDC_H__
#define __AX_WXDC_H__

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "axwxdc.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


#include "axdc.h"

/*
    This class is a wrapper to wxDCs
    All functionnalities are not implemented
*/
class AxWxDC: public AxDC
{
public:

    AxWxDC ( wxDC *dc );
    virtual ~AxWxDC();
    
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
    wxDC *m_dc;
    wxBrush m_brush;
    wxPen m_pen;
    
    wxColour GetColour( int colour );
        
        
};

#endif // __AX_WXDC_H__
