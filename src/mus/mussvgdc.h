/////////////////////////////////////////////////////////////////////////////
// Name:        mussvgdc.h 
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_SVG_DC_H__
#define __MUS_SVG_DC_H__

#include "musdc.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

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

    MusSvgDC ( std::string f, int width, int height );
    virtual ~MusSvgDC();
    
    // Setters
    
    virtual void SetBrush( int colour, int style = AxSOLID );
    
    virtual void SetBackground( int colour, int style = AxSOLID );
    
    virtual void SetBackgroundImage( void *image, double opacity = 1.0 );
    
    virtual void SetBackgroundMode( int mode );
    
    virtual void SetPen( int colour, int width = 1, int style = AxSOLID );
    
    virtual void SetFont( MusFontInfo *font_info );
        
    virtual void SetTextForeground( int colour );
    
    virtual void SetTextBackground( int colour );
    
    virtual void ResetBrush( );
    
    virtual void ResetPen( );
    
    virtual void SetLogicalOrigin( int x, int y );
    
    virtual void SetUserScale( double xScale, double yScale );
    
    // Getters
    
    virtual void GetTextExtent( const std::string& string, int *w, int *h );
    
    virtual MusPoint GetLogicalOrigin( );

    // Drawing methods
    
    virtual void DrawComplexBezierPath(int x, int y, int bezier1_coord[6], int bezier2_coord[6]);
    
    virtual void DrawCircle(int x, int y, int radius);
    
    virtual void DrawEllipse(int x, int y, int width, int height);
    
    virtual void DrawEllipticArc(int x, int y, int width, int height, double start, double end);
    
    virtual void DrawLine(int x1, int y1, int x2, int y2);
    
    virtual void DrawPolygon(int n, MusPoint points[], int xoffset, int yoffset, int fill_style = AxODDEVEN_RULE);
    
    virtual void DrawRectangle(int x, int y, int width, int height);
    
    virtual void DrawRotatedText(const std::string& text, int x, int y, double angle);
    
    virtual void DrawRoundedRectangle(int x, int y, int width, int height, double radius);
    
    virtual void DrawText(const std::string& text, int x, int y);
    
    virtual void DrawMusicText(const std::string& text, int x, int y);
    
    virtual void DrawSpline(int n, MusPoint points[]);
    
    virtual void DrawBackgroundImage( int x = 0, int y = 0 );
    
    // 
    virtual void StartGraphic( MusDocObject *object, std::string gClass, std::string gId );
    
    virtual void EndGraphic( MusDocObject *object, MusRC *rc  );
    
    virtual void StartPage();
    
    virtual void EndPage();
    
    // RZ FIXME testing
    std::stringstream m_outdata;
    void Commit(); //was private
    
private:
    
    bool copy_wxTransferFileToStream(const std::string& filename, std::ostream& dest);
    
    // we use a std::stringstream because we want to prepend the <defs> which will know only when we reach the end of the page
    // some viewer seem to support to have the <defs> at the end, but some do not (pdf2svg, for example)
    // for this reason, the file is finally written only from the destructor or when Flush() is called
    std::stringstream m_outfile;
    
    bool m_committed; // did we flushed the file?
    std::string m_filename;
    int m_graphics;
    int m_indents;
    int m_width, m_height;
    int m_originX, m_originY;
    double m_userScaleX, m_userScaleY;
    MusFontInfo m_font;
      
    // holds the list of glyphs from the leipzig font used so far
    // they will be added at the end of the file as <defs>
    std::vector<std::string> m_leipzig_glyphs;
    
    void WriteLine( std::string );
    
    //
    std::string m_brushColour;
    std::string m_brushStyle;
    std::string m_penColour;
    std::string m_penWidth;
    std::string m_penStyle;
        
    std::string GetColour( int colour );
        
};

#endif // __MUS_SVG_DC_H__
