
#ifndef NEUMEDEF_H
#define NEUMEDEF_H

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
#define nNEXT_PITCH 't'		// end-of-line next pitch marker

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

#endif // NEUMEDEF_H
