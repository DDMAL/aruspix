/*
 *  neumedef.h
 *  aruspix
 *
 *  Created by Chris Niven on 10/06/10.
 *	
 */

/* Festa Dies A */

#define nQUILISMA 64		// '@' - quilisma
#define nWHITEPUNCT 66		// 'B' - nota excavata (white punctum)
#define nDIAMOND_SMALL 67	// 'C' - Punctum inclinatum parvum 
#define nDIAMOND 68			// 'D' - diamond punctum
#define nCEPHALICUS 75		// 'K' - cephalicus (downward punctum)
#define nPUNCT_UP 76		// 'L' - punctum (upwards)
#define nPES 77				// 'M' - podatus bottom (normal)
#define nAPOSTROPHA 88		// 'X' - apostropha
#define nPODATUS_EP 89		// 'Y' - podatus bottom (epiphonus)
#define nNATURAL 98			// 'b' - natural sign
#define nB_FLAT 99			// 'c' - b flat
#define nPUNCTUM 100		// 'd' - square punctum
#define nLIQUES_UP 101		// 'e' - liquescent (upward)
#define nLIQUES_DOWN 102	// 'f' - liquescent (downward)
#define nPORRECT_1 106		// 'j' - one step down porrectus
#define nPORRECT_2 107		// 'k' - two steps down porrectus
#define nPORRECT_3 108		// 'l' - three steps down porrectus
#define nPORRECT_4 109		// 'm' - four steps down porrectus
#define nNEXT_PITCH 116		// 't' - end-of-line next pitch marker


#define nVIRGA 5	// convenience - for use with SetValue(int) 

// festa dies stems
//TODO: should put these in an enum
#define nSTEM_T_LEFT	53	// '5'
#define nSTEM_B_LEFT	51	// '3'
#define nSTEM_T_RIGHT	43	// '+' 
#define nSTEM_B_RIGHT	35	// '#'
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

// names

//#define PUNCT 0
//#define VIRGA 1
//#define PRECT 2
//#define PODAT 3
//#define CLVIS 4

// x spacings

#define PUNCT_PADDING 25 // space between punctums in open editing mode
#define PUNCT_WIDTH 10 // 'guesstimation' of punctum width -- this number is fairly arbitrary
#define CLIVIS_X_SAME 15	// spacing between punctums of the same pitch in a clivis
#define CLIVIS_X_DIFF 9	// distance is tighter between altering pitches in clivis

