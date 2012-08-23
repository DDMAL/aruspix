//
//  musleipzigbbox.h
//  aruspix
//
//  Created by Rodolfo Zitellini on 30/07/12.
//  Copyright (c) 2012 com.aruspix.www. All rights reserved.
//

#ifndef aruspix_musleipzigbbox_h
#define aruspix_musleipzigbbox_h

#include "musobject.h"

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
#define LEIPZIG_BBOX_METER_SYMB_2_CUT 11
#define LEIPZIG_BBOX_METER_SYMB_CUT 12
#define LEIPZIG_BBOX_HEAD_WHOLE 13
#define LEIPZIG_BBOX_HEAD_WHOLE_FILL 14
#define LEIPZIG_BBOX_HEAD_HALF 15
#define LEIPZIG_BBOX_HEAD_QUARTER 16
#define LEIPZIG_BBOX_SLASH_UP 17
#define LEIPZIG_BBOX_SLASH_DOWN 18
#define LEIPZIG_BBOX_CLEF_G 19
#define LEIPZIG_BBOX_CLEF_F 20
#define LEIPZIG_BBOX_CLEF_C 21
#define LEIPZIG_BBOX_CLEF_G8 22
#define LEIPZIG_BBOX_ALT_SHARP 23
#define LEIPZIG_BBOX_ALT_NATURAL 24
#define LEIPZIG_BBOX_ALT_FLAT 25
#define LEIPZIG_BBOX_ALT_DOUBLE_SHARP 26
#define LEIPZIG_BBOX_REST_4 27
#define LEIPZIG_BBOX_REST_8 28
#define LEIPZIG_BBOX_REST_16 29
#define LEIPZIG_BBOX_REST_32 30
#define LEIPZIG_BBOX_REST_64 31
#define LEIPZIG_BBOX_REST_128 32
#define LEIPZIG_BBOX_HEAD_WHOLE_DIAMOND 33
#define LEIPZIG_BBOX_HEAD_WHOLE_FILLDIAMOND 34
#define LEIPZIG_BBOX_HEAD_HALF_DIAMOND 35
#define LEIPZIG_BBOX_HEAD_QUARTER_FILLDIAMOND 36
#define LEIPZIG_BBOX_SLASH_UP_MENSURAL 37
#define LEIPZIG_BBOX_SLASH_DOWN_MENSURAL 38
#define LEIPZIG_BBOX_CLEF_G_MENSURAL 39
#define LEIPZIG_BBOX_CLEF_F_MENSURAL 40
#define LEIPZIG_BBOX_CLEF_C_MENSURAL 41
#define LEIPZIG_BBOX_CLEF_G_CHIAVETTE 42
#define LEIPZIG_BBOX_ALT_SHARP_MENSURAL 43
#define LEIPZIG_BBOX_ALT_NATURAL_MENSURAL 44
#define LEIPZIG_BBOX_ALT_FLAT_MENSURAL 45
#define LEIPZIG_BBOX_ALT_DOUBLE_SHARP_MENSURAL 46
#define LEIPZIG_BBOX_REST_4_MENSURAL 47
#define LEIPZIG_BBOX_REST_8_MENSURAL 48
#define LEIPZIG_BBOX_REST_16_MENSURAL 49
#define LEIPZIG_BBOX_REST_32_MENSURAL 50
#define LEIPZIG_BBOX_REST_64_MENSURAL 51
#define LEIPZIG_BBOX_REST_128_MENSURAL 52
#define LEIPZIG_BBOX_METER_SYMB_3_CUT 53
#define LEIPZIG_BBOX_METER_SYMB_COMMON 54
#define LEIPZIG_BBOX_OBLIQUE_FIGURE_8 55
#define LEIPZIG_BBOX_OBLIQUE_FIGURE_1 56
#define LEIPZIG_BBOX_OBLIQUE_FIGURE_6 57
#define LEIPZIG_BBOX_OBLIQUE_FIGURE_0 58
#define LEIPZIG_BBOX_OBLIQUE_FIGURE_2 59
#define LEIPZIG_BBOX_OBLIQUE_FIGURE_4 60
#define LEIPZIG_BBOX_OBLIQUE_FIGURE_5 61
#define LEIPZIG_BBOX_OBLIQUE_FIGURE_3 62
#define LEIPZIG_BBOX_OBLIQUE_FIGURE_7 63
#define LEIPZIG_BBOX_OBLIQUE_FIGURE_9 64


/**
 * This class is used for getting the bounding box of the Leipzig glyphs.
 * The values are used obtained with the ./varia/svg/split.xsl.
 */
class MusLeipzigBBox
{
public:
    
    // Get the bounds of a char in the array
    static void GetCharBounds(const unsigned char c, int *x, int *y, int *w, int *h);
    
    
private:
    typedef struct _BoundingBox
    {
        double m_x;
        double m_y;
        double m_width;
        double m_height;
    } BoundingBox;
    
    static BoundingBox *InitializeStatic();
  
public:

    
private:
    
    static BoundingBox *m_bBox;
    
    static bool m_initialized;
};


#endif
