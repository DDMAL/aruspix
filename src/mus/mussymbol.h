/////////////////////////////////////////////////////////////////////////////
// Name:        mussymbol.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_SYMBOL_H__
#define __MUS_SYMBOL_H__

#ifdef __GNUG__
    #pragma interface "mussymbol.cpp"    
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

class MusStaff;
class MusNote;
class MusNeume;

#include "muselement.h"


//----------------------------------------------------------------------------
// MusSymbol
//----------------------------------------------------------------------------

class MusSymbol: public MusElement
{
public:
    // constructors and destructors
    MusSymbol();
	MusSymbol( unsigned char _flag, unsigned char _calte, unsigned short _code );
	MusSymbol( const MusSymbol& symbol );
    virtual ~MusSymbol();
    
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
//	virtual void SetPitch( int code, int oct, MusStaff *staff = NULL );
	virtual void SetPitch( int code, int oct );	
	virtual void SetValue( int code, MusStaff *staff = NULL, int vflag = 0 );
	//
	void ResetToClef( );
	void ResetToSymbol( );
	void ResetToProportion( );
	
	void InsertCharInLyricAt( int x, char letter );
	bool DeleteCharInLyricAt( int x );
	bool IsLastLyricElementInNote( );
	bool IsLyricEmpty( );
    bool IsLyric( );
    
public:
        /** type de symbol
     @see JwgDef#BARRE JwgDef, valeurs de flag de Symbole pour les differents types */
    unsigned char flag;
    /** sous - type de symbol
     @see JwgDef#SOUSCATEG_1 JwgDef, valeurs de calte
     @see JwgDef#SILENCESPECIAL JwgDef, valeurs de calte (fonte = JwgDef.MARQUE_REPERE)
     @see JwgDef#DIESE JwgDef, valeurs de calte (flag = JwgDef.ALTER) */
    unsigned char calte;
    /** style de caractere. 0 = droit1 = italique2 = gras */
    unsigned char carStyle;
    /** orientation des caracteres. 0 = a gauche1 = a droite2 = haut3 = bas */
    unsigned char carOrient;
    /** categorie de texte (DYN, LYRIC, ...)
     @see JwgDef#DYN JwgDef, valeurs de fonte de Symbole pour les chaines de caracteres */
    unsigned char fonte;
    /** symbol de liaison limites. 0 = aucun1 = debut2 = fin */
    unsigned char s_lie_l;
    /** type de point. false = simple pointtrue = double point */
    char point;
    /** code du symbol
     @see JwgDef#SOL2 JwgDef, valeurs des codes de clefs */
    //unsigned short code; // moved in element
    /** longueur de la chaine de caracteres */
    unsigned short l_ptch;
	
	MusNote *m_note_ptr;
	MusNeume *m_neume_ptr; //temporary? --chrisniven
    
    static int s_durNum; // used in SetValue and can be change from the MusToolPanel (for example)
    static int s_durDen;

private:
    };


#endif
