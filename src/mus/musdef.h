/* WGDEF.H */
/* COMMENTAIRES IMPORTANTS. MODIFICATIONS PAR RAPPORT AUX FICHIERS DE
VERSION DOS.*/

/* TRUE if x is in the closed interval [a,b] */
/***
#define in(x,a,b) (((x) >= min((a),(b))) && ((x) <= max((a),(b)))) 
#define toZoom(x) (((float)(x)*zoomNum)/zoomDen)
#define toReel(x) (((float)(x)*zoomDen)/zoomNum)
***/

#define CMP_MATCH 1
#define CMP_DEL 2
#define CMP_SUBST 3
#define CMP_INS 4


#define EOF (-1)
#define LIGNE 300	// 16bit: 180

#define NOTE 0	/* pour type (bit) de struct element */
#define SYMB 1

#define _NOT 0	/* pour bitflag silence/note de struct note */
#define _SIL 1

#define BARRE 0	/* pour flag (3 bits) de struct symbole */
#define ALTER 1
#define PNT	2
#define LIAI 3
#define CHAINE 4
#define IND_MES 5
#define CLE 6
#define VECT 7
#define BEZIER 8
#define DYNAMIC 9
#define CROCHET 10
#define VALBARRES 11
#define METAFILE_W 12
#define AX_VARIANT 13
// #define AX_LYRICS 14 // It is not used, is it? Added an AX_ prefix to avoid confusion with wolfgang #defines


///#define PS_MODECOURANT 2	// A METTRE DANS UNE VARIABLE d'environnement!!!
	// valeur de discontinu pour traitill‚s

/* pour qualifier differents genres de CHAINE (texte), et permettre des
 * operations */

/* groupe stocke in fonte, 3 bits in structure symbole */
#define DYN 0	/* indication pp, ff, mf etc. */
#define LYRIC 1	/* paroles de chant: plusieurs couches possibles */
#define INSTRUM 2	/* nom, identification de l'instr. */
#define MARQ_REPERE 3	/* reperes pour orchestre, etc. */
#define SYMB_ORN 4	/* trilles, etc. */
#define TEMPO 5
#define TITRE_DIV 6

/* v. 5.5 - groupe stocke in calte, 3 bits in structure symbole */
#define SOUSCATEG_1 0
#define SOUSCATEG_2 1
#define SOUSCATEG_3 2
#define SOUSCATEG_4 3
#define SOUSCATEG_5 4
#define SOUSCATEG_6 5
#define SOUSCATEG_7 6

/* groupe stocke in calte, 3 bits in structure symbole, si MARQ_REPERE */
#define SILENCESPECIAL 0	// jusqu'… v. 5.3, VARIA
#define NUMMES 1
#define CHIFFRAGE 2
#define MARQUEGLOBALE 3
#define VARIA 4

/* pour analyse de var. _param2.entetePied, et lecture/ecriture eventuelle 
   a la fin du fichier (offset: mx_byte_off + sizeof(sep)). Faire par ex: 
   entetePied & ENTETE */

#define PAGINATION 1
#define ENTETE 2
#define PIEDDEPAGE 4
#define MARQUEDECOUPE 8
#define MASQUEFIXE 16
#define MASQUEVARIABLE 32
#define FILEINFO	64

#define MAXFILEINFO	2048	// 16bit: 512
//extern LPSTR ptFileInfo;


/* Definitions relatives aux valeurs */

#define LG 0	/* longa */
#define BR 1	/* brevis */
#define RD 2	/* ronde etc. */
#define BL 3
#define NR 4
#define CR 5
#define DC 6
#define TC 7
#define QC 8
#define Q5C 9	/* quintuple croche */
#define Q6C 10	/* quintuple croche */
#define VALMIN 10	/* plus petite valeur admise */
#define CUSTOS 11
#define VALSilSpec 15	/* val indiquant silence sp‚cial avec chiffre */

/* pointages */
#define POINTAGE 1
#define D_POINTAGE 2


/* clefs */
#define SOL2 1
#define SOL1 2
#define FA4 3
#define FA3 4
#define UT1 5
#define UT2 6
#define UT3 7
#define UT4 8
#define SOLva 9
#define FA5 10
#define UT5 11
#define CLEPERC 12

/* F1...F10-> clavier musical. Dans le fichier ne sont codees que les
   valeurs F2 -- F8 (=1-7). F9 devient F1 a l'octave sup. */
#define F1 0
#define F2 1
#define F3 2
#define F4 3
#define F5 4
#define F6 5
#define F7 6
#define F8 7
#define F9 8
#define F10 9


