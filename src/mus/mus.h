/////////////////////////////////////////////////////////////////////////////
// Name:        mus.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_H__
#define __MUS_H__

#ifdef __GNUG__
    #pragma interface "mus.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "musdef.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// MusFont
//----------------------------------------------------------------------------

class MusFont
{
public:
    // constructors and destructors
    MusFont();
    virtual ~MusFont();
    
    // WDR: method declarations for MusFont
    
public:
    // WDR: member variable declarations for MusFont
	signed char fonteJeu;
	wxString fonteNom;
    
private:
    // WDR: handler declarations for MusFont

};

//----------------------------------------------------------------------------
// MusParametersMidi
//----------------------------------------------------------------------------

class MusParametersMidi
{
public:
    // constructors and destructors
    MusParametersMidi();
    virtual ~MusParametersMidi();
    
    // WDR: method declarations for MusParametersMidi
    
public:
    // WDR: member variable declarations for MusParametersMidi
    /** tempo */
    long tempo;
    /** ???? */
    unsigned char minVeloc;
    /** ???? */
    unsigned char maxVeloc;
    /** style legato de jeu. <pre>0 = aucun<br>1 = beam<br>2 = liaison</pre> */
    unsigned char collerBeamLiai;
    /** valeur de la pedale. <pre>1 = silences<br>2 = pedale</pre> */
    unsigned char pedale;
    /** tolerance horizontale x max pour la simultaneite */
    unsigned char xResolution1;
    /** tolerance horizontale x max en alignement verticale pour la simultaneite */
    unsigned char xResolution2;
    /** traiter les notes ornementales comme des appogiatures */
    unsigned char appogg;
    /** traiter les mesures commes de proportions temporelles */
    unsigned char mes_proportion;
    /** table des numeros de canal pour chaque instrument */
	unsigned char canal2patch[MAXMIDICANAL];	// contient le no d'instr pour chaque canal
	unsigned char volume[MAXMIDICANAL];	// 64, volume, entre 0 et 128
	unsigned char piste2canal[MAXPORTNBRE+1];
    
private:
    // WDR: handler declarations for MusParametersMidi

};

//----------------------------------------------------------------------------
// MusParameters2
//----------------------------------------------------------------------------

class MusParameters2
{
public:
    // constructors and destructors
    MusParameters2();
    virtual ~MusParameters2();
    
    // WDR: method declarations for MusParameters2
    
public:
    // WDR: member variable declarations for MusParameters2
	/** transposition des silences */
    unsigned char transp_sil;
    /** rapport entre grande et petite portee, numerateur */
    unsigned char rapportPorteesNum;
    /** rapport entre grande et petite portee, denominateur */
    unsigned char rapportPorteesDen;
    /** rapport entre grand et petit element, numerateur */
    unsigned char rapportDiminNum;
    /** rapport entre grand et petit element, denominateur */
    unsigned char rapportDiminDen;
    /** valeur automatique de silence ???? */
    unsigned char autoval_sil;
    /** nombre de page contenues dans une page en continu */
    unsigned char nbPagesEncontinu;
    /** correction vertical pour l'affichage a l'ecran */
    signed char vertCorrEcr;
    /** correction vertical pour l'impression */
    signed char vertCorrPrn;
    /** correction de la position de hampes*/
    signed char hampesCorr;
    /** epaisseur des courbes de bezier */
    signed char epaisBezier;
    /** type d'entete et de pied de page. <p>Le type d'entete est determine par un &amp; logique
     avec les differents types (Faire par ex: entetePied &amp; JwgDef.PAGINATION)
     @see JwgDef#PAGINATION JwgDef,type d'en-entete et de pied de page */
    unsigned int entetePied;
    /** table de transpositions de voix */
    signed char transposition[MAXPORTNBRE+1];
    
private:
    // WDR: handler declarations for MusParameters2

};

//----------------------------------------------------------------------------
// MusParameters
//----------------------------------------------------------------------------

class MusParameters 
{
public:
    // constructors and destructors
    MusParameters();
    virtual ~MusParameters();
    
    // WDR: method declarations for MusParameters
    
public:
    // WDR: member variable declarations for MusParameters
    /** orientation de la page */
    char orientation;
    /** epaisseur des lignes de portees */
    unsigned char EpLignesPortee;
    /** epaisseur des hampes */
    unsigned char EpQueueNote;
    /** epaisseur des barres de mesures */
    unsigned char EpBarreMesure;
    /** epaisseur des barres de valeur */
    unsigned char EpBarreValeur;
    /** epaisseur de l'espace entre les barres de valeur */
    unsigned char EpBlancBarreValeur;
    /** pente maximale des barres de valeur */
    unsigned char beamPenteMax;
    /** pente minimale des barres de valeur */
    unsigned char beamPenteMin;
    /** largueur de la page */
    int pageFormatHor;
    /** hauteur de la page */
    int pageFormatVer;
    /** marge au sommet */
    short MargeSOMMET;
    /** marge gauche sur la pages impaires */
    short MargeGAUCHEIMPAIRE;
    /** marge droite sur les pages paires */
    short MargeGAUCHEPAIRE;
    /** epaisseur 1 */
    unsigned char Epais1;
    /** epaisseur 2 */
    unsigned char Epais2;
    /** epaisseur 3 */
    unsigned char Epais3;
    
private:
    // WDR: handler declarations for MusParameters

};

