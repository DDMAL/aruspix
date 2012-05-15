/////////////////////////////////////////////////////////////////////////////
// Name:        musbboxdc.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musbboxdc.h"
#include "musdef.h"

#define LEIPZIG_BBOX_ORN_MORDENT 0
#define LEIPZIG_BBOX_FIGURE_0 1
#define LEIPZIG_BBOX_FIGURE_1 2
#define LEIPZIG_BBOX_FIGURE_2 3
#define LEIPZIG_BBOX_FIGURE_3 4
#define LEIPZIG_BBOX_FIGURE_4 5
#define LEIPZIG_BBOX_FIGURE_5 6
#define LEIPZIG_BBOX_FIGURE_6 7
#define LEIPZIG_BBOX_FIGURE_7 8
#define LEIPZIG_BBOX_FIGURE_8 9
#define LEIPZIG_BBOX_FIGURE_9 10
#define LEIPZIG_BBOX_HEAD_WHOLE 11
#define LEIPZIG_BBOX_HEAD_WHOLE_FILL 12
#define LEIPZIG_BBOX_HEAD_HALF 13
#define LEIPZIG_BBOX_HEAD_QUARTER 14
#define LEIPZIG_BBOX_SLASH_UP 15
#define LEIPZIG_BBOX_SLASH_DOWN 16
#define LEIPZIG_BBOX_CLEF_G 17
#define LEIPZIG_BBOX_CLEF_F 18
#define LEIPZIG_BBOX_CLEF_C 19
#define LEIPZIG_BBOX_CLEF_G8 20
#define LEIPZIG_BBOX_ALT_SHARP 21
#define LEIPZIG_BBOX_ALT_NATURAL 22
#define LEIPZIG_BBOX_ALT_FLAT 23
#define LEIPZIG_BBOX_ALT_DOUBLE_SHARP 24
#define LEIPZIG_BBOX_REST_4 25
#define LEIPZIG_BBOX_REST_8 26
#define LEIPZIG_BBOX_REST_16 27
#define LEIPZIG_BBOX_REST_32 28
#define LEIPZIG_BBOX_REST_64 29
#define LEIPZIG_BBOX_REST_128 30
#define LEIPZIG_BBOX_HEAD_WHOLE_DIAMOND 31
#define LEIPZIG_BBOX_HEAD_WHOLE_FILLDIAMOND 32
#define LEIPZIG_BBOX_HEAD_HALF_DIAMOND 33
#define LEIPZIG_BBOX_HEAD_QUARTER_FILLDIAMOND 34
#define LEIPZIG_BBOX_SLASH_UP_MENSURAL 35
#define LEIPZIG_BBOX_SLASH_DOWN_MENSURAL 36
#define LEIPZIG_BBOX_CLEF_G_MENSURAL 37
#define LEIPZIG_BBOX_CLEF_F_MENSURAL 38
#define LEIPZIG_BBOX_CLEF_C_MENSURAL 39
#define LEIPZIG_BBOX_CLEF_G_CHIAVETTE 40
#define LEIPZIG_BBOX_ALT_SHARP_MENSURAL 41
#define LEIPZIG_BBOX_ALT_NATURAL_MENSURAL 42
#define LEIPZIG_BBOX_ALT_FLAT_MENSURAL 43
#define LEIPZIG_BBOX_ALT_DOUBLE_SHARP_MENSURAL 44
#define LEIPZIG_BBOX_REST_4_MENSURAL 45
#define LEIPZIG_BBOX_REST_8_MENSURAL 46
#define LEIPZIG_BBOX_REST_16_MENSURAL 47
#define LEIPZIG_BBOX_REST_32_MENSURAL 48
#define LEIPZIG_BBOX_REST_64_MENSURAL 49
#define LEIPZIG_BBOX_REST_128_MENSURAL 50


/** 
 * This class is used for getting the bounding box of the Leipzig glyphs.
 * The values are used obtained with the ./varia/svg/split.xsl.
 */ 
