/////////////////////////////////////////////////////////////////////////////
// Name:        muspage.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_PAGE_H__
#define __MUS_PAGE_H__

#ifdef __GNUG__
	#pragma interface "muspage.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/dynarray.h"

class AxDC;

#include "musobject.h"


class MusStaff;
WX_DECLARE_OBJARRAY( MusStaff, ArrayOfMusStaves);

class MusStaffFunctor;

enum {
    PAGE_VALUES_VOICES = 0,
    PAGE_VALUES_INDENT
};


//----------------------------------------------------------------------------
// MusPage
//----------------------------------------------------------------------------

class MusPage: public MusObject
{
public:
    // constructors and destructors
    MusPage();
    virtual ~MusPage();
    
    void CheckIntegrity();
	
	MusStaff *GetFirst( );
	MusStaff *GetLast( );
	MusStaff *GetNext( MusStaff *staff );
	MusStaff *GetPrevious( MusStaff *staff );
	MusStaff *GetAtPos( int x, int y );
	void Clear( );
	/**
		dessine la page en cours = draw_it
	  */
	void DrawPage( AxDC *dc, bool background = true );
	//void ClearStaves( AxDC *dc , MusStaff *start = NULL );

	void UpdateStavesPosition();
    void SetValues( int type );

	/** musbarmes */
	void DrawBarres( AxDC *dc ); // mus == barres
	void braces ( AxDC *dc, int x, int y1, int y2, int cod, int pTaille);
	void accolade ( AxDC *dc, int x, int y1, int y2, int pTaille);
	void bardroit ( AxDC *dc, int x, int y1, int y2, int pTaille);
	void bar_mes ( AxDC *dc, int x, int cod, int porteeAutonome, MusStaff *pportee);
	void bigbarre( AxDC *dc, int x, char code, int porteeAutonome, MusStaff *pportee);
	void barMesPartielle ( AxDC *dc, int x, MusStaff *pportee);
    // moulinette
    virtual void Process(MusStaffFunctor *functor, wxArrayPtrVoid params );
    // functors
    void ProcessStaves( wxArrayPtrVoid params );
    void ProcessVoices( wxArrayPtrVoid params );
    void CountVoices( wxArrayPtrVoid params );
    
    
private:
    
public:
    ArrayOfMusStaves m_staves;
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
    
};


//----------------------------------------------------------------------------
// abstract base class MusPageFunctor
//------------------------------------------------------------------------- ---
class MusPageFunctor
{
private:
    void (MusPage::*fpt)( wxArrayPtrVoid params );   // pointer to member function

public:

    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    MusPageFunctor( void(MusPage::*_fpt)( wxArrayPtrVoid )) { fpt=_fpt; };
	virtual ~MusPageFunctor() {};

    // override function "Call"
    virtual void Call( MusPage *ptr, wxArrayPtrVoid params )
        { (*ptr.*fpt)( params);};          // execute member function
};



#endif
