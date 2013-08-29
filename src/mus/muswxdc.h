/////////////////////////////////////////////////////////////////////////////
// Name:        muswxdc.h 
// Author:      Laurent Pugin
// Created:     2010
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_WX_DC_H__
#define __MUS_WX_DC_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "musdef.h"
#include "musdc.h"
#include "musobject.h"

// ---------------------------------------------------------------------------
// MusDC
// ---------------------------------------------------------------------------


/**
 * This class is a wrapper to wxDCs.
 * All functionnalities are not implemented yet.
*/
class MusWxDC: public MusDC
{
public:

    MusWxDC ( wxDC *dc );
    virtual ~MusWxDC();
    
    // Setters
    
    virtual void SetBrush( int colour, int style = wxSOLID );
    
    virtual void SetBackground( int colour, int style = wxSOLID );
    
    virtual void SetBackgroundMode( int mode );
    
    virtual void SetBackgroundImage( void *image, double opacity = 1.0 );
    
    virtual void SetPen( int colour, int width = 1, int style = wxSOLID );
    
    virtual void SetFont( MusFontInfo *font_info );
        
    virtual void SetTextForeground( int colour );
    
    virtual void SetTextBackground( int colour );
    
    virtual void ResetBrush( );
    
    virtual void ResetPen( );
    
    virtual void SetLogicalOrigin( int x, int y );
    
    // Getters
    
    virtual void GetTextExtent( wxString& string, int *w, int *h );
    
    virtual MusPoint GetLogicalOrigin( );

    // Drawing methods

    virtual void DrawComplexBezierPath(int x, int y, int bezier1_coord[6], int bezier2_coord[6]);
    
    virtual void DrawCircle(int x, int y, int radius);
    
    virtual void DrawEllipse(int x, int y, int width, int height);
    
    virtual void DrawEllipticArc(int x, int y, int width, int height, double start, double end);
    
    virtual void DrawLine(int x1, int y1, int x2, int y2);
    
    virtual void DrawPolygon(int n, MusPoint points[], int xoffset, int yoffset, int fill_style = wxODDEVEN_RULE);
    
    virtual void DrawRectangle(int x, int y, int width, int height);
    
    virtual void DrawRotatedText(const wxString& text, int x, int y, double angle);
    
    virtual void DrawRoundedRectangle(int x, int y, int width, int height, double radius);
    
    virtual void DrawText(const wxString& text, int x, int y);
    
    virtual void DrawMusicText(const wxString& text, int x, int y);
    
    virtual void DrawSpline(int n, MusPoint points[]);
    
    virtual void DrawBackgroundImage( int x = 0, int y = 0 );
   
    // Method for starting and ending a graphic - for example for grouping shapes in <g></g> in SVG
    
    virtual void StartGraphic( MusDocObject *object, wxString gClass, wxString gId ) {};
    
    virtual void EndGraphic( MusDocObject *object, MusRC *rc  );
    
    virtual void StartPage( ) { m_dc->EndPage(); };
    
    virtual void EndPage( ) { m_dc->StartPage(); };
    
private:
    wxDC *m_dc;
    wxBrush m_brush;
    wxPen m_pen;
    wxImage *m_backgroundImage;
    
    wxColour GetColour( int colour );
};

#endif // __AX_WXDC_H__
