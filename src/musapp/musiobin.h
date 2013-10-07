/////////////////////////////////////////////////////////////////////////////
// Name:        musiobin.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IOBIN_H__
#define __MUS_IOBIN_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"

#include "mus/musdoc.h"

enum
{
	MUS_BIN_ARUSPIX
};

class MusStaff;
class MusLayer;


//----------------------------------------------------------------------------
// MusBinInput_1_X
//----------------------------------------------------------------------------

/**
 * This class is a file input stream for binary object serialization for files
 * before version 2.0
*/
class MusBinInput_1_X: public wxFileInputStream
{
public:
    // constructors and destructors
    MusBinInput_1_X( MusDoc *file, wxString filename, int flag = MUS_BIN_ARUSPIX );
    virtual ~MusBinInput_1_X();
    
    bool ImportFile( );
	bool ReadFileHeader( unsigned short *nbpage );
	bool ReadSeparator( );
	bool ReadPage( MusPage *page );
	bool ReadStaff( MusStaff *staff, MusLayer *layer, int staffNo );
	bool ReadNote( MusLayer *layer );
	bool ReadSymbol( MusLayer *layer, bool isLyric = false );
    bool ReadNeume( );
    bool ReadLyric( );
	bool ReadElementAttr( );
    /*
     // We don't need to read this because it will be useless
	bool ReadPagination( MusPagination *pagination ) {};
	bool ReadHeaderFooter( MusWWGData *headerfooter) {};
     */
    
private:
    wxUint16 uint16;
	wxInt16 int16;
	wxUint32 uint32;
	wxInt32 int32;
	int m_flag;
    int m_vmaj, m_vmin, m_vrev;
	// logical tree
	MusStaff *m_logStaff;
    MusLayer *m_logLayer;
    // for reading files before 2.0.0
    unsigned short m_noLigne;
    char m_indent;
    char m_indentDroite;
    unsigned short ecarts[1024];
    
    /* old MusElement members */
    /* element a l'interieur d'un liaison */
    char liaison;
    /* element debut d'une liaison */
    char dliai;
    /* element fin d'une liaison */
    char fliai;
    /* liaison vers le haut ou non */
    char lie_up;
    /* element a l'interieur d'un beaming */
    char rel;
    /* element debut d'un beaming */
    char drel;
    /* element fin d'un beaming */
    char frel;
    /* octave de l'element */
    unsigned char oct;
    /* element diminue */
    unsigned char dimin;
    /* groupe de valeurs */
    unsigned char grp;
    /* element sur une portee voisine */
    unsigned char _shport;
    /* ligature */
    char ligat;
    /* element invisible */
    char ElemInvisible;
    /* point de l'element invisible */
    char pointInvisible;
    /* contient une chaine de debordement */
    char existDebord;
    /* element fin de ligature */
    char fligat;
    /* affichage du groupe. 0 = automatique1 = ni chiffre ni barre2 = chiffre seul */
    unsigned char notschowgrp;
    /* acceleration des valeurs */
    char cone;
    /* ???? */
    unsigned char liaisonPointil;
    /* reserve */
    unsigned char reserve1;
    /* reserve */
    unsigned char reserve2;
    /* element a l'octave */
    unsigned char ottava;
    /* numerateur de duree */
    unsigned short durNum;
    /* denominateur de duree*/
    unsigned short durDen;
    /* offset de l'element par rapport a sa position x*/
    unsigned short offset;
    /* position x de l'element */
    unsigned int x_abs;
    /* decalage y de l'element */
    int dec_y;
    /* chars de debordement de l'element */
    void* pdebord;
    /* code de la chaine de debordement */
    unsigned short debordCode;
    /* taille de la chaine de debordement, y compris code et taille */
    unsigned short debordSize;
    /* note / symbol code */
    unsigned short code;
    
    /* old MusNote members */
    /* silence (true) ou note (false) */
    char sil;
    /* valeur de la note */
    unsigned char val;
    /* couleur inverse */
    char inv_val;
    /* point. 0 = aucun1 = point2 = double point */
    unsigned char note_point;
    /* interruption de beaming */
    char stop_rel;
    /* alteration
     @see JwgDef#DIESE JwgDef, types d'alterations */
    unsigned char acc;
    /* accident invisible */
    char accInvis;
    /* position des hampes automatique */
    char q_auto;
    /* position de la hampe */
    char queue;
    /* staccato */
    char stacc;
    /* ???? */
    char oblique;
    /* ???? */
    char queue_lig;
    /* note faisant partie d'un accord */
    char chord;
    /* derniere note d'un accord */
    char fchord;
    /* note exentrique */
    char lat;
    /* ???? */
    char haste;
    /* code de la note (F2 a F8) */
    unsigned char tetenot;
    /* type de staccato (0 a 6) */
    unsigned char typStac;
    
    /* old MusSymbol members */
    unsigned char flag;
    /* sous - type de symbol */
    unsigned char calte;
    /* style de caractere. 0 = droit1 = italique2 = gras */
    unsigned char carStyle;
    /* orientation des caracteres. 0 = a gauche1 = a droite2 = haut3 = bas */
    unsigned char carOrient;
    /* categorie de texte */
    unsigned char fonte;
    /* symbol de liaison limites. 0 = aucun1 = debut2 = fin */
    unsigned char s_lie_l;
    /* type de point. false = simple pointtrue = double point */
    char symbol_point;
    /* code du symbol */
    unsigned short l_ptch;

protected:
    MusDoc *m_doc;
	
};


#endif