//----------------------------------------------------------------------------
// MusPosKey
//----------------------------------------------------------------------------

class MusPosKey
{
public:
    // constructors and destructors
    MusPosKey();
    virtual ~MusPosKey();
    
    // WDR: method declarations for MusPosKey
    
public:
    // WDR: member variable declarations for MusPosKey
	int posx[MAXCLE]; /* x-position des clefs */
	int dec[MAXCLE]; /* decalage y qu'elles entrainent*/
	int compte;	/* compteur de reperage horizontal */
	float yp;
    
private:
    // WDR: handler declarations for MusPosKey

};

//----------------------------------------------------------------------------
// MusFileHeader
//----------------------------------------------------------------------------

class MusFileHeader
{
public:
    // constructors and destructors
    MusFileHeader();
    virtual ~MusFileHeader();
    
    // WDR: method declarations for MusFileHeader
    
public:
    // WDR: member variable declarations for MusFileHeader
    /** version */
    unsigned short maj_ver;
    /** sous-version */
    unsigned short min_ver;
    /** nom du fichier */
    wxString name;
    /** taille du fichier en bytes */
    unsigned int filesize;
    /** nombre de pages */
    unsigned short nbpage;
    /** numero de page courante */
    unsigned short nopage;
    /** numero de ligne courante */
    unsigned short noligne;
    /** position x courante */
    unsigned int xpos;
    /** Parametres du Header */
    MusParameters param;
    /** reserve */
    signed char reserve[2];
	//		reserve[0] = epais. trait liaisons
	//		reserve[1] = augm/dimin nbre de passes liaisons

private:
    // WDR: handler declarations for MusFileHeader


};

//----------------------------------------------------------------------------
// MusPagination
//----------------------------------------------------------------------------

class MusPagination
{
public:
    // constructors and destructors
    MusPagination();
    virtual ~MusPagination();
    
    // WDR: method declarations for MusPagination
    
public:
    // WDR: member variable declarations for MusPagination
	/** premier numero */
	short numeroInitial;	
	char aussiPremierPage;
	/** 0, pas de pagination; sinon :**/
	/* Code haut/bas, centrage, etc. comme suit:
			HAUT: 1-49
			BAS: 51-100
			Position horizontale:
			CENTRE: 1 (haut) ou 51 (bas)
			ou:
				sur chaque page: GAUCHE/DROITE 2/3 (52/53)
				sur deux pages: EXTERIEUR/INTERIEUR 12/13	(62/63)
	*/
	char position;	
	/** 0-32 **/
	char numeroFonte;

	char carStyle;
	/** grandeur de la police **/
	char taille;
	/** distance en interlignes du sommet/base de feuille **/
	char offsetDuBord;	

private:
    // WDR: handler declarations for MusPagination

};


//----------------------------------------------------------------------------
// MusHeaderFooter
//----------------------------------------------------------------------------

class MusHeaderFooter
{
public:
    // constructors and destructors
    MusHeaderFooter();
    virtual ~MusHeaderFooter();
    
    // WDR: method declarations for MusHeaderFooter
    
public:
    // WDR: member variable declarations for MusHeaderFooter **/
	wxString texte;	
	// premier num‚ro: 16 bits, 100 BYTES **/
	char aussiPremierPage;
	/* Code haut/bas, centrage, etc. comme suit:
			HAUT: 1-49
			BAS: 51-100
			Position horizontale:
				CENTRE: 1 (haut) ou 51 (bas)
			ou:
				sur chaque page: GAUCHE/DROITE 2/3 (52/53)
				sur deux pages: EXTERIEUR/INTERIEUR 12/13	(62/63)
	*/
	char position;
	/** 0-32 **/	
	char numeroFonte;
	/** 0, norm; 1, ital; 2, gras; 3, ital. gras **/
	char carStyle;
	/** 0, norm; 1, ital; 2, gras; 3, ital. gras **/
	char taille;
	/** distance en interlignes du sommet/base de feuille **/
	char offsetDuBord;
    
private:
    // WDR: handler declarations for MusHeaderFooter

};

#endif

