/////////////////////////////////////////////////////////////////////////////
// Name:        musbboxdc.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musleipzigbbox.h"
#include "musbboxdc.h"
#include "musrc.h"
#include "musdef.h"

static inline double DegToRad(double deg) { return (deg * M_PI) / 180.0; }
static inline double RadToDeg(double deg) { return (deg * 180.0) / M_PI; }

//----------------------------------------------------------------------------
// MusBBoxDC
//----------------------------------------------------------------------------


MusBBoxDC::MusBBoxDC ( MusRC *rc, int width, int height):
    MusDC()
{	
    m_correctMusicAscent = false; // do not correct the ascent in the Leipzig font    
    
    m_rc = rc;
    m_width = width;
    m_height = height;
    
    m_userScaleX = 1.0;
    m_userScaleY = 1.0;
    m_originX = 0;
    m_originY = 0;
    
    SetBrush( AxBLACK, wxSOLID );
    SetPen( AxBLACK, 1, wxSOLID );
}


MusBBoxDC::~MusBBoxDC ( )
{
}

void MusBBoxDC::StartGraphic( MusLayoutObject *object, wxString gClass, wxString gId )
{
    // add object
    object->ResetBB();
    m_objects.Add( object );
}
      
void MusBBoxDC::EndGraphic(MusLayoutObject *object, MusRC *rc ) 
{
    // detach the object
    m_objects.Detach(m_objects.Index(*object));
}

void MusBBoxDC::StartPage( )
{
}
 
void MusBBoxDC::EndPage() 
{
}

void MusBBoxDC::SetBrush( int colour, int style )
{
}
        
void MusBBoxDC::SetBackground( int colour, int style )
{
    // nothing to do, we do not handle Background
}
        
void MusBBoxDC::SetBackgroundMode( int mode )
{
    // nothing to do, we do not handle Background Mode
}
        
void MusBBoxDC::SetPen( int colour, int width, int style )
{
    m_penWidth = width;
}
        
void MusBBoxDC::SetFont( MusFontInfo *font_info )
{
    m_font = *font_info;
}
            

void MusBBoxDC::SetTextForeground( int colour )
{
}
        
void MusBBoxDC::SetTextBackground( int colour )
{
    // nothing to do, we do not handle Text Background Mode
}
       
void MusBBoxDC::ResetBrush( )
{
}
        
void MusBBoxDC::ResetPen( )
{
    SetPen( AxBLACK, 1, wxSOLID );
} 

void MusBBoxDC::SetLogicalOrigin( int x, int y ) 
{
    //// no idea how to handle this with the BB
    m_originX = -x;
    m_originY = -y;
} 

void MusBBoxDC::SetUserScale( double xScale, double yScale ) 
{
    //// no idea how to handle this with the BB
    m_userScaleX = xScale;
    m_userScaleY = yScale;
}       

void MusBBoxDC::GetTextExtent( wxString& string, int *w, int *h )
{
    //// cannot be done easily
}
       

MusPoint MusBBoxDC::GetLogicalOrigin( ) 
{
    return MusPoint( m_originX, m_originY );
}


// Drawing mething

void MusBBoxDC::DrawCQBezier(int x, int y, int x1, int height, int width, bool direction)
{
    if (direction) // true = up
        UpdateBB(x, y, x1, y + height);
    else {
        UpdateBB(x, y, x1, y - height);
    }
}

void MusBBoxDC::DrawCircle(int x, int y, int radius)
{
    DrawEllipse(x - radius, y - radius, 2*radius, 2*radius);
}


