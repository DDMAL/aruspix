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

// update: 08/18/2010 - changing offset of characters
// removing lines from festa dies and updating characters here

#define nQUILISMA 64	// '@' - quilisma
#define nDIAMOND 68		// 'D' - diamond punctum
#define nPES 77			// 'M' - bottom of podatus
#define nPUNCTUM 100	// 'd' - square punctum
#define nNEXT_PITCH 116 // 't' - end-of-line next pitch marker
#define nPORRECT_1 125	// '}' - one step down porrectus
#define nPORRECT_2 172	// two steps down porrectus
#define nDIAMOND_SMALL 202 //punctum inclinatum parvum (small diamond)
#define nPORRECT_3 217	// 3 steps down porrectus
#define nNATURAL 235	// becuadros (natural sign)
#define nWHITEPUNCT 732 // nota excavata (white punctum)

// should make use of enums for festa dies lines
#define nRIGHTLINE 42	// 
#define nLEFTLINE 52	// 
#define nVIRGA "d" //virga, to be used with festa_string
// MEI-related defs

#define INEUME 0 // 'broken' neume (climacus or scandicus)
#define UNEUME 1 // 'unbroken' neume (clivis, pes, etc)

// forms
#define QUIL 1 // "quilismatic"
#define RHOM 2 // "rhombic"
#define LIQ1 3 // "liquescent1"

// lines to connect punctums for ligature drawing
#define TOP_LEFT 53
#define TOP_RIGHT 222
#define BOTTOM_LEFT 51
#define BOTTOM_RIGHT 8220

// names

#define PUNCT 0
#define VIRGA 1
#define PRECT 2
#define PODAT 3
#define CLVIS 4

#define PUNCT_PADDING 25 // space between punctums in open editing mode
#define PUNCT_WIDTH 5 // 'guesstimation' of punctum width -- this number is fairly arbitrary


