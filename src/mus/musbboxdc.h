/////////////////////////////////////////////////////////////////////////////
// Name:        musbboxdc.h 
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_BBOX_DC_H__
#define __MUS_BBOX_DC_H__


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "musdc.h"
#include "musobject.h"

//----------------------------------------------------------------------------
// MusBBoxDC
//----------------------------------------------------------------------------

/** 
 * This class calculates the bouding box of the object that are drawn.
 * It can be used when doing the layout of the object in order to manage object spacing.
 * The drawing primitives do not draw anything but update the bounding box values of the 
 * layout objects currently drawn. The layout objects store their own bounding box and a
 * bounding box of their content. The own bouding box is updated only for the object being
 * drawn (the top one on the stack). The content bounding box is updated for all objects
 * the stack
 */
class MusBBoxDC: public MusDC
{
public:

    MusBBoxDC ( wxString f, int width, int height );
    virtual ~MusBBoxDC();
    
    // Setters
    
    virtual void SetBrush( int colour, int style = wxSOLID );
    
    virtual void SetBackground( int colour, int style = wxSOLID );
    
    virtual void SetBackgroundMode( int mode );
    
    virtual void SetPen( int colour, int width = 1, int style = wxSOLID );
    
    virtual void SetFont( MusFontInfo *font_info );
        
    virtual void SetTextForeground( int colour );
    
    virtual void SetTextBackground( int colour );
    
    virtual void ResetBrush( );
    
    virtual void ResetPen( );
    
    virtual void SetLogicalOrigin( int x, int y );
    
    virtual void SetUserScale( double xScale, double yScale );
    
    // Getters
    
    virtual void GetTextExtent( wxString& string, int *w, int *h );
    
    virtual MusPoint GetLogicalOrigin( );

    // Drawing methods
    
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
    
    // 
    virtual void StartGraphic( MusLayoutObject *object, wxString gClass, wxString gId );
    
    virtual void EndGraphic();
    
    virtual void StartPage();
    
    virtual void EndPage();
    
    
private:
    int m_width, m_height;
    int m_originX, m_originY;
    double m_userScaleX, m_userScaleY;
    
    /**
     * The array containing the object for which the bounding box needs to be updated
     */ 
    ArrayOfMusLayoutObjects m_objects;
   
    //
    int m_penWidth;
        
};

#endif // __MUS_BBOX_DC_H__
