/////////////////////////////////////////////////////////////////////////////
// Name:        wgstaff.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __WG_STAFF_H__
#define __WG_STAFF_H__

#ifdef AX_WG

#ifdef __GNUG__
    #pragma interface "wgstaff.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/dynarray.h"

// WDR: class declarations

#include "wgobject.h"

class WgElement;
WX_DECLARE_OBJARRAY( WgElement, ArrayOfWgElements);

class WgWindow;
class WgSymbole;
class WgNote;


//----------------------------------------------------------------------------
// WgStaff
//----------------------------------------------------------------------------

// sorting function
int SortElements(WgElement **first, WgElement **second);

class WgStaff: public WgObject
{
public:
    // constructors and destructors
    WgStaff();
	WgStaff( const WgStaff& staff ); // copy contructor
    virtual ~WgStaff();
    
    // WDR: method declarations for WgStaff
	void CheckIntegrity();
	void Clear( );
	void CopyAttributes( WgStaff *staff ); // copy all attributes but none of the elements
	// draw
	void DrawStaff( wxDC *dc, int i = -1 );
	void DrawStaffLines( wxDC *dc, int i = -1);
	void ClearElements( wxDC *dc , WgElement *start = NULL );
	WgElement *GetFirst( );
	WgElement *GetLast(  );
	WgElement *GetNext( WgElement *element );
	WgElement *GetPrevious( WgElement *element );
	WgElement *GetAtPos( int x );
	WgElement *Insert( WgElement *element ); // return a pointer on the inserted element
	void Delete( WgElement *element );
	WgElement *no_note ( WgElement *chk, unsigned int sens, unsigned int flg, int *succ);
	int trouveCodNote( int y_n, int x_pos, int *octave);
	int getOctCl ( WgElement *test, char *cle_id, int mlf = 0);
	void getOctDec (int ft, int _ot, int rupt, int *oct);
	int y_note (int code, int dec_clef, int oct);
	void updat_pscle (int i, WgElement *chk);
	void DrawSlur( wxDC *dc, int x1, int y1, int x2, int y2, bool up, int height = -1);

	void place_clef( wxDC *dc );
	int armatDisp ( wxDC *dc );
	/** wg_cle **/
	int testcle (int a);

    
public:
    // WDR: member variable declarations for WgStaff
	ArrayOfWgElements m_elements;
	/** nombre d'element sur la portee */
	unsigned int nblement;
	/** voix de la portee*/
	unsigned short voix;
	/** numero dans le groupe auquel appartient la portee */
	unsigned short noGrp;
	/** nombre total de portee dans le groupe */
	unsigned short totGrp;
	/** numero de systeme */
	unsigned short noLigne;
	/** numero de portee */
	unsigned short no;
	/** type d'armure. <pre>0 = aucune<br>1 = dieses<br>2 = bemols</pre> */
	unsigned char armTyp;
	/** nombre d'alterations a l'armures */
	unsigned char armNbr;
	/** portee en notation ancienne */
	char notAnc;
	/** portee grise */
	char grise;
	/** portee invisible */
	char invisible;
	/** ecart jusqu'a la portee, en interlignes */
	unsigned short ecart;
	/** barre verticale. <pre>0 = aucun<br>1 = debut<br>2 = fin</pre> */
	unsigned char vertBarre;
	/** accolade ou bloc. <pre>0 = aucun<br>1 = debut<br>2 = fin</pre> */
	unsigned char brace;
	/** taille. <pre>0 = normale<br>1 = petite </pre> */
	unsigned char pTaille;
	/** portee indentee
		char dans Wolfgang - ici indentation par portee
	*/
	int indent; 
	/** portee indentee a droite */
	char indentDroite;
	/** type de portee (nombre de lignes) */
	unsigned char portNbLine;
	/** type d'accolade. <pre>0 = bloc<br>1 = accolade ronde</pre> */
	unsigned char accol;
	/** ???? */
	unsigned char accessoire;
	/** reserve */
	unsigned short reserve;
	/** position y relative de la portee (non-enregistre dans les fichiers) */
	unsigned int yrel;
	/** postion x relative de la portee (non-enregistre dans les fichiers) */
	unsigned int xrel;

private:
	// WDR: handler declarations for WgStaff

};

#endif //AX_WG

#endif
