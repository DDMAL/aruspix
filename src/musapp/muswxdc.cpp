/////////////////////////////////////////////////////////////////////////////
// Name:        muswxdc.cpp
// Author:      Laurent Pugin
// Created:     2010
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "muswxdc.h"
#include "view.h"

using namespace vrv;

//----------------------------------------------------------------------------
// AxWxDc
//----------------------------------------------------------------------------

MusWxDC::MusWxDC ( wxDC *dc ):
    DeviceContext()
{
    m_dc = dc;
    m_backgroundImage= NULL;
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

void MusWxDC::SetBackgroundImage( void *image, double opacity )
{
    if ( image == NULL ) {
        m_backgroundImage = NULL;
    }
    else {
        m_backgroundImage = (wxImage*)image;
    }
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
        
void MusWxDC::SetFont( FontMetricsInfo *font_info )
{
    wxFont font( font_info->pointSize, (wxFontFamily)font_info->family, font_info->style,
        (wxFontWeight)font_info->weight, font_info->underlined, font_info->faceName.c_str(),
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
       

void MusWxDC::GetTextExtent( const std::string& string, int *w, int *h )
{
    m_dc->GetTextExtent( string.c_str(), w, h );
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

void MusWxDC::DrawComplexBezierPath(int x, int y, int bezier1_coord[6], int bezier2_coord[6])
{
    wxGraphicsContext* gc = m_dc->GetGraphicsContext(); 
    wxGraphicsPath gp;
    gp = gc->CreatePath();
    
    gp.MoveToPoint(x, y);
    
    
    gp.AddCurveToPoint(bezier1_coord[0], bezier1_coord[1], bezier1_coord[2], bezier1_coord[3], bezier1_coord[4], bezier1_coord[5]);
    gp.AddCurveToPoint(bezier2_coord[0], bezier2_coord[1], bezier2_coord[2], bezier2_coord[3], bezier2_coord[4], bezier2_coord[5]);
    
    gc->SetPen(wxPen("black", 1)); 
    gc->SetBrush(wxBrush("black"));
    
    gc->FillPath(gp);
    
    gc->SetPen(wxNullPen); 
    gc->SetBrush(wxNullBrush);  
}

void MusWxDC::DrawRectangle(int x, int y, int width, int height)
{
    m_dc->DrawRectangle( x, y, width, height );
}

void MusWxDC::DrawRoundedRectangle(int x, int y, int width, int height, double radius)
{
    m_dc->DrawRoundedRectangle( x, y, width, height, radius );
}
        
void MusWxDC::DrawText(const std::string& text, int x, int y)
{
    m_dc->DrawText( text.c_str(), x, y );
}

void MusWxDC::DrawMusicText(const std::string& text, int x, int y)
{
    m_dc->DrawText( text.c_str(), x, y );
}

void MusWxDC::DrawRotatedText(const std::string& text, int x, int y, double angle)
{
    m_dc->DrawRotatedText( text.c_str(), x, y, angle );
}

void MusWxDC::DrawSpline(int n, MusPoint points[])
{
    m_dc->DrawSpline( n, (wxPoint*)points );
}

void MusWxDC::DrawBackgroundImage( int x, int y)
{
    if ( !m_backgroundImage || !m_backgroundImage->IsOk() ) {
        return;
    }
    m_dc->DrawBitmap( wxBitmap( *m_backgroundImage ), x, y, false );
    
}

void MusWxDC::EndGraphic( DocObject *object, View *rc )
{
    // actually draw the two bounding boxes  
    // bounding boxes are given in logical coordinates - we need to convert them
    // this is why we have a View object
    
    //return;
    if (GetDrawBoundingBoxes()) {
        SetPen( AxRED, 1, wxDOT_DASH );
        if ( object->HasSelfBB() ) {
           m_dc->DrawRectangle( rc->ToDeviceContextX( object->m_selfBB_x1 ), rc->ToDeviceContextY( object->m_selfBB_y1 ), 
                rc->ToDeviceContextX( object->m_selfBB_x2 ) - rc->ToDeviceContextX( object->m_selfBB_x1 ), 
                rc->ToDeviceContextY( object->m_selfBB_y2 ) - rc->ToDeviceContextY( object->m_selfBB_y1 ));
        }

        SetPen( AxBLUE, 1, wxDOT);
        if ( object->HasContentBB() ) {
            m_dc->DrawRectangle( rc->ToDeviceContextX( object->m_contentBB_x1 ), rc->ToDeviceContextY( object->m_contentBB_y1 ), 
                rc->ToDeviceContextX( object->m_contentBB_x2 ) - rc->ToDeviceContextX( object->m_contentBB_x1 ), 
                rc->ToDeviceContextY( object->m_contentBB_y2 ) - rc->ToDeviceContextY( object->m_contentBB_y1 )); 
        }
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