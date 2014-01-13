/////////////////////////////////////////////////////////////////////////////
// Name:        musdef.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_DEF_H__
#define __MUS_DEF_H__

//----------------------------------------------------------------------------
// ARUSPIX 2
//----------------------------------------------------------------------------

#include <algorithm>

namespace vrv {

#define is_in(x,a,b) (((x) >= std::min((a),(b))) && ((x) <= std::max((a),(b))))

#define AX_UNSET -0x7FFFFFFF

/**
 * Codes returned by MusFunctors.
 * Default is FUNCTOR_CONTINUE.
 * FUNCTOR_SIBLINGS will no go any deeper in the tree.
 * FUNCTOR_STOP wil stop the tree processing.
 */
enum MusFunctorCode {
    FUNCTOR_CONTINUE = 0,
    FUNCTOR_SIBLINGS,
    FUNCTOR_STOP
};

// Is this score for Mensural, neumes, or CMN
enum MusNotationMode {
    MUS_MENSURAL_MODE = 0,
    MUS_NEUMATIC_MODE,
    MUS_CMN_MODE
};

// Is the app in insert mode or edit (drag) mode
enum MusEditorMode {
	MUS_EDITOR_EDIT,
	MUS_EDITOR_INSERT
};

// SPACING
#define MIN_TIE_HEIGHT 12
#define MIN_TIE_THICKNESS 6

// PITCHES
#define PITCH_C 1
#define PITCH_D 2
#define PITCH_E 3
#define PITCH_F 4
#define PITCH_G 5
#define PITCH_A 6
#define PITCH_B 7
// used for automatic rest positions
#define REST_AUTO 255

// DURATION
#define DUR_LG 0  // longa
#define DUR_BR 1  // brevis
#define DUR_1 2   // whole note
#define DUR_2 3   // ...
#define DUR_4 4
#define DUR_8 5
#define DUR_16 6
#define DUR_32 7
#define DUR_64 8
#define DUR_128 9
#define DUR_256 10
// used for alignement
#define DUR_MAX 1024

// ACCID
#define ACCID_SHARP 1
#define ACCID_FLAT 2
#define ACCID_NATURAL 3
#define ACCID_DOUBLE_SHARP 4
#define ACCID_DOUBLE_FLAT 5
#define ACCID_QUARTER_SHARP 6
#define ACCID_QUARTER_FLAT 7

// SYSTEMS
#define START 1
#define END 2
#define START_END 3

// DIRECTIONS
#define FORWARD 1
#define BACKWARD 0
#define ON 1
#define OFF 0

// BEZIER
#define BEZIER_NB_POINTS 20

// TODO // ax2

// RESTS
#define VALSilSpec 15	/* val indiquant silence sp�cial avec chiffre */ // ???

// pour le flag tetenote
#define LOSANGEVIDE 1
#define OPTIONLIBRE 6
#define SANSQUEUE 7

/* valeurs des attributs de staccato in menu (getcode2()) */
#define STACC 0
#define LOURE 1
#define ACCENT_OBL 2
#define ACCENT_VERT 3
#define BEBUNG 4
#define STAC_AIGU 5
#define ACCENT_OBL_PNT 6
#define ACCENT_VERT_PNT 7


// the maximum is 255 (unsigned char)
enum BarlineType {
    BARLINE_NONE = 0, // None is used to indicated we are actually drawing the staffGrp
    BARLINE_SINGLE,
    BARLINE_END,
    BARLINE_RPTBOTH,
    BARLINE_RPTSTART,
    BARLINE_RPTEND,
    BARLINE_DBL
};

// the maximum is 255 (unsigned char)
enum ClefId {
    SOL2 = 1,
    SOL1,
    FA4,
    FA3,
    UT1,
    UT2,
    UT3,
    UT4,
    SOLva,
    FA5,
    UT5,
    CLEPERC,
    //neumatic clefs
    nC1,
    nC2,
    nC3,
    nC4,
    nF1,
    nF2,
    nF3,
    nF4
};

// the maximum is 255 (unsigned char)
enum StaffGrpSymbol {
    STAFFGRP_NONE = 0,
    STAFFGRP_LINE,
    STAFFGRP_BRACE,
    STAFFGRP_BRACKET
};

// the maximum is 255 (unsigned char)
enum MeterSymb {
    METER_SYMB_NONE = 0,
    METER_SYMB_COMMON,
    METER_SYMB_CUT,
    METER_SYMB_2, // no MEI equivalent
    METER_SYMB_3, // no MEI equivalent
    METER_SYMB_2_CUT, // no MEI equivalent
    METER_SYMB_3_CUT // no MEI equivalent
};

// the maximum is 255 (unsigned char)
enum MensurSign {
    MENSUR_SIGN_NONE = 0,
    MENSUR_SIGN_C,
    MENSUR_SIGN_O
};

// the maximum is 255 (unsigned char)
enum SymbolType {
    SYMBOL_UNDEFINED = 0, // needed for default constructor
    SYMBOL_DOT = 1, 
    SYMBOL_ACCID = 2,
    SYMBOL_CUSTOS = 3,
    SYMBOL_SB = 4,
    SYMBOL_PB = 5
};


//----------------------------------------------------------------------------
// Neumes - Festa Dies font
//----------------------------------------------------------------------------

// NEUMES
#define nQUILISMA '@'		// quilisma
#define nWHITEPUNCT 'B'		// nota excavata (white punctum)
#define nDIAMOND_SMALL 'C'	// Punctum inclinatum parvum 
#define nDIAMOND 'D'		// diamond punctum
#define nCEPHALICUS 'K'		// cephalicus (downward punctum)
#define nPUNCT_UP 'L'		// punctum (upwards)
#define nPES 'M'			// podatus bottom (normal)
#define nAPOSTROPHA 'X'		// apostropha
#define nPODATUS_EP 'Y'		// podatus bottom (epiphonus)
#define nNATURAL 'b'		// natural sign
#define nB_FLAT 'c'			// b flat
#define nPUNCTUM 'd'		// square punctum
#define nLIQUES_UP 'e'		// liquescent (upward)
#define nLIQUES_DOWN 'f'	// liquescent (downward)
#define nPORRECT_1 'j'		// one step down porrectus
#define nPORRECT_2 'k'		// two steps down porrectus
#define nPORRECT_3 'l'		// three steps down porrectus
#define nPORRECT_4 'm'		// four steps down porrectus
#define nNEXT_PITCH 't'		// end-of-line next pitch marker (custos)
#define nC_CLEF 'y'         // C clef
#define nF_CLEF 'z'			// F clef
#define nCOMMA ';'			// comma division
#define nDIV_MINOR ')'		// half barline
#define nDIV_FINAL '('		// double barline
#define nDIV_SMALL '0'		// small barline
#define nDIV_MAJOR '&'		// full barline
#define nDOT 'g'			// dot
//
#define nVIRGA 5	// convenience - for use with SetValue(int) 

// festa dies stems
enum NeumeStem {
	nSTEM_T_LEFT = '5',
	nSTEM_B_LEFT = '3',
	nSTEM_T_RIGHT = '+',
	nSTEM_B_RIGHT = '#'
};

// MEI-related defs
#define INEUME 0 // 'broken' neume (climacus or scandicus)
#define UNEUME 1 // 'unbroken' neume (clivis, pes, etc)

// forms
#define QUIL 1 // "quilismatic"
#define RHOM 2 // "rhombic"
#define LIQ1 3 // "liquescent1"

// for neume_line
#define LEFT_STEM 0
#define RIGHT_STEM 1

// x spacings
#define PUNCT_PADDING 25 // space between punctums in open editing mode
#define PUNCT_WIDTH 10 // 'guesstimation' of punctum width -- this number is fairly arbitrary
#define CLIVIS_X_SAME 15	// spacing between punctums of the same pitch in a clivis
#define CLIVIS_X_DIFF 9	// distance is tighter between altering pitches in clivis


//----------------------------------------------------------------------------
// Music - Leipzig font
//----------------------------------------------------------------------------

#define LEIPZIG_OFFSET_IN_FONT 200
#define LEIPZIG_OFFSET_MENSURAL 20
#define LEIPZIG_OFFSET_NOTE_HEAD 249
//
#define LEIPZIG_FERMATA_UP 63
#define LEIPZIG_FERMATA_DOWN 64
//
#define LEIPZIG_EMB_TRILL 116
//
#define LEIPZIG_HEAD_WHOLE 201
#define LEIPZIG_HEAD_WHOLE_FILLED 202
#define LEIPZIG_HEAD_HALF 203
#define LEIPZIG_HEAD_QUARTER 204
#define LEIPZIG_STEM_FLAG_UP 205
#define LEIPZIG_STEM_FLAG_DOWN 206
//
#define LEIPZIG_CLEF_PERC 152
#define LEIPZIG_CLEF_G 207
#define LEIPZIG_CLEF_F 208
#define LEIPZIG_CLEF_C 209
#define LEIPZIG_CLEF_8va 210
//
#define LEIPZIG_ACCID_SHARP 211
#define LEIPZIG_ACCID_NATURAL 212
#define LEIPZIG_ACCID_FLAT 213
#define LEIPZIG_ACCID_DOUBLE_SHARP 214
#define LEIPZIG_ACCID_QUARTER_FLAT 246
#define LEIPZIG_ACCID_QUARTER_SHARP 244
//
#define LEIPZIG_REST_QUARTER 215
//
#define LEIPZIG_METER_SYMB_COMMON 140
#define LEIPZIG_METER_SYMB_CUT 183 // was 129, works only with font 4.8
#define LEIPZIG_METER_SYMB_2_CUT 127
#define LEIPZIG_METER_SYMB_3_CUT 249    // was 128
#define LEIPZIG_METER_SYMB_2 '2'
#define LEIPZIG_METER_SYMB_3 '3'
// TODO
#define sSTACC 0
#define sLOURE 'k'
#define sACCENT_OBL '>'
#define sACCENT_VERT_SUP '<'
#define sACCENT_VERT_INF ','
#define sBEBUNG 'k'
#define sSTAC_AIGU_SUP ':'
#define sSTAC_AIGU_INF ';'

/*
 The following values have been obtain using FreeType2
 See ./varia/glyph_info.c
 For now, hardcoded values are ok because we do not change the music/neumes fonts
 
 If we want to enable this (changing font at runtime) we would need to
 add a method similar to glyph_info.c to get the glyph size for the note head (and others)
 */

#define LEIPZIG_UNITS_PER_EM 2048.0
#define LEIPZIG_ASCENT 1183.0
#define LEIPZIG_WHOLE_NOTE_HEAD_HEIGHT 266.0
#define LEIPZIG_WHOLE_NOTE_HEAD_WIDTH 405.0
#define LEIPZIG_HALF_NOTE_HEAD_WIDTH 314.0
#define LEIPZIG_SHARP_WIDTH 197.0

} // namespace vrv

#endif // __MUS_DEF_H__