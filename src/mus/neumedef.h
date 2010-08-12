/*
 *  neumedef.h
 *  aruspix
 *
 *  Created by Chris Niven on 10/06/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

/* Festa Dies A */

// individual punctum characters from festa dies

#define nPODATUS "\'" // podatus (epiphonus)
#define nBEMOLE 46	// b flat
#define nVIRGULA 59 // comma
#define nCEPHALICUS "L" 
#define nPES "M"		// bottom of podatus
#define nDIAMOND "S"
#define nAPOSTROPHE 84
#define nQUILISMA "]" // squiggly
#define nPORRECT_4 112 // 4 steps down porrectus
#define nPUNCTUM "s" // square punctum
#define nGUIONE 116 // 'next pitch' indicator 
#define nPORRECT_1 126 // one step down porrectus
#define nPORRECT_2 170 // two steps down porrectus
#define nFCLEF 182 // f clef
#define nCCLEF 186 // c clef
#define nDIAMOND_SMALL 194 //punctum inclinatum parvum (diamond)
#define nPORRECT_3 218 // 3 steps down porrectus
#define nNATURAL 227 // becuadros (natural sign)
#define nWHITEPUNCT 402 // nota excavata (white punctum)
#define nRIGHTLINE 42
#define nLEFTLINE 52
#define nVIRGA "s*" //virga, to be used with festa_string

// MEI-related defs

#define INEUME 0 // 'broken' neume (climacus or scandicus)
#define UNEUME 1 // 'unbroken' neume (clivis, pes, etc)

// forms
#define QUIL 1 // "quilismatic"
#define RHOM 2 // "rhombic"
#define LIQ1 3 // "liquescent1"


#define PUNCT_PADDING 25 // space between punctums in open editing mode
#define PUNCT_WIDTH 5 // 'guesstimation' of punctum width -- this number is fairly arbitrary


