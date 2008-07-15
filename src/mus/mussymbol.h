/////////////////////////////////////////////////////////////////////////////
// Name:        mussymbol.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_SYMBOL_H__
#define __MUS_SYMBOL_H__

#ifdef __GNUG__
    #pragma interface "mussymbole.cpp"    
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

class MusStaff;
class MusNote;
#include "muselement.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// MusSymbol
//----------------------------------------------------------------------------

class MusSymbol: public MusElement
{
public:
    // constructors and destructors
    MusSymbol();
	MusSymbol( unsigned char _flag, unsigned char _calte, unsigned short _code );
    virtual ~MusSymbol();
    
    // WDR: method declarations for MusSymbol
	void Draw( wxDC *dc, MusStaff *staff);
	void dess_symb( wxDC *dc, int x, int y, int symc, int symf, MusStaff *pportee);
	/** muscle **/
	static void calcoffs (int *offst, int clid);
	void dess_cle ( wxDC *dc, int i, MusStaff *pportee);
	void afficheMesure ( wxDC *dc, MusStaff *staff);
	void mesCercle ( wxDC *dc, int x, int yy, MusStaff *staff );
	void demi_cercle ( wxDC *dc, int x, int yy, MusStaff *staff );
	void  inv_d_cercle ( wxDC *dc, int x, int yy, MusStaff *staff );
	void prol ( wxDC *dc, int x, int yy, MusStaff *staff );
	void stroke ( wxDC *dc, int a, int yy, MusStaff *staff );
	void chiffres ( wxDC *dc, int x, int y, MusStaff *staff);
	// changement
	virtual void SetPitch( int code, int oct, MusStaff *staff = NULL );
	virtual void SetValue( int code, MusStaff *staff = NULL, int vflag = 0 );
    
public:
    // WDR: member variable declarations for MusSymbol
    /** type de symbole
     @see JwgDef#BARRE JwgDef, valeurs de flag de Symbole pour les differents types */
    unsigned char flag;
    /** sous - type de symbole
     @see JwgDef#SOUSCATEG_1 JwgDef, valeurs de calte
     @see JwgDef#SILENCESPECIAL JwgDef, valeurs de calte (fonte = JwgDef.MARQUE_REPERE)
     @see JwgDef#DIESE JwgDef, valeurs de calte (flag = JwgDef.ALTER) */
    unsigned char calte;
    /** style de caractere. <pre>0 = droit<br>1 = italique<br>2 = gras</pre> */
    unsigned char carStyle;
    /** orientation des caracteres. <pre>0 = a gauche<br>1 = a droite<br>2 = haut<br>3 = bas</pre> */
    unsigned char carOrient;
    /** categorie de texte (DYN, LYRIC, ...)
     @see JwgDef#DYN JwgDef, valeurs de fonte de Symbole pour les chaines de caracteres */
    unsigned char fonte;
    /** symbole de liaison limites. <pre>0 = aucun<br>1 = debut<br>2 = fin</pre> */
    unsigned char s_lie_l;
    /** type de point. <pre>false = simple point<br>true = double point</pre> */
    char point;
    /** code du symbole
     @see JwgDef#SOL2 JwgDef, valeurs des codes de clefs */
    //unsigned short code; // moved in element
    /** longueur de la chaine de caracteres */
    unsigned short l_ptch;
	
	MusNote *m_note_ptr;
	bool m_hasAssociatedNote;

private:
    // WDR: handler declarations for MusSymbol

};


#endif
