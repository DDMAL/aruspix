/////////////////////////////////////////////////////////////////////////////
// Name:        muswxdc.cpp
// Author:      Laurent Pugin
// Created:     2010
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "muswxdc.h"
#include "musrc.h"

//----------------------------------------------------------------------------
// AxWxDc
//----------------------------------------------------------------------------

MusWxDC::MusWxDC ( wxDC *dc ):
    MusDC()
{
    m_dc = dc;
}


MusWxDC::~MusWxDC ( )
{
}
        
        
void MusWxDC::SetBrush( int colour, int style )
{
    m_brush.SetColour( GetColour( colour ) );
    m_brush.SetStyle( style );
    m_dc->SetBrush( m_brush );
}
        
void MusWxDC::SetBackground( int colour, int style )
{
    m_dc->SetBackground( wxBrush( GetColour( colour ), style ) );
}
        
void MusWxDC::SetBackgroundMode( int mode )
{
    m_dc->SetBackgroundMode( mode );
}
        
void MusWxDC::SetPen( int colour, int width, int style )
{
    m_pen.SetColour( GetColour( colour ) );
    m_pen.SetWidth( width );
    m_pen.SetStyle( style );
    m_dc->SetPen( m_pen );
}
        
void MusWxDC::SetFont( MusFontInfo *font_info )
{
    wxFont font( font_info->pointSize, (wxFontFamily)font_info->family, font_info->style,
        (wxFontWeight)font_info->weight, font_info->underlined, font_info->faceName,
        (wxFontEncoding)font_info->encoding );
    m_dc->SetFont( font );
}
            

void MusWxDC::SetTextForeground( int colour )
{
    m_dc->SetTextForeground(  GetColour( colour ) );
}
        
void MusWxDC::SetTextBackground( int colour )
{
    m_dc->SetTextBackground(  GetColour( colour ) );
}
       
void MusWxDC::ResetBrush( )
{
    m_dc->SetBrush( wxNullBrush );
}
        
void MusWxDC::ResetPen( )
{
    m_dc->SetPen( wxNullPen );
} 

void MusWxDC::SetLogicalOrigin( int x, int y ) 
{
    m_dc->SetLogicalOrigin( x, y );
}
       

void MusWxDC::GetTextExtent( wxString& string, int *w, int *h )
{
    m_dc->GetTextExtent( string, w, h );
} 


MusPoint MusWxDC::GetLogicalOrigin( ) 
{
    wxPoint origin = m_dc->GetLogicalOrigin();
    return MusPoint(origin.x, origin.y );
}

// Drawing mething
        
void MusWxDC::DrawCircle(int x, int y, int radius)
{
    m_dc->DrawCircle( x, y, radius );
}

void MusWxDC::DrawEllipse(int x, int y, int width, int height)
{
    m_dc->DrawEllipse( x, y, width, height );
}
        
void MusWxDC::DrawEllipticArc(int x, int y, int width, int height, double start, double end)
{
    m_dc->DrawEllipticArc( x, y, width, height, start, end );
}
        
void MusWxDC::DrawLine(int x1, int y1, int x2, int y2)
{
    m_dc->DrawLine( x1, y1, x2, y2 );
}
        
void MusWxDC::DrawPolygon(int n, MusPoint points[], int xoffset, int yoffset, int fill_style)
{
    m_dc->DrawPolygon( n, (wxPoint*)points, xoffset, yoffset, fill_style );
}
        
void MusWxDC::DrawRectangle(int x, int y, int width, int height)
{
    m_dc->DrawRectangle( x, y, width, height );
}

void MusWxDC::DrawRoundedRectangle(int x, int y, int width, int height, double radius)
{
    m_dc->DrawRoundedRectangle( x, y, width, height, radius );
}
        
void MusWxDC::DrawText(const wxString& text, int x, int y)
{
    m_dc->DrawText( text, x, y );
}

void MusWxDC::DrawMusicText(const wxString& text, int x, int y)
{
    m_dc->DrawText( text, x, y );
}

void MusWxDC::DrawRotatedText(const wxString& text, int x, int y, double angle)
{
    m_dc->DrawRotatedText( text, x, y, angle );
}

void MusWxDC::DrawSpline(int n, MusPoint points[])
{
    m_dc->DrawSpline( n, (wxPoint*)points );
}

void MusWxDC::EndGraphic( MusLayoutObject *object, MusRC *rc )
{
    // actually draw the two bounding boxes  
    // bounding boxes are given in logical coordinates - we need to convert them
    // this is why we have a MusRC object
    SetPen( AxRED, 1, wxDOT_DASH );
    if ( object->HasSelfBB() ) {
       m_dc->DrawRectangle( rc->ToRendererX( object->m_selfBB_x1 ), rc->ToRendererY( object->m_selfBB_y1 ), 
            rc->ToRendererX( object->m_selfBB_x2 ) - rc->ToRendererX( object->m_selfBB_x1 ), 
            rc->ToRendererY( object->m_selfBB_y2 ) - rc->ToRendererY( object->m_selfBB_y1 ));
    }

    SetPen( AxBLUE, 1, wxDOT);
    if ( object->HasContentBB() ) {
        m_dc->DrawRectangle( rc->ToRendererX( object->m_contentBB_x1 ), rc->ToRendererY( object->m_contentBB_y1 ), 
            rc->ToRendererX( object->m_contentBB_x2 ) - rc->ToRendererX( object->m_contentBB_x1 ), 
            rc->ToRendererY( object->m_contentBB_y2 ) - rc->ToRendererY( object->m_contentBB_y1 )); 
    }
}

wxColour MusWxDC::GetColour( int colour )
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
        int blue =  (colour & 255);
        int green = (colour >> 8) & 255;
        int red = (colour >> 16) & 255;
        wxColour rgb(red, blue, green);
        return wxColour(red, blue, green);
    }
}