#define DIESE 1
#define BEMOL 2
#define BECAR 3
#define D_DIESE 4
#define D_BEMOL 5
#define Q_DIESE 6
#define Q_BEMOL 7

// equivalence version DOS
#define CLICK_GAUCHE WM_LBUTTONDOWN
#define CLICK_DROITE WM_RBUTTONDOWN
#define D_CLICK_DROITE WM_RBUTTONDBLCLK
#define D_CLICK_GAUCHE WM_LBUTTONDBLCLK
//#define GAUCHE_ENFONCE

#define CTRL_PgUp 206	/* voir CTRL_X */
#define CTRL_LEFT 207	/* voir INS */
#define CTRL_RIGHT 178
#define CTRL_PgDn 179
#define CTRL_HOME 180
#define CTRL_UP 181
#define CTRL_DOWN 182

// centrage du curseur d'octave 
#define LF 10

// commandes des grosseurs de caracteres en mode texte 
#define CTRL_K 11
#define CTRL_L 12

// interrupt or mode exit key (clears buffer+macros); exit mode texte 
#define ESC 27

// used to delete characters in getline(),getnum()...; r‚glage fin curseur 
#define BACK_S 8
#define CTRL_BS 127
#define SP 32

// used to terminate an entry definition; exit mode mesure 
#define RETURN 13


/* Pour le regleur */
#define ECARTS_2 7
#define AJOUTSYS 10
#define AJOUTACC 11
#define ACCOL 1
#define ACCOL2 5
#define BARGROUP 0


//#define MAXECART 63	//	valeur sur 6 bits pour ecarts entre portees
#define MAXECART 1024	// valeur sur 10 bits pour ecarts entre portees 
#define MAXPORTNBRE 127      // nombre max de portees
#define NODERNIEREPORTEE	127
// ATTENTION!!! Changer aussi cette valeur MAXPORTNBRE in wgmidi.h


//#define MAXECART 63	/* valeur sur 6 bits pour ecarts entre portees */
//#define MAXPORTNBRE 63       /* nombre max de portees */

#define MAXCLE 30	// nombre de clefs possible sur une ligne (attention encontinu) 16bit: 15
#define MAXUNDO	10	// nombre max de retours Undo


/* pour traitement des bitfields de type 3 et du ruler etc... */
#define DEBUT 1
#define FIN 2
#define DEB_FIN 3
#define ECARTS 0
#define INDENT 1
#define VERTBAR 2
#define BRACE 3
#define BRACE2 5
#define PORT_TYP 4
#define P_TAILLE 6

#define LIGNE5	0
#define PERC	1
#define LIGNE4	2

#define OCTBIT 4	/* decalage en 3 bits de code in struct et val -4+3 */

/* Arguments bidons pour fonctions capricieuses et chiantes */
//extern float ffdum;
//extern double dddum;
#define fDUM ffdum
#define dDUM dddum
#define iDUM 0

#define UP 1
#define DOWN 2
#define RIGHT 3
#define LEFT 4
#define HAUT 5
#define BAS  6
#define GAUCHE 7
#define DROITE 8
#define TROUVE 2222


#define AV 1	/* "avant", pour fonctions check() */
#define AR 0	/* "arriere", id */
#define ON 1
#define OFF 0


#define DEC_OCT (oct*_octave[pTaille])		/* hauteur fonction oct courante */
#define TUE_HCUR //delhcur()
#define TUE_TCUR //deltcur()
//#define TUE_RECT delbox(hdc)
#define TUE_RECT xor_rect(hdc, 0,0,0,0)

//#define SECURSIZE 10	// taille du buffer de securite pour passage a la ligne

#define COULEURFANTOME 4	// valeur dans table couleurPolyph[] pour fantome


// FICHIER.H

#define MAJ_VERS_FICH	2
#define MIN_VERS_FICH	1	// 16 bits: 1.8 et 1.9 (identiques)


#define MAXPOLICES 32	// nombre max de polices dans un fichier (header)
#define MAXPOLICENAME	32


#define HEADER_RESERVE 600	// reserve pour header: 100 bytes - OLD 200. ATTENTION: la struct param2 est deduite de cette valeur

#define POLICEOFFS (sizeof(fil_header) + sizeof(struct midipar) + HEADER_RESERVE)
#define FILOFFS (sizeof(fil_header) + sizeof(struct midipar) + HEADER_RESERVE + (MAXPOLICES*sizeof(Police)) + sizeof(index))
#define INDOFFS (sizeof(fil_header) + sizeof(struct midipar) + HEADER_RESERVE + (MAXPOLICES*sizeof(Police)))
		// offset initial de l'index dans fichier: apres le header 

