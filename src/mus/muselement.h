/////////////////////////////////////////////////////////////////////////////
// Name:        muselement.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_ELEMENT_H__
#define __MUS_ELEMENT_H__

#ifdef __GNUG__
    #pragma interface "muselement.cpp"    
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "musobject.h"
#include "musdef.h"

class MusStaff;

// WDR: class declarations

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
	void MusElement::ClearElement( wxDC *dc, MusStaff *staff );
	int MusElement::filtrcod( int codElement, int *oct );
	bool MusElement::IsNote() { return (TYPE != SYMB ); }
	bool IsSymbole() { return (TYPE == SYMB ); }
	wxClientDC *MusElement::InitAndClear( MusStaff *staff ); // init dc and clear element only if necessary
	void MusElement::DrawAndRelease( wxDC *dc, MusStaff *staff ); // draw element only if necessary and release dc

    // WDR: method declarations for MusElement
	virtual void MusElement::SetPitch( int code, int oct, MusStaff *staff = NULL ) {};
	virtual void MusElement::SetValue( int value, MusStaff *staff = NULL, int flag = 0 ) {};
	virtual void MusElement::ChangeColoration( MusStaff *staff = NULL ) {};
	virtual void MusElement::ChangeStem( MusStaff *staff = NULL ) {};
	virtual void MusElement::SetLigature( MusStaff *staff = NULL ) {};
	virtual void MusElement::Draw( wxDC *dc, MusStaff *staff ) {};

      
public:
    // WDR: member variable declarations for MusElement
    /** type de l'element. <pre>JwgDef.NOTE = note<br>JwgDef.SYMB = symbole</pre> */
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
    /** affichage du groupe. <pre>0 = automatique<br>1 = ni chiffre ni barre<br>2 = chiffre seul</pre> */
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
	/** from symbole and note !! à l'exporation en wwg unsigned char dans note !! */
	unsigned short code;
	
	// additional information for comparison
	wxString m_im_filename;
	int m_im_staff;
	int m_im_staff_segment;
	int m_im_pos;
	int m_cmp_flag;

	int no;

private:
	// WDR: handler declarations for MusElement
    

};


#endif
