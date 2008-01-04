/////////////////////////////////////////////////////////////////////////////
// Name:        wgelement.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __WG_ELEMENT_H__
#define __WG_ELEMENT_H__

#ifdef AX_WG

#ifdef __GNUG__
    #pragma interface "wgelement.cpp"    
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wgobject.h"
#include "wgdef.h"

class WgStaff;

// WDR: class declarations

//----------------------------------------------------------------------------
// WgElement
//----------------------------------------------------------------------------

class WgElement: public WgObject
{
public:
    // constructors and destructors
    WgElement();
	WgElement( const WgElement& element ); // copy contructor
	WgElement& operator=( const WgElement& element); // copy assignement;
    virtual ~WgElement();

	// common method
	void WgElement::ClearElement( wxDC *dc, WgStaff *staff );
	int WgElement::filtrcod( int codElement, int *oct );
	bool WgElement::IsNote() { return (TYPE != SYMB ); }
	bool IsSymbole() { return (TYPE == SYMB ); }
	wxClientDC *WgElement::InitAndClear( WgStaff *staff ); // init dc and clear element only if necessary
	void WgElement::DrawAndRelease( wxDC *dc, WgStaff *staff ); // draw element only if necessary and release dc

    // WDR: method declarations for WgElement
	virtual void WgElement::SetPitch( int code, int oct, WgStaff *staff = NULL ) {};
	virtual void WgElement::SetValue( int value, WgStaff *staff = NULL, int flag = 0 ) {};
	virtual void WgElement::ChangeColoration( WgStaff *staff = NULL ) {};
	virtual void WgElement::ChangeStem( WgStaff *staff = NULL ) {};
	virtual void WgElement::SetLigature( WgStaff *staff = NULL ) {};
	virtual void WgElement::Draw( wxDC *dc, WgStaff *staff ) {};

      
public:
    // WDR: member variable declarations for WgElement
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
	// WDR: handler declarations for WgElement
    

};

#endif //AX_WG

#endif
