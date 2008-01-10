/////////////////////////////////////////////////////////////////////////////
// Name:        wgpage.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_PAGE_H__
#define __MUS_PAGE_H__

#ifdef AX_WG

#ifdef __GNUG__
	#pragma interface "muspage.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/dynarray.h"

#include "musobject.h"

class WgStaff;
WX_DECLARE_OBJARRAY( WgStaff, ArrayOfWgStaves);

// WDR: class declarations

//----------------------------------------------------------------------------
// WgPage
//----------------------------------------------------------------------------

class WgPage: public WgObject
{
public:
    // constructors and destructors
    WgPage();
    virtual ~WgPage();
    
    // WDR: method declarations for WgPage
	void CheckIntegrity();
	
	WgStaff *GetFirst( );
	WgStaff *GetLast( );
	WgStaff *GetNext( WgStaff *staff );
	WgStaff *GetPrevious( WgStaff *staff );
	WgStaff *WgPage::GetAtPos( int y );
	void Clear( );
	/**
		dessine la page en cours = draw_it
	  */
	void DrawPage( wxDC *dc, bool background = true );
	void ClearStaves( wxDC *dc , WgStaff *start = NULL );

	void UpdateStavesPosition();

	/** wg_barmes */
	void DrawBarres( wxDC *dc ); // wg == barres
	void braces ( wxDC *dc, int x, int y1, int y2, int cod, int pTaille);
	void accolade ( wxDC *dc, int x, int y1, int y2, int pTaille);
	void bardroit ( wxDC *dc, int x, int y1, int y2, int pTaille);
	void bar_mes ( wxDC *dc, int x, int cod, int porteeAutonome, WgStaff *pportee);
	void bigbarre( wxDC *dc, int x, char code, int porteeAutonome, WgStaff *pportee);
	void barMesPartielle ( wxDC *dc, int x, WgStaff *pportee);
    
private:
    // WDR: member variable declarations for WgPage

public:
    // WDR: member variable declarations for WgPage
	ArrayOfWgStaves m_staves;
    /** numero de page */
    int npage;
    /** nombre de portees dans la page */
    short nbrePortees;
    /** contient un masque fixe */
    char noMasqueFixe;
    /** contient un masque variable */
    char noMasqueVar;
    /** reserve */
    unsigned char reserve;
    /** definition en mm des portees de la page */
    unsigned char defin;
    /** longueur en mm de l'indentation des portees de la page */
    int indent;
    /** longueur en mm de l'indentation droite des portees de la page */
    int indentDroite;
    /** longueur en mm des lignes de la pages */
    int lrg_lign;

private:
    // WDR: handler declarations for WgPage

};

#endif //AX_WG

#endif
