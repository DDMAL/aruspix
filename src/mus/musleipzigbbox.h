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
#define LEIPZIG_BBOX_FERMATA_UP 11
#define LEIPZIG_BBOX_FERMATA_DOWN 12
#define LEIPZIG_BBOX_METER_SYMB_2_CUT 13
#define LEIPZIG_BBOX_METER_SYMB_CUT 14
#define LEIPZIG_BBOX_HEAD_WHOLE 15
#define LEIPZIG_BBOX_HEAD_WHOLE_FILL 16
#define LEIPZIG_BBOX_HEAD_HALF 17
#define LEIPZIG_BBOX_HEAD_QUARTER 18
#define LEIPZIG_BBOX_SLASH_UP 19
#define LEIPZIG_BBOX_SLASH_DOWN 20
#define LEIPZIG_BBOX_CLEF_G 21
#define LEIPZIG_BBOX_CLEF_F 22
#define LEIPZIG_BBOX_CLEF_C 23
#define LEIPZIG_BBOX_CLEF_G8 24
#define LEIPZIG_BBOX_ALT_SHARP 25
#define LEIPZIG_BBOX_ALT_NATURAL 26
#define LEIPZIG_BBOX_ALT_FLAT 27
#define LEIPZIG_BBOX_ALT_DOUBLE_SHARP 28
#define LEIPZIG_BBOX_REST_4 29
#define LEIPZIG_BBOX_REST_8 30
#define LEIPZIG_BBOX_REST_16 31
#define LEIPZIG_BBOX_REST_32 32
#define LEIPZIG_BBOX_REST_64 33
#define LEIPZIG_BBOX_REST_128 34
#define LEIPZIG_BBOX_HEAD_WHOLE_DIAMOND 35
#define LEIPZIG_BBOX_HEAD_WHOLE_FILLDIAMOND 36
#define LEIPZIG_BBOX_HEAD_HALF_DIAMOND 37
#define LEIPZIG_BBOX_HEAD_QUARTER_FILLDIAMOND 38
#define LEIPZIG_BBOX_SLASH_UP_MENSURAL 39
#define LEIPZIG_BBOX_SLASH_DOWN_MENSURAL 40
#define LEIPZIG_BBOX_CLEF_G_MENSURAL 41
#define LEIPZIG_BBOX_CLEF_F_MENSURAL 42
#define LEIPZIG_BBOX_CLEF_C_MENSURAL 43
#define LEIPZIG_BBOX_CLEF_G_CHIAVETTE 44
#define LEIPZIG_BBOX_ALT_SHARP_MENSURAL 45
#define LEIPZIG_BBOX_ALT_NATURAL_MENSURAL 46
#define LEIPZIG_BBOX_ALT_FLAT_MENSURAL 47
#define LEIPZIG_BBOX_ALT_DOUBLE_SHARP_MENSURAL 48
#define LEIPZIG_BBOX_REST_4_MENSURAL 49
#define LEIPZIG_BBOX_REST_8_MENSURAL 50
#define LEIPZIG_BBOX_REST_16_MENSURAL 51
#define LEIPZIG_BBOX_REST_32_MENSURAL 52
#define LEIPZIG_BBOX_REST_64_MENSURAL 53
#define LEIPZIG_BBOX_REST_128_MENSURAL 54
#define LEIPZIG_BBOX_METER_SYMB_3_CUT 55
#define LEIPZIG_BBOX_METER_SYMB_COMMON 56
#define LEIPZIG_BBOX_OBLIQUE_FIGURE_8 57
#define LEIPZIG_BBOX_OBLIQUE_FIGURE_1 58
#define LEIPZIG_BBOX_OBLIQUE_FIGURE_6 59
#define LEIPZIG_BBOX_OBLIQUE_FIGURE_0 60
#define LEIPZIG_BBOX_OBLIQUE_FIGURE_2 61
#define LEIPZIG_BBOX_OBLIQUE_FIGURE_4 62
#define LEIPZIG_BBOX_OBLIQUE_FIGURE_5 63
#define LEIPZIG_BBOX_OBLIQUE_FIGURE_3 64
#define LEIPZIG_BBOX_OBLIQUE_FIGURE_7 65
#define LEIPZIG_BBOX_OBLIQUE_FIGURE_9 66


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
