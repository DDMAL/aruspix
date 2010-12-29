/////////////////////////////////////////////////////////////////////////////
// Name:        axcairodc.cpp
// Author:      Laurent Pugin
// Created:     2010
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "axcairodc.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "axcairodc.h"

#include <algorithm>
using std::min;
using std::max;


//----------------------------------------------------------------------------
// AxCairoDC
//----------------------------------------------------------------------------

AxCairoDC::AxCairoDC ( cairo_t *cr ):
    AxDC()
{
    m_cr = cr;
}


AxCairoDC::~AxCairoDC ( )
{
}
        
        
void AxCairoDC::SetBrush( int colour, int style )
{
    SetColour( colour );
    // TODO : handle style  
}
        
void AxCairoDC::SetBackground( int colour, int style )
{
    SetColour( colour ); 
}
        
void AxCairoDC::SetBackgroundMode( int mode )
{
    // TODO
}
        
void AxCairoDC::SetPen( int colour, int width, int style )
{
    cairo_set_line_width (m_cr, width);
    SetColour( colour ); 
}
        
void AxCairoDC::SetFont( AxFontInfo *font_info )
{
    cairo_select_font_face (m_cr, font_info->faceName.c_str(),
        CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD ); 
    cairo_set_font_size(m_cr, font_info->pointSize);

}
            

void AxCairoDC::SetTextForeground( int colour )
{
}
        
void AxCairoDC::SetTextBackground( int colour )
{
}
       
void AxCairoDC::ResetBrush( )
{
}
        
void AxCairoDC::ResetPen( )
{
}        

void AxCairoDC::GetTextExtent( wxString& string, int *w, int *h )
{

} 


// Drawing mething
        
void AxCairoDC::DrawCircle(int x, int y, int radius)
{

}
        
void AxCairoDC::DrawEllipticArc(int x, int y, int width, int height, double start, double end)
{
    cairo_new_path ( m_cr );
    cairo_save( m_cr );
    cairo_translate(m_cr, x + width / 2, y + height / 2);  // make (ex, ey) == (0, 0)
    cairo_scale( m_cr, width, height); 
    cairo_arc( m_cr, 0.0, 0.0, 0.5, start * (M_PI / 180.), end * (M_PI / 180.) );
    cairo_restore( m_cr );  // back to opaque black
    cairo_stroke( m_cr);
}
        
void AxCairoDC::DrawLine(int x1, int y1, int x2, int y2)
{
    cairo_new_path ( m_cr );
    cairo_move_to (m_cr, (double)x1 - 0.5, (double)y1 - 0.5);
    cairo_line_to (m_cr, (double)x2 - 0.5, (double)y2 - 0.5);
    cairo_stroke (m_cr);
}
        
void AxCairoDC::DrawPolygon(int n, AxPoint points[], int xoffset, int yoffset, int fill_style)
{

}
        
void AxCairoDC::DrawRectangle(int x, int y, int width, int height)
{
    cairo_new_path ( m_cr );
    cairo_rectangle (m_cr, x, y, width, height);
    cairo_fill (m_cr);
}

// cairo string is in utf-8 but we get ascii 8bits string
// so they need to be converted
// This function just convert ONE character

char *ascii_to_utf8(unsigned char c)
{
    char *out;
	
	if(c < 128)
	{
		out = (char *)calloc(2, sizeof(char));
		out[0] = c;
		out[1] = '\0';
	}
	else
	{
		out = (char *)calloc(3, sizeof(char));
		out[0] = (c >> 6) | 0xC0;
		out[1] = (c & 0x3F) | 0x80;
		out[2] = '\0';
	}
	
	return (char*)out;
}
        
void AxCairoDC::DrawText(const wxString& text, int x, int y)
{
    
    cairo_move_to ( m_cr, x, y );
    
    unsigned char buf[1024];
    memcpy( buf, text.c_str(), min( (int)text.Length() + 1, 1024 ) );
    buf[1023] = 0;
    char *utf8 = ascii_to_utf8( buf[0] ); // Testing: just the first character!
    cairo_show_text ( m_cr, utf8  ); // Works, but I am giving up because of the encoding problems
                                     // We get the wrong ones in Leipzig...
    free( utf8 );

    
}

void AxCairoDC::DrawSpline(int n, AxPoint points[])
{

}


void AxCairoDC::SetColour( int colour )
{
    double r, g, b;
    GetColour( colour, &r, &g, &b );
    cairo_set_source_rgb(m_cr, r, g, b);
}


void AxCairoDC::GetColour( int colour, double *red, double *green, double *blue )
{ 
    int b =  (colour & 255);
    int g = (colour >> 8) & 255;
    int r = (colour >> 16) & 255;
    
    (*red) = (double)r / 255.0;
    (*green) = (double)g / 255.0;
    (*blue) = (double)b / 255.0;
}