void MusBBoxDC::DrawEllipse(int x, int y, int width, int height)
{
    UpdateBB(x, y, x + width, y + height);
}

        
void MusBBoxDC::DrawEllipticArc(int x, int y, int width, int height, double start, double end)
{
    /*
    Draws an arc of an ellipse. The current pen is used for drawing the arc
    and the current brush is used for drawing the pie. This function is
    currently only available for X window and PostScript device contexts.

    x and y specify the x and y coordinates of the upper-left corner of the
    rectangle that contains the ellipse.

    width and height specify the width and height of the rectangle that
    contains the ellipse.

    start and end specify the start and end of the arc relative to the
    three-o'clock position from the center of the rectangle. Angles are
    specified in degrees (360 is a complete circle). Positive values mean
    counter-clockwise motion. If start is equal to end, a complete ellipse
    will be drawn. */

    //known bug: SVG draws with the current pen along the radii, but this does not happen in wxMSW

    wxString s ;
    //radius
    double rx = width / 2 ;
    double ry = height / 2 ;
    // center
    double xc = x + rx ;
    double yc = y + ry ;

    double xs, ys, xe, ye ;
    xs = xc + rx * cos (DegToRad(start)) ;
    xe = xc + rx * cos (DegToRad(end)) ;
    ys = yc - ry * sin (DegToRad(start)) ;
    ye = yc - ry * sin (DegToRad(end)) ;

    ///now same as circle arc...

    double theta1 = atan2(ys-yc, xs-xc);
    double theta2 = atan2(ye-yc, xe-xc);

    int fArc  ;                  // flag for large or small arc 0 means less than 180 degrees
    if ( (theta2 - theta1) > 0 ) fArc = 1; else fArc = 0 ;

    int fSweep ;
    if ( fabs(theta2 - theta1) > M_PI) fSweep = 1; else fSweep = 0 ;

    //WriteLine( wxString::Format("<path d=\"M%d %d A%d %d 0.0 %d %d  %d %d \" />",
    //    int(xs), int(ys), int(rx), int(ry),
    //    fArc, fSweep, int(xe), int(ye) ) );
}
  
              
void MusBBoxDC::DrawLine(int x1, int y1, int x2, int y2)
{
    if ( x1 > x2 ) {
        int tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if ( y1 > y2 ) {
        int tmp = y1;
        y1 = y2;
        y2 = tmp;
    }
    int penWidth = m_penWidth;
    if ( penWidth % 2 ) {
        penWidth += 1;
    }
    
    UpdateBB( x1 - penWidth / 2, y1 - penWidth / 2, x2 + penWidth / 2, y2 + penWidth / 2);
}
 
               
void MusBBoxDC::DrawPolygon(int n, MusPoint points[], int xoffset, int yoffset, int fill_style)
{
    if ( n == 0 ) {
        return;
    }
    int x1 = points[0].x + xoffset;
    int x2 = x1;
    int y1 = points[0].y + yoffset;
    int y2 = y1;
    
    for (int i = 0; i < n;  i++)
    {
        if ( points[i].x + xoffset < x1 ) x1 = points[i].x + xoffset;
        if ( points[i].x + xoffset > x2 ) x2 = points[i].x + xoffset;
        if ( points[i].y + yoffset < y1 ) y1 = points[i].y + yoffset;
        if ( points[i].y + yoffset > y2 ) y2 = points[i].y + yoffset;

    }
    UpdateBB(x1, y1, x2, y2);
}
    
            
void MusBBoxDC::DrawRectangle(int x, int y, int width, int height)
{
    DrawRoundedRectangle( x, y, width, height, 0 );
}


void MusBBoxDC::DrawRoundedRectangle(int x, int y, int width, int height, double radius)
{
    // avoid negative heights or widths
    if ( height < 0 ) {
        height = -height;
        y -= height;
    }
    if ( width < 0 ) {
        width = -width;
        x -= width;
    }
    int penWidth = m_penWidth;
    if ( penWidth % 2 ) {
        penWidth += 1;
    }
    
    UpdateBB( x - penWidth / 2, y - m_penWidth / 2, x + width + m_penWidth / 2, y + height + m_penWidth / 2);
}

        
void MusBBoxDC::DrawText(const wxString& text, int x, int y)
{
    DrawMusicText( text, x, y);
}



void MusBBoxDC::DrawRotatedText(const wxString& text, int x, int y, double angle)
{
    //known bug; if the font is drawn in a scaled DC, it will not behave exactly as wxMSW

    wxString s;

    // calculate bounding box
    wxCoord w, h, desc;
    //DoGetTextExtent(sText, &w, &h, &desc);
    w = h = desc = 0;

    //double rad = DegToRad(angle);

    
    //if (m_backgroundMode == wxSOLID)
    {
        //WriteLine("/*- MusSVGFileDC::DrawRotatedText - Backgound not implemented */") ;
    }
}


void MusBBoxDC::DrawMusicText(const wxString& text, int x, int y)
{
    unsigned int glyph;
    
    //// Use the MusLeipzigBBox class above    
    unsigned char c = (unsigned char)text[0];
    MusLeipzigBBox *bbox = new MusLeipzigBBox();
    
    switch (c) {
            /* figures */
        case 48: glyph = LEIPZIG_BBOX_FIGURE_0; break;
        case 49: glyph = LEIPZIG_BBOX_FIGURE_1; break;
        case 50: glyph = LEIPZIG_BBOX_FIGURE_2; break;
        case 51: glyph = LEIPZIG_BBOX_FIGURE_3; break;
        case 52: glyph = LEIPZIG_BBOX_FIGURE_4; break;
        case 53: glyph = LEIPZIG_BBOX_FIGURE_5; break;
        case 54: glyph = LEIPZIG_BBOX_FIGURE_6; break;
        case 55: glyph = LEIPZIG_BBOX_FIGURE_7; break;
        case 56: glyph = LEIPZIG_BBOX_FIGURE_8; break;
        case 57: glyph = LEIPZIG_BBOX_FIGURE_9; break;
            /* clef */
        case LEIPZIG_CLEF_G: glyph = LEIPZIG_BBOX_CLEF_G; break;
        case LEIPZIG_CLEF_F: glyph = LEIPZIG_BBOX_CLEF_F; break;
        case LEIPZIG_CLEF_C: glyph = LEIPZIG_BBOX_CLEF_C; break;
        case LEIPZIG_CLEF_8va: glyph = LEIPZIG_BBOX_CLEF_G8; break;
        case LEIPZIG_CLEF_G + LEIPZIG_OFFSET_MENSURAL: glyph = LEIPZIG_BBOX_CLEF_G_MENSURAL; break;
        case LEIPZIG_CLEF_F + LEIPZIG_OFFSET_MENSURAL: glyph = LEIPZIG_BBOX_CLEF_F_MENSURAL; break;
        case LEIPZIG_CLEF_C + LEIPZIG_OFFSET_MENSURAL: glyph = LEIPZIG_BBOX_CLEF_C_MENSURAL; break;
        case LEIPZIG_CLEF_8va + LEIPZIG_OFFSET_MENSURAL: glyph = LEIPZIG_BBOX_CLEF_G_MENSURAL; break; // ??
            /* meter */
        case LEIPZIG_METER_SYMB_2_CUT: glyph = LEIPZIG_BBOX_METER_SYMB_2_CUT; break;
        case LEIPZIG_METER_SYMB_3_CUT: glyph = LEIPZIG_BBOX_METER_SYMB_3_CUT; break;
        case LEIPZIG_METER_SYMB_CUT: glyph = LEIPZIG_BBOX_METER_SYMB_CUT; break;
        case LEIPZIG_METER_SYMB_COMMON: glyph = LEIPZIG_BBOX_METER_SYMB_COMMON; break;        
            /* alterations */
        case LEIPZIG_ACCID_SHARP: glyph = LEIPZIG_BBOX_ALT_SHARP; break;
        case LEIPZIG_ACCID_NATURAL: glyph = LEIPZIG_BBOX_ALT_NATURAL; break;
        case LEIPZIG_ACCID_FLAT: glyph = LEIPZIG_BBOX_ALT_FLAT; break;
        case LEIPZIG_ACCID_DOUBLE_SHARP: glyph = LEIPZIG_BBOX_ALT_DOUBLE_SHARP; break;
        case LEIPZIG_ACCID_SHARP + LEIPZIG_OFFSET_MENSURAL: glyph = LEIPZIG_BBOX_ALT_SHARP_MENSURAL; break;
        case LEIPZIG_ACCID_NATURAL + LEIPZIG_OFFSET_MENSURAL: glyph = LEIPZIG_BBOX_ALT_NATURAL_MENSURAL; break;
        case LEIPZIG_ACCID_FLAT + LEIPZIG_OFFSET_MENSURAL: glyph = LEIPZIG_BBOX_ALT_FLAT_MENSURAL; break;
        case LEIPZIG_ACCID_DOUBLE_SHARP + LEIPZIG_OFFSET_MENSURAL: glyph = LEIPZIG_BBOX_ALT_DOUBLE_SHARP_MENSURAL; break;
            /* rests */
        case LEIPZIG_REST_QUARTER: glyph = LEIPZIG_BBOX_REST_4; break;
        case LEIPZIG_REST_QUARTER + 1: glyph = LEIPZIG_BBOX_REST_8; break;
        case LEIPZIG_REST_QUARTER + 2: glyph = LEIPZIG_BBOX_REST_16; break;
        case LEIPZIG_REST_QUARTER + 3: glyph = LEIPZIG_BBOX_REST_32; break;
        case LEIPZIG_REST_QUARTER + 4: glyph = LEIPZIG_BBOX_REST_64; break;
        case LEIPZIG_REST_QUARTER + 5: glyph = LEIPZIG_BBOX_REST_128; break;
        case LEIPZIG_REST_QUARTER + LEIPZIG_OFFSET_MENSURAL: glyph = LEIPZIG_BBOX_REST_4_MENSURAL; break;
        case LEIPZIG_REST_QUARTER + 1 + LEIPZIG_OFFSET_MENSURAL: glyph = LEIPZIG_BBOX_REST_8_MENSURAL; break;
        case LEIPZIG_REST_QUARTER + 2 + LEIPZIG_OFFSET_MENSURAL: glyph = LEIPZIG_BBOX_REST_16_MENSURAL; break;
        case LEIPZIG_REST_QUARTER + 3 + LEIPZIG_OFFSET_MENSURAL: glyph = LEIPZIG_BBOX_REST_32_MENSURAL; break;
        case LEIPZIG_REST_QUARTER + 4 + LEIPZIG_OFFSET_MENSURAL: glyph = LEIPZIG_BBOX_REST_64_MENSURAL; break;
        case LEIPZIG_REST_QUARTER + 5 + LEIPZIG_OFFSET_MENSURAL: glyph = LEIPZIG_BBOX_REST_128_MENSURAL; break;
            /* note heads */
        case LEIPZIG_HEAD_WHOLE: glyph = LEIPZIG_BBOX_HEAD_WHOLE; break;
        case LEIPZIG_HEAD_WHOLE_FILLED: glyph = LEIPZIG_BBOX_HEAD_WHOLE_FILL; break;
        case LEIPZIG_HEAD_HALF: glyph = LEIPZIG_BBOX_HEAD_HALF; break;
        case LEIPZIG_HEAD_QUARTER: glyph = LEIPZIG_BBOX_HEAD_QUARTER; break;
        case LEIPZIG_HEAD_WHOLE + LEIPZIG_OFFSET_MENSURAL: glyph = LEIPZIG_BBOX_HEAD_WHOLE_DIAMOND; break;
        case LEIPZIG_HEAD_WHOLE_FILLED + LEIPZIG_OFFSET_MENSURAL: glyph = LEIPZIG_BBOX_HEAD_WHOLE_FILLDIAMOND; break;
        case LEIPZIG_HEAD_HALF + LEIPZIG_OFFSET_MENSURAL: glyph = LEIPZIG_BBOX_HEAD_HALF_DIAMOND; break;
        case LEIPZIG_HEAD_QUARTER + LEIPZIG_OFFSET_MENSURAL: glyph = LEIPZIG_BBOX_HEAD_QUARTER_FILLDIAMOND; break;
            /* slashes */
        case LEIPZIG_STEM_FLAG_UP: glyph = LEIPZIG_BBOX_SLASH_UP; break;
        case LEIPZIG_STEM_FLAG_DOWN: glyph = LEIPZIG_BBOX_SLASH_DOWN; break;
        case LEIPZIG_STEM_FLAG_UP + LEIPZIG_OFFSET_MENSURAL: glyph = LEIPZIG_BBOX_SLASH_UP_MENSURAL; break;
        case LEIPZIG_STEM_FLAG_DOWN + LEIPZIG_OFFSET_MENSURAL: glyph = LEIPZIG_BBOX_SLASH_DOWN_MENSURAL; break;
            /* ornaments */
        case 35: glyph = LEIPZIG_BBOX_ORN_MORDENT; break;
            /* todo */
        default: glyph = LEIPZIG_BBOX_ALT_DOUBLE_SHARP; // ??
    }

    
    int x_off = x + (bbox->m_bBox[glyph].m_x * ((double)(m_font.GetPointSize() / LEIPZIG_UNITS_PER_EM)) );
    // because we are in the rendering context, y position are already flipped
    int y_off = y - (bbox->m_bBox[glyph].m_y * ((double)(m_font.GetPointSize() / LEIPZIG_UNITS_PER_EM)) );
    
    UpdateBB(x_off, y_off, 
        x_off + (bbox->m_bBox[glyph].m_width * ((double)(m_font.GetPointSize() / LEIPZIG_UNITS_PER_EM)) ),
        // idem, y position are flipped
        y_off - (bbox->m_bBox[glyph].m_height * ((double)(m_font.GetPointSize() / LEIPZIG_UNITS_PER_EM)) ));
    
}


void MusBBoxDC::DrawSpline(int n, MusPoint points[])
{
    //m_dc->DrawSpline( n, (wxPoint*)points );
}

void MusBBoxDC::UpdateBB(int x1, int y1, int x2, int y2) 
{
    /*
    MusLayoutObject *first = &m_objects[m_objects.Count() - 1];
    
    first->UpdateOwnBB(x1, y1, x2, y2);
    
    // Stretch the content BB of the other objects
    // Check that we are not the only elem in the list
    if (m_objects.Count() > 1) {
        
        // The second element in the list stretches in base of the new BBox of the first
        m_objects[m_objects.Count() - 2].UpdateContentBB(first->m_selfBB_x1, first->m_selfBB_y1, first->m_selfBB_x2, first->m_selfBB_y2);
        
        // All the next ones, stretch using contentBB
        for (int i = m_objects.Count() - 3; i >= 0; i--) {
            MusLayoutObject *precedent = &m_objects[i + 1];
            m_objects[i].UpdateContentBB(precedent->m_contentBB_x1, precedent->m_contentBB_y1, precedent->m_contentBB_x2, precedent->m_contentBB_y2);
        }
    }
    */
    
    // simpler version 
    
    // the array should not be empty
    wxASSERT_MSG( !m_objects.IsEmpty(), "Array cannot be empty" ) ;
    
    // we need to store logical coordinates in the objects, we need to convert them back (this is why we need a MusRC object)
    (&m_objects.Last())->UpdateSelfBB( m_rc->ToLogicalX(x1), m_rc->ToLogicalY(y1), m_rc->ToLogicalX(x2), m_rc->ToLogicalY(y2) );
    
    int i;
    for (i = 0; i < (int)m_objects.GetCount(); i++) {
        (&m_objects[i])->UpdateContentBB( m_rc->ToLogicalX(x1), m_rc->ToLogicalY(y1), m_rc->ToLogicalX(x2), m_rc->ToLogicalY(y2) );
    }
}