class MusLeipzigBBox
{
public:
    MusLeipzigBBox() {
        m_bBox[LEIPZIG_BBOX_ORN_MORDENT].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_ORN_MORDENT].m_y = -2.0;
        m_bBox[LEIPZIG_BBOX_ORN_MORDENT].m_width = 605.0;
        m_bBox[LEIPZIG_BBOX_ORN_MORDENT].m_height = 242.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_0].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_0].m_y = 0.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_0].m_width = 427.7;
        m_bBox[LEIPZIG_BBOX_FIGURE_0].m_height = 516.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_1].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_1].m_y = 0.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_1].m_width = 296.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_1].m_height = 496.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_2].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_2].m_y = -15.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_2].m_width = 402.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_2].m_height = 513.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_3].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_3].m_y = 0.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_3].m_width = 383.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_3].m_height = 515.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_4].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_4].m_y = 0.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_4].m_width = 387.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_4].m_height = 494.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_5].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_5].m_y = 0.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_5].m_width = 357.7;
        m_bBox[LEIPZIG_BBOX_FIGURE_5].m_height = 494.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_6].m_x = -1.3;
        m_bBox[LEIPZIG_BBOX_FIGURE_6].m_y = 0.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_6].m_width = 395.3;
        m_bBox[LEIPZIG_BBOX_FIGURE_6].m_height = 516.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_7].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_7].m_y = 0.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_7].m_width = 404.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_7].m_height = 511.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_8].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_8].m_y = 0.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_8].m_width = 386.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_8].m_height = 515.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_9].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_9].m_y = 0.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_9].m_width = 397.0;
        m_bBox[LEIPZIG_BBOX_FIGURE_9].m_height = 513.0;
        m_bBox[LEIPZIG_BBOX_HEAD_WHOLE].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_HEAD_WHOLE].m_y = -133.0;
        m_bBox[LEIPZIG_BBOX_HEAD_WHOLE].m_width = 405.0;
        m_bBox[LEIPZIG_BBOX_HEAD_WHOLE].m_height = 266.0;
        m_bBox[LEIPZIG_BBOX_HEAD_WHOLE_FILL].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_HEAD_WHOLE_FILL].m_y = -133.0;
        m_bBox[LEIPZIG_BBOX_HEAD_WHOLE_FILL].m_width = 405.0;
        m_bBox[LEIPZIG_BBOX_HEAD_WHOLE_FILL].m_height = 266.0;
        m_bBox[LEIPZIG_BBOX_HEAD_HALF].m_x = -15.0;
        m_bBox[LEIPZIG_BBOX_HEAD_HALF].m_y = -135.0;
        m_bBox[LEIPZIG_BBOX_HEAD_HALF].m_width = 314.0;
        m_bBox[LEIPZIG_BBOX_HEAD_HALF].m_height = 270.0;
        m_bBox[LEIPZIG_BBOX_HEAD_QUARTER].m_x = -15.0;
        m_bBox[LEIPZIG_BBOX_HEAD_QUARTER].m_y = -135.0;
        m_bBox[LEIPZIG_BBOX_HEAD_QUARTER].m_width = 314.0;
        m_bBox[LEIPZIG_BBOX_HEAD_QUARTER].m_height = 270.0;
        m_bBox[LEIPZIG_BBOX_SLASH_UP].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_SLASH_UP].m_y = -597.0;
        m_bBox[LEIPZIG_BBOX_SLASH_UP].m_width = 276.0;
        m_bBox[LEIPZIG_BBOX_SLASH_UP].m_height = 577.0;
        m_bBox[LEIPZIG_BBOX_SLASH_DOWN].m_x = 164.0;
        m_bBox[LEIPZIG_BBOX_SLASH_DOWN].m_y = 20.0;
        m_bBox[LEIPZIG_BBOX_SLASH_DOWN].m_width = 276.0;
        m_bBox[LEIPZIG_BBOX_SLASH_DOWN].m_height = 577.0;
        m_bBox[LEIPZIG_BBOX_CLEF_G].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_CLEF_G].m_y = -655.0;
        m_bBox[LEIPZIG_BBOX_CLEF_G].m_width = 679.0;
        m_bBox[LEIPZIG_BBOX_CLEF_G].m_height = 1809.0;
        m_bBox[LEIPZIG_BBOX_CLEF_F].m_x = 4.0;
        m_bBox[LEIPZIG_BBOX_CLEF_F].m_y = -566.0;
        m_bBox[LEIPZIG_BBOX_CLEF_F].m_width = 694.0;
        m_bBox[LEIPZIG_BBOX_CLEF_F].m_height = 832.0;
        m_bBox[LEIPZIG_BBOX_CLEF_C].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_CLEF_C].m_y = -536.0;
        m_bBox[LEIPZIG_BBOX_CLEF_C].m_width = 659.0;
        m_bBox[LEIPZIG_BBOX_CLEF_C].m_height = 1082.0;
        m_bBox[LEIPZIG_BBOX_CLEF_G8].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_CLEF_G8].m_y = -844.0;
        m_bBox[LEIPZIG_BBOX_CLEF_G8].m_width = 679.0;
        m_bBox[LEIPZIG_BBOX_CLEF_G8].m_height = 1998.0;
        m_bBox[LEIPZIG_BBOX_ALT_SHARP].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_ALT_SHARP].m_y = -364.0;
        m_bBox[LEIPZIG_BBOX_ALT_SHARP].m_width = 197.0;
        m_bBox[LEIPZIG_BBOX_ALT_SHARP].m_height = 746.0;
        m_bBox[LEIPZIG_BBOX_ALT_NATURAL].m_x = 20.0;
        m_bBox[LEIPZIG_BBOX_ALT_NATURAL].m_y = -377.0;
        m_bBox[LEIPZIG_BBOX_ALT_NATURAL].m_width = 157.0;
        m_bBox[LEIPZIG_BBOX_ALT_NATURAL].m_height = 754.0;
        m_bBox[LEIPZIG_BBOX_ALT_FLAT].m_x = 22.0;
        m_bBox[LEIPZIG_BBOX_ALT_FLAT].m_y = -184.0;
        m_bBox[LEIPZIG_BBOX_ALT_FLAT].m_width = 198.0;
        m_bBox[LEIPZIG_BBOX_ALT_FLAT].m_height = 678.0;
        m_bBox[LEIPZIG_BBOX_ALT_DOUBLE_SHARP].m_x = -20.0;
        m_bBox[LEIPZIG_BBOX_ALT_DOUBLE_SHARP].m_y = -114.0;
        m_bBox[LEIPZIG_BBOX_ALT_DOUBLE_SHARP].m_width = 271.0;
        m_bBox[LEIPZIG_BBOX_ALT_DOUBLE_SHARP].m_height = 228.0;
        m_bBox[LEIPZIG_BBOX_REST_4].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_REST_4].m_y = -146.0;
        m_bBox[LEIPZIG_BBOX_REST_4].m_width = 324.0;
        m_bBox[LEIPZIG_BBOX_REST_4].m_height = 808.0;
        m_bBox[LEIPZIG_BBOX_REST_8].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_REST_8].m_y = 0.0;
        m_bBox[LEIPZIG_BBOX_REST_8].m_width = 290.0;
        m_bBox[LEIPZIG_BBOX_REST_8].m_height = 455.0;
        m_bBox[LEIPZIG_BBOX_REST_16].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_REST_16].m_y = -249.0;
        m_bBox[LEIPZIG_BBOX_REST_16].m_width = 341.0;
        m_bBox[LEIPZIG_BBOX_REST_16].m_height = 702.0;
        m_bBox[LEIPZIG_BBOX_REST_32].m_x = 1.0;
        m_bBox[LEIPZIG_BBOX_REST_32].m_y = -248.0;
        m_bBox[LEIPZIG_BBOX_REST_32].m_width = 419.0;
        m_bBox[LEIPZIG_BBOX_REST_32].m_height = 946.0;
        m_bBox[LEIPZIG_BBOX_REST_64].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_REST_64].m_y = -248.0;
        m_bBox[LEIPZIG_BBOX_REST_64].m_width = 503.0;
        m_bBox[LEIPZIG_BBOX_REST_64].m_height = 1191.0;
        m_bBox[LEIPZIG_BBOX_REST_128].m_x = -1.0;
        m_bBox[LEIPZIG_BBOX_REST_128].m_y = -510.0;
        m_bBox[LEIPZIG_BBOX_REST_128].m_width = 602.0;
        m_bBox[LEIPZIG_BBOX_REST_128].m_height = 1485.0;
        m_bBox[LEIPZIG_BBOX_HEAD_WHOLE_DIAMOND].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_HEAD_WHOLE_DIAMOND].m_y = -211.0;
        m_bBox[LEIPZIG_BBOX_HEAD_WHOLE_DIAMOND].m_width = 359.0;
        m_bBox[LEIPZIG_BBOX_HEAD_WHOLE_DIAMOND].m_height = 445.0;
        m_bBox[LEIPZIG_BBOX_HEAD_WHOLE_FILLDIAMOND].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_HEAD_WHOLE_FILLDIAMOND].m_y = -211.0;
        m_bBox[LEIPZIG_BBOX_HEAD_WHOLE_FILLDIAMOND].m_width = 359.0;
        m_bBox[LEIPZIG_BBOX_HEAD_WHOLE_FILLDIAMOND].m_height = 445.0;
        m_bBox[LEIPZIG_BBOX_HEAD_HALF_DIAMOND].m_x = -34.0;
        m_bBox[LEIPZIG_BBOX_HEAD_HALF_DIAMOND].m_y = -211.0;
        m_bBox[LEIPZIG_BBOX_HEAD_HALF_DIAMOND].m_width = 358.0;
        m_bBox[LEIPZIG_BBOX_HEAD_HALF_DIAMOND].m_height = 445.0;
        m_bBox[LEIPZIG_BBOX_HEAD_QUARTER_FILLDIAMOND].m_x = -34.0;
        m_bBox[LEIPZIG_BBOX_HEAD_QUARTER_FILLDIAMOND].m_y = -211.0;
        m_bBox[LEIPZIG_BBOX_HEAD_QUARTER_FILLDIAMOND].m_width = 358.0;
        m_bBox[LEIPZIG_BBOX_HEAD_QUARTER_FILLDIAMOND].m_height = 445.0;
        m_bBox[LEIPZIG_BBOX_SLASH_UP_MENSURAL].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_SLASH_UP_MENSURAL].m_y = -237.0;
        m_bBox[LEIPZIG_BBOX_SLASH_UP_MENSURAL].m_width = 162.0;
        m_bBox[LEIPZIG_BBOX_SLASH_UP_MENSURAL].m_height = 237.0;
        m_bBox[LEIPZIG_BBOX_SLASH_DOWN_MENSURAL].m_x = 2.0;
        m_bBox[LEIPZIG_BBOX_SLASH_DOWN_MENSURAL].m_y = 6.0;
        m_bBox[LEIPZIG_BBOX_SLASH_DOWN_MENSURAL].m_width = 162.0;
        m_bBox[LEIPZIG_BBOX_SLASH_DOWN_MENSURAL].m_height = 237.0;
        m_bBox[LEIPZIG_BBOX_CLEF_G_MENSURAL].m_x = 145.3;
        m_bBox[LEIPZIG_BBOX_CLEF_G_MENSURAL].m_y = -180.7;
        m_bBox[LEIPZIG_BBOX_CLEF_G_MENSURAL].m_width = 461.7;
        m_bBox[LEIPZIG_BBOX_CLEF_G_MENSURAL].m_height = 936.3;
        m_bBox[LEIPZIG_BBOX_CLEF_F_MENSURAL].m_x = 109.0;
        m_bBox[LEIPZIG_BBOX_CLEF_F_MENSURAL].m_y = -753.0;
        m_bBox[LEIPZIG_BBOX_CLEF_F_MENSURAL].m_width = 286.0;
        m_bBox[LEIPZIG_BBOX_CLEF_F_MENSURAL].m_height = 1108.0;
        m_bBox[LEIPZIG_BBOX_CLEF_C_MENSURAL].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_CLEF_C_MENSURAL].m_y = -602.0;
        m_bBox[LEIPZIG_BBOX_CLEF_C_MENSURAL].m_width = 349.0;
        m_bBox[LEIPZIG_BBOX_CLEF_C_MENSURAL].m_height = 1197.0;
        m_bBox[LEIPZIG_BBOX_CLEF_G_CHIAVETTE].m_x = 13.0;
        m_bBox[LEIPZIG_BBOX_CLEF_G_CHIAVETTE].m_y = -273.0;
        m_bBox[LEIPZIG_BBOX_CLEF_G_CHIAVETTE].m_width = 619.0;
        m_bBox[LEIPZIG_BBOX_CLEF_G_CHIAVETTE].m_height = 1031.0;
        m_bBox[LEIPZIG_BBOX_ALT_SHARP_MENSURAL].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_ALT_SHARP_MENSURAL].m_y = -130.0;
        m_bBox[LEIPZIG_BBOX_ALT_SHARP_MENSURAL].m_width = 240.0;
        m_bBox[LEIPZIG_BBOX_ALT_SHARP_MENSURAL].m_height = 260.0;
        m_bBox[LEIPZIG_BBOX_ALT_NATURAL_MENSURAL].m_x = 30.0;
        m_bBox[LEIPZIG_BBOX_ALT_NATURAL_MENSURAL].m_y = -334.0;
        m_bBox[LEIPZIG_BBOX_ALT_NATURAL_MENSURAL].m_width = 134.0;
        m_bBox[LEIPZIG_BBOX_ALT_NATURAL_MENSURAL].m_height = 713.0;
        m_bBox[LEIPZIG_BBOX_ALT_FLAT_MENSURAL].m_x = 30.0;
        m_bBox[LEIPZIG_BBOX_ALT_FLAT_MENSURAL].m_y = -112.0;
        m_bBox[LEIPZIG_BBOX_ALT_FLAT_MENSURAL].m_width = 177.0;
        m_bBox[LEIPZIG_BBOX_ALT_FLAT_MENSURAL].m_height = 641.0;
        m_bBox[LEIPZIG_BBOX_ALT_DOUBLE_SHARP_MENSURAL].m_x = 0.0;
        m_bBox[LEIPZIG_BBOX_ALT_DOUBLE_SHARP_MENSURAL].m_y = -114.0;
        m_bBox[LEIPZIG_BBOX_ALT_DOUBLE_SHARP_MENSURAL].m_width = 271.0;
        m_bBox[LEIPZIG_BBOX_ALT_DOUBLE_SHARP_MENSURAL].m_height = 228.0;
        m_bBox[LEIPZIG_BBOX_REST_4_MENSURAL].m_x = 80.0;
        m_bBox[LEIPZIG_BBOX_REST_4_MENSURAL].m_y = 0.0;
        m_bBox[LEIPZIG_BBOX_REST_4_MENSURAL].m_width = 170.0;
        m_bBox[LEIPZIG_BBOX_REST_4_MENSURAL].m_height = 170.0;
        m_bBox[LEIPZIG_BBOX_REST_8_MENSURAL].m_x = 80.0;
        m_bBox[LEIPZIG_BBOX_REST_8_MENSURAL].m_y = 0.0;
        m_bBox[LEIPZIG_BBOX_REST_8_MENSURAL].m_width = 170.0;
        m_bBox[LEIPZIG_BBOX_REST_8_MENSURAL].m_height = 170.0;
        m_bBox[LEIPZIG_BBOX_REST_16_MENSURAL].m_x = 80.0;
        m_bBox[LEIPZIG_BBOX_REST_16_MENSURAL].m_y = 0.0;
        m_bBox[LEIPZIG_BBOX_REST_16_MENSURAL].m_width = 162.0;
        m_bBox[LEIPZIG_BBOX_REST_16_MENSURAL].m_height = 262.0;
        m_bBox[LEIPZIG_BBOX_REST_32_MENSURAL].m_x = 80.0;
        m_bBox[LEIPZIG_BBOX_REST_32_MENSURAL].m_y = 0.0;
        m_bBox[LEIPZIG_BBOX_REST_32_MENSURAL].m_width = 162.0;
        m_bBox[LEIPZIG_BBOX_REST_32_MENSURAL].m_height = 362.0;
        m_bBox[LEIPZIG_BBOX_REST_64_MENSURAL].m_x = 80.0;
        m_bBox[LEIPZIG_BBOX_REST_64_MENSURAL].m_y = 0.0;
        m_bBox[LEIPZIG_BBOX_REST_64_MENSURAL].m_width = 162.0;
        m_bBox[LEIPZIG_BBOX_REST_64_MENSURAL].m_height = 461.0;
        m_bBox[LEIPZIG_BBOX_REST_128_MENSURAL].m_x = 80.0;
        m_bBox[LEIPZIG_BBOX_REST_128_MENSURAL].m_y = 0.0;
        m_bBox[LEIPZIG_BBOX_REST_128_MENSURAL].m_width = 162.0;
        m_bBox[LEIPZIG_BBOX_REST_128_MENSURAL].m_height = 558.0;
        
    };
    
    struct BoundingBox
    {
        double m_x;
        double m_y;
        double m_width;
        double m_height;
    } m_bBox[51];
};

 
#include "app/axapp.h"