//#define POSDEMOVAR (sizeof(fil_header) + sizeof(struct midipar) + (sizeof(_param2)+17))

#define SZIDX 255		// dimension de l'index (nbre de pages possibles) 
#define AUTOBUS 1500	// taille max du buf diskautobus 

#define OffPGinECR (sizeof (sep))

#define LEGROSBUF 130400		// taille max d'une page sous forme condensee in disk





/*** WGSYMB.H ***/
/* numero d'ordre des symboles dans les fontes - Inclure in:
	note.c, cle.c, input.c, main.c, symb2.c, symb.c
*/

#define OFFSETNOTinFONT 200

#define N_FONT 20	/* nbr de caract. par groupe */
#define DoubleN_FONT 40	/* double du nbr de caract. par groupe */
#define OffsetTETE_NOTE 249	/* offset des tetes in AA Ascent */
#define FONTE_CARMUS 0
#define MAX_NBR_FONTE 8

// pour le flag tetenote
#define LOSANGEVIDE 1
#define OPTIONLIBRE 6
#define SANSQUEUE 7


#define sRONDE_B 201
#define sRONDE_N 202
#define sBLANCHE 203
#define sNOIRE 204
#define sCROCHET_H 205
#define sCROCHET_B 206

#define sPERC 152	// traiter comme sol2
#define sSOL 207
#define sFA 208
#define sUT 209
#define sSOL_OCT 210

#define sDIESE 211
#define sBECARRE 212
#define sBEMOL 213
#define sDDIESE 214
#define sQBEMOL 246
#define sQDIESE 244

#define sSilNOIRE 215	// OffsetNoteInFont + 15 ...

#define MES_CSimple 140
#define MES_CBar 129
#define MES_2 127
#define MES_3 128
#define MES_2Simple '2'
#define MES_3Simple '3'

/* valeurs des attributs de staccato in menu (getcode2()) */
#define STACC 0
#define LOURE 1
#define ACCENT_OBL 2
#define ACCENT_VERT 3
#define BEBUNG 4
#define STAC_AIGU 5
#define ACCENT_OBL_PNT 6
#define ACCENT_VERT_PNT 7

/* positions des symboles correspondants in fonte speciale */
#define sSTACC 0
#define sLOURE 'k'
#define sACCENT_OBL '>'
#define sACCENT_VERT_SUP '<'
#define sACCENT_VERT_INF ','
#define sBEBUNG 'k'
#define sSTAC_AIGU_SUP ':'
#define sSTAC_AIGU_INF ';'

// pour input
#define ORNEM_On 'M'
#define ORNEM_Off 'm'
#define DIESEid 'd'
#define BEMOLid 'b'
#define BECARid 'h'
#define DDIESEid 'D'
#define DBEMOLid 'B'
#define QDIESEid 'U'
#define QBEMOLid 'u'
#define ECHO 's'
#define PAUSES 'S'
#define GOPORTEE 'P'
#define GETPORT_CHABLON '\\'
#define SILENCE_SPECIAL 'j'
#define MESUREPRECEDENTE 'e'
#define MESURESUIVANTE 'r'

// autres lettres employ‚es: 'w' pointil, 'k', midiONOFF

#define MULTFACT	10	// facteur d'augmentation du facteur d'ecart des textes

// pour les numeros d'ordre (ID) des boutons

#define B_UNDO		0
#define B_UNDORETOUR	1
#define B_EDIT		2
#define B_INSERE	3
#define B_GRP		4
#define B_ORN		5
#define B_ACC		6
#define B_INV		7
#define B_LIG		8
#define B_STAC		9
#define B_POINTIL	10
#define B_CONTEMP	11
#define B_LIAI		12
#define B_LSNHB		13
#define B_REL		14
#define B_RELPLUS	15
#define B_SHPORT1	16
#define B_SHPORT2	17
#define B_AUTO		18
#define B_QUEUE1	19
#define B_QUEUE0	20
#define B_VAL		21
#define B_VALREF	22



/*** dans wgmidi.h ***/
#define MAXMIDICANAL 16

/*** define windows ***/
#define WIN_MAX_FNAME 256
#define WIN_MAX_EXT 256

/*** ajout ***/
#define HEADER_FOOTER_TEXT 100


//#ifndef __WXMSW__
	#define min(X,Y) ((X) < (Y) ? (X) : (Y))
	#define max(X,Y) ((X) > (Y) ? (X) : (Y))
//#endif

#define in(x,a,b) (((x) >= min((a),(b))) && ((x) <= max((a),(b)))) 


#define PTCONTROL 20


