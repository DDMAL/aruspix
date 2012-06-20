/////////////////////////////////////////////////////////////////////////////
// Name:        mussvgdc.h 
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_SVG_DC_H__
#define __MUS_SVG_DC_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/wfstream.h"
#include "wx/mstream.h"

#include "musdc.h"

//----------------------------------------------------------------------------
// MusBBoxDC
//----------------------------------------------------------------------------

/** 
 * This class implements a drawing context for generating SVG files.
 * The Leipzig font is embedded by incorporating ./data/svg/xxx.xml glyphs within
 * the SVG file.
 */
class MusSvgDC: public MusDC
{
public:

    MusSvgDC ( wxString f, int width, int height );
    virtual ~MusSvgDC();
    
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
    
    virtual void EndGraphic( MusLayoutObject *object, MusRC *rc  );
    
    virtual void StartPage();
    
    virtual void EndPage();
    
    
private:
    //wxFileOutputStream * m_outfile ;
    // changed to a memory stream because we want to prepend the <defs> which will know only when we reach the end of the page
    // some viewer seem to support to have the <defs> at the end, but some do not (pdf2svg, for example)
    // for this reason, the file is written only from the destructor or when Flush() is called
    wxMemoryOutputStream * m_outfile;
    bool m_committed; // did we flushed the file?
    wxString m_filename;
    int m_graphics;
    int m_indents;
    int m_width, m_height;
    int m_originX, m_originY;
    double m_userScaleX, m_userScaleY;
    MusFontInfo m_font;
      
    // holds the list of glyphs from the leipzig font used so far
    // they will be added at the end of the file as <defs>
    wxArrayString m_leipzig_glyphs;
        
    void Commit();    
    
    void WriteLine( wxString );
    
    //
    wxString m_brushColour;
    wxString m_brushStyle;
    wxString m_penColour;
    wxString m_penWidth;
    wxString m_penStyle;
        
    wxString GetColour( int colour );
        
};

#endif // __MUS_SVG_DC_H__
