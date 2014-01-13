/////////////////////////////////////////////////////////////////////////////
// Name:        vrv.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_WWG_H__
#define __MUS_WWG_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <string>

/* dans wgmidi.h */
#define MAXMIDICANAL 16

/* portees */
#define MAXECART 1024	// valeur sur 10 bits pour ecarts entre portees 
#define MAXPORTNBRE 127      // nombre max de portees
#define NODERNIEREPORTEE 127


/* nombre max de polices dans un fichier (header) */
#define MAXPOLICES 32
#define MAXPOLICENAME	32

//----------------------------------------------------------------------------
// MusWWGFont
//----------------------------------------------------------------------------

/** 
 * This class contains the font information as represented in Wolfgang WWG files. 
 */

struct MusWWGFontStr {	
    signed char fonteJeu;
    char fonteNom[MAXPOLICENAME];
};
typedef struct MusWWGFontStr MusWWGFont;


//----------------------------------------------------------------------------
// MusWWGData
//----------------------------------------------------------------------------

/** 
 * This class stores data of WWG files that are not used in Aruspix.
 */

class MusWWGData
{
public:
    // constructors and destructors
    MusWWGData();
    virtual ~MusWWGData();
    
        
public:
    // ------ //
    // header //
    std::string h_texte;	
	char h_aussiPremierPage; // premier numéro: 16 bits, 100 BYTES
	char h_position; // see wgdef.h

	char h_numeroFonte; // 0-32
	char h_carStyle; // 0, norm; 1, ital; 2, gras; 3, ital. gras
	char h_taille;
	char h_offsetDuBord; // distance en interlignes du sommet/base de feuille
    
    // -------------------- //
    // footer - idem header //
    std::string f_texte;	
	char f_aussiPremierPage;
	char f_position;	
	char f_numeroFonte;
	char f_carStyle;
	char f_taille;
	char f_offsetDuBord;
    
    // ------------------------ //
    // pagination - idem header //
	short p_numeroInitial;	
	char p_aussiPremierPage; // p for paginatino
	char p_position;	
	char p_numeroFonte;
	char p_carStyle;
	char p_taille;
	char p_offsetDuBord; // distance en interlignes du sommet/base de feuille
    
    // ----------------- //
    // unused parameters
    unsigned char transp_sil; // transposition des silences
    unsigned char autoval_sil; // valeur automatique de silence ????
    unsigned char nbPagesEncontinu; // nombre de page contenues dans une page en continu
    signed char vertCorrEcr; // correction verticale pour l'affichage a l'ecran
    signed char vertCorrPrn; // correction vertical pour l'impression
    signed char epaisBezier; // epaisseur des courbes de bezier
    signed char transposition[MAXPORTNBRE+1]; // table de transpositions de voix
    
    // ----------- //
    // file header //
    unsigned short maj_ver; // version
    unsigned short min_ver; // sous-version
    std::string name; // nom du fichier 
    unsigned int filesize; // taille du fichier en bytes
    unsigned short nbpage; // nombre de pages
    unsigned short nopage; // numero de page courante
    unsigned short noligne; // numero de ligne courante
    unsigned int xpos; // position x courante
    unsigned char Epais1; // epaisseur 1
    unsigned char Epais2; // epaisseur 2
    unsigned char Epais3; // epaisseur 3
    signed char reserve[2];
    
    // --------------- //
    // midi parameters //
    long tempo; // tempo
    unsigned char minVeloc; // ??
    unsigned char maxVeloc; // ??
    unsigned char collerBeamLiai; // style legato de jeu. 0 = aucun1 = beam2 = liaison
    unsigned char pedale; // valeur de la pedale. 1 = silences2 = pedale
    unsigned char xResolution1; // tolerance horizontale x max pour la simultaneite
    unsigned char xResolution2; // tolerance horizontale x max en alignement verticale pour la simultaneite
    unsigned char appogg; // traiter les notes ornementales comme des appogiatures
    unsigned char mes_proportion; // traiter les mesures commes de proportions temporelles
    /// table des numeros de canal pour chaque instrument
	unsigned char canal2patch[MAXMIDICANAL];	// contient le no d'instr pour chaque canal
	unsigned char volume[MAXMIDICANAL];	// 64, volume, entre 0 et 128
	unsigned char piste2canal[MAXPORTNBRE+1];  
    
    // ----- //
    // fonts //
    MusWWGFont fonts[MAXPOLICES];
    
private:
    
};

#endif

