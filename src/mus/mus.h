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


//----------------------------------------------------------------------------
// MusParameters
//----------------------------------------------------------------------------

class MusParameters 
{
public:
    // constructors and destructors
    MusParameters();
    virtual ~MusParameters();
    
        
public:
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
    ///unsigned char Epais1;
    /** epaisseur 2 */
    ///unsigned char Epais2;
    /** epaisseur 3 */
    ///unsigned char Epais3;
    
    
    /* the following variables were originally in MusParameters2 */
    /** rapport entre grande et petite portee, numerateur */
    unsigned char rapportPorteesNum;
    /** rapport entre grande et petite portee, denominateur */
    unsigned char rapportPorteesDen;
    /** rapport entre grand et petit element, numerateur */
    unsigned char rapportDiminNum;
    /** rapport entre grand et petit element, denominateur */
    unsigned char rapportDiminDen;
    /** correction de la position de hampes*/
    signed char hampesCorr;
    /** type d'entete et de pied de page. */
    unsigned int entetePied;
    
    // unused variables
	/** transposition des silences */
    //unsigned char transp_sil; // unused
    /** valeur automatique de silence ???? */
    //unsigned char autoval_sil; // unused
    /** nombre de page contenues dans une page en continu */
    //unsigned char nbPagesEncontinu; // unused
    /** correction vertical pour l'affichage a l'ecran */
    //signed char vertCorrEcr; // unused
    /** correction vertical pour l'impression */
    //signed char vertCorrPrn; // unused
    /** epaisseur des courbes de bezier */
    //signed char epaisBezier; // unused
    /** type d'entete et de pied de page. <p>Le type d'entete est determine par un &amp; logique
     avec les differents types (Faire par ex: entetePied &amp; JwgDef.PAGINATION)
     @see JwgDef#PAGINATION JwgDef,type d'en-entete et de pied de page */
    //unsigned int entetePied; // unused
    /** table de transpositions de voix */
    //signed char transposition[MAXPORTNBRE+1]; // unused
    
private:
    
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
    
        
public:
    int posx[MAXCLE]; /* x-position des clefs */
	int dec[MAXCLE]; /* decalage y qu'elles entrainent*/
	int compte;	/* compteur de reperage horizontal */
	float yp;
    
private:
    
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
    
        
public:
        /** version */
    unsigned short maj_ver; // unused
    /** sous-version */
    unsigned short min_ver; // unused
    /** nom du fichier */
    wxString name;
    /** taille du fichier en bytes */
    // unsigned int filesize; // unused
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
    ///signed char reserve[2];
	//		reserve[0] = epais. trait liaisons
	//		reserve[1] = augm/dimin nbre de passes liaisons

private:
    

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
    
        
public:
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
    
        
public:
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
    
};

#endif

