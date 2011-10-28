/////////////////////////////////////////////////////////////////////////////
// Name:        muselement.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_ELEMENT_H__
#define __MUS_ELEMENT_H__

#ifdef __GNUG__
    #pragma interface "muselement.cpp"    
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

class AxDC;

#include "musobject.h"
#include "musdef.h"

class MusLaidOutStaff;


//----------------------------------------------------------------------------
// MusElement
//----------------------------------------------------------------------------

class MusElement: public MusObject
{
public:
    // constructors and destructors
    MusElement();
	MusElement( const MusElement& element ); // copy contructor
	MusElement& operator=( const MusElement& element); // copy assignement;
    virtual ~MusElement();		

	// common method
	int  filtrcod( int codElement, int *oct );
	int  filtrpitch(int pitchElement, int *oct);
    std::string PitchToStr(int pitch);
    int StrToPitch(std::string pitch);
	bool IsNote() { return ( TYPE == NOTE ); }
	bool IsSymbol() { return ( TYPE == SYMB ); }
    bool IsNeume() { return ( TYPE == NEUME ); }
	bool IsNeumeSymbol() { return (TYPE == NEUME_SYMB ); }
	virtual void deleteMeiRef() {};

    //	virtual void SetPitch( int code, int oct, MusLaidOutStaff *staff = NULL ) {};
	virtual void SetPitch( int code, int oct ) {};
	virtual void SetValue( int value, MusLaidOutStaff *staff = NULL, int flag = 0 ) {};
	virtual void ChangeColoration( MusLaidOutStaff *staff = NULL ) {};
	virtual void ChangeStem( MusLaidOutStaff *staff = NULL ) {};
	virtual void SetLigature( MusLaidOutStaff *staff = NULL ) {};
	virtual void Draw( AxDC *dc, MusLaidOutStaff *staff ) {};

      
public:
	/** type de l'element. JwgDef.NOTE = noteJwgDef.SYMB = symbol */
    char TYPE;
    /** element a l'interieur d'un liaison */
    char liaison;
    /** element debut d'une liaison */
    char dliai;
    /** element fin d'une liaison */
    char fliai;
    /** liaison vers le haut ou non */
    char lie_up;
    /** element a l'interieur d'un beaming */
    char rel;
    /** element debut d'un beaming */
    char drel;
    /** element fin d'un beaming */
    char frel;
    /** octave de l'element */
    unsigned char oct;
    /** element diminue */
    unsigned char dimin;
    /** groupe de valeurs */
    unsigned char grp;
    /** element sur une portee voisine */
    unsigned char _shport;
    /** ligature */
    char ligat;
    /** element invisible */
    char ElemInvisible;
    /** point de l'element invisible */
    char pointInvisible;
    /** contient une chaine de debordement */
    char existDebord;
    /** element fin de ligature */
    char fligat;
    /** affichage du groupe. 0 = automatique1 = ni chiffre ni barre2 = chiffre seul */
    unsigned char notschowgrp;
    /** acceleration des valeurs */
    char cone;
    /** ???? */
    unsigned char liaisonPointil;
    /** reserve */
    unsigned char reserve1;
    /** reserve */
    unsigned char reserve2;
    /** element a l'octave */
    unsigned char ottava;
    /** numerateur de duree */
    unsigned short durNum;
    /** denominateur de duree*/
    unsigned short durDen;
    /** offset de l'element par rapport a sa position x*/
    unsigned short offset;
    /** position x de l'element */
    unsigned int xrel;
    /** decalage y de l'element */
    int dec_y;
    /** chars de debordement de l'element */
    void* pdebord;
    /** code de la chaine de debordement */
    unsigned short debordCode;
    /** taille de la chaine de debordement, y compris code et taille */
    unsigned short debordSize;
	/** from symbol and note !! � l'exporation en wwg unsigned char dans note !! */
	unsigned short code;
	unsigned short pitch; //for neumes; this is the pitch of the first note in a neume.
	
	// additional information for comparison
	wxString m_im_filename;
	int m_im_staff;
	int m_im_pos;
	int m_cmp_flag;
	wxString m_debord_str;

	int no;

private:
	    

};


#endif