static inline double DegToRad(double deg) { return (deg * M_PI) / 180.0; }
static inline double RadToDeg(double deg) { return (deg * 180.0) / M_PI; }

//----------------------------------------------------------------------------
// MusBBoxDC
//----------------------------------------------------------------------------


MusBBoxDC::MusBBoxDC (int width, int height):
    MusDC()
{	
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
    m_objects.Add( object );
}
      
void MusBBoxDC::EndGraphic() 
{
    // remove object
    //m_objects.Remove;
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
        
void MusBBoxDC::DrawCircle(int x, int y, int radius)
{
    DrawEllipse(x - radius, y - radius, 2*radius, 2*radius);
}


void MusBBoxDC::DrawEllipse(int x, int y, int width, int height)
{
    //// Example : 
    //// call UpdateContentBB for all objects
    //// call UpdateOwnBB for top one only
    
    
    //int rh = height / 2;
    //int rw = width  / 2;

    //WriteLine(wxString::Format("<ellipse cx=\"%d\" cy=\"%d\" rx=\"%d\" ry=\"%d\" />", x+rw,y+rh, rw, rh ));
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
    //WriteLine( wxString::Format("<path d=\"M%d %d L%d %d\" style=\"%s\" />", x1,y1,x2,y2, m_penWidth.c_str()) );
}
 
               
void MusBBoxDC::DrawPolygon(int n, MusPoint points[], int xoffset, int yoffset, int fill_style)
{

    wxString s ;
    s = "<polygon style=\"";
    //if ( fillStyle == wxODDEVEN_RULE )
    //    s = s + wxT("fill-rule:evenodd; ");
    //else
    s += "fill-rule:nonzero; ";

    s += "\" points=\"" ;

    for (int i = 0; i < n;  i++)
    {
        s += wxString::Format("%d,%d ", points [i].x+xoffset, points[i].y+yoffset );
        //CalcBoundingBox ( points [i].x+xoffset, points[i].y+yoffset);
    }
    s += wxT("\" /> ") ;
    //WriteLine(s);
}
    
            
void MusBBoxDC::DrawRectangle(int x, int y, int width, int height)
{
    DrawRoundedRectangle( x, y, width, height, 0 );
}


void MusBBoxDC::DrawRoundedRectangle(int x, int y, int width, int height, double radius)
{

    wxString s ;
    
    // negative heights or widths are not allowed in SVG
    if ( height < 0 ) {
        height = -height;
        y -= height;
    }
     if ( width < 0 ) {
        width = -width;
        x -= width;
    }   

    //WriteLine ( wxString::Format(" <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" rx=\"%.2g\" />", x, y, width, height, radius ) );

}

        
void MusBBoxDC::DrawText(const wxString& text, int x, int y)
{
    //DrawRotatedText( text, x, y, 0.0 );
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
    //// Use the MusLeipzigBBox class above
    
    wxString glyph;
    char c = (char)text[0];
    switch (c) {
        /* figures */
        case 48: glyph = "figure_0"; break;
        case 49: glyph = "figure_1"; break;
        case 50: glyph = "figure_2"; break;
        case 51: glyph = "figure_3"; break;
        case 52: glyph = "figure_4"; break;
        case 53: glyph = "figure_5"; break;
        case 54: glyph = "figure_6"; break;
        case 55: glyph = "figure_7"; break;
        case 56: glyph = "figure_8"; break;
        case 57: glyph = "figure_9"; break;
        /* clef */
        case LEIPZIG_CLEF_G: glyph = "clef_G"; break;
        case LEIPZIG_CLEF_F: glyph = "clef_F"; break;
        case LEIPZIG_CLEF_C: glyph = "clef_C"; break;
        case LEIPZIG_CLEF_8va: glyph = "clef_G8"; break;
        case LEIPZIG_CLEF_G + LEIPZIG_OFFSET_MENSURAL: glyph = "clef_G_mensural"; break;
        case LEIPZIG_CLEF_F + LEIPZIG_OFFSET_MENSURAL: glyph = "clef_F_mensural"; break;
        case LEIPZIG_CLEF_C + LEIPZIG_OFFSET_MENSURAL: glyph = "clef_C_mensural"; break;
        case LEIPZIG_CLEF_8va + LEIPZIG_OFFSET_MENSURAL: glyph = "clef_G_chiavette"; break;
        /* alterations */
        case LEIPZIG_ACCID_SHARP: glyph = "alt_sharp"; break;
        case LEIPZIG_ACCID_NATURAL: glyph = "alt_natural"; break;
        case LEIPZIG_ACCID_FLAT: glyph = "alt_flat"; break;
        case LEIPZIG_ACCID_DOUBLE_SHARP: glyph = "alt_double_sharp"; break;
        case LEIPZIG_ACCID_SHARP + LEIPZIG_OFFSET_MENSURAL: glyph = "alt_sharp_mensural"; break;
        case LEIPZIG_ACCID_NATURAL + LEIPZIG_OFFSET_MENSURAL: glyph = "alt_natural_mensural"; break;
        case LEIPZIG_ACCID_FLAT + LEIPZIG_OFFSET_MENSURAL: glyph = "alt_flat_mensural"; break;
        case LEIPZIG_ACCID_DOUBLE_SHARP + LEIPZIG_OFFSET_MENSURAL: glyph = "alt_double_sharp_mensural"; break;
        /* rests */
        case LEIPZIG_REST_QUARTER: glyph = "rest_4"; break;
        case LEIPZIG_REST_QUARTER + 1: glyph = "rest_8"; break;
        case LEIPZIG_REST_QUARTER + 2: glyph = "rest_16"; break;
        case LEIPZIG_REST_QUARTER + 3: glyph = "rest_32"; break;
        case LEIPZIG_REST_QUARTER + 4: glyph = "rest_64"; break;
        case LEIPZIG_REST_QUARTER + 5: glyph = "rest_128"; break;
        case LEIPZIG_REST_QUARTER + LEIPZIG_OFFSET_MENSURAL: glyph = "rest_4_mensural"; break;
        case LEIPZIG_REST_QUARTER + 1 + LEIPZIG_OFFSET_MENSURAL: glyph = "rest_8_mensural"; break;
        case LEIPZIG_REST_QUARTER + 2 + LEIPZIG_OFFSET_MENSURAL: glyph = "rest_16_mensural"; break;
        case LEIPZIG_REST_QUARTER + 3 + LEIPZIG_OFFSET_MENSURAL: glyph = "rest_32_mensural"; break;
        case LEIPZIG_REST_QUARTER + 4 + LEIPZIG_OFFSET_MENSURAL: glyph = "rest_64_mensural"; break;
        case LEIPZIG_REST_QUARTER + 5 + LEIPZIG_OFFSET_MENSURAL: glyph = "rest_128_mensural"; break;
        /* note heads */
        case LEIPZIG_HEAD_WHOLE: glyph = "head_whole"; break;
        case LEIPZIG_HEAD_WHOLE_FILLED: glyph = "head_whole_fill"; break;
        case LEIPZIG_HEAD_HALF: glyph = "head_half"; break;
        case LEIPZIG_HEAD_QUARTER: glyph = "head_quarter"; break;
        case LEIPZIG_HEAD_WHOLE + LEIPZIG_OFFSET_MENSURAL: glyph = "head_whole_diamond"; break;
        case LEIPZIG_HEAD_WHOLE_FILLED + LEIPZIG_OFFSET_MENSURAL: glyph = "head_whole_filldiamond"; break;
        case LEIPZIG_HEAD_HALF + LEIPZIG_OFFSET_MENSURAL: glyph = "head_half_diamond"; break;
        case LEIPZIG_HEAD_QUARTER + LEIPZIG_OFFSET_MENSURAL: glyph = "head_quarter_filldiamond"; break;
        /* slashes */
        case LEIPZIG_STEM_FLAG_UP: glyph = "slash_up"; break;
        case LEIPZIG_STEM_FLAG_DOWN: glyph = "slash_down"; break;
        case LEIPZIG_STEM_FLAG_UP + LEIPZIG_OFFSET_MENSURAL: glyph = "slash_up_mensural"; break;
        case LEIPZIG_STEM_FLAG_DOWN + LEIPZIG_OFFSET_MENSURAL: glyph = "slash_down_mensural"; break;
        /* ornaments */
        case 35: glyph = "orn_mordent"; break;
        /* todo */
        default: glyph = "unknown";
    }
        
    //WriteLine ( wxString::Format("<use xlink:href=\"#%s\" transform=\"translate(%d, %d) scale(%f, %f)\"/>",
		//glyph.c_str(), x, y, ((double)(m_font.GetPointSize() / 2048.0)), ((double)(m_font.GetPointSize() / 2048.0)) ) );
}


void MusBBoxDC::DrawSpline(int n, MusPoint points[])
{
    //m_dc->DrawSpline( n, (wxPoint*)points );
}


