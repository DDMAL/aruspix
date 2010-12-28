/////////////////////////////////////////////////////////////////////////////
// Name:        axwxdc.cpp
// Author:      Laurent Pugin
// Created:     2010
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "axwxdc.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "axwxdc.h"

//----------------------------------------------------------------------------
// AxWxDc
//----------------------------------------------------------------------------

AxWxDC::AxWxDC ( wxDC *dc ):
    AxDC()
{
    m_dc = dc;
}


AxWxDC::~AxWxDC ( )
{
}
        
        
void AxWxDC::SetBrush( int colour, int style )
{
    m_brush.SetColour( GetColour( colour ) );
    m_brush.SetStyle( style );
    m_dc->SetBrush( m_brush );
}
        
void AxWxDC::SetBackground( int colour, int style )
{
    m_dc->SetBackground( wxBrush( GetColour( colour ), style ) );
}
        
void AxWxDC::SetBackgroundMode( int mode )
{
    m_dc->SetBackgroundMode( mode );
}
        
void AxWxDC::SetPen( int colour, int width, int style )
{
    m_pen.SetColour( GetColour( colour ) );
    m_pen.SetWidth( width );
    m_pen.SetStyle( style );
    m_dc->SetPen( m_pen );
}
        
void AxWxDC::SetFont( AxFontInfo *font_info )
{
    wxFont font( font_info->pointSize, (wxFontFamily)font_info->family, font_info->style,
        (wxFontWeight)font_info->weight, font_info->underlined, font_info->faceName,
        (wxFontEncoding)font_info->encoding );
    m_dc->SetFont( font );
}
            

void AxWxDC::SetTextForeground( int colour )
{
    m_dc->SetTextForeground(  GetColour( colour ) );
}
        
void AxWxDC::SetTextBackground( int colour )
{
    m_dc->SetTextBackground(  GetColour( colour ) );
}
       
void AxWxDC::ResetBrush( )
{
    m_dc->SetBrush( wxNullBrush );
}
        
void AxWxDC::ResetPen( )
{
    m_dc->SetPen( wxNullPen );
}        

void AxWxDC::GetTextExtent( wxString& string, int *w, int *h )
{
    m_dc->GetTextExtent( string, w, h );
} 


// Drawing mething
        
void AxWxDC::DrawCircle(int x, int y, int radius)
{
    m_dc->DrawCircle( x, y, radius );
}
        
void AxWxDC::DrawEllipticArc(int x, int y, int width, int height, double start, double end)
{
    m_dc->DrawEllipticArc( x, y, width, height, start, end );
}
        
void AxWxDC::DrawLine(int x1, int y1, int x2, int y2)
{
    m_dc->DrawLine( x1, y1, x2, y2 );
}
        
void AxWxDC::DrawPolygon(int n, AxPoint points[], int xoffset, int yoffset, int fill_style)
{
    m_dc->DrawPolygon( n, (wxPoint*)points, xoffset, yoffset, fill_style );
}
        
void AxWxDC::DrawRectangle(int x, int y, int width, int height)
{
    m_dc->DrawRectangle( x, y, width, height );
}
        
void AxWxDC::DrawText(const wxString& text, int x, int y)
{
    m_dc->DrawText( text, x, y );
}

void AxWxDC::DrawSpline(int n, AxPoint points[])
{
    m_dc->DrawSpline( n, (wxPoint*)points );
}

wxColour AxWxDC::GetColour( int colour )
{ 
    switch ( colour )
    {
    case (AxBLACK): return *wxBLACK;
    case (AxWHITE): return *wxWHITE;
    case (AxRED): return *wxRED;
    case (AxGREEN): return *wxGREEN;
    case (AxBLUE): return *wxBLUE;
    case (AxCYAN): return *wxCYAN;
    case (AxLIGHT_GREY): return *wxLIGHT_GREY;
    default:
        char blue =  (colour & 255);
        char green = (colour >> 8) & 255;
        char red = (colour >> 16) & 255;
        wxColour rgb(red, blue, green);
        return wxColour(red, blue, green);
    }
}


